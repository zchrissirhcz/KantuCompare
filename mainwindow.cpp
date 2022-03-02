#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "image_compare_core.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("ImageCompare");

    // 容差 滑块
    ui->toleranceSlider->setRange(0, 255);//设置范围
    ui->toleranceSlider->setValue(0); //设置当前值
    int val = ui->toleranceSlider->value(); //获取值
    ui->toleranceLabel->setText(QString::number(val));//把获取到的值显示在文本框

    // 路径 可以让鼠标选择复制
    ui->pathLeft->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->pathRight->setTextInteractionFlags(Qt::TextSelectableByMouse);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage QImageFromMat(const cv::Mat& mat)
{
    // https://blog.51cto.com/u_15064638/4107835  这 step 略坑， 不填step会导致可能歪斜。。
    return QImage(mat.data, mat.cols, mat.rows, mat.step1(), QImage::Format_RGB888);
}

void autoSize(QLabel* imgLabel, QScrollArea* scrollArea) // 自适应窗口
{
    QImage Img;
    double ImgRatio = 1.0 * imgLabel->pixmap()->toImage().width() / imgLabel->pixmap()->toImage().height();     // 图像宽高比
    double WinRatio = 1.0 * (scrollArea->width() - 2) / (scrollArea->height() - 2); // 窗口宽高比
    if (ImgRatio > WinRatio)        // 图像宽高比大于图像宽高比
    {
        Img = imgLabel->pixmap()->toImage().scaled((scrollArea->width() - 2), (scrollArea->width() - 2) / ImgRatio, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    else                            // 图像宽高比小于等于图像宽高比
    {
        Img = imgLabel->pixmap()->toImage().scaled((scrollArea->height() - 2) * ImgRatio, (scrollArea->height() - 2), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    imgLabel->setPixmap(QPixmap::fromImage(Img));   // 显示图像
    imgLabel->resize(Img.width(), Img.height());
}

void autoResize(cv::Mat& image, QScrollArea* scrollArea)
{
    // TODO: 不要对原图做resize。这样会导致 compare 结果很怪异。
    // 应当保持原图， 但是对 QImage 做 resize。
    // 参考：https://blog.csdn.net/u013165921/article/details/79380638

    //cv::resize(image, image, cv::Size(300, 300));
    double width_scale = image.cols * 1.0 / (scrollArea->width() * 0.9);
    double height_scale = image.rows * 1.0 / (scrollArea->height() * 0.9);
    double scale = std::max(width_scale, height_scale);
    //cv::Size dsize = image.size() / scale;
    cv::resize(image, image, cv::Size(0, 0), 1.0/scale, 1.0/scale, cv::INTER_LINEAR);
}

void MainWindow::on_OpenImageLeft_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr(supported_image_exts.c_str()),
        "",
        tr(supported_image_exts.c_str()));
    if (filename.isEmpty())
    {
        return;
    }
    else
    {
        std::string str = filename.toStdString();
        image_left = imk::loadImage(str);
        cv::cvtColor(image_left, image_left, cv::COLOR_BGR2RGB);
        //autoResize(image_left, ui->scrollAreaLeft);
        QImage img = QImageFromMat(image_left);
        label_left = new QLabel();
        label_left->setPixmap(QPixmap::fromImage(img));
        label_left->resize(QSize(img.width(), img.height()));
        autoSize(label_left, ui->scrollAreaLeft);
        ui->scrollAreaLeft->setWidget(label_left);

        ui->pathLeft->setText(filename);

        compare_and_show_image();
    }
}

void MainWindow::on_OpenImageRight_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr(supported_image_exts.c_str()),
        "",
        tr(supported_image_exts.c_str()));
    if (filename.isEmpty())
    {
        return;
    }
    else
    {
        std::string str = filename.toStdString();
        image_right = imk::loadImage(str);
        cv::cvtColor(image_right, image_right, cv::COLOR_BGR2RGB);
        //autoResize(image_right, ui->scrollAreaRight);
        QImage img = QImageFromMat(image_right);
        label_right = new QLabel();
        label_right->setPixmap(QPixmap::fromImage(img));
        label_right->resize(QSize(img.width(), img.height()));
        autoSize(label_right, ui->scrollAreaRight);
        ui->scrollAreaRight->setWidget(label_right);

        ui->pathRight->setText(filename);

        compare_and_show_image();
    }
}

void MainWindow::compare_and_show_image()
{
    if (image_left.empty() && image_right.empty())
    {
        return;
    }

    if (!image_left.empty() && !image_right.empty())
    {
        cv::Scalar pixel_diff;
        cv::Mat diff_image_left;
        cv::Mat diff_image_right;
        cv::Mat diff_image_compare;
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
                    for (int k = 0; k < 3; k++)
                    {
                        image_left_big.ptr(i, j)[k] = image_left.ptr(i, j)[k];
                    }
                }
            }

            for (int i = 0; i < image_right.rows; i++)
            {
                for (int j = 0; j < image_right.cols; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        image_right_big.ptr(i, j)[k] = image_right.ptr(i, j)[k];
                    }
                }
            }

            cv::addWeighted(image_left_big, 1.0, image_right_big, 1.0, 0.0, image_compare);

            const int roi_width = std::min(image_left.size().width, image_right.size().width);
            const int roi_height = std::min(image_left.size().height, image_right.size().height);
            cv::Rect rect(0, 0, roi_width, roi_height);

            diff_image_left = image_left(rect);
            diff_image_right = image_right(rect);
            diff_image_compare = image_compare(rect);
        }
        else  // size equal
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
        QImage::Format format = QImage::Format_RGB888;
        if (sum == 0) {
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
                }
            }
        }
        else
        {
            cv::Scalar above_color(255-50, 0, 0);
            cv::Scalar below_color(0, 0, 255-50);
            cv::Mat diff;
            imk::getDiffImage(diff_image_left, diff_image_right, diff, toleranceThresh, below_color, above_color);

            cv::Mat blend;
            cv::addWeighted(diff_image_left, 0.5, diff_image_right, 0.5, 0.0, blend);

            addWeighted( diff, 0.7, blend, 0.3, 0.0, diff_image_compare);
        }

        QImage img = QImageFromMat(image_compare);
        label_compare = new QLabel();
        label_compare->setPixmap(QPixmap::fromImage(img));
        label_compare->resize(QSize(img.width(), img.height()));
        autoSize(label_compare, ui->scrollAreaCompareResult);
        ui->scrollAreaCompareResult->setWidget(label_compare);

        std::string v0 = std::to_string((int)(pixel_diff.val[0]));
        std::string v1 = std::to_string((int)(pixel_diff.val[1]));
        std::string v2 = std::to_string((int)(pixel_diff.val[2]));
        std::string pixel_diff_text = "pixel diff sum:<br/>[" + v0 + ", " + v1 + ", " + v2 + "]";
        ui->pixelDiff->setText(QString::fromStdString(pixel_diff_text));

        return;
    }
    if (!image_left.empty() || !image_right.empty())
    {
        if (!image_left.empty())
        {
            image_compare = image_left.clone();
        }
        else // !image_right.empty()
        {
            image_compare = image_right.clone();
        }

        QImage img = QImageFromMat(image_compare);
        label_compare = new QLabel();
        label_compare->setPixmap(QPixmap::fromImage(img));
        label_compare->resize(QSize(img.width(), img.height()));
        autoSize(label_compare, ui->scrollAreaCompareResult);
        ui->scrollAreaCompareResult->setWidget(label_compare);

        return;
    }
}



void MainWindow::on_toleranceSlider_valueChanged(int value)
{
    ui->toleranceLabel->setText(QString::number(value));
    toleranceThresh = value;
    compare_and_show_image();
}
