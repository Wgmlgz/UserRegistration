#include "stdafx.h"
#include "canvas.h"
#include "video_stream.h"

Canvas::Canvas(cv::Scalar color)
    : prompt(prompt),
      CvInteractWindowBase("Enter string", cv::Size(512, 512), CV_8UC3) {
  canvas = cv::Mat::zeros(canv_size, canv_type);
  pos = cv::Point(100, 100);
  background_color = color;
}

void Canvas::Run() {
  for (;;) {
    Update();
    Render();
  }
}

void Canvas::Render() {
  canvas = background_color;

  for (auto i : ui_elements) {
    static_cast<VideoStream*>(i)->Render();
  }
}

void Canvas::Update() {
  key = pawlin::debugImg(winname, canvas, 1, 200, false);
  for (auto element : ui_elements) {
    element->Update(key, mouse_x, mouse_y, mouse_event);
  }
}

void Canvas::AddUIElement(UIElement* element) {
  element->SetParentCanvas(&canvas);
  ui_elements.push_back(element);
}
