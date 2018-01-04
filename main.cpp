#include <iostream>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>

cv::Mat resize_frame(cv::Mat&);
cv::Scalar cluster(cv::Mat&);
void generate_framebars(cv::Scalar&);

auto frame_count = 0;
cv::Mat final_image(300, 1000, CV_8UC3);


int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout<<"Usage: ./output <video_file_name>"<<std::endl;
        return -1;
    }

    const std::string source = argv[1];
    int counter = 1;
    cv::Mat frame;
    cv::Scalar main_cluster;

    cv::VideoCapture video = cv::VideoCapture(source);

    if(!video.isOpened())
    {
        std::cout<<"Couldn't open the file "<<source<<"."<<std::endl;
        return -2;
    }

    // using one frame per second of video
    int frame_skip = int{video.get(cv::CAP_PROP_FPS)};
    int total_frames = int{video.get(CV_CAP_PROP_FRAME_COUNT)};

    std::cout<<"Frame Rate: "<<frame_skip<<"\n"<<"Total Frames: "<<total_frames<<std::endl;

    // resizing final_image as per length of video
    int width = total_frames/frame_skip;
    cv::resize(final_image, final_image, cv::Size(width, width/4.67));


    while(true)
    {
        // http://answers.opencv.org/question/24714/
        video.grab();
        if(counter != frame_skip)
        {
            counter++;
            continue;
        }

        // decode the frame
        video.retrieve(frame);
        if(frame.empty())
        {
            std::cout<<"\nEnd of the video."<<std::endl;
            break;
        }

        // resizing the frame for better speed
        frame = resize_frame(frame);

        // flatten the frame for clustering
        frame = frame.reshape(3, frame.rows*frame.cols);

        // get RGB of biggest kmeans cluster
        main_cluster = cluster(frame);

        // add to final_image
        generate_framebars(main_cluster);

        // https://stackoverflow.com/questions/14858262/
        std::cout<<"Processing Frame "<<frame_count<<"/"<<width<<"\r";
        std::cout.flush();

        // reset counter
        counter = 1;
    }

    // saves final_image as png
    std::vector<int> compression_params = {CV_IMWRITE_PNG_COMPRESSION, 9};
    cv::imwrite(source + "_framebars.png", final_image, compression_params);

    std::cout<<frame_count<<" frames processed.\n";

    return 0;


}

cv::Mat resize_frame(cv::Mat& image)
{
    // taken from https://www.alanzucconi.com/2015/11/18/gamebarcode-a-study-of-colours-in-games/
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

    int cluster_count = 3;
    std::vector<int> count(cluster_count);
    int attempts = 1;
    cv::Mat labels, centers, dest;

    // converts Mat object to suitable type
    image.convertTo(image, CV_32F);
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 3, 0.1);

    double compactness = cv::kmeans(image, cluster_count, labels, criteria, attempts, cv::KMEANS_PP_CENTERS, centers);

    centers.convertTo(centers, CV_8UC3);

    // summing number of points per cluster
    for(auto i = 0; i < labels.rows; i++)
    {
        count[labels.at<int>(i)]++;
    }

    // index of biggest cluster
    auto index = std::distance(count.begin(), std::max_element(count.begin(), count.end()));

    // https://stackoverflow.com/questions/8932893/
    return centers.at<cv::Vec3b>(index);
}

void generate_framebars(cv::Scalar& scalar)
{
    // generate a single pixel wide line in final_image
    for(auto i = 0; i < final_image.rows; i++)
    {
        final_image.at<cv::Vec3b>(i, frame_count)[0] = scalar[0];
        final_image.at<cv::Vec3b>(i, frame_count)[1] = scalar[1];
        final_image.at<cv::Vec3b>(i, frame_count)[2] = scalar[2];
    }

    frame_count++;
}
