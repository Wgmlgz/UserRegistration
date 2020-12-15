#pragma once
#include "stdafx.h"
#include "ui_element.h"
#include "image.h"

class VideoStream : public UIElement {
  cv::VideoCapture cap;
 public:
  VideoStream(std::string file = "", cv::Point n_pos = cv::Point(100, 100),
              cv::Size n_size = cv::Size(100, 100)) {
    pos = n_pos;
    size = n_size;
    cap.open(0);
  }

  void Render() {
    cv::Mat frame;
    cap >> frame;

    if (frame.empty()) return;

    Image i(frame, pos, size);
    i.SetParentCanvas(parent_canvas);
    i.Render();
  }
  void Update(int key, int x, int y, int event) { Render(); }
};