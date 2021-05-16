#include "opencvthread.h"
#include <QElapsedTimer>

OpenCVThread::OpenCVThread(int whichDev)
{
    dev = whichDev;
    should_finish = false;
}

OpenCVThread::~OpenCVThread()
{

}

void OpenCVThread::run()
{
    camera.open(dev);
    Mat srcImg;
    QElapsedTimer time;
    int frames = 0;

    Mat lookUpTable(1, 256, CV_8U);
    uchar *p = lookUpTable.ptr();
    for(int i=0; i<256; i++)
        p[i] = (uchar)(20*(i/20));

    time.start();

    while(!should_finish)
    {
        camera.read(srcImg);
        Mat temp, temp1;
        cvtColor(srcImg, temp, COLOR_BGR2RGB);
        temp.copyTo(temp1);
        QImage imgSrc = QImage((const uchar*)(temp1.data), temp1.cols, temp1.rows, temp1.step, QImage::Format_RGB888);
//        LUT(temp, lookUpTable, temp);   //查表法，像素变换
        GaussianBlur(temp, temp, Size(21, 21), 0, 0);
        line(temp, Point(0, 100), Point(100,0), Scalar(255, 0, 0));
        QImage imgChanged = QImage((const uchar*)(temp.data), temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        frames ++;
        float fps = frames*1000.0/time.elapsed();
        emit sendFrames(imgSrc, imgChanged, fps);
    }
    camera.release();
}

void OpenCVThread::closeCamera()
{
    should_finish = true;
}
