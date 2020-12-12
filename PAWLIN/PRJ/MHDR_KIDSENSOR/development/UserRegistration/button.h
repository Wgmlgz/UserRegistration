#pragma once
#include "ui_element.h"
#include "button.h"
#include "rectangle.h"


class Button : public UIElement {
 public:
  std::string text;
  std::function <void()> func;
  Button(cv::Point n_pos = cv::Point(100, 100),
		 cv::Size n_size = cv::Size(100, 50),
		 std::function <void()> n_func = []() {printf("%s", "NO FUNCTION FOR BUTTON\n");},
		 cv::Scalar n_color = CV_RGB(0, 0, 0),
		 std::string line = "Sample text");
  void Render();
  void Update(int key, int x, int y, int event);
};
