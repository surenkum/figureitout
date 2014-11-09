#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "surf_detect.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace fs = ::boost::filesystem;

// Global Variables
static const std::string OPENCV_WINDOW = "Input Frame";
cv::Mat obj_im; // Image of the target object we are looking for
std::string obj_name; // Name of the target object we are looking for
bool det_decision; // To find out if the object has been found
std::vector<cv::Point2f> scene_corners(4); // To store points after transformation

// return the filenames of all files that have the specified extension
// in the specified directory and all subdirectories
void find_classes(const fs::path& root,const std::string& ext,std::vector<std::string>& all_classes)
{  
    if (!fs::exists(root)) return;

    if (fs::is_directory(root))
    {
        fs::directory_iterator it(root);
        fs::directory_iterator endit;
        while(it != endit)
        {
            if (fs::is_regular_file(*it) and it->path().extension() == ext)
            {
                std::string current_class = it->path().stem().string();
                all_classes.push_back(current_class);
            }
            ++it;
        }
    }
}

class ImageConverter
{
    ros::NodeHandle nh_;
    image_transport::ImageTransport it_;
    image_transport::Subscriber image_sub_;
    image_transport::Publisher image_pub_;

    public:
    ImageConverter()
        : it_(nh_)
    {
        // Subscrive to input video feed and publish output video feed
        image_sub_ = it_.subscribe("/ardrone/front/image_raw", 1, 
                &ImageConverter::imageCb, this);
        image_pub_ = it_.advertise("/image_converter/output_video", 1);

        cv::namedWindow(OPENCV_WINDOW);
    }

    ~ImageConverter()
    {
        cv::destroyWindow(OPENCV_WINDOW);
    }

    void imageCb(const sensor_msgs::ImageConstPtr& msg)
    {
        cv_bridge::CvImagePtr cv_ptr;
        try
        {
            cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        }
        catch (cv_bridge::Exception& e)
        {
            ROS_ERROR("cv_bridge exception: %s", e.what());
            return;
        }

        // Do the main processing of displaying detections on image

        // Update GUI Window
        //cv::imshow(OPENCV_WINDOW, cv_ptr->image);
        //cv::waitKey(3);

        // Output modified video stream
        //image_pub_.publish(cv_ptr->toImageMsg());
        cv::Mat frame(cv_ptr->image);
        det_decision = detect_obj(obj_im,frame,scene_corners);
        if (det_decision){
            std::cout<<"Target Object"<<obj_name<<" Found"<<std::endl;
            //std::cout<<"Detection Decision "<<det_decision<<std::endl;
            //-- Draw lines between the corners (the mapped object in the scene - image_2 )
            line( frame, scene_corners[0] , scene_corners[1] , cv::Scalar(255, 255, 0), 4 );
            line( frame, scene_corners[1] , scene_corners[2] , cv::Scalar( 255, 255, 0), 4 );
            line( frame, scene_corners[2] , scene_corners[3] , cv::Scalar( 255, 255, 0), 4 );
            line( frame, scene_corners[3] , scene_corners[0] , cv::Scalar( 255, 255, 0), 4 );

        }
        cv::imshow(OPENCV_WINDOW,frame);
        cv::imshow("Target Object",obj_im);
        cv::waitKey(10);

    }
};

int main(int argc, char** argv)
{
    ros::init(argc, argv, "image_converter");
    // Do other processing here
    std::vector<std::string> all_classes;
   // Find all the available modules available
    fs::path data_path = "../data/"; // Path where all the images are stored
  // Extension of the files
    std::string files_ext = ".png"; // File extension of the cropped images that are stored
  find_classes(data_path,files_ext,all_classes); // Find all files in this folder with .png extension

    // List all the classes found
    std::cout<<"Currently available classes are: ";
    for (uint8_t i =0;i<all_classes.size();i++){
        std::cout<<all_classes[i]<<",";
    }
    std::cout<<std::endl;
    std::cout<<"Please enter the name of class you would like to find :";
    std::cin>>obj_name;
    std::cout<<std::endl<<"Will match class "<<obj_name<<" in incoming video stream"<<std::endl;

    obj_im = cv::imread(data_path.string()+obj_name+files_ext);

    ImageConverter ic;
    ros::spin();
    return 0;
}
