#pragma once
#include "stdafx.h"
#include "ui_element.h"

using namespace pawlin;

class InputField : public UIElement {
  string prompt;
  string entered;
  string window_name;

  int char_size;
  int cursor_pos;
  bool cursor_state;
  bool insert_state;

  cv::Scalar text_color;
  cv::Scalar cursor_color;
  cv::Scalar background_color;
  cv::Scalar title_color;
public:
  InputField(const string& prompt, cv::Point pos, cv::Size size);
  void drawToRect(cv::Mat& mat, cv::Rect target, int face, int thickness, cv::Scalar color, const std::string& str);
  void drawMonospaceText(cv::Mat& mat, cv::Rect target, int face, int thickness, cv::Scalar color, const std::string& str, int char_size = 15);
  void drawCursor(cv::Mat& mat, cv::Point pos, int thickness, cv::Scalar color, const int cursor_pos, int char_size);
  void drawInsertCursor(cv::Mat& mat, cv::Point pos, int thickness, cv::Scalar color, const int cursor_pos, int char_size);
  void Render();
  void Update(int key, int x, int y, int event);
  string getText() const;
};