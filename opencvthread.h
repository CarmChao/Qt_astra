#ifndef OPENCVTHREAD_H
#define OPENCVTHREAD_H
#include <QThread>
#include <QImage>

#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
class OpenCVThread : public QThread
{
    Q_OBJECT

public:
    OpenCVThread(int whichDev=0);
    ~OpenCVThread();

    void run();

signals:
    void sendFrames(QImage imgSrc, QImage imgChanged, float fps);

public slots:
    void closeCamera();

private:
    int dev;
    bool should_finish;
    VideoCapture camera;
};

#endif // OPENCVTHREAD_H
