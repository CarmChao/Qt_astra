#include "uvc_driver.h"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <QtDebug>
#include <QImage>


CameraDriver::CameraDriver(QMainWindow* ptr):
    mstate(k_initial),
    mprgb_frame(NULL)
{
    ;
}

enum uvc_frame_format CameraDriver::GetVideoMode(std::string vmode){
  if(vmode == "uncompressed") {
    return UVC_COLOR_FORMAT_UNCOMPRESSED;
  } else if (vmode == "compressed") {
    return UVC_COLOR_FORMAT_COMPRESSED;
  } else if (vmode == "yuyv") {
    return UVC_COLOR_FORMAT_YUYV;
  } else if (vmode == "uyvy") {
    return UVC_COLOR_FORMAT_UYVY;
  } else if (vmode == "rgb") {
    return UVC_COLOR_FORMAT_RGB;
  } else if (vmode == "bgr") {
    return UVC_COLOR_FORMAT_BGR;
  } else if (vmode == "mjpeg") {
    return UVC_COLOR_FORMAT_MJPEG;
  } else if (vmode == "gray8") {
    return UVC_COLOR_FORMAT_GRAY8;
  } else {
    std::cout<<"Invalid Video Mode: "<<vmode<<std::endl;
    std::cout<<"Warnning: Continue using video mode: uncompressed"<<std::endl;
    return UVC_COLOR_FORMAT_UNCOMPRESSED;
  }
};

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
    Reconfig(mconfig);

    return mstate == k_running;
}

void CameraDriver::Reconfig(CameraConfig &config)
{
    if(mstate == k_running)
        CloseCamera();
    if(mstate == k_stopped)
        OpenCamera(config);
}

void CameraDriver::OpenCamera(CameraConfig &new_config)
{
    qDebug("open camera");
    int vendor_id = std::stoi("0x2bc5", 0, 16);
    int product_id = std::stoi("0x0502", 0, 16);

    uvc_error_t find_err = uvc_find_device(mpctx, &mpdev, vendor_id, product_id, NULL);
    qDebug("after find");
    if(find_err != UVC_SUCCESS)
    {
        uvc_perror(find_err, "uvc_find_device");
        return;
    }

//    uvc_device_t **devs;
//    uvc_error_t find_err = uvc_find_devices(
//      mpctx, &devs,
//      vendor_id,
//      product_id,
//      NULL);

//    if (find_err != UVC_SUCCESS) {
//      uvc_perror(find_err, "uvc_find_device");
//      return;
//    }

//    // select device by index
//    mpdev = NULL;
//    int dev_idx = 0;
//    while (devs[dev_idx] != NULL) {
//      if(dev_idx == 0) {
//        mpdev = devs[dev_idx];
//      }
//      else {
//        uvc_unref_device(devs[dev_idx]);
//      }

//      dev_idx++;
//    }

//    if(mpdev == NULL) {
//      qDebug("Unable to find device at index %d", 0);
//      return;
//    }

    uvc_error_t open_err = uvc_open(mpdev, &mpdevh);
    qDebug("after open");
    if (open_err != UVC_SUCCESS) {
        std::cout<<"uvc open error!"<<std::endl;

        uvc_unref_device(mpdev);
        return;
    }
    qDebug("before status callback\n");
    uvc_set_status_callback(mpdevh, &CameraDriver::AutoControlsCallbackAdapter, this);
    qDebug("after callback\n");
    uvc_stream_ctrl_t ctrl;
    uvc_error_t mode_err = uvc_get_stream_ctrl_format_size(
       mpdevh, &ctrl,
       GetVideoMode(new_config.video_mode),
       new_config.width, new_config.height,
       new_config.frame_rate);

    if(mode_err != UVC_SUCCESS)
    {
        uvc_perror(mode_err, "uvc_get_stream_ctrl_format_size");
        uvc_close(mpdevh);
        uvc_unref_device(mpdev);
        std::cout<<"check video_mode/width/height/frame_rate are available\n";
        uvc_print_diag(mpdevh, NULL);
        return;
    }
    qDebug("start streaming");
    uvc_error_t stream_err = uvc_start_streaming(mpdevh, &ctrl, &CameraDriver::ImageCallbackAdapter, this, 0);

    if (stream_err != UVC_SUCCESS) {
      uvc_perror(stream_err, "uvc_start_streaming");
      uvc_close(mpdevh);
      uvc_unref_device(mpdev);
      return;
    }
    qDebug("stop streaming");

    if (mprgb_frame)
      uvc_free_frame(mprgb_frame);

    mprgb_frame = uvc_allocate_frame(new_config.width * new_config.height * 3);
    if(!mprgb_frame)
    {
        qDebug("rgb_frame allocate failed!\n");
        return;
    }

    mstate = k_running;
}

void CameraDriver::Stop() {
  boost::recursive_mutex::scoped_lock(mmutex);

  assert(mstate != k_initial);

  if (mstate == k_running)
    CloseCamera();

  if(mstate != k_stopped)
  {
      std::cout<<"stop failed!\n";
      return;
  }

  uvc_exit(mpctx);
  mpctx = NULL;

  mstate = k_initial;
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

void CameraDriver::AutoControlsCallbackAdapter(enum uvc_status_class status_class, int event, int selector, enum uvc_status_attribute status_attribute, void *data, size_t data_len, void *ptr)
{
    CameraDriver *driver = static_cast<CameraDriver*>(ptr);

    driver->AutoControlsCallback(status_class, event, selector,
                                 status_attribute, data, data_len);
}

void CameraDriver::AutoControlsCallback(enum uvc_status_class status_class, int event, int selector, enum uvc_status_attribute status_attribute, void *data, size_t data_len)
{
    boost::recursive_mutex::scoped_lock lock(mmutex);
    qDebug("Controls callback. class: %d, event: %d, selector: %d, attr: %d, data_len: %zu\n",
            status_class, event, selector, status_attribute, data_len);
    qDebug("camera config change callback, change the saved parameter");
}

void CameraDriver::ImageCallbackAdapter(uvc_frame_t *frame, void *ptr) {
  CameraDriver *driver = static_cast<CameraDriver*>(ptr);

  driver->ImageCallback(frame);
}

void CameraDriver::ImageCallback(uvc_frame_t *frame) {
//  ros::Time timestamp = ros::Time(frame->capture_time.tv_sec, frame->capture_time.tv_usec);
//  if ( timestamp == ros::Time(0) ) {
//    timestamp = ros::Time::now();
//  }

  boost::recursive_mutex::scoped_lock lock(mmutex);

  if(mstate != k_running || (!mprgb_frame))
  {
      std::cout<<"camera not streaming now!\n";
      return;
  }

  cv::Mat image(mconfig.height, mconfig.width, CV_8UC3);


  if (frame->frame_format == UVC_FRAME_FORMAT_BGR){
    memcpy(image.ptr(), frame->data, frame->data_bytes);
    cv::cvtColor(image, image, CV_BGR2RGB);
  }
  else if (frame->frame_format == UVC_FRAME_FORMAT_RGB)
  {
    memcpy(image.ptr(), frame->data, frame->data_bytes);
  }
  else if (frame->frame_format == UVC_FRAME_FORMAT_UYVY)
  {    //one storage way for YUV422 sample
      uvc_error_t conv_ret = uvc_uyvy2rgb(frame, mprgb_frame);
      if (conv_ret != UVC_SUCCESS) {
        uvc_perror(conv_ret, "Couldn't convert frame to RGB");
        return;
      }
    memcpy(image.ptr(), mprgb_frame->data, mprgb_frame->data_bytes);
  }
  else if (frame->frame_format == UVC_FRAME_FORMAT_YUYV) {    //another storage way for YUV422 sample
    // FIXME: uvc_any2bgr does not work on "yuyv" format, so use uvc_yuyv2bgr directly.
    uvc_error_t conv_ret = uvc_yuyv2rgb(frame, mprgb_frame);
    if (conv_ret != UVC_SUCCESS) {
      uvc_perror(conv_ret, "Couldn't convert frame to RGB");
      return;
    }
    memcpy(image.ptr(), mprgb_frame->data, mprgb_frame->data_bytes);
  }
  else
  {
    uvc_error_t conv_ret = uvc_any2rgb(frame, mprgb_frame);
    if (conv_ret != UVC_SUCCESS) {
      uvc_perror(conv_ret, "Couldn't convert frame to RGB");
      return;
    }
    memcpy(image.ptr(), mprgb_frame->data, mprgb_frame->data_bytes);
  }

  qDebug("get img");
  QImage imgSrc = QImage((const uchar*)(image.data), image.cols, image.rows, image.step, QImage::Format_RGB888);
  emit sendFrames(imgSrc, 10);
}

CameraDriver::~CameraDriver()
{
    if (mprgb_frame)
      uvc_free_frame(mprgb_frame);

    if (mpctx)
      uvc_exit(mpctx);  // Destroys dev_, devh_, etc.
}

