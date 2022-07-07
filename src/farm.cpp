#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <unistd.h>

#include <ff/ff.hpp>

#include "Utimer.cpp"
#include "ff_nodes_implementations.cpp"
#include "ConcurrentDeque.cpp"

void cpp_threads(std::string path, int k_size, float thresh, int nw, bool stats)
{
    ConcurrentDeque<Mat> deque;
    std::atomic<int> detected(0);
    VideoMotionDetection detector =
        VideoMotionDetection(path, k_size, thresh);
    int total_frames = detector.get_num_frames() - 1;

    if (stats)
    {
        detector.print_info();
        std::cout << "Number of farm workers: " << nw << std::endl;
    }

    Utimer timer_read, timer_worker, timer_completion;
    long total_completion = 0;
    int iters = 5;

    for (int i = 0; i < iters; i++)
    {
        timer_completion.start();
        std::vector<std::thread> workers = std::vector<std::thread>(nw);
        for (int j = 0; j < nw; j++)
        {
            workers.at(j) = std::thread(
                [&deque, &detector, &detected]
                {
                    while (true)
                    {
                        Mat frame = deque.pop();
                        if (frame.empty())
                            break;
                        Mat f_padded;
                        detector.pad_frame(frame, f_padded);

                        Mat f_grey = Mat::zeros(f_padded.rows, f_padded.cols, CV_8UC1);
                        detector.to_greyscale(f_padded, f_grey);

                        Mat f_convolved = Mat::zeros(f_grey.rows, f_grey.cols, CV_8UC1);
                        detected += detector.convolve_detect(f_grey);
                    }
                });
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

        total_completion += timer_completion.stop();

        // reset to first frame (background excluded) for next iteration
        detector.reset_video();
    }

    total_completion = total_completion / iters;

    std::cout << "---------- RESULTS(" << nw << " workers): average on " << iters << " iterations ----------" << std::endl;
    std::cout << "(CPP) Total frames: " << total_frames << std::endl;
    std::cout << "(CPP) Detected: " << detected / iters << " frames" << std::endl;
    std::cout << "(CPP) Completion time: " << total_completion << std::endl;
    std::cout << "(CPP) Service time: " << total_completion / total_frames << std::endl;
}

void fast_flow(std::string path, int k_size, float thresh, int nw, bool auto_scheduling)
{
    int iters = 10;
    std::atomic<int> detected(0);
    VideoMotionDetection detector =
        VideoMotionDetection(path, k_size, thresh);
    int total_frames = detector.get_num_frames() - 1;

    Utimer timer_completion;
    long total_completion = 0;

    for (int i = 0; i < iters; i++)
    {
        timer_completion.start();

        std::vector<std::unique_ptr<ff::ff_node>> workers(nw);
        for (int j = 0; j < nw; j++)
            workers[j] = std::make_unique<FullWorker>(&detector, &detected);

        Loader loader(&detector);
        ff::ff_Farm<Mat> farm(std::move(workers), loader);

        farm.remove_collector();
        if (auto_scheduling)
            farm.set_scheduling_ondemand();

        farm.run_and_wait_end();

        total_completion += timer_completion.stop();

        // reset to first frame (background excluded) for next iteration
        detector.reset_video();
    }

    total_completion = total_completion / iters;

    std::cout << "(FF " << auto_scheduling << ") Total frames: " << total_frames << std::endl;
    std::cout << "(FF " << auto_scheduling << ") Detected: " << detected / iters << " frames" << std::endl;
    std::cout << "(FF " << auto_scheduling << ") Completion time: " << total_completion << std::endl;
    std::cout << "(FF " << auto_scheduling << ") Service time: " << total_completion / total_frames << std::endl;
}

int main(int argc, char **argv)
{
    std::string path = "";
    int k_size = 0, nw = 0;
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
    if (k_size < 0 || k_size == 1 || k_size % 2 == 0 || path.empty() || thresh < 0 || thresh > 100 || nw < 1)
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

    cpp_threads(path, k_size, thresh, nw, stats);
    fast_flow(path, k_size, thresh, nw, 0);
    fast_flow(path, k_size, thresh, nw, 1);

    return 0;
}