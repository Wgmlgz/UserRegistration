#include "stdafx.h"
#include "canvas.h"
#include "video_stream.h"
#include "image.h"
#include "rectangle.h"

Canvas::Canvas(cv::Scalar color)
    : prompt(prompt),
      CvInteractWindowBase("Enter string", cv::Size(1200, 720), CV_8UC3) {
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
    if (i->type == "VideoStream") static_cast<VideoStream*>(i)->Render();
    else if (i->type == "Image") static_cast<Image*>(i)->Render();
    else if (i->type == "RectangleLine") static_cast<RectangleLine*>(i)->Render();
    else i->Render();
  }
}

void Canvas::Update() {
  key = pawlin::debugImg(winname, canvas, 1, 1, false);
  for (auto element : ui_elements) {
    element->Update(key, mouse_x, mouse_y, mouse_event);
  }
}

void Canvas::AddUIElement(UIElement* element) {
  element->SetParentCanvas(&canvas);
  ui_elements.push_back(element);
}
