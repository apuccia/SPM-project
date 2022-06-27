#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <unistd.h>

#include "Utimer.cpp"
#include "VideoMotionDetection.cpp"
#include "ConcurrentDeque.cpp"

int main(int argc, char **argv)
{
    std::string path = "";
    int k_size = 0, nw = 1;
    float thresh = -1;
    bool stats = false;

    int opt;
    while ((opt = getopt(argc, argv, "k:f:t:sn:")) != -1)
    {
        switch (opt)
        {
        case 'k':
            k_size = atoi(optarg);
            break;
        case 'f':
            path = optarg;
            break;
        case 't':
            thresh = atof(optarg);
            break;
        case 'n':
            nw = atoi(optarg);
            break;
        case 's':
            stats = true;
            break;
        case '?':
            std::cout << "Usage:\n"
                      << argv[0] << "\n"
                                    "[-k] kernel size\n"
                                    "[-f] file path\n"
                                    "[-t] detect threshold\n"
                                    "[-n] number of workers"
                      << std::endl;
        }
    }
    if (k_size == 0 || path.empty() || thresh == -1 || nw < 1)
    {
        std::cout << "Usage:\n"
                  << argv[0] << "\n"
                                "[-k] kernel size\n"
                                "[-f] file path\n"
                                "[-t] detect threshold\n"
                                "[-n] number of workers"
                  << std::endl;
        return -1;
    }

    ConcurrentDeque<Mat> read_deque;
    ConcurrentDeque<Mat> s1s2_deque;
    ConcurrentDeque<Mat> s2s3_deque;
    std::atomic<int> detected(0);
    VideoMotionDetection detector =
        VideoMotionDetection(path, k_size, thresh);
    int total_frames = detector.get_num_frames();

    if (stats)
    {
        detector.print_info();
        std::cout << "Number of farm workers: " << nw << std::endl;
    }

    Utimer timer;
    int iters = 5;
    long total = 0;

    for (int i = 0; i < iters; i++)
    {
        timer.start();
        // creating thread that will perform padding
        std::thread t_padding = std::thread([nw, &read_deque, &s1s2_deque, &detector]
                                            {
            while (true) {
                Mat frame = read_deque.pop();
                
                Mat f_padded;

                if (frame.empty()) {
                    s1s2_deque.push(frame);
                    break;
                }
                
                detector.pad_frame(frame, f_padded);
                
                s1s2_deque.push(f_padded);
            } });

        // creating thread that will perform greying
        std::thread t_greying = std::thread([nw, &s1s2_deque, &s2s3_deque, &detector]
                                            {
            while (true) {
                Mat frame = s1s2_deque.pop();
                Mat f_grey = Mat::zeros(frame.rows, frame.cols, CV_8UC1);

                if (frame.empty()) {
                    s2s3_deque.push_empty(nw, frame);
                    break;
                }
                
                detector.to_greyscale(frame, f_grey);

                s2s3_deque.push(f_grey);
            } });

        // creating thread that will perform convolution & detection
        std::vector<std::thread> workers = std::vector<std::thread>(nw);
        for (int j = 0; j < nw; j++)
        {
            workers.at(j) = std::thread([&s2s3_deque, &detector, &detected]
                                        {
            while (true) {
                Mat frame = s2s3_deque.pop();
                Mat f_convolved = Mat::zeros(frame.rows, frame.cols, CV_8UC1);

                if (frame.empty()) break;

                if (detector.convolve_detect(frame, f_convolved)) 
                    detected++;
            } }); 
        } 

        while (true)
        {
            Mat frame;
            detector.next_frame(frame);

            if (frame.empty())
            {
                read_deque.push(frame);
                break;
            }

            read_deque.push(frame);
        }

        t_padding.join();
        t_greying.join();
        for (int j = 0; j < nw; j++)
            workers.at(j).join();
        total += timer.stop();

        // reset to first frame (background excluded)
        detector.reset_video();
    }

    long avg = total / iters;
    std::cout << "---------- RESULTS: average on " << iters << " iterations ----------" << std::endl;
    std::cout << "Total frames " << total_frames << std::endl;
    std::cout << "Detected " << detected / iters << " frames" << std::endl;
    std::cout << "Total time to process the whole stream " << avg << std::endl;
    std::cout << "Service time: " << avg / total_frames << std::endl;

    return 0;
}