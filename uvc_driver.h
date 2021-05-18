#ifndef UVC_DRIVER_H
#define UVC_DRIVER_H

#include "opencv2/core.hpp"
#include "libuvc/libuvc.h"
#include <boost/thread/recursive_mutex.hpp>
//#include <mutex>
#include <string>
#include <QMainWindow>
#include <QObject>

class CameraConfig
{
public:
    CameraConfig()
    {
        video_mode = "yuyv";
        frame_rate = 30;
        width = 640;
        height = 480;
    };
    ~CameraConfig(){};

    std::string video_mode;
    int width;
    int height;
    int frame_rate;
};

class CameraDriver : public QObject
{
        Q_OBJECT
public:
    CameraDriver(QMainWindow *ptr);
    ~CameraDriver();

    bool Start();
    void Stop();
    void Reconfig(CameraConfig &config);
    void CloseCamera();
    void OpenCamera(CameraConfig &new_config);

    QImage rgb_img;
signals:
    void sendFrames(float fps);
private:
    enum State{
        k_initial = 0,
        k_stopped = 1,
        k_running = 2,
    };

    void AutoControlsCallback(enum uvc_status_class status_class,
                              int event,
                              int selector,
                              enum uvc_status_attribute status_attribute,
                              void *data, size_t data_len);
    static void AutoControlsCallbackAdapter(enum uvc_status_class status_class,
                                            int event,
                                            int selector,
                                            enum uvc_status_attribute status_attribute,
                                            void *data, size_t data_len,
                                            void *ptr);
    // Accept a new image frame from the camera
    void ImageCallback(uvc_frame_t *frame);
    static void ImageCallbackAdapter(uvc_frame_t *frame, void *ptr);

    uvc_frame_format GetVideoMode(std::string mode);
    State mstate;

    uvc_context_t *mpctx;
    uvc_device_t *mpdev;
    uvc_device_handle_t *mpdevh;
    uvc_frame_t *mprgb_frame;

    boost::recursive_mutex mmutex;

    CameraConfig mconfig;
};


#endif // UVC_DRIVER_H
