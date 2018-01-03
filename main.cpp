#include <iostream>
#include <string>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>


auto frame_count = 0;
cv::Mat final_image(300, 1000, CV_8UC3);

cv::Mat resize_frame(cv::Mat&);
cv::Scalar cluster(cv::Mat&);
void generate_framebars(cv::Scalar&);

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout<<"Usage: ./output <video_file_name>"<<std::endl;
        return -1;
    }

    const std::string source = argv[1];
    int frame_counter = 0, temp = 1;
    cv::VideoCapture video = cv::VideoCapture(source);
    cv::Mat frame;
    int frame_skip = int{video.get(cv::CAP_PROP_FPS)};
    //int frame_skip = 12;
    int frame_number = int{video.get(CV_CAP_PROP_FRAME_COUNT)};
    cv::Scalar main_cluster;
    std::cout<<"Frame Rate: "<<frame_skip<<"\n"<<"Total Frames: "<<frame_number<<std::endl;

    int width = frame_number/frame_skip;
    cv::resize(final_image, final_image, cv::Size(width, width/4.67));

    int counter = 0;
    //cv::namedWindow("Display", cv::WINDOW_NORMAL);
    //cv::resizeWindow("Display", 600, 300);

    //cv::namedWindow("Framebars", cv::WINDOW_NORMAL);
    //cv::resizeWindow("Framebars", 1000, 300);

    if(!video.isOpened())
    {
        std::cout<<"Couldn't open the file "<<source<<"."<<std::endl;
    }


    while(true)
    {
        // http://answers.opencv.org/question/24714/
        video >> frame;
        //cv::imshow("Display", frame);
        //cv::waitKey(1);
        if(temp != frame_skip)
        {
            temp++;
            continue;
        }

        if(frame.empty())
        {
            std::cout<<"\nEnd of the video."<<std::endl;
            break;
        }
        //cv::imshow("Display", frame);
        //cv::waitKey(1);
        //cv::imshow("Display", frame);


        frame = resize_frame(frame);
        //cv::imshow("Display", frame);

        frame = frame.reshape(3, frame.rows*frame.cols);
        main_cluster = cluster(frame);
        generate_framebars(main_cluster);
        counter++;

        // https://stackoverflow.com/questions/14858262/stdcout-wont-print
        std::cout<<"Processing Frame "<<counter<<"/"<<width<<"\r";
        std::cout.flush();
        //cv::waitKey(1);
        temp = 1;
        // break;
    }

    std::vector<int>compression_params = {CV_IMWRITE_PNG_COMPRESSION, 9};
    cv::imwrite(source + "_framebars.png", final_image, compression_params);
    std::cout<<counter<<" frames processed.\n";

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

cv::Scalar cluster(cv::Mat& image)
{
    // http://aishack.in/tutorials/kmeans-clustering-opencv/
    // https://docs.opencv.org/3.4.0/d1/d5c/tutorial_py_kmeans_opencv.html
    // https://docs.opencv.org/3.4.0/de/d63/kmeans_8cpp-example.html
    // https://stackoverflow.com/questions/10167534/

    int cluster_count = 5;
    std::vector<int> count(cluster_count);
    int attempts = 2;
    cv::Mat labels, centers, dest;

    // converts Mat object to suitable type
    image.convertTo(image, CV_32F);
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 10, 1.0);

    double compactness = cv::kmeans(image, cluster_count, labels, criteria, attempts, cv::KMEANS_PP_CENTERS, centers);

    centers.convertTo(centers, CV_8UC3);

    for(auto i = 0; i < labels.rows; i++)
    {
        count[labels.at<int>(i)]++;
    }


    auto index = std::distance(count.begin(), std::max_element(count.begin(), count.end()));

    // https://stackoverflow.com/questions/8932893/
    return centers.at<cv::Vec3b>(index);
}

void generate_framebars(cv::Scalar& scalar)
{
    for(auto i = 0; i < final_image.rows; i++)
    {
        final_image.at<cv::Vec3b>(i, frame_count)[0] = scalar[0];
        final_image.at<cv::Vec3b>(i, frame_count)[1] = scalar[1];
        final_image.at<cv::Vec3b>(i, frame_count)[2] = scalar[2];
    }

    frame_count++;
    //cv::imshow("Framebars", final_image);
    //cv::waitKey(1);
}
