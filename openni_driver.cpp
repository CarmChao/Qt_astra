#include "openni_driver.h"
#include <QImage>
#define MAX_DEPTH_VALUE  65535
using namespace openni;

void FrameProcess::onNewFrame(openni::VideoStream &stream)
{
    stream.readFrame(&m_frame);
    if(mstream == 1)
    {
        p_ptr->ProcessDepth(m_frame);
    }
    else
    {
        p_ptr->ProcessIR(m_frame);
    }
}


void OpenNIDriver::ProcessDepth(VideoFrameRef &m_frame)
{
    if(!m_frame.isValid())
    {
        qDebug("this frame is invalid, skip it");
        return;
    }

    if (m_frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && m_frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
            {
                printf("Unexpected frame format\n");
                return;
            }

    DepthPixel* pDepth = (DepthPixel*)m_frame.getData();
    cv::Mat depthRaw = cv::Mat(m_frame.getVideoMode().getResolutionY(), m_frame.getVideoMode().getResolutionX(), CV_16UC1, (unsigned char*)pDepth);

    float* pDepthHist = NULL;
    if (pDepthHist == NULL) {
        pDepthHist = new float[MAX_DEPTH_VALUE];
    }
    memset(pDepthHist, 0, MAX_DEPTH_VALUE * sizeof(float));


    int numberOfPoints = 0;
    openni::DepthPixel nValue;

    int totalPixels = m_frame.getVideoMode().getResolutionY() * m_frame.getVideoMode().getResolutionX();

    for (int i = 0; i < totalPixels; i ++) {
        nValue = pDepth[i];
        if (nValue != 0) {
            pDepthHist[nValue] ++;
            numberOfPoints ++;
        }
    }

    for (int i = 1; i < MAX_DEPTH_VALUE; i ++) {
        pDepthHist[i] += pDepthHist[i - 1];
    }

    for (int i = 1; i < MAX_DEPTH_VALUE; i ++) {
        if (pDepthHist[i] != 0) {
            pDepthHist[i] = (numberOfPoints - pDepthHist[i]) / (float)numberOfPoints;
        }
    }

    int height = m_frame.getVideoMode().getResolutionY();
    int width = m_frame.getVideoMode().getResolutionX();
    cv::Mat depthImg(height, width, CV_8UC3);
    for (int row = 0; row < height; row++) {
        DepthPixel* depthCell = pDepth + row * width;
        uchar * showcell = (uchar *)depthImg.ptr<uchar>(row);
        for (int col = 0; col < width; col++)
        {
            char depthValue = pDepthHist[*depthCell] * 255;
            *showcell++ = depthValue;
            *showcell++ = depthValue;
            *showcell++ = 0;
            depthCell++;
        }
    }

    depth_img = QImage((const uchar*)(depthImg.data), depthImg.cols, depthImg.rows, depthImg.step, QImage::Format_RGB888);
    emit sendFrames(10);
}

void OpenNIDriver::ProcessIR(VideoFrameRef &m_frame)
{
    if(!m_frame.isValid())
    {
        qDebug("this frame is invalid, skip it");
        return;
    }

//    if (m_frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && m_frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
//            {
//                printf("Unexpected frame format\n");
//                return;
//            }

    DepthPixel* pDepth = (DepthPixel*)m_frame.getData();
    cv::Mat depthRaw = cv::Mat(m_frame.getVideoMode().getResolutionY(), m_frame.getVideoMode().getResolutionX(), CV_16UC1, (unsigned char*)pDepth);

    float* pDepthHist = NULL;
    if (pDepthHist == NULL) {
        pDepthHist = new float[MAX_DEPTH_VALUE];
    }
    memset(pDepthHist, 0, MAX_DEPTH_VALUE * sizeof(float));


    int numberOfPoints = 0;
    openni::DepthPixel nValue;

    int totalPixels = m_frame.getVideoMode().getResolutionY() * m_frame.getVideoMode().getResolutionX();

    for (int i = 0; i < totalPixels; i ++) {
        nValue = pDepth[i];
        if (nValue != 0) {
            pDepthHist[nValue] ++;
            numberOfPoints ++;
        }
    }

    for (int i = 1; i < MAX_DEPTH_VALUE; i ++) {
        pDepthHist[i] += pDepthHist[i - 1];
    }

    for (int i = 1; i < MAX_DEPTH_VALUE; i ++) {
        if (pDepthHist[i] != 0) {
            pDepthHist[i] = (numberOfPoints - pDepthHist[i]) / (float)numberOfPoints;
        }
    }

    int height = m_frame.getVideoMode().getResolutionY();
    int width = m_frame.getVideoMode().getResolutionX();
    cv::Mat depthImg(height, width, CV_8UC3);
    for (int row = 0; row < height; row++) {
        DepthPixel* depthCell = pDepth + row * width;
        uchar * showcell = (uchar *)depthImg.ptr<uchar>(row);
        for (int col = 0; col < width; col++)
        {
            char depthValue = pDepthHist[*depthCell] * 255;
            *showcell++ = depthValue;
            *showcell++ = depthValue;
            *showcell++ = 0;
            depthCell++;
        }
    }

    ir_img = QImage((const uchar*)(depthImg.data), depthImg.cols, depthImg.rows, depthImg.step, QImage::Format_RGB888);
    emit sendirFrames(10);
}

OpenNIDriver::OpenNIDriver()
{
    mdepth_processor = new FrameProcess(1, this);
    mir_processor = new FrameProcess(0, this);
    if(openniInit())
        mstate = k_running;
}

bool OpenNIDriver::openniInit()
{
    Status rc = OpenNI::initialize();

    if(rc != STATUS_OK)
    {
        qDebug("initialize failed!");
        return false;
    }

    OpenNI::addDeviceConnectedListener(&device_listener);
    OpenNI::addDeviceDisconnectedListener(&device_listener);
    OpenNI::addDeviceStateChangedListener(&device_listener);

    OpenNI::enumerateDevices(&device_list);
    for(int i=0; i<device_list.getSize(); i++)
    {
        qDebug("Device \"%s\" already connected", device_list[i].getUri());
    }

    rc = device.open(ANY_DEVICE);
    if(rc != STATUS_OK)
    {
        qDebug("Couldn't open device\n%s\n", OpenNI::getExtendedError());
        return false;
    }
//    mdepth_stream.addNewFrameListener(mdepth_processor);
//    mir_stream.addNewFrameListener(mir_processor);
    return true;
}

void OpenNIDriver::StartOpenNI(int i)
{
    Status rc;
    if(i == 1)
    {
        rc = mdepth_stream.create(device, SENSOR_DEPTH);
        if (rc != STATUS_OK)
        {
            qDebug("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
            return;
        }

        rc = mdepth_stream.start();
        if (rc != STATUS_OK)
        {
            qDebug("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
            return;
        }
        mdepth_stream.addNewFrameListener(mdepth_processor);

    }
    else
    {
        rc = mir_stream.create(device, SENSOR_IR);
        if (rc != STATUS_OK)
        {
            qDebug("Couldn't create ir stream\n%s\n", OpenNI::getExtendedError());
            return;
        }

        rc = mir_stream.start();
        if (rc != STATUS_OK)
        {
            qDebug("Couldn't start the ir stream\n%s\n", OpenNI::getExtendedError());
            return;
        }
        mir_stream.addNewFrameListener(mir_processor);
    }
}

void OpenNIDriver::Stop()
{
    mdepth_stream.removeNewFrameListener(mdepth_processor);
    mir_stream.removeNewFrameListener(mir_processor);
    mdepth_stream.stop();
    mir_stream.destroy();
    mir_stream.destroy();
    device.close();
    OpenNI::shutdown();
}
