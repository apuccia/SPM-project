# SPM-project

Simple motion detection may be implemented by subtracting video frames from some background
images. We require to implement a parallel video motion detector processing video such that:

1. The first frame of the video is taken as "background picture".
2. Each frame is turned to greyscale, smoothed, and subtracted to the greyscale smoothed
background picture.
3. Motion detect flag will be true in case more the k% of the pixels differ.
4. The output of the program is represented by the number of frames that are motion detected.

OpenCV may be used to open video file and read frames only (greyscale and smoothing should be
programmed explicitly. Converting an image to greyscale can be achieved by substituting each (R,G,B)
pixel with a grey pixel with a grey value which is the average of the R, G and B values. Smoothing requires
pixel is obtained “averaging” its value with the value of the surrounding pixels, some padding is added to ease the computation for boundaries pixels.

The project requires to devise a parallel application implemented by using *C++ standard threads* and *FastFlow* library, then performs analysises regarding its *completion time*, *scalability*, *speedup* and *efficiency*. The solutions that were proposed are two:

1. The first implements a normal form: Farm(Comp(padding, greying, convoluting + detecting)). The emitter of the farm is a thread that implements the reading of a frame and there is no collector.
2. The second implements a pipeline: Pipe(reading, padding, greying, Farm(convoluting + detecting)). As before the emitter for the farm component is a thread that implements the reading of a frame and there is no collector.
