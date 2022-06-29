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

cv::Mat imcmp::compare_two_mat(const cv::Mat& image_left, const cv::Mat& image_right, int toleranceThresh, bool& is_exactly_same)
{
    if (image_left.channels() != 4 || image_right.channels() != 4)
    {
        fprintf(stderr, "only support BGRA image for comparision\n");
        return cv::Mat();
    }
    const int channels = 4;

    // TODO: support 1, 2, 4 channel image comparison
    cv::Mat diff;
    if (image_left.empty() && image_right.empty())
    {
        diff.create(256, 256, CV_8UC3);
        diff = cv::Scalar(128, 128, 128);
        is_exactly_same = true;
    }
    else if (image_right.empty())
    {
        diff = image_right.clone();
        is_exactly_same = false;
    }
    else if (image_left.empty())
    {
        diff = image_left.clone();
        is_exactly_same = false;
    }
    else
    {
        cv::Scalar pixel_diff;
        cv::Mat diff_image_left;
        cv::Mat diff_image_right;
        cv::Mat diff_image_compare;

        cv::Mat image_compare;
        if (image_left.size() != image_right.size())
        {
            cv::Size big_size;
            big_size.height = std::max(image_left.size().height, image_right.size().height);
            big_size.width = std::max(image_left.size().width, image_right.size().width);

            cv::Mat image_left_big(big_size, image_left.type(), cv::Scalar(0));
            cv::Mat image_right_big(big_size, image_right.type(), cv::Scalar(0));
            for (int i = 0; i < image_left.rows; i++)
            {
                for (int j = 0; j < image_left.cols; j++)
                {
                    for (int k = 0; k < channels; k++)
                    {
                        image_left_big.ptr(i, j)[k] = image_left.ptr(i, j)[k];
                    }
                }
            }

            for (int i = 0; i < image_right.rows; i++)
            {
                for (int j = 0; j < image_right.cols; j++)
                {
                    for (int k = 0; k < channels; k++)
                    {
                        image_right_big.ptr(i, j)[k] = image_right.ptr(i, j)[k];
                    }
                }
            }

            // TODO: fix the transparency(alpha) for diff region in diff region(the intersection) and non-diff region(the union minus the intersection)
            cv::addWeighted(image_left_big, 1.0, image_right_big, 1.0, 0.0, image_compare);

            const int roi_width = std::min(image_left.size().width, image_right.size().width);
            const int roi_height = std::min(image_left.size().height, image_right.size().height);
            cv::Rect rect(0, 0, roi_width, roi_height);

            diff_image_left = image_left(rect);
            diff_image_right = image_right(rect);
            diff_image_compare = image_compare(rect);
        }
        else // size equal
        {
            diff_image_left = image_left;
            diff_image_right = image_right;
            image_compare.create(image_left.size(), image_left.type());
            diff_image_compare = image_compare;
        }

        cv::absdiff(diff_image_left, diff_image_right, diff_image_compare);
        pixel_diff = cv::sum(diff_image_compare);
        int sum = pixel_diff.val[0] + pixel_diff.val[1] + pixel_diff.val[2] + pixel_diff.val[3];
        //cv::setNumThreads(1);

        if (sum == 0)
        {
            // if the left and right image is differnt size, but same in the overlaped region, we compute the gray image, but assign to RGB pixels
            cv::Size diff_size = diff_image_compare.size();
            for (int i = 0; i < diff_size.height; i++)
            {
                for (int j = 0; j < diff_size.width; j++)
                {
                    float R2Y = 0.299;
                    float G2Y = 0.587;
                    float B2Y = 0.114;
                    int B = diff_image_left.ptr(i, j)[0];
                    int G = diff_image_left.ptr(i, j)[1];
                    int R = diff_image_left.ptr(i, j)[2];
                    int gray = cv::saturate_cast<uchar>(R2Y * R + G2Y * G + B2Y * B);

                    diff_image_compare.ptr(i, j)[0] = gray;
                    diff_image_compare.ptr(i, j)[1] = gray;
                    diff_image_compare.ptr(i, j)[2] = gray;
                    diff_image_compare.ptr(i, j)[3] = 255;
                }
            }
            is_exactly_same = true;
        }
        else
        {
            cv::Scalar above_color(0, 0, 255 - 50);
            cv::Scalar below_color(255 - 50, 0, 0);
            imcmp::getDiffImage(diff_image_left, diff_image_right, diff_image_compare, toleranceThresh, below_color, above_color);
            is_exactly_same = false;
        }

        diff = image_compare.clone();
        printf("Compare get pixel diff: (%d, %d, %d) with thresh %d\n",
               (int)pixel_diff.val[0],
               (int)pixel_diff.val[1],
               (int)pixel_diff.val[2],
               toleranceThresh);
    }

    return diff;
}
