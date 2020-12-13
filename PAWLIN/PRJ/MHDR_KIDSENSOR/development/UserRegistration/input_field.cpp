#include "stdafx.h"
#include "input_field.h"


InputField::InputField(const string n_prompt, const string n_default, cv::Point n_pos, cv::Size n_size) {
  prompt = n_prompt;
  pos = n_pos;
  size = n_size;
  char_size = 15;
  cursor_state = false;
  cursor_pos = 0;
  insert_state = false;
  active = false;
  entered = n_default;
  
  text_color = CV_RGB(120, 120, 120);
  cursor_color = CV_RGB(0, 255, 0);
  background_color = CV_RGB(255, 255, 255);
  title_color = CV_RGB(33, 33, 33);
}
void InputField::drawToRect(cv::Mat& mat, cv::Rect target, int face, int thickness, cv::Scalar color, const std::string& str)
{
 cv::Size rect = cv::getTextSize(str, face, 1.0, thickness, 0);
  double scalex = (double)target.width / (double)rect.width;
  double scaley = (double)target.height / (double)rect.height;
  double scale = (std::min)(scalex, scaley);
  int marginx = scale == scalex ? 0 : (int)((double)target.width * (scalex - scale) / scalex * 0.5);
  int marginy = scale == scaley ? 0 : (int)((double)target.height * (scaley - scale) / scaley * 0.5);
  cv::putText(mat, str, cv::Point(target.x + marginx, target.y + target.height - marginy), face, scale, color, thickness, 8, false);
}
void InputField::drawMonospaceText(cv::Mat& mat, cv::Rect target, int face, int thickness, cv::Scalar color, const std::string& str, int char_size)
{
  cv::Size rect = cv::getTextSize(str, face, 1.0, thickness, 0);
  rect.width = static_cast<int>(str.size() * char_size);
  for (int i = 0; i < str.size(); ++i)
    cv::putText(mat, (str[i] + string("")), cv::Point(target.x + char_size * i, target.y + 20), face, 1.5, color, thickness, 8, false);
}
void InputField::drawCursor(cv::Mat& mat, cv::Point pos, int thickness, cv::Scalar color, const int cursor_pos, int char_size)
{
  cv::line(mat, cv::Point(pos.x + char_size * cursor_pos, pos.y + 2),
    cv::Point(pos.x + char_size * cursor_pos, static_cast<int>(pos.y + char_size * 1.5 - 2)),
    color, thickness);
}
void InputField::drawInsertCursor(cv::Mat& mat, cv::Point pos, int thickness, cv::Scalar color, const int cursor_pos, int char_size) {
  cv::rectangle(mat, cv::Rect(cv::Point(pos.x + char_size * cursor_pos, pos.y + 2),
    cv::Point(pos.x + char_size * (cursor_pos + 1), pos.y + 25)),
    color, thickness);
}
void InputField::Update(int key, int x, int y, int event) {
  if (event == cv::EVENT_LBUTTONDOWN) {
    if (y < pos.y + 20 && y > pos.y - 5 && x > pos.x &&
        x < pos.x + entered.size() * char_size + char_size) {
      cursor_pos = (x - pos.x + char_size / 2) / char_size;
      if (cursor_pos > static_cast<int>(entered.size())) cursor_pos = static_cast<int>(entered.size());
      cursor_state = false;
      active = true;
    } else {
      active = false;
    }
  }
  if (active) {
    if (key == 27) {
      entered = "";
      return;
    }
    // ->
    if (key == OPENCV_RIGHT_ARROW_CODE) {
      cursor_state = false;
      if (cursor_pos < entered.size()) cursor_pos++;
    }
    // <-
    if (key == OPENCV_LEFT_ARROW_CODE) {
      cursor_state = false;
      if (cursor_pos > 0) cursor_pos--;
    }
    // backspace
    if (key == 8) {
      cursor_state = false;
      if (cursor_pos > 0) entered.erase(entered.begin() + --cursor_pos);
    }
    // delite
    if (key == 127) {
      cursor_state = false;
      if (cursor_pos < entered.size())
        entered.erase(entered.begin() + cursor_pos);
    }
    // enter
    if (key == 13) {
      return;
    }
    // insert
    if (key == 2949120) {
      cursor_state = false;
      insert_state = !insert_state;
    }
    // ASCII symbol entered
    if (key >= 32 && key < 128) {
      cursor_state = false;
      if (insert_state) {
        if (cursor_pos == entered.size())
          entered.push_back((char)key);
        else
          entered[cursor_pos] = (char)key;
      } else {
        entered.insert(cursor_pos, 1, (char)key);
      }
      cursor_pos++;
    }
  }
}
void InputField::Render() {
  cursor_state = !cursor_state;  // cursor blink effect
  // draw what user entered
  cv::Rect rect(pos, cv::Size(((int)entered.size() + 1) * char_size + 1, static_cast<int>(char_size * 1.5)));
  drawMonospaceText(*parent_canvas, rect, cv::FONT_HERSHEY_PLAIN, 1, text_color, entered);
   // draw cursor
  if (active) {
    if (insert_state)
      drawInsertCursor(*parent_canvas, pos, 2, cursor_color, cursor_pos, char_size);
    else
      drawCursor(*parent_canvas, pos, 2, cursor_color, cursor_pos, char_size);
  }
  // draw prompt
  cv::Rect rect2(pos - cv::Point(0, 32), cv::Size((int)prompt.size() * 16, 16));
  drawToRect(*parent_canvas, rect2, cv::FONT_HERSHEY_PLAIN, 1, title_color, prompt);
}
string InputField::getText() const { return entered; }