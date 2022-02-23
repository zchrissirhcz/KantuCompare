#include <iostream>
#include <opencv2/opencv.hpp>

#include "image_compare_core.hpp"

using namespace std;
using namespace cv;


int main( int argc, char** argv ){
    double alpha = 0.5; double beta; double input;
    
    Mat src1, src2, dst;
    
    /// Ask the user enter alpha
    // cout << " Simple Linear Blender " << std::endl;
    // cout << "-----------------------" << std::endl;
    // cout << "* Enter alpha [0-1]: ";
    // cin >> input;
    input = 0.5;
    
    // We use the alpha provided by the user iff it is between 0 and 1
    if( alpha >= 0 && alpha <= 1 )
        { alpha = input; }
    
    /// Read image ( same size, same type )
    std::string load_prefix = "/home/zz/data";
    std::string path1 = load_prefix + "/background.png";
    std::string path2 = load_prefix + "/frontground.png";
    src1 = imread(path1);
    src2 = imread(path2);
    
    if( !src1.data ) { printf("Error loading src1 \n"); return -1; }
    if( !src2.data ) { printf("Error loading src2 \n"); return -1; }
    
    /// Create Windows
    namedWindow("Linear Blend", 1);
    
    beta = ( 1.0 - alpha );
    addWeighted( src1, alpha, src2, beta, 0.0, dst);
    
    imshow( "Linear Blend", dst );
    imwrite( "girl_blending.jpg", dst );

    int thresh = 22;

    cv::Mat shadow = dst.clone();
    cv::Mat under = cv::Mat(src1.size(), src1.type(), cv::Scalar(255, 0, 0));
    cv::Mat above = cv::Mat(src1.size(), src1.type(), cv::Scalar(0, 0, 255));

    // cv::Mat diff;
    // cv::absdiff(src1, src2, diff);
    // for (int i = 0; i < dst.rows; i++)
    // {
    //     for (int j = 0; j < dst.cols; j++)
    //     {
    //         uchar* diff_pixel = diff.ptr(i, j);
    //         bool bigger = false;
    //         for (int k = 0; k < 3; k++)
    //         {
    //             if (diff_pixel[k] > threshold)
    //             {
    //                 bigger = true;
    //                 break;
    //             }
    //         }
    //         if (bigger)
    //         {
    //             diff_pixel[0] = 0;
    //             diff_pixel[1] = 0;
    //             diff_pixel[2] = 255;
    //         }
    //         else
    //         {
    //             diff_pixel[0] = 255;
    //             diff_pixel[1] = 0;
    //             diff_pixel[2] = 0;
    //         }
    //     }
    // }
    cv::Mat diff;
    cv::Scalar below_color(220, 0, 0);
    cv::Scalar above_color(0, 0, 220);
    imk::getDiffImage(src1, src2, diff, thresh, below_color, above_color);
    imshow( "diff", diff );

    cv::Mat final;
    alpha = 0.7;
    beta = 0.3;
    addWeighted( diff, alpha, dst, beta, 0.0, final);
    imshow( "final", final );
    
    waitKey(0);
    return 0;
}
