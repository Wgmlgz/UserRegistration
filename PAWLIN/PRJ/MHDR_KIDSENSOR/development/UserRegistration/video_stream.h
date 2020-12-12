#pragma once
#include "stdafx.h"
#include "ui_element.h"

class VideoStream : public UIElement {
  cv::VideoCapture cap;
 public:
  VideoStream(std::string file = "");
  void Render();
};