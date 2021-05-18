#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    opened = false;
    dev = 0;
    ui->pushButton_enter->setText("打开");
    uvc_driver = new CameraDriver(this);
    openni_driver = new OpenNIDriver();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_enter_clicked()
{
    if(!opened)
    {
        opened = true;
        ui->pushButton_enter->setText("关闭");
//        thred_showImg = new OpenCVThread(dev);
//        connect(thred_showImg, &OpenCVThread::sendFrames, this, &MainWindow::setImage);
        connect(uvc_driver, &CameraDriver::sendFrames, this, &MainWindow::setImage);
//        connect(openni_driver, &OpenNIDriver::sendFrames, this, &MainWindow::setImage);
//        connect(openni_driver, &OpenNIDriver::sendirFrames, this, &MainWindow::setImage1);
//
        connect(this, &MainWindow::over, uvc_driver, &CameraDriver::Stop);
//        connect(, &OpenCVThread::finished, thred_showImg, &OpenCVThread::deleteLater);
//        thred_showImg->start();
//        openni_driver->StartOpenNI(1);
//        openni_driver->StartOpenNI(0);
        uvc_driver->Start();
    }
    else
    {
        opened = false;
        ui->pushButton_enter->setText("打开");
//        disconnect(thred_showImg, &OpenCVThread::sendFrames, this, &MainWindow::setImage);
//        uvc_driver->Stop();
//        ui->
//        ui->label_showChanged->clear();
//        ui->label_show->setText("请打开摄像头");
//        ui->label_showChanged->setText("请打开摄像头");
        emit over();
    }
}

void MainWindow::setImage(float fps)
{
    QImage tmp_img = openni_driver->depth_img.copy();
    ui->label_rgb_show->setPixmap(QPixmap::fromImage(tmp_img));
//    ui->label_showChanged->setPixmap(QPixmap::fromImage(imgChanged.mirrored(true, false)));
//    ui->label_title->setText(QString("fps: %1").arg(fps));
}

void MainWindow::setImage1(float fps)
{
    QImage tmp_img = openni_driver->ir_img.copy();
    ui->label_ir_show->setPixmap(QPixmap::fromImage(tmp_img));
//    ui->label_showChanged->setPixmap(QPixmap::fromImage(tmp_img.mirrored(true, false)));
//    ui->label_title->setText(QString("fps: %1").arg(fps));
}
