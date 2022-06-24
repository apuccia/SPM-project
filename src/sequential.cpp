#include <iostream>
#include <unistd.h>

#include "Utimer.cpp"
#include "VideoMotionDetection.cpp"

int main(int argc, char **argv)
{
    std::string path = "";
    int k_size = 0;
    float thresh = -1;
    bool stats = false;

    int opt;
    while ((opt = getopt(argc, argv, "k:f:t:s")) != -1)
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
        case 's':
            stats = true;
            break;
        case '?':
            std::cout << "Usage:\n"
                      << argv[0] << "\n"
                                    "[-k] kernel size\n"
                                    "[-f] file path\n"
                                    "[-t] detect threshold"
                      << std::endl;
        }
    }
    if (k_size == 0 || path.empty() || thresh == -1)
    {
        std::cout << "Usage:\n"
                  << argv[0] << "\n"
                                "[-k] kernel size\n"
                                "[-f] file path\n"
                                "[-t] detect threshold"
                  << std::endl;
        return -1;
    }

    VideoMotionDetection detector =
        VideoMotionDetection(path, k_size, thresh);
    Utimer timer = Utimer();
    if (stats) detector.print_info();

    int detected = 0;
    long t_read = 0, t_padding = 0, t_grey = 0, t_convolute = 0, t_detect = 0;
    while (1)
    {
        Mat frame;

        timer.start();
        frame = detector.next_frame();
        if (frame.empty())
            break;
        t_read += timer.stop();

        timer.start();
        frame = detector.pad_frame(frame);
        t_padding += timer.stop();

        timer.start();
        frame = detector.to_greyscale(frame);
        t_grey += timer.stop();

        timer.start();
        detector.convolve(frame);
        t_convolute += timer.stop();

        timer.start();
        detected += detector.detect(frame);
        t_detect += timer.stop();
    }

    int t_frames = detector.get_num_frames();
    std::cout << "---------- RESULTS ----------" << std::endl;
    std::cout << "Detected " << detected << " frames" << std::endl;
    std::cout << "Average time spent on reading: " << t_read / t_frames << std::endl;
    std::cout << "Average time spent on padding: " << t_padding / t_frames << std::endl;
    std::cout << "Average time spent on greyscaling: " << t_grey / t_frames << std::endl;
    std::cout << "Average time spent on convoluting: " << t_convolute / t_frames << std::endl;
    std::cout << "Average time spent on detecting: " << t_detect / t_frames << std::endl;
    std::cout << "-----------------------------" << std::endl;

    return 0;
}