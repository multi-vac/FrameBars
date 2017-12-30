#include <iostream>
#include <string>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>


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
    }



    return 0;


}
