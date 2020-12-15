#pragma once
#include <PWNGeneral/ArgParser.h>
#include <PWNGeneral/pwnutil.h>
#include <PWNOpenCvExt/cvmathdrawingnew.h>

#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

#include "stdafx.h"
#include "ui_element.h"
#include "video_stream.h"
#include "image.h"
#include "rectangle.h"
#include "text.h"
#include "button.h"
#include "input_field.h"
using namespace pawlin;

class Canvas {
 public:
  string prompt;
  string window_name;
  cv::Size window_size;
  cv::Point pos;
  cv::Scalar background_color;

  cv::Mat canvas;

  vector<UIElement*> ui_elements;

  // input
  int key, mouse_x, mouse_y, mouse_event;

  bool was_cleared;

  Canvas(cv::Scalar color = CV_RGB(255, 255, 255)) {
    window_name = "User registration";
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    canvas = cv::Mat::zeros(cv::Size(1280, 720), 16);
    pos = cv::Point(200, 300);
    background_color = color;
  }

  void Run(int wait = 1) {
    for (;;) {
      bool do_render = Update(wait);
      if (do_render) {
        Render();
      }
      cv::imshow(window_name, canvas);
    }
  }
  void Render() {
    canvas = background_color;

    for (auto i : ui_elements) {
      i->Render();
    }
  }
  bool Update(int wait = 1) {
    key = cv::waitKeyEx(wait);
    bool flag = false;
    for (auto i : ui_elements) {
      if (was_cleared) {
        was_cleared = false;
        return true;
      }
      i->Update(key, mouse_x, mouse_y, mouse_event);
      if (static_cast<InputField*>(i) != nullptr) {
        flag = true;
      }
    }
  }
  void AddUIElement(UIElement* element) {
    element->SetParentCanvas(&canvas);
    ui_elements.push_back(element);
  }

  void ClearCanvas() {
    ui_elements.clear();
    was_cleared = true;
  }

  void Canvas::CallBackFunc(int event, int x, int y, int flags) {
    mouse_x = x;
    mouse_y = y;
    mouse_event = event;
    Update();
  }
};
