#include <iostream>
#include <string>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

auto frame_count = 0.0;
cv::Mat final_image(300, 1000, CV_8UC3);
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
    int frame_counter = 0, temp = 0;
    cv::VideoCapture video = cv::VideoCapture(source);
    cv::Mat frame;
    // int frame_skip = int{video.get(cv::CAP_PROP_FPS)};
    int frame_skip = 15;
    int frame_number = int{video.get(CV_CAP_PROP_FRAME_COUNT)};

    std::cout<<frame_skip<<"  "<<frame_number;

    int width = (frame_number/frame_skip) + 1;
    cv::resize(final_image, final_image, cv::Size(width, width/4.67));

    int counter = 0;
    cv::namedWindow("Display");

    if(!video.isOpened())
    {
        std::cout<<"Couldn't open the file "<<source<<"."<<std::endl;
    }


    while(true)
    {
        // http://answers.opencv.org/question/24714/
        video.grab();
        if(counter != frame_skip)
        {
            counter++;
            continue;
        }
        video.retrieve(frame);
        temp++;
        if(frame.empty())
        {
            std::cout<<"\nEnd of the video."<<std::endl;
            break;
        }

        // cv::imshow("Display", frame);


        resize_frame(frame);
	    cv::imshow("Display", frame);

        frame = frame.reshape(3, frame.rows*frame.cols);
        cluster(frame);
        cv::waitKey(1);
        counter = 0;
        // break;
    }

    cv::imwrite(source + "_framebars.png", final_image);
    std::cout<<temp<<" frames processed.\n";

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

    std::vector<int>::iterator result = std::max_element(count.begin(), count.end());
    auto index = std::distance(count.begin(), result);

    // https://stackoverflow.com/questions/8932893/
    cv::Scalar main_cluster = centers.at<cv::Vec3b>(index);

    cv::Mat mat(480, 640, CV_8UC3, main_cluster);
    //cv::namedWindow("Clusters");
    //cv::imshow("Clusters", mat);

    for(auto i = 0; i < final_image.rows; i++)
    {
        final_image.at<cv::Vec3b>(i, frame_count)[0] = main_cluster[0];
        final_image.at<cv::Vec3b>(i, frame_count)[1] = main_cluster[1];
        final_image.at<cv::Vec3b>(i, frame_count)[2] = main_cluster[2];
    }

    frame_count += 1;
    //cv::namedWindow("Framebars");
    //cv::imshow("Framebars", final_image);
    //cv::waitKey(1);


}
