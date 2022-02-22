#include "image_compare_core.hpp"

void imk::getDiffImage(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& diff, int thresh, cv::Scalar below, cv::Scalar above)
{
    CV_Assert(src1.rows == src2.rows && src1.cols == src2.cols);
    
    cv::absdiff(src1, src2, diff);
    for (int i = 0; i < diff.rows; i++)
    {
        for (int j = 0; j < diff.cols; j++)
        {
            uchar* diff_pixel = diff.ptr(i, j);
            bool bigger = false;
            for (int k = 0; k < 3; k++)
            {
                if (diff_pixel[k] > thresh)
                {
                    bigger = true;
                    break;
                }
            }
            if (bigger)
            {
                diff_pixel[0] = above.val[0];
                diff_pixel[1] = above.val[1];
                diff_pixel[2] = above.val[2];
            }
            else
            {
                diff_pixel[0] = below.val[0];
                diff_pixel[1] = below.val[1];
                diff_pixel[2] = below.val[2];
            }
        }
    }
}
