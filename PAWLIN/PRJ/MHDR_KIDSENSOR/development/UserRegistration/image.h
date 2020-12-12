#pragma once
#include "stdafx.h"
#include "ui_element.h"

class Image : public UIElement {
  cv::Mat image;
 public:
  Image(cv::Mat n_image, cv::Point n_pos = cv::Point(100, 100),
        cv::Size n_size = cv::Size(100, 100),
        cv::Scalar n_color = CV_RGB(255, 255, 255));
  void Render();
};