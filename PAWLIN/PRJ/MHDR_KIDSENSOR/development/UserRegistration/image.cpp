#include "stdafx.h"
#include "image.h"

Image::Image(cv::Mat n_image, cv::Point n_pos, cv::Size n_size,
             cv::Scalar n_color) {
  image = n_image;
  pos = n_pos;
  size = n_size;
  color = n_color;
}

void Image::Render() {
  cv::Mat tmp = image;
  cv::resize(tmp, tmp, size);

  cv::Rect roi(pos, size);
  cv::Mat destinationROI = (*parent_canvas)(roi);
  tmp.copyTo(destinationROI);
}