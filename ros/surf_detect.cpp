#include "surf_detect.hpp"
using namespace cv;

// Function that will actually find the object
bool detect_obj(cv::Mat img_object,cv::Mat img_scene,std::vector<cv::Point2f>& scene_corners)
{

    if( !img_object.data || !img_scene.data )
    { std::cout<< " --(!) Error reading images " << std::endl; return false; }

    // Minimum number of good matches by which we declare if we found an object
    unsigned int min_matches = 20; 
    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;

    SurfFeatureDetector detector( minHessian );

    std::vector<KeyPoint> keypoints_object, keypoints_scene;

    detector.detect( img_object, keypoints_object );
    detector.detect( img_scene, keypoints_scene );

    // Making sure that there are atleast more keypoints that min_matches
    if ((keypoints_object.size()>min_matches) && (keypoints_scene.size()>min_matches)){
            //-- Step 2: Calculate descriptors (feature vectors)
            SurfDescriptorExtractor extractor;

            Mat descriptors_object, descriptors_scene;

            extractor.compute( img_object, keypoints_object, descriptors_object );
            extractor.compute( img_scene, keypoints_scene, descriptors_scene );

            //-- Step 3: Matching descriptor vectors using FLANN matcher
            FlannBasedMatcher matcher;
            std::vector< DMatch > matches;
            matcher.match( descriptors_object, descriptors_scene, matches );

            double max_dist = 0; double min_dist = 100;

            //-- Quick calculation of max and min distances between keypoints
            for( int i = 0; i < descriptors_object.rows; i++ )
            { double dist = matches[i].distance;
            if( dist < min_dist ) min_dist = dist;
            if( dist > max_dist ) max_dist = dist;
            }

            //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
            std::vector< DMatch > good_matches;

            for(int i = 0; i < descriptors_object.rows; i++ )
            { if( matches[i].distance < 3*min_dist )
                { good_matches.push_back( matches[i]); }
            }

            // Check if there are sufficient number of inliers
            if (good_matches.size()>min_matches){ 
                //-- Localize the object
                //std::cout<<"Number of feature matches"<<good_matches.size()<<std::endl;
                std::vector<Point2f> obj;
                std::vector<Point2f> scene;

                for( unsigned int i = 0; i < good_matches.size(); i++ )
                {
                    //-- Get the keypoints from the good matches
                    obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
                    scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
                }

                Mat H = findHomography( obj, scene, CV_RANSAC );
                // To compute SVD of returned homography matrix
                Mat w,u,vt; 
                cv::SVD::compute(H,w,u,vt,cv::SVD::NO_UV);
                // Sanity check on obtained homogeneous transform
                if (w.at<double>(2,0)>2*1e-3){
                    //std::cout<<w.at<double>(0,0)<<","<<w.at<double>(2,0)<<std::endl;

                    //-- Get the corners from the image_1 ( the object to be "detected" )
                    std::vector<Point2f> obj_corners(4);
                    obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
                    obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );

                    perspectiveTransform( obj_corners, scene_corners, H);

                    /*
                    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
                    line( img_scene, scene_corners[0] , scene_corners[1] , Scalar(255, 255, 0), 4 );
                    line( img_scene, scene_corners[1] , scene_corners[2] , Scalar( 255, 255, 0), 4 );
                    line( img_scene, scene_corners[2] , scene_corners[3] , Scalar( 255, 255, 0), 4 );
                    line( img_scene, scene_corners[3] , scene_corners[0] , Scalar( 255, 255, 0), 4 );

                    //-- Show detected matches
                    imshow( "Detected Object", img_scene );

                    waitKey(0);*/
                    return true;
                }
                else{
                    // False as no valid homogeneous transform could be found
                    return false;
                }
            }
            else{
                // The object was not found
                // False as sufficient number of features could not be matched
                return false;
            }
    }
    else{
        // Not enought keypoints either in the scene or on the object
        return false;
    }
}
