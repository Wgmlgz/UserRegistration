#pragma once
#include "stdafx.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <PWNGeneral/ArgParser.h>
#include <PWNGeneral/pwnutil.h>
#include <PWNOpenCvExt/cvmathdrawingnew.h>

class IImageProvider {
 public:
  virtual cv::Mat getImage() {
    static int counter = 0;
    ++counter;
    if (counter > 3) counter = 0;
    if (counter == 0) return cv::imread("0.png");
    else if (counter == 1) return cv::imread("1.jpg");
	else return cv::imread("2.jpg");
  }
};