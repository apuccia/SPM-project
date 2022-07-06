#include <iostream>
#include <unistd.h>

#include "Utimer.cpp"
#include "VideoMotionDetection.cpp"

int main(int argc, char **argv)
{
    std::string path = "";
    int k_size = 0;
    float thresh = -1;
    bool stats = false, conv_det = false;

    int opt;
    while ((opt = getopt(argc, argv, "k:f:t:s:c")) != -1)
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
        case 'c':
            conv_det = true;
            break;
        case '?':
            std::cout << "Usage:\n"
                      << argv[0] << "\n"
                                    "[-k] kernel size\n"
                                    "[-f] file path\n"
                                    "[-t] detect threshold\n"
                                    "[-c] (to use convolution + detection)"
                      << std::endl;

            return -1;
        }
    }
    if (k_size < 0 || k_size == 1 || k_size % 2 == 0 || path.empty() || thresh < 0 || thresh > 100)
    {
        std::cout << "Usage:\n"
                  << argv[0] << "\n"
                                "[-k] kernel size\n"
                                "[-f] file path\n"
                                "[-t] detect threshold\n"
                                "[-c] (to use convolution + detection)"
                  << std::endl;
        return -1;
    }

    VideoMotionDetection detector =
        VideoMotionDetection(path, k_size, thresh);
    if (stats)
        detector.print_info();
    Utimer timer = Utimer();

    int t_frames = detector.get_num_frames() - 1;
    int detected = 0;

    int iters = 10;
    // total times for all iterations
    long t_read = 0, t_padding = 0, t_grey = 0, t_convolute = 0, t_detect = 0, t_total = 0;
    for (int i = 0; i < iters; i++)
    {
        // total times per iteration
        long it_read = 0, it_padding = 0, it_grey = 0, it_convolute = 0, it_detect = 0;
        while (1)
        {
            timer.start();
            Mat frame;
            detector.next_frame(frame);
            if (frame.empty())
                break;
            it_read += timer.stop();

            timer.start();
            Mat f_padded;
            detector.pad_frame(frame, f_padded);
            it_padding += timer.stop();

            timer.start();
            Mat f_grey = Mat::zeros(f_padded.rows, f_padded.cols, CV_8UC1);
            detector.to_greyscale(f_padded, f_grey);
            it_grey += timer.stop();

            if (conv_det)
            {
                timer.start();
                detected += detector.convolve_detect(f_grey);
                it_convolute += timer.stop();
            }
            else
            {
                timer.start();
                Mat f_convolved = Mat::zeros(f_grey.rows, f_grey.cols, CV_8UC1);
                detector.convolve(f_grey, f_convolved);
                it_convolute += timer.stop();

                timer.start();
                detected += detector.detect(f_convolved);
                it_detect += timer.stop();
            }
        }

        t_read += it_read;
        t_padding += it_padding;
        t_grey += it_grey;
        t_convolute += it_convolute;
        t_detect += it_detect;

        // reset to first frame (background excluded)
        detector.reset_video();
    }

    t_total += (t_read + t_padding + t_grey + t_convolute + t_detect) / iters;
    t_read = t_read / iters;
    t_padding = t_padding / iters;
    t_grey = t_grey / iters;
    t_convolute = t_convolute / iters;
    t_detect = t_detect / iters;

    std::cout << "---------- RESULTS: average on " << iters << " iterations ----------" << std::endl;
    std::cout << "Total frames: " << t_frames << std::endl;
    std::cout << "Detected " << detected / iters << " frames" << std::endl;
    std::cout << "***** Read" << std::endl;
    std::cout << "Average time spent on reading a frame: " << t_read / t_frames << std::endl;
    std::cout << "Total time spent on reading frames: " << t_read << std::endl;
    std::cout << "***** Pad" << std::endl;
    std::cout << "Average time spent on padding a frame: " << t_padding / t_frames << std::endl;
    std::cout << "Total time spent on padding a frame: " << t_padding << std::endl;
    std::cout << "***** Greyscale" << std::endl;
    std::cout << "Average time spent on greyscaling a frame: " << t_grey / t_frames << std::endl;
    std::cout << "Total time spent on greyscaling a frame: " << t_grey << std::endl;

    if (conv_det)
    {
        std::cout << "***** Convolution & Detection" << std::endl;
        std::cout << "Average time spent on convoluting & detecting a frame: " << t_convolute / t_frames << std::endl;
        std::cout << "Total time spent on convoluting & detecting a frame: " << t_convolute << std::endl;
    }
    else
    {
        std::cout << "***** Convolution" << std::endl;
        std::cout << "Average time spent on convoluting a frame: " << t_convolute / t_frames << std::endl;
        std::cout << "Total time spent on convoluting a frame: " << t_convolute << std::endl;
        std::cout << "***** Detection" << std::endl;
        std::cout << "Average time spent on detecting a frame: " << t_detect / t_frames << std::endl;
        std::cout << "Total time spent on detecting a frame: " << t_detect << std::endl;
    }
    std::cout << "***** Total" << std::endl;
    std::cout << "Total time spent to process the whole stream: " << t_total << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl;

    return 0;
}