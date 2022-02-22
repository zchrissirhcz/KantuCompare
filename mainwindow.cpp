#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "image_compare_core.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_OpenImageLeft_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("选择图片"),
        "",
        tr("Images(*.png *.bmp *.jpg *.tif *.GIF)"));
    if (filename.isEmpty())
    {
        return;
    }
    else
    {
        std::string str = filename.toStdString();  // 将filename转变为string类型；
        image_left = cv::imread(str);
        cv::cvtColor(image_left, image_left, cv::COLOR_BGR2RGB);
        cv::resize(image_left, image_left, cv::Size(300, 300));
        QImage img = QImage((const unsigned char*)(image_left.data), image_left.cols, image_left.rows, QImage::Format_RGB888);
        label_left = new QLabel();
        label_left->setPixmap(QPixmap::fromImage(img));
        label_left->resize(QSize(img.width(), img.height()));
        ui->scrollAreaLeft->setWidget(label_left);

        ui->pathLeft->setText(filename);
    }
}

void MainWindow::on_OpenImageRight_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("选择图片"),
        "",
        tr("Images(*.png *.bmp *.jpg *.tif *.GIF)"));
    if (filename.isEmpty())
    {
        return;
    }
    else
    {
        std::string str = filename.toStdString();  // 将filename转变为string类型；
        image_right = cv::imread(str);
        cv::cvtColor(image_right, image_right, cv::COLOR_BGR2RGB);
        cv::resize(image_right, image_right, cv::Size(300, 300));
        QImage img = QImage((const unsigned char*)(image_right.data), image_right.cols, image_right.rows, QImage::Format_RGB888);
        label_right = new QLabel();
        label_right->setPixmap(QPixmap::fromImage(img));
        label_right->resize(QSize(img.width(), img.height()));
        ui->scrollAreaRight->setWidget(label_right);

        ui->pathRight->setText(filename);
    }
}

void MainWindow::on_Compare_clicked()
{

    if (image_left.empty() && image_right.empty())
    {
        return;
    }

    if (!image_left.empty() && !image_right.empty())
    {
        cv::absdiff(image_left, image_right, image_compare);
        cv::Scalar sum_pixel = cv::sum(image_compare);
        int sum = sum_pixel.val[0] + sum_pixel.val[1] + sum_pixel.val[2] + sum_pixel.val[3];
        //cv::setNumThreads(1);
        QImage::Format format = QImage::Format_RGB888;
        if (sum == 0) {
            cv:cvtColor(image_left, image_compare, cv::COLOR_BGR2GRAY);
            format = QImage::Format_Grayscale8;
        } else {
            int thresh = 100;
            cv::Scalar above_color(255-50, 0, 0);
            cv::Scalar below_color(0, 0, 255-50);
            imk::getDiffImage(image_left, image_right, image_compare, thresh, below_color, above_color);
        }

        QImage img = QImage((const unsigned char*)(image_compare.data), image_compare.cols, image_compare.rows, format);
        label_compare = new QLabel();
        label_compare->setPixmap(QPixmap::fromImage(img));
        label_compare->resize(QSize(img.width(), img.height()));
        ui->scrollAreaCompareResult->setWidget(label_compare);

        return;
    }
    if (!image_left.empty())
    {
        image_compare = image_left.clone();
        QImage img = QImage((const unsigned char*)(image_compare.data), image_compare.cols, image_compare.rows, QImage::Format_RGB888);
        label_compare = new QLabel();
        label_compare->setPixmap(QPixmap::fromImage(img));
        label_compare->resize(QSize(img.width(), img.height()));
        ui->scrollAreaCompareResult->setWidget(label_compare);

        return;
    }

    if (!image_right.empty())
    {
        image_compare = image_right.clone();
        QImage img = QImage((const unsigned char*)(image_compare.data), image_compare.cols, image_compare.rows, QImage::Format_RGB888);
        label_compare = new QLabel();
        label_compare->setPixmap(QPixmap::fromImage(img));
        label_compare->resize(QSize(img.width(), img.height()));
        ui->scrollAreaCompareResult->setWidget(label_compare);

        return;
    }

}

