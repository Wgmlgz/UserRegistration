#include "stdafx.h"
#include "rectangle.h"

RectangleLine::RectangleLine(cv::Point n_pos, cv::Size n_size,
                             cv::Scalar n_color) {
  pos = n_pos;
  size = n_size;
  color = n_color;
}
void RectangleLine::Render() {
  cv::rectangle(
      *parent_canvas,
      cv::Rect(cv::Point(pos.x, pos.y),
               cv::Point(pos.x + size.width, pos.y + size.height)),
      color, 2);
}