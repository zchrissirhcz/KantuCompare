#include "image_compare.hpp"

void imcmp::getDiffImage(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& diff, int thresh, cv::Scalar below, cv::Scalar above)
{
    CV_Assert(src1.rows == src2.rows && src1.cols == src2.cols);
    CV_Assert(src1.channels() == src2.channels());
    CV_Assert(thresh >= 0);

    const int channels = src1.channels();
    
    cv::absdiff(src1, src2, diff);
    for (int i = 0; i < diff.rows; i++)
    {
        for (int j = 0; j < diff.cols; j++)
        {
            uchar* diff_pixel = diff.ptr(i, j);
            const uchar* src_pixel = src1.ptr(i, j);
            bool bigger = false;
            int diff_sum = 0;
            for (int k = 0; k < channels; k++)
            {
                diff_sum += diff_pixel[k];
                if (diff_pixel[k] > thresh)
                {
                    bigger = true;
                    break;
                }
            }
            if (diff_sum == 0)
            {
                float R2Y = 0.299;
                float G2Y = 0.587;
                float B2Y = 0.114;
                
                int B = src_pixel[0];
                int G = src_pixel[1];
                int R = src_pixel[2];

                int Gray = R2Y * R + G2Y * G + B2Y * B;

                diff_pixel[0] = Gray;
                diff_pixel[1] = Gray;
                diff_pixel[2] = Gray;
                diff_pixel[3] = 255;
            }
            else
            {
                if (bigger)
                {
                    diff_pixel[0] = above.val[0];
                    diff_pixel[1] = above.val[1];
                    diff_pixel[2] = above.val[2];
                    diff_pixel[3] = 255;
                }
                else
                {
                    diff_pixel[0] = below.val[0];
                    diff_pixel[1] = below.val[1];
                    diff_pixel[2] = below.val[2];
                    diff_pixel[3] = 255;
                }
            }
        }
    }
}
