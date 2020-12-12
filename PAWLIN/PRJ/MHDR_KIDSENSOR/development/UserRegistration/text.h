#pragma once
#include "stdafx.h"
#include "ui_element.h"

class Text : public UIElement {
  string text;
  int face;
  double scale;
public:
  Text(string txt, int n_face = cv::FONT_HERSHEY_PLAIN, double n_scale = 1, cv::Point n_pos = cv::Point(400, 400),
    cv::Size n_size = cv::Size(100, 100),
    cv::Scalar n_color = CV_RGB(0, 0, 0));
  void Render();
};