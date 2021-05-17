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
//        connect(uvc_driver, &CameraDriver::sendFrames, this, &MainWindow::setImage);
        connect(openni_driver, &OpenNIDriver::sendFrames, this, &MainWindow::setImage);
//        connect(this, SIGNAL(over()), thred_showImg, SLOT(closeCamera()));
//        connect(thred_showImg, &OpenCVThread::finished, thred_showImg, &OpenCVThread::deleteLater);
//        thred_showImg->start();
        openni_driver->StartOpenNI(1);
    }
    else
    {
        opened = false;
        ui->pushButton_enter->setText("打开");
//        disconnect(thred_showImg, &OpenCVThread::sendFrames, this, &MainWindow::setImage);
        uvc_driver->Stop();
        ui->label_show->clear();
        ui->label_showChanged->clear();
        ui->label_show->setText("请打开摄像头");
        ui->label_showChanged->setText("请打开摄像头");
        emit over();
    }
}

void MainWindow::setImage(QImage imgSrc, float fps)
{
    ui->label_show->setPixmap(QPixmap::fromImage(imgSrc.mirrored(true, false)));
//    ui->label_showChanged->setPixmap(QPixmap::fromImage(imgChanged.mirrored(true, false)));
    ui->label_title->setText(QString("fps: %1").arg(fps));
}
