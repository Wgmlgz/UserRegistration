#include "stdafx.h"
#include "canvas.h"
#include "video_stream.h"
#include "image.h"
#include "rectangle.h"
#include "text.h"
#include "button.h"

Canvas::Canvas(cv::Scalar color)
    : prompt(prompt),
      CvInteractWindowBase("User registration", cv::Size(1280, 720), CV_8UC3) {
  canvas = cv::Mat::zeros(canv_size, canv_type);
  pos = cv::Point(200, 300);
  background_color = color;
}

void Canvas::Run(int wait) {
  for (;;) {
    Update(wait);
    Render();
  }
}

void Canvas::Render() {
  canvas = background_color;

  for (auto i : ui_elements) {
    i->Render();
  }
}

void Canvas::Update(int wait) {
  key = pawlin::debugImg(winname, canvas, 1, wait, false);
  for (auto i : ui_elements) {
    i->Update(key, mouse_x, mouse_y, mouse_event);
  }
}

void Canvas::ClearCanvas() {
  ui_elements.clear();
}

void Canvas::AddUIElement(UIElement* element) {
  element->SetParentCanvas(&canvas);
  ui_elements.push_back(element);
}
