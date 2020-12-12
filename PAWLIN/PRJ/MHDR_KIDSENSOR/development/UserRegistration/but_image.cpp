#include "stdafx.h"
#include "but_image.h"
#include "button.h"
#include "text.h"

ButImage::ButImage(cv::Point n_pos, cv::Size n_size,
                   std::function<void()> n_func1, std::function<void()> n_func2,
                   cv::Scalar n_color, std::string n_text1, std::string n_text2,
                   int n_buttonHeight, cv::Mat n_image) {
  pos = n_pos;
  size = n_size;
  func1 = n_func1;
  func2 = n_func2;
  color = n_color;
  text1 = n_text1;
  text2 = n_text2;
  buttonHeight = n_buttonHeight;
  image = n_image;
}

void ButImage::Render() {
  auto button1 =
      Button(pos, cv::Size(size.width, buttonHeight), func1, color, text1);
  button1.SetParentCanvas(parent_canvas);
  button1.Render();

  auto img = Image(image, cv::Point(pos.x, pos.y + buttonHeight),
                   cv::Size(size.width, size.height - buttonHeight * 2));
  img.SetParentCanvas(parent_canvas);
  img.Render();

  auto button2 =
      Button(cv::Point(pos.x, pos.y + size.height - buttonHeight),
             cv::Size(size.width, buttonHeight), func2, color, text2);
  button2.SetParentCanvas(parent_canvas);
  button2.Render();
}

void ButImage::Update(int key, int x, int y, int event) {
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

void ButImage::ResetPhoto(cv::Mat n_image) { image = n_image; }