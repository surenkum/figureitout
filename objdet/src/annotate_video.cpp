#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

cv::Rect obj_box;
// To handle mouse callback
static void mouseHandler(int event, int x, int y, int flags, void* param){
    // Check various key presses of the mouse
    switch( event )
    {
        case CV_EVENT_LBUTTONDOWN:
            {
                obj_box = cv::Rect( x, y, 0, 0 );
            }
            break;
        case CV_EVENT_LBUTTONUP:
            {
               obj_box.width = x-obj_box.x;
               obj_box.height = y-obj_box.y;
            }
            break;
        default:
            break;
    }
}

int main(int argc,char** argv){
    cv::VideoCapture cap(1); // Open the secondary camera, use 0 for default
    if(!cap.isOpened()){ // Check if we succeded
        std::cout<<"No camera found \n";
    }
    // All the declarations of variables
    cv::Mat frame,dummy_frame; // For storing the current frame from camera and displaying
    cv::Mat obj_im; // To store the target object image
    bool callback = true; // To enable callback for mouse event
    char inp_string; // To get user input
    std::string obj_name; // To get the name of the object from user

    
    cv::namedWindow("Camera Frame",1);
    cv::setMouseCallback("Camera Frame",mouseHandler,0);
    std::cout<<"Annotate an object in the camera frame by left click on upper-left corner of the image and draw the mouse pointer until the lower-right corner and then release the mouse button"<<std::endl;

    // Keep processing images from camera in infinite loop until 'Esc' is pressed
    while(1){
        cap>>frame; // Get a new frame from the camera
        // Check if a box is drawn
        if ((obj_box.width>0)&&(obj_box.height>0)){
            dummy_frame = frame.clone();
            cv::rectangle(dummy_frame, cv::Point(obj_box.x,obj_box.y),cv::Point(obj_box.x+obj_box.width,obj_box.y+obj_box.height),cv::Scalar(0,255,255),4,8);
            cv::imshow("Camera Frame",dummy_frame);
            // Check if the user is satified with the box drawn
            std::cout<<"Are you satisfied with annotation? Press a to accept and anything else to reject."<<std::endl;
            std::cout<<"Press keyboard input with the image display window in focus"<<std::endl;
            // Wait for user keypress
            inp_string = cv::waitKey(0);
            if (inp_string=='a'){
                std::cout<<"Annotation accepted by user"<<std::endl;
                std::cout<<"Please enter the name of current object: ";
                std::cin>>obj_name;
                // Extact the image within drawn bounding box and save it
                frame(obj_box).copyTo(obj_im);
                cv::imwrite(obj_name+".png",obj_im);
            }
            else{
                std::cout<<"Annotation rejected by user"<<std::endl;
                std::cout<<"Please draw your bounding box again"<<std::endl;
            }
            // Make the object bounding box zeros again
            obj_box = cv::Rect(0,0,0,0);
        }
        // Display the image
        cv::imshow("Camera Frame",frame);

        // Press Esc key for exit
        if(cv::waitKey(30) >= 0) break;
    }
    return 0;
}
