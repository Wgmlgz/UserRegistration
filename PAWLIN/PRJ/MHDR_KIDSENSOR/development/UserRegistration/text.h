#pragma once
#include "stdafx.h"
#include "ui_element.h"

class Text : public UIElement {
  string text;
  int face;
  double scale;
public:
  Text(string txt, int face, double scale, cv::Point pos = cv::Point(100, 100),
    cv::Size size = cv::Size(100, 100),
    cv::Scalar color = CV_RGB(255, 255, 255));
  void Render();
};