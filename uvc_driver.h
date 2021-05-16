#ifndef UVC_DRIVER_H
#define UVC_DRIVER_H

#include "libuvc/libuvc.h"

#include <string>

class CameraDriver
{
public:
    CameraDriver();
    ~CameraDriver();

    bool Start();
    void Stop();
    void Reconfig();
    void CloseCamera();
    void OpenCamera();

private:
    enum State{
        k_initial = 0,
        k_stopped = 1,
        k_running = 2,
    };

    State mstate;

    uvc_context_t *mpctx;
    uvc_device_t *mpdev;
    uvc_device_handle_t *mpdevh;
    uvc_frame_t *mprgb_frame;
};


#endif // UVC_DRIVER_H
