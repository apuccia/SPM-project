#include <opencv2/opencv.hpp>

using namespace cv;

class VideoMotionDetection
{
public:
    std::string path;
    int k_size;
    int p_size;
    int t_pixels;
    int f_width;
    int f_height;
    float thresh;
    Mat f_bg;
    VideoCapture capture;

    VideoMotionDetection(std::string path, int k_size, float thresh)
    {
        this->path = path;
        this->k_size = k_size;
        this->thresh = thresh;
        
        p_size = k_size / 2;

        capture = VideoCapture(path);
        CV_Assert(capture.isOpened());

        f_width = capture.get(CAP_PROP_FRAME_WIDTH);
        f_height = capture.get(CAP_PROP_FRAME_HEIGHT);
        f_bg = next_frame();
        t_pixels = f_width * f_height;

        f_bg = to_greyscale(f_bg);
        imwrite("./bin/grey_frame.jpg", f_bg);
        convolve(f_bg);
        imwrite("./bin/convolved_frame.jpg", f_bg);
    }

    void print_info() {
        std::cout << "File path " << path << std::endl;
        std::cout << "Number of frames " << capture.get(CAP_PROP_FRAME_COUNT) << std::endl;
        std::cout << "Pixels per frame " << t_pixels << std::endl;
        std::cout << "Kernel size " << k_size * k_size << std::endl;
        std::cout << "Pad rows/columns " << p_size << std::endl;
        std::cout << "Frame width " << f_width << std::endl;
        std::cout << "Frame height " << f_height << std::endl;
    }

    Mat next_frame()
    {
        Mat frame;
        capture >> frame;

        if (frame.empty()) 
            return frame;

        copyMakeBorder(
            frame, frame,
            p_size, p_size, p_size, p_size, 
            BORDER_CONSTANT, Scalar(0));
        return frame;
    }

    Mat to_greyscale(Mat &frame)
    {
        Mat f_grey = Mat::zeros(frame.rows, frame.cols, CV_8UC1);

        for (int i = p_size; i < f_height; i++)
            for (int j = p_size; j < f_width; j++)
            {
                Vec3b pixel = frame.at<Vec3b>(i, j);

                f_grey.at<uchar>(i, j) = round((pixel[0] + pixel[1] + pixel[2]) / 3);
            }

        return f_grey;
    }

    void convolve(Mat &frame)
    {
        float total;
        for (int i = p_size; i < f_height; i++)
            for (int j = p_size; j < f_width; j++)
            {
                total = 0;

                for (int k = i - p_size; k <= i + p_size; k++)
                    for (int z = j - p_size; z <= j + p_size; z++) 
                        total += frame.at<uchar>(k, z);

                frame.at<uchar>(i, j) = round(total / (k_size * k_size));
            }
    }

    bool detect(Mat &frame)
    {
        int total = 0;

        for (int i = p_size; i < f_height; i++)
            for (int j = p_size; j < f_width; j++)
                total += f_bg.at<uchar>(i, j) != frame.at<uchar>(i, j);

        float perc = ((float)total / t_pixels) * 100;

        return perc > thresh;
    }
};