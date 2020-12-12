#include "stdafx.h"
#include "text.h"
#include "ui_element.h"

Text::Text(string txt, int n_face, double n_scale, 
           cv::Point n_pos,
           cv::Size n_size,
           cv::Scalar n_color) {
  type = "Text";
  text = txt;
  face = n_face;
  scale = n_scale;
  pos = n_pos;
  size = n_size;
  color = n_color;
}

void Text::Render() {
  cv::putText(*parent_canvas, text, pos, 1, 2, color);
}