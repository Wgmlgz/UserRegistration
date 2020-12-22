#pragma once

#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

#include <memory>

#include "stdafx.h"
#include "ui_element.h"

using namespace pawlin;

class Canvas {
 public:
  std::vector<std::function<void()>> on_update;
  string prompt;
  string window_name;
  cv::Size window_size;
  cv::Point pos;
  cv::Scalar background_color;
  cv::Mat canvas;
  std::vector<std::shared_ptr<UIElement>> ui_elements;

  int key, mouse_x, mouse_y, mouse_event;
  bool was_cleared;

  bool run;

  Canvas(cv::Scalar color = CV_RGB(255, 255, 255)) {
    window_name = "User registration";
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    canvas = cv::Mat::zeros(cv::Size(1280, 720), 16);
    pos = cv::Point(200, 300);
    background_color = color;

    run = true;
  }
  void Run(int wait = 1) {
    for (;run;) {
      bool do_render = Update(wait);
      if (true) Render();
      cv::imshow(window_name, canvas);
    }
  }
  void Render() {
    canvas = background_color;
    for (int j = 0; j < ui_elements.size(); ++j) {
      ui_elements[j]->Render();
    }
  }
  bool Update(int wait = 1) {
    key = cv::waitKeyEx(1);
    bool flag = false;
    bool flag2 = true;
    for (int j = 0; j < ui_elements.size(); ++j) {
      if (was_cleared) {
        was_cleared = false;
        return true;
      } else {
        ui_elements[j]->Update(key, mouse_x, mouse_y, mouse_event);
      }
    }
    for (auto i : on_update) i();
  }
  void AddUIElement(UIElement* element) {
    element->SetParentCanvas(&canvas);
    ui_elements.push_back(std::shared_ptr<UIElement>(element));
  }
  void ClearCanvas() {
    ui_elements.clear();
    was_cleared = true;
  }
  void Exit() {
    run = false;
  }
  void CallBackFunc(int event, int x, int y, int flags) {
    mouse_x = x;
    mouse_y = y;
    mouse_event = event;
    if (event == cv::EVENT_LBUTTONDOWN) Update();
  }
};
