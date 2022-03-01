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
        autoResize(image_left, ui->scrollAreaLeft);
        QImage img = QImageFromMat(image_left);
        label_left = new QLabel();
        label_left->setPixmap(QPixmap::fromImage(img));
        label_left->resize(QSize(img.width(), img.height()));
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
        autoResize(image_right, ui->scrollAreaRight);
        QImage img = QImageFromMat(image_right);
        label_right = new QLabel();
        label_right->setPixmap(QPixmap::fromImage(img));
        label_right->resize(QSize(img.width(), img.height()));
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
        cv::Mat left_image_ref;
        cv::Mat right_image_ref;
        if (image_left.size() != image_right.size())
        {
            cv::Size big_size;
            big_size.height = std::max(image_left.size().height, image_right.size().height);
            big_size.width = std::max(image_left.size().width, image_right.size().width);

            cv::Mat image_left_big(big_size, CV_8UC3, cv::Scalar(0));
            cv::Mat image_right_big(big_size, CV_8UC3, cv::Scalar(0));
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

            left_image_ref = image_left_big;
            right_image_ref = image_right_big;
        }
        else  // size equal
        {
            left_image_ref = image_left;
            right_image_ref = image_right;
        }

        cv::absdiff(left_image_ref, right_image_ref, image_compare);
        cv::Scalar pixel_diff = cv::sum(image_compare);
        int sum = pixel_diff.val[0] + pixel_diff.val[1] + pixel_diff.val[2] + pixel_diff.val[3];
        //cv::setNumThreads(1);
        QImage::Format format = QImage::Format_RGB888;
        if (sum == 0) {
            cv::cvtColor(image_left, image_compare, cv::COLOR_BGR2GRAY);
            format = QImage::Format_Grayscale8;
        } else {
            cv::Scalar above_color(255-50, 0, 0);
            cv::Scalar below_color(0, 0, 255-50);
            cv::Mat diff;
            imk::getDiffImage(left_image_ref, right_image_ref, diff, toleranceThresh, below_color, above_color);

            cv::Mat blend;
            cv::addWeighted(left_image_ref, 0.5, right_image_ref, 0.5, 0.0, blend);

            addWeighted( diff, 0.7, blend, 0.3, 0.0, image_compare);
        }

        QImage img = QImageFromMat(image_compare);
        label_compare = new QLabel();
        label_compare->setPixmap(QPixmap::fromImage(img));
        label_compare->resize(QSize(img.width(), img.height()));
        ui->scrollAreaCompareResult->setWidget(label_compare);

        std::string v0 = std::to_string((int)(pixel_diff.val[0]));
        std::string v1 = std::to_string((int)(pixel_diff.val[1]));
        std::string v2 = std::to_string((int)(pixel_diff.val[2]));
        std::string pixel_diff_text = "pixel diff sum:<br/>[" + v0 + ", " + v1 + ", " + v2 + "]";
        ui->pixelDiff->setText(QString::fromStdString(pixel_diff_text));

        return;
    }
    if (!image_left.empty())
    {
        image_compare = image_left.clone();
        QImage img = QImageFromMat(image_compare);
        label_compare = new QLabel();
        label_compare->setPixmap(QPixmap::fromImage(img));
        label_compare->resize(QSize(img.width(), img.height()));
        ui->scrollAreaCompareResult->setWidget(label_compare);

        return;
    }

    if (!image_right.empty())
    {
        image_compare = image_right.clone();
        QImage img = QImageFromMat(image_compare);
        label_compare = new QLabel();
        label_compare->setPixmap(QPixmap::fromImage(img));
        label_compare->resize(QSize(img.width(), img.height()));
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
