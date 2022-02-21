#include "mainwindow.h"
#include "./ui_mainwindow.h"

using namespace std;
using namespace cv;

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
        string str = filename.toStdString();  // 将filename转变为string类型；
        image_left = imread(str);
        cvtColor(image_left, image_left, cv::COLOR_BGR2RGB);
        cv::resize(image_left, image_left, Size(300, 300));
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
        string str = filename.toStdString();  // 将filename转变为string类型；
        image_right = imread(str);
        cvtColor(image_right, image_right, cv::COLOR_BGR2RGB);
        cv::resize(image_right, image_right, Size(300, 300));
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

        QImage img = QImage((const unsigned char*)(image_compare.data), image_compare.cols, image_compare.rows, QImage::Format_RGB888);
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

