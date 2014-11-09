#include <opencv2/opencv.hpp>
#include <surf_detect.hpp>
#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace fs = ::boost::filesystem;

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

int main(int argc,char** argv){
    cv::VideoCapture cap(1); // Open the secondary camera, use 0 for default
    if(!cap.isOpened()){ // Check if we succeded
        std::cout<<"No camera found \n";
    }
    // Find all the available modules available
    fs::path data_path = "../data/"; // Path where all the images are stored
    // To store all the classes that are found
    std::vector<std::string> all_classes;
    // Extension of the files
    std::string files_ext = ".png"; // File extension of the cropped images that are stored
    // What class are we looking for
    std::string obj_name;

    cv::Mat obj_im; // To store the image of target domain
    cv::Mat frame; // Incoming video stream

    bool det_decision; // Detection decision
    std::vector<cv::Point2f> scene_corners(4); // To store points after transformation

    find_classes(data_path,files_ext,all_classes); // Find all files in this folder with .png extension

    // List all the classes found
    std::cout<<"Currently available classes are: ";
    for (int i =0;i<all_classes.size();i++){
        std::cout<<all_classes[i]<<",";
    }
    std::cout<<std::endl;
    std::cout<<"Please enter the name of class you would like to find :";
    std::cin>>obj_name;
    std::cout<<std::endl<<"Will match class "<<obj_name<<" in incoming video stream"<<std::endl;

    obj_im = cv::imread(data_path.string()+obj_name+files_ext);
    // Keep processing the video and looking for our target object
    while(1){
        cap>>frame;
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
        cv::imshow("Input Frame",frame);
        cv::imshow("Target Object",obj_im);
        // Press Esc key for exit
        if(cv::waitKey(30) >= 0) break;

    }
    return 0;
}
