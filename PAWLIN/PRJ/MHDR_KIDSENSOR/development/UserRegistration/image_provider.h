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
 cv::Mat mat0 = cv::imread("0.png");
 cv::Mat mat1 = cv::imread("1.png");
 cv::Mat mat2 = cv::imread("2.png");
 public:
  virtual cv::Mat getImage() {
    static int counter = 0;
    ++counter;
    if (counter > 200) counter = 0;
    if (counter > 132) return mat0;
    else if (counter > 66) return mat1;
	else return mat2;
  }
};