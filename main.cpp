#include <iostream>
#include <string>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

cv::Mat resize_frame(cv::Mat&);
void cluster(cv::Mat&);

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout<<"Usage: ./output <video_file_name>"<<std::endl;
        return -1;
    }

    const std::string source = argv[1];

    cv::VideoCapture video = cv::VideoCapture(source);
    cv::Mat frame, kmeans_labels;
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

        resize_frame(frame);
        cv::imshow("Display", frame);
        cv::waitKey(0);


        frame = frame.reshape(3, frame.rows*frame.cols);
        cv::imshow("Display", frame);
        cluster(frame);
        cv::waitKey(0);
        break;
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

void cluster(cv::Mat& image)
{
    // http://aishack.in/tutorials/kmeans-clustering-opencv/
    // https://docs.opencv.org/3.4.0/d1/d5c/tutorial_py_kmeans_opencv.html
    // https://docs.opencv.org/3.4.0/de/d63/kmeans_8cpp-example.html
    // https://stackoverflow.com/questions/10167534/
    int cluster_count = 5;
    int attempts = 2;
    cv::Mat labels, centers, dest;

    // converts Mat object to suitable type
    image.convertTo(image, CV_32F);
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 10, 1.0);

    double compactness = cv::kmeans(image, cluster_count, labels, criteria, attempts, cv::KMEANS_PP_CENTERS, centers);

    std::cout<<"Before conversion:\n\n"<<centers;

    centers.convertTo(centers, CV_8UC3);
    std::cout<<"\n\nAfter conversion:\n\n"<<centers;


    cv::namedWindow("Clusters");
    cv::imshow("Clusters", centers);
    cv::waitKey(0);


}
