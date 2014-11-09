#include <stdio.h>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

bool detect_obj(cv::Mat img_object,cv::Mat img_scene,std::vector<cv::Point2f>& scene_corners);
