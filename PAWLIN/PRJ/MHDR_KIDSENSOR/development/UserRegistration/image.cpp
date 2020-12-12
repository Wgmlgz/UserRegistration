#include "stdafx.h"
#include "image.h"

Image::Image(cv::Mat n_image, cv::Point n_pos, cv::Size n_size,
             cv::Scalar n_color) {
  type = "Image";
  image = n_image;
  pos = n_pos;
  size = n_size;
  color = n_color;
}

void Image::Render() {
  cv::Mat tmp = image;
  cv::resize(tmp, tmp, size);

  for (int i = pos.x; i < size.width + pos.x; ++i) {
    for (int j = pos.y; j < size.height + pos.y; ++j) {
      parent_canvas->at<cv::Vec3b>(cv::Point(i, j)) =
          tmp.at<cv::Vec3b>(cv::Point(i - pos.x, j - pos.y));
    }
  }
}