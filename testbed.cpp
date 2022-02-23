#include <iostream>
#include <opencv2/opencv.hpp>

#include "image_compare_core.hpp"

using namespace std;
using namespace cv;


int test_tolerance( int argc, char** argv ){
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

int test_diff_hist()
{
    std::string load_prefix = "/home/zz/data";
    std::string path1 = load_prefix + "/background.png";
    std::string path2 = load_prefix + "/frontground.png";

    Mat srcImage = imread(path1);
    imshow( "素材图", srcImage );

    int bins = 256;
    int hist_size[] = {bins};
    float range[] = { 0, 256 };
    const float* ranges[] = { range};
    MatND redHist,grayHist,blueHist;

    int channels_r[] = {0};
    //进行直方图的计算（红色分量部分）
    calcHist( &srcImage, 1, channels_r, Mat(), //不使用掩膜
        redHist, 1, hist_size, ranges,
        true, false );

    //进行直方图的计算（绿色分量部分）
    int channels_g[] = {1};
    calcHist( &srcImage, 1, channels_g, Mat(), // do not use mask
        grayHist, 1, hist_size, ranges,
        true, // the histogram is uniform
        false );

    //进行直方图的计算（蓝色分量部分）
    int channels_b[] = {2};
    calcHist( &srcImage, 1, channels_b, Mat(), // do not use mask
        blueHist, 1, hist_size, ranges,
        true, // the histogram is uniform
        false );

    //-----------------------绘制出三色直方图------------------------
    //参数准备
    double maxValue_red,maxValue_green,maxValue_blue;
    minMaxLoc(redHist, 0, &maxValue_red, 0, 0);
    minMaxLoc(grayHist, 0, &maxValue_green, 0, 0);
    minMaxLoc(blueHist, 0, &maxValue_blue, 0, 0);
    int scale = 1;
    int histHeight=256;
    Mat histImage = Mat::zeros(histHeight,bins*3, CV_8UC3);

    //正式开始绘制
    for(int i=0;i<bins;i++)
    {
        //参数准备
        float binValue_red = redHist.at<float>(i);
        float binValue_green = grayHist.at<float>(i);
        float binValue_blue = blueHist.at<float>(i);
        int intensity_red = cvRound(binValue_red*histHeight/maxValue_red);  //要绘制的高度
        int intensity_green = cvRound(binValue_green*histHeight/maxValue_green);  //要绘制的高度
        int intensity_blue = cvRound(binValue_blue*histHeight/maxValue_blue);  //要绘制的高度

        //绘制红色分量的直方图
        line(histImage,Point(i,histHeight-1),Point(i, histHeight - intensity_red),CV_RGB(255,0,0));
        //绘制绿色分量的直方图
        line(histImage,Point(i+bins,histHeight-1),Point(i+bins, histHeight - intensity_green),CV_RGB(0,255,0));
        //绘制蓝色分量的直方图
        line(histImage,Point(i+bins*2,histHeight-1),Point(i+bins*2, histHeight - intensity_blue),CV_RGB(0,0,255));
    }
    imshow( "图像的RGB直方图", histImage );



    cv::Mat dstImage = histImage;
    //画   坐  标  值//
    double bin_w = (double)dstImage.cols / 256;  // hdims: 条的个数，则 bin_w 为条的宽度
    double bin_u = (double)dstImage.rows / maxValue_red;   // max: 最高条的像素个数，则 bin_u 为单个像素的高度
     //画纵坐标刻度（像素个数）
    int kedu = 0;
    for (int i = 1; kedu<maxValue_red; i++)
    {
        kedu = i * maxValue_red / 10;
        //itoa(kedu, string, 10);//把一个整数转换为字符串
        std::string kedu_str = std::to_string(kedu);
        //在图像中显示文本字符串
        cv::putText(dstImage, kedu_str, cv::Point(0, dstImage.rows - kedu * bin_u), cv::FONT_HERSHEY_PLAIN, 0.6, cv::Scalar(255, 255, 255));
    }
    //画横坐标刻度（像素灰度值）
    kedu = 0;
    for (int i = 1; kedu<256; i++)
    {
        kedu = i * 20;
        //itoa(kedu, string, 10);//把一个整数转换为字符串
                                //在图像中显示文本字符串
        std::string kedu_str = std::to_string(kedu);
        cv::putText(dstImage, kedu_str, cv::Point(kedu*(dstImage.cols / 256), dstImage.rows), cv::FONT_HERSHEY_PLAIN, 0.6, cv::Scalar(0, 0, 0));
    }

    imshow( "dstImage", dstImage );


    waitKey(0);
    return 0;
}

int main()
{

    //test_tolerance();
    test_diff_hist();

    return 0;
}