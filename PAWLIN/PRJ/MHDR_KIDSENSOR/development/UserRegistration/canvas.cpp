#include "stdafx.h"
#include "canvas.h"
#include "video_stream.h"
#include "image.h"
#include "rectangle.h"
#include "text.h"
#include "button.h"
#include "input_field.h"


void Canvas::CallBackFunc(int event, int x, int y, int flags) {
  mouse_x = x;
  mouse_y = y;
  mouse_event = event;
  Update();
}
Canvas::Canvas(cv::Scalar color) {
  window_name = "User registration";
  cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
  canvas = cv::Mat::zeros(cv::Size(1280, 720), 16);
  pos = cv::Point(200, 300);
  background_color = color;
}

void Canvas::Run(int wait) {
  for (;;) {
    bool do_render = Update(wait);
    if (do_render) {
      Render();
    }
    cv::imshow(window_name, canvas);
  }
}

void Canvas::Render() {
  canvas = background_color;

  for (auto i : ui_elements) {
    i->Render();
  }
}

bool Canvas::Update(int wait) {
  key = cv::waitKeyEx(wait);
  bool flag = false;
  for (auto i : ui_elements) {
    if (was_cleared) {
      was_cleared = false;
      return true;
    }
    i->Update(
        key, 
        mouse_x, 
        mouse_y, 
        mouse_event);
    if (static_cast<InputField*>(i) != nullptr) {
      flag = true;
    }
  }
}

void Canvas::ClearCanvas() {
  ui_elements.clear();
  was_cleared = true;
}

void Canvas::AddUIElement(UIElement* element) {
  element->SetParentCanvas(&canvas);
  ui_elements.push_back(element);
}
