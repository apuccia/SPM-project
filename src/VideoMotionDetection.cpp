#include <opencv2/opencv.hpp>

using namespace cv;

class VideoMotionDetection
{
private:
    std::string path;
    int k_size, p_size;
    int t_pixels;
    int t_frames;
    int f_width, f_height;
    int f_width_padded, f_height_padded;
    float thresh;
    Mat f_bg;
    VideoCapture capture;

public:
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
        t_frames = capture.get(CAP_PROP_FRAME_COUNT);
        f_width_padded = f_width + p_size;
        f_height_padded = f_height + p_size;
        t_pixels = f_width * f_height;

        f_bg = next_frame();
        imwrite("./bin/original_frame.jpg", f_bg);
        f_bg = pad_frame(f_bg);
        imwrite("./bin/padded_frame.jpg", f_bg);
        f_bg = to_greyscale(f_bg);
        imwrite("./bin/grey_frame.jpg", f_bg);
        convolve(f_bg);
        imwrite("./bin/convolved_frame.jpg", f_bg);
    }

    void print_info()
    {
        std::cout << "********** INFO **********" << std::endl;
        std::cout << "File path " << path << std::endl;
        std::cout << "Number of frames " << t_frames << std::endl;
        std::cout << "Pixels per frame " << t_pixels << std::endl;
        std::cout << "Kernel size " << k_size << std::endl;
        std::cout << "Pad rows/columns " << p_size << std::endl;
        std::cout << "Frame width " << f_width << std::endl;
        std::cout << "Frame height " << f_height << std::endl;
    }

    int get_num_frames()
    {
        return t_frames;
    }

    Mat next_frame()
    {
        Mat frame;
        capture >> frame;

        return frame;
    }

    Mat pad_frame(Mat& frame) {
        Mat f_padded;

        copyMakeBorder(
            frame, f_padded,
            p_size, p_size, p_size, p_size,
            BORDER_CONSTANT, Scalar(0));
        return f_padded;
    }

    Mat to_greyscale(Mat &frame)
    {
        Mat f_grey = Mat::zeros(frame.rows, frame.cols, CV_8UC1);

        for (int i = p_size; i < f_height_padded; i++)
            for (int j = p_size; j < f_width_padded; j++)
            {
                Vec3b pixel = frame.at<Vec3b>(i, j);

                f_grey.at<uchar>(i, j) = round((pixel[0] + pixel[1] + pixel[2]) / 3);
            }

        return f_grey;
    }

    void convolve(Mat &frame)
    {
        float total;
        for (int i = p_size; i < f_height_padded; i++)
            for (int j = p_size; j < f_width_padded; j++)
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

        for (int i = p_size; i < f_height_padded; i++)
            for (int j = p_size; j < f_width_padded; j++)
                total += f_bg.at<uchar>(i, j) != frame.at<uchar>(i, j);

        float perc = ((float)total / t_pixels) * 100;

        return perc > thresh;
    }
};