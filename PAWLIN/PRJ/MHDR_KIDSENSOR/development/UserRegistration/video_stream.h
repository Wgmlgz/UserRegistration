#pragma once
#include "stdafx.h"
#include "ui_element.h"
#include "image.h"

class VideoStream : public UIElement {
  cv::VideoCapture cap;
 public:
  VideoStream(std::string file = "", cv::Point n_pos = cv::Point(100, 100),
              cv::Size n_size = cv::Size(100, 100));
  void Render();
};