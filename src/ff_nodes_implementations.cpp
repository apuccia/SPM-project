#include <ff/ff.hpp>
#include "VideoMotionDetection.cpp"

class Loader : public ff::ff_node_t<Mat>
{
private:
    VideoMotionDetection *detector;

public:
    Loader(VideoMotionDetection *detector)
    {
        this->detector = detector;
    }

    Mat *svc(Mat *)
    {
        while (true)
        {
            Mat *frame = new Mat();
            detector->next_frame(*frame);

            if ((*frame).empty())
                return EOS;

            return frame;
        }
    }
};

class FullWorker : public ff::ff_node_t<Mat, void>
{
private:
    VideoMotionDetection *detector;
    std::atomic_int *detected;

public:
    FullWorker(VideoMotionDetection *detector, std::atomic_int *detected)
    {
        this->detector = detector;
        this->detected = detected;
    }

    void *svc(Mat *frame)
    {
        Mat f_padded;
        detector->pad_frame(*frame, f_padded);

        Mat f_grey = Mat::zeros(f_padded.rows, f_padded.cols, CV_8UC1);
        detector->to_greyscale(f_padded, f_grey);

        Mat f_convolved = Mat::zeros(f_grey.rows, f_grey.cols, CV_8UC1);
        *detected += detector->convolve_detect(f_grey);

        delete (frame);

        return GO_ON;
    }
};

class Padder : public ff::ff_node_t<Mat>
{
private:
    VideoMotionDetection *detector;

public:
    Padder(VideoMotionDetection *detector)
    {
        this->detector = detector;
    }

    Mat *svc(Mat *frame)
    {
        Mat *f_padded = new Mat();
        *f_padded = Mat::zeros((*frame).rows, (*frame).cols, CV_8UC1);

        detector->pad_frame(*frame, *f_padded);

        delete (frame);

        return f_padded;
    }
};

class Greyscaler : public ff::ff_node_t<Mat>
{
private:
    VideoMotionDetection *detector;

public:
    Greyscaler(VideoMotionDetection *detector)
    {
        this->detector = detector;
    }

    Mat *svc(Mat *frame)
    {
        Mat *f_grey = new Mat();
        *f_grey = Mat::zeros((*frame).rows, (*frame).cols, CV_8UC1);

        detector->to_greyscale(*frame, *f_grey);

        delete (frame);

        return f_grey;
    }
};

class ConvolveDetectWorker : public ff::ff_node_t<Mat, void>
{
private:
    VideoMotionDetection *detector;
    std::atomic_int *detected;

public:
    ConvolveDetectWorker(VideoMotionDetection *detector, std::atomic_int *detected)
    {
        this->detector = detector;
        this->detected = detected;
    }

    void *svc(Mat *frame)
    {
        Mat *f_convolved = new Mat();
        *f_convolved = Mat::zeros((*frame).rows, (*frame).cols, CV_8UC1);

        *detected += detector->convolve_detect(*frame);

        delete (frame);

        return GO_ON;
    }
};