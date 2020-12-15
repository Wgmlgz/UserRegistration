#pragma once
#include "ui_element.h"
#include "button.h"
#include "rectangle.h"
#include "text.h"

class Button : public UIElement {
 public:
  std::string text;
  std::function <void()> func;
  Button(cv::Point n_pos = cv::Point(100, 100),
		 cv::Size n_size = cv::Size(100, 50),
		 std::function <void()> n_func = []() {printf("%s", "NO FUNCTION FOR BUTTON\n");},
      cv::Scalar n_color = CV_RGB(0, 0, 0),
      std::string n_text = "Sample text") {
    pos = n_pos;
    size = n_size;
    func = n_func;
    color = n_color;
    text = n_text;
  }
  void Render() {
    auto border = RectangleLine(pos, size, color);
    border.SetParentCanvas(parent_canvas);
    border.Render();

    auto txt = Text(text, pos, size, color, true);
    txt.SetParentCanvas(parent_canvas);
    txt.Render();
  }
  void Update(int key, int x, int y, int event) {
    if (event == cv::EVENT_LBUTTONDOWN) {
      if (x >= pos.x && pos.x + size.width >= x && y >= pos.y &&
          pos.y + size.height >= y) {
        func();
      }
    }
  }
};
