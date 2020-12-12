#include "stdafx.h"
#include "text.h"
#include "ui_element.h"

Text::Text(string txt, int face, double scale, cv::Point pos, cv::Size size,
  cv::Scalar color) {
  type = "Text";
  text = txt;
  face = face;
  scale = scale;
  pos = pos;
  size = size;
  color = color;
}

void Text::Render() {
  cv::putText(*parent_canvas, text, pos, face, scale, color);
}