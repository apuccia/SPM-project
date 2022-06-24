#include <iostream>
#include <unistd.h>

#include "Utimer.cpp"
#include "VideoMotionDetection.cpp"

int main(int argc, char** argv) {
    std::string path = "";
    int k_size = 0;
    float thresh = -1;

    int opt;
    while((opt = getopt(argc, argv, "k:f:t:")) != -1) {
        switch(opt) {
        case 'k':
            k_size = atoi(optarg);
            break;
        case 'f':
            path = optarg;
            break;
        case 't': 
            thresh = atof(optarg);
            break;
        case '?':
            std::cout << 
                "Usage:\n" << argv[0] << "\n"
                "[-k] kernel size\n"
                "[-f] file path\n"
                "[-t] detect threshold" << std::endl;
        }
    }
    if (k_size == 0 || path.empty() || thresh == -1) {
        std::cout << 
            "Usage:\n" << argv[0] << "\n"
            "[-k] kernel size\n"
            "[-f] file path\n"
            "[-t] detect threshold" << std::endl;
        return -1;
    }


    VideoMotionDetection detector = 
        VideoMotionDetection(path, k_size, thresh);
    detector.print_info();

    int detected = 0;
    while(1) {
        Mat frame = detector.next_frame();

        if (frame.empty()) break;

        frame = detector.to_greyscale(frame);
        detector.convolve(frame);

        detected += detector.detect(frame);    
    }

    std::cout << "Detected " << detected << " frames." << std::endl;

    return 0;
}