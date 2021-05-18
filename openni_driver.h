#ifndef OPENNI_DRIVER_H
#define OPENNI_DRIVER_H

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "OpenNI.h"

#include <QObject>
#include <QDebug>
#include <QImage>

class OpenNIDriver;

class OpenNiDeviceListener : public openni::OpenNI::DeviceConnectedListener,
                             public openni::OpenNI::DeviceDisconnectedListener,
                             public openni::OpenNI::DeviceStateChangedListener
{
public:
    virtual void onDeviceStateChanged(const openni::DeviceInfo* pInfo, openni::DeviceState state)
    {
        qDebug("Device \"%s\" error state changed to %d\n", pInfo->getUri(), state);
    }

    virtual void onDeviceConnected(const openni::DeviceInfo* pInfo)
    {
        qDebug("Device \"%s\" connected\n", pInfo->getUri());
    }

    virtual void onDeviceDisconnected(const openni::DeviceInfo* pInfo)
    {
        qDebug("Device \"%s\" disconnected\n", pInfo->getUri());
    }
};

class FrameProcess : public openni::VideoStream::NewFrameListener
//                     public QObject
{
//            Q_OBJECT
public:
    FrameProcess(int w_stream, OpenNIDriver *ptr):mstream(w_stream), p_ptr(ptr){}

    void onNewFrame(openni::VideoStream & stream) override;

//signals:
//    void sendFrames(QImage imgSrc,  float fps);

private:

    openni::VideoFrameRef m_frame;
    int mstream;

    OpenNIDriver *p_ptr;
};


class OpenNIDriver : public QObject
{
    Q_OBJECT
public:
    OpenNIDriver();
    ~OpenNIDriver(){};

    bool openniInit();

    void StartOpenNI(int i);

    void Stop();

    void ProcessDepth(openni::VideoFrameRef &m_frame);

    void ProcessIR(openni::VideoFrameRef &m_frame);



    FrameProcess *mdepth_processor;
    FrameProcess *mir_processor;
    QImage depth_img;
    QImage ir_img;

signals:
    void sendFrames(float fps);
    void sendirFrames(float fps);

private:

    enum NIState
    {
        k_initial = 0,
        k_running = 1,
        k_stopped = 2
    };

    NIState mstate;

    OpenNiDeviceListener device_listener;
    openni::Array<openni::DeviceInfo> device_list;

    openni::Device device;

    openni::VideoStream mdepth_stream;
    openni::VideoStream mir_stream;

};

#endif // OPENNI_DRIVER_H
