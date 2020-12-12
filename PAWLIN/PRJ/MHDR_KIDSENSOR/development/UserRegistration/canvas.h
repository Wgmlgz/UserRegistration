#pragma once
#include "stdafx.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <PWNGeneral/ArgParser.h>
#include <PWNGeneral/pwnutil.h>
#include <PWNOpenCvExt/cvmathdrawingnew.h>

#include "ui_element.h"

using namespace pawlin;

class Canvas : public CvInteractWindowBase {
  string prompt;
  string window_name;
  cv::Size window_size;
  cv::Point pos;
  cv::Scalar background_color;

  cv::Mat canvas;

  vector<UIElement*> ui_elements;

  // input
  int key, mouse_x, mouse_y, mouse_event;

 public:
  Canvas(cv::Scalar color = CV_RGB(255, 255, 255));

  void Run(int wait = 1);
  void Render();
  void Update(int wait = 1);

  void AddUIElement(UIElement* element);
  void ClearCanvas();

  virtual void updateClick(int x, int y, int event, int flags) override {
    mouse_x = x;
    mouse_y = y;
    mouse_event = event;
    Update();
  }
};
