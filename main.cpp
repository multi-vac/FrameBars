#include <iostream>
#include <string>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

cv::Mat resize_frame(cv::Mat&);

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout<<"Usage: ./output <video_file_name>"<<std::endl;
        return -1;
    }

    const std::string source = argv[1];

    cv::VideoCapture video = cv::VideoCapture(source);
    cv::Mat frame;
    cv::namedWindow("Display");

    if(!video.isOpened())
    {
        std::cout<<"Couldn't open the file "<<source<<"."<<std::endl;
    }


    while(true)
    {
        video >> frame;
        if(frame.empty())
        {
            std::cout<<"End of the video."<<std::endl;
            break;
        }

        cv::imshow("Display", frame);
        cv::waitKey(0);

        resize_frame(frame);
        cv::imshow("Display", frame);
        cv::waitKey(0);

        frame = frame.reshape(3, frame.rows*frame.cols);
        cv::imshow("Display", frame);
        cv::waitKey(0);
    }



    return 0;


}

cv::Mat resize_frame(cv::Mat& image)
{
    cv::Size size = image.size();

    double height = size.height;
    double width = size.width;

    double great = (height > width)?height:width;
    int height_new = int{100 * height/great};
    int width_new = int{100 * width/great};

    size.height = height_new;
    size.width = width_new;

    cv::resize(image, image, size);

    return image;
}
