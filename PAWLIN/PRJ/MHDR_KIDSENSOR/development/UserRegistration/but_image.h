#pragma once
#include "button.h"
#include "image.h"
#include "rectangle.h"
#include "stdafx.h"
#include "ui_element.h"

class ButImage : public UIElement {
 public:
  std::function<void()> func1;
  std::function<void()> func2;
  std::string text1;
  std::string text2;
  int buttonHeight;
  cv::Mat image;
  bool fin = false;
  ButImage(
      cv::Point n_pos = cv::Point(100, 100),
      cv::Size n_size = cv::Size(200, 200),
      std::function<void()> n_func1 =
          []() { printf("%s", "NO FUNCTION FOR BUTTON\n"); },
      std::function<void()> n_func2 =
          []() { printf("%s", "NO FUNCTION FOR BUTTON\n"); },
      cv::Scalar n_color = CV_RGB(0, 0, 0), std::string n_text1 = "Sample text",
      std::string n_text2 = "Sample text", int n_buttonHeight = 20,
      cv::Mat n_image = cv::imread("empty.png"), bool n_fin = false) {
    pos = n_pos;
    size = n_size;
    func1 = n_func1;
    func2 = n_func2;
    color = n_color;
    text1 = n_text1;
    text2 = n_text2;
    buttonHeight = n_buttonHeight;
    image = n_image;
    fin = n_fin;
  }
  void Render() {
    auto button1 =
        Button(pos, cv::Size(size.width, buttonHeight), func1, color, text1);
    button1.SetParentCanvas(parent_canvas);
    button1.Render();

    auto img = Image(image, cv::Point(pos.x, pos.y + buttonHeight),
                     cv::Size(size.width, size.height - buttonHeight * 2));
    img.SetParentCanvas(parent_canvas);
    img.Render();
    if (fin) return;
    auto button2 =
        Button(cv::Point(pos.x, pos.y + size.height - buttonHeight),
               cv::Size(size.width, buttonHeight), func2, color, text2);
    button2.SetParentCanvas(parent_canvas);
    button2.Render();
  }
  void Update(int key, int x, int y, int event) {
    if (event == cv::EVENT_LBUTTONDOWN) {
      if (x >= pos.x && pos.x + size.width >= x && y >= pos.y &&
          pos.y + buttonHeight >= y) {
        func1();
      }
      if (x >= pos.x && pos.x + size.width >= x &&
          y >= pos.y + size.height - buttonHeight && pos.y + size.height >= y) {
        func2();
      }
    }
  }
  void ResetPhoto(cv::Mat n_image) { image = n_image; }
};