This code connects our original objdet code with ROS.
Follow the direction to install tum_ardrone
https://github.com/tum-vision/tum_ardrone

Make a folder named ~/catkin_ws/src/tum_ardrone/src/ub_hack
and place the files ros2opencv.cpp, surf_detect.cpp, surf_detect.hpp
in that folder

Copy the CMakeLists.txt and replace the tum_ardrone package CMakeLists.txt located at 
~/catkin_ws/src/tum_ardrone/CMakeLists.txt
