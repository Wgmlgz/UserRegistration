#pragma once
#include "stdafx.h"

#include <PWNGeneral/ArgParser.h>
#include <PWNGeneral/pwnutil.h>
#include <PWNOpenCvExt/cvmathdrawingnew.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>

using namespace pawlin;

class UIElement {
 public:
  string type;
  cv::Point pos;
  cv::Size size;
  cv::Scalar color;
  cv::Mat* parent_canvas;
  UIElement(cv::Point n_pos = cv::Point(100, 100),
            cv::Size n_size = cv::Size(100, 100),
            cv::Scalar n_color = CV_RGB(255, 255, 255));
  void SetParentCanvas(cv::Mat* n_parent_canvas);
  void Render();
  void Update(int key, int x, int y, int event);
};
