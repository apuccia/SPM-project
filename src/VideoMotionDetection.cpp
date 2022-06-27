#include <opencv2/opencv.hpp>

using namespace cv;

class VideoMotionDetection
{
private:
    // file path of the video
    std::string path;
    // kernel square side size
    int k_size;
    // rows/columns to add in each frame side
    int p_size;
    // total pixels per frame
    int t_pixels;
    // total frames of the video
    int t_frames;
    // frame sizes
    int f_width, f_height;
    // frame sizes + one side padding
    int f_width_padded, f_height_padded;
    // detection pixel threshold
    float thresh;
    // background frame
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

        Mat f;
        next_frame(f);
        imwrite("./bin/original_frame.jpg", f);

        Mat f_padded;
        pad_frame(f, f_padded);
        imwrite("./bin/padded_frame.jpg", f_padded);

        Mat f_grey = Mat::zeros(f_padded.rows, f_padded.cols, CV_8UC1);
        to_greyscale(f_padded, f_grey);
        imwrite("./bin/grey_frame.jpg", f_grey);

        Mat f_convolved = Mat::zeros(f_padded.rows, f_padded.cols, CV_8UC1); 
        convolve(f_grey, f_convolved);
        imwrite("./bin/convolved_frame.jpg", f_convolved);

        f_convolved.copyTo(f_bg);
    }

    void print_info()
    {
        std::cout << "********** INFO **********" << std::endl;
        std::cout << "File path: " << path << std::endl;
        std::cout << "Number of frames: " << t_frames << std::endl;
        std::cout << "Pixels per frame: " << t_pixels << std::endl;
        std::cout << "Kernel size: " << k_size << std::endl;
        std::cout << "Pad rows/columns: " << p_size << std::endl;
        std::cout << "Frame width: " << f_width << std::endl;
        std::cout << "Frame height: " << f_height << std::endl;
    }

    int get_num_frames()
    {
        return t_frames;
    }

    void reset_video()
    {
        capture.set(CAP_PROP_POS_FRAMES, 1);
    }
    
    void next_frame(Mat &frame)
    {
        capture >> frame;
    }

    void pad_frame(Mat &frame, Mat &f_padded)
    {
        copyMakeBorder(
            frame, f_padded,
            p_size, p_size, p_size, p_size,
            BORDER_CONSTANT, Scalar(0));
    }

    void to_greyscale(Mat &frame, Mat &f_grey)
    {
        for (int i = p_size; i < f_height_padded; i++)
            for (int j = p_size; j < f_width_padded; j++)
            {
                Vec3b pixel = frame.at<Vec3b>(i, j);
                f_grey.at<uchar>(i, j) = (pixel[0] + pixel[1] + pixel[2]) / 3;
            }
    }

    void convolve(Mat &frame, Mat &f_convolved)
    {
        long total;
        int ker_dim = k_size * k_size;
        for (int i = p_size; i < f_height_padded; i++)
            for (int j = p_size; j < f_width_padded; j++)
            {
                total = 0;

                for (int k = i - p_size; k <= i + p_size; k++)
                    for (int z = j - p_size; z <= j + p_size; z++)
                        total += frame.at<uchar>(k, z);

                f_convolved.at<uchar>(i, j) = total / ker_dim;
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

    bool convolve_detect(Mat &frame, Mat &f_convolved) {
        long sum;
        int diff_pixels= 0;
        int ker_dim = k_size * k_size;
        for (int i = p_size; i < f_height_padded; i++)
            for (int j = p_size; j < f_width_padded; j++)
            {
                sum = 0;

                for (int k = i - p_size; k <= i + p_size; k++)
                    for (int z = j - p_size; z <= j + p_size; z++)
                        sum += frame.at<uchar>(k, z);

                f_convolved.at<uchar>(i, j) = sum / ker_dim;
                diff_pixels += f_bg.at<uchar>(i, j) != f_convolved.at<uchar>(i, j);
            }

        float perc = ((float)diff_pixels / t_pixels) * 100;

        return perc > thresh;
    }
};