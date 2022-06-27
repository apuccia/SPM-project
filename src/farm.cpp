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

    ConcurrentDeque<Mat> deque;
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
        std::vector<std::thread> workers = std::vector<std::thread>(nw);
        for (int j = 0; j < nw; j++)
        {
            workers.at(j) = std::thread([&deque, &detector, &detected]
                                        {
            while (true) {
                Mat frame = deque.pop();

                if (frame.empty()) break;

                Mat f_padded;
                detector.pad_frame(frame, f_padded);
                Mat f_grey = Mat::zeros(f_padded.rows, f_padded.cols, CV_8UC1);
                detector.to_greyscale(f_padded, f_grey);
                Mat f_convolved = Mat::zeros(f_grey.rows, f_grey.cols, CV_8UC1);
                
                if (detector.convolve_detect(f_grey, f_convolved)) 
                    detected++;
            } });
        }

        while (true)
        {
            Mat frame;
            detector.next_frame(frame);

            if (frame.empty())
            {
                deque.push_empty(nw, frame);
                break;
            }

            deque.push(frame);
        }

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