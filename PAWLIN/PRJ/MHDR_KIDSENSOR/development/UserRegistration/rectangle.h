#pragma once
#include <PWNGeneral/ArgParser.h>
#include <PWNGeneral/pwnutil.h>
#include <PWNOpenCvExt/cvmathdrawingnew.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

#include "stdafx.h"
#include "ui_element.h"

class RectangleLine : public UIElement {
 public:
  RectangleLine(cv::Point n_pos = cv::Point(100, 100),
            cv::Size n_size = cv::Size(100, 100),
                cv::Scalar n_color = CV_RGB(255, 255, 255)) {
    pos = n_pos;
    size = n_size;
    color = n_color;
  }
  void Render() {
    cv::rectangle(*parent_canvas,
                  cv::Rect(cv::Point(pos.x, pos.y),
                           cv::Point(pos.x + size.width, pos.y + size.height)),
                  color, 2);
  }
};