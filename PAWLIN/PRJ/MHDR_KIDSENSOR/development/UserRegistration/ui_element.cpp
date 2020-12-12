#include "stdafx.h"
#include "ui_element.h"

UIElement::UIElement(cv::Point n_pos, cv::Size n_size, cv::Scalar n_color) {
  pos = n_pos;
  size = n_size;
  color = n_color;
}
void UIElement::Render() {
  cv::circle(*parent_canvas, pos, 20, color);
}
void UIElement::SetParentCanvas(cv::Mat* n_parent_canvas) {
  parent_canvas = n_parent_canvas;
}
void UIElement::Update(int key, int x, int y, int event) {
  printf("%i\n", x);
}