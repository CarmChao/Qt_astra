#include "uvc_driver.h"
#include <iostream>
#include <string>
#include <cmath>

CameraDriver::CameraDriver():
    mstate(k_initial)
{
    ;
}

bool CameraDriver::Start()
{
    if(mstate != k_initial)
    {
        std::cout<<"not right state!"<<std::endl;
        return false;
    }

    uvc_error_t err;
    err = uvc_init(&mpctx, NULL);

    if(err != UVC_SUCCESS)
    {
        uvc_perror(err, "ERROR: uvc_init");
        return false;
    }

    mstate = k_stopped;
    Reconfig();

    return mstate == k_running;
}

void CameraDriver::Reconfig()
{
    if(mstate == k_running)
        CloseCamera();
    if(mstate == k_stopped)
        OpenCamera();
}

void CameraDriver::OpenCamera()
{
    int vendor_id = std::stoi("0x2bc5", 0, 16);
    int product_id = std::stoi("0x0505", 0, 16);

    uvc_error_t find_err = uvc_find_device(mpctx, &mpdev, vendor_id, product_id, NULL);

    if(find_err != UVC_SUCCESS)
    {
        uvc_perror(find_err, "uvc_find_device");
        return;
    }

    uvc_error_t open_err = uvc_open(mpdev, &mpdevh);

    if (open_err != UVC_SUCCESS) {
        std::cout<<"uvc open error!"<<std::endl;

        uvc_unref_device(mpdev);
        return;
    }

}

void CameraDriver::CloseCamera()
{
    if(mstate == k_running)
    {
        uvc_close(mpdevh);
        mpdevh = NULL;

        uvc_unref_device(mpdev);
        mpdev = NULL;

        mstate = k_stopped;
    }
}
