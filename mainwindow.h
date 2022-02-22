#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include <QGraphicsScene>
#include <QGraphicsView>                   //graphicsview类
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>  //opencv申明
#include <qfiledialog.h>                //getopenfilename 类申明
#include <qlabel.h>                     //label类
using namespace cv;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_OpenImageLeft_clicked();
    void on_OpenImageRight_clicked();
    void on_Compare_clicked();


private:
    Ui::MainWindow* ui;
    cv::Mat image_left, image_right, image_compare;
    QLabel* label_left;
    QLabel* label_right;
    QLabel* label_compare;
    std::string supported_image_exts = "Images(*.png *.bmp *.jpg *.tif *.GIF *.NV21 *.NV12 *.RGB24 *.GRAY)";

};
#endif // MAINWINDOW_H
