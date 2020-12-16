#pragma once
#include "stdafx.h"

#include <PWNGeneral/ArgParser.h>
#include <PWNGeneral/pwnutil.h>
#include <PWNOpenCvExt/cvmathdrawingnew.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>
#include <chrono>
#include <ctime>

using namespace pawlin;

class UIElement {
 public:
  cv::Point pos;
  cv::Size size;
  cv::Scalar color;
  cv::Mat* parent_canvas;
  UIElement(cv::Point n_pos = cv::Point(100, 100),
            cv::Size n_size = cv::Size(100, 100),
            cv::Scalar n_color = CV_RGB(255, 255, 255)) {
    pos = n_pos;
    size = n_size;
    color = n_color;
  }
  void SetParentCanvas(cv::Mat* n_parent_canvas) {
    parent_canvas = n_parent_canvas;
  }
  virtual void Render() { cv::circle(*parent_canvas, pos, 20, color); }
  virtual void Update(int key, int x, int y, int event) {}
};

class RectangleLine : public UIElement {
 public:
  RectangleLine(cv::Point n_pos = cv::Point(100, 100),
                cv::Size n_size = cv::Size(100, 100),
                cv::Scalar n_color = CV_RGB(255, 255, 255)) {
    pos = n_pos;
    size = n_size;
    color = n_color;
  }
  void Render() {
    cv::rectangle(*parent_canvas,
                  cv::Rect(cv::Point(pos.x, pos.y),
                           cv::Point(pos.x + size.width, pos.y + size.height)),
                  color, 2);
  }
};

class Text : public UIElement {
  string text;
  int face;
  double scale;
  bool autosize_mode;

 public:
  Text(string txt, cv::Point n_pos = cv::Point(400, 400),
       cv::Size n_size = cv::Size(100, 100),
       cv::Scalar n_color = CV_RGB(0, 0, 0), bool n_autosize_mode = false,
       int n_face = cv::FONT_HERSHEY_PLAIN, double n_scale = 1) {
    text = txt;
    autosize_mode = n_autosize_mode;
    face = n_face;
    scale = n_scale;
    pos = n_pos;
    size = n_size;
    color = n_color;
  }
  void Render() {
    if (autosize_mode) {
      cv::Size rect = cv::getTextSize(text, face, 1.0, 1, 0);
      double scalex = (double)size.width / (double)rect.width;
      double scaley = (double)size.height / (double)rect.height;
      double scale = (std::min)(scalex, scaley);
      scale *= 0.8;
      int marginx =
          scale == scalex
              ? 0
              : (int)((double)size.width * (scalex - scale) / scalex * 0.5);
      int marginy =
          scale == scaley
              ? 0
              : (int)((double)size.height * (scaley - scale) / scaley * 0.5);

      cv::putText(*parent_canvas, text,
                  cv::Point(pos.x + marginx, pos.y + size.height - marginy),
                  face, scale, color, scale, 8, false);
    } else {
      cv::putText(*parent_canvas, text, pos, face, scale, color, scale);
    }
  }
  void Update() {}
};

class Image : public UIElement {
  cv::Mat image;

 public:
  Image(cv::Mat n_image, cv::Point n_pos = cv::Point(100, 100),
        cv::Size n_size = cv::Size(100, 100),
        cv::Scalar n_color = CV_RGB(255, 255, 255)) {
    image = n_image;
    pos = n_pos;
    size = n_size;
    color = n_color;
  }
  void Render() {
    cv::Mat tmp = image;
    cv::resize(tmp, tmp, size);

    cv::Rect roi(pos, size);
    cv::Mat destinationROI = (*parent_canvas)(roi);
    tmp.copyTo(destinationROI);
  }
};

class Button : public UIElement {
 public:
  std::string text;
  std::function<void()> func;
  Button(
      cv::Point n_pos = cv::Point(100, 100),
      cv::Size n_size = cv::Size(100, 50),
      std::function<void()> n_func =
          []() { printf("%s", "NO FUNCTION FOR BUTTON\n"); },
      cv::Scalar n_color = CV_RGB(0, 0, 0),
      std::string n_text = "Sample text") {
    pos = n_pos;
    size = n_size;
    func = n_func;
    color = n_color;
    text = n_text;
  }
  void Render() {
    auto border = RectangleLine(pos, size, color);
    border.SetParentCanvas(parent_canvas);
    border.Render();

    auto txt = Text(text, pos, size, color, true);
    txt.SetParentCanvas(parent_canvas);
    txt.Render();
  }
  void Update(int key, int x, int y, int event) {
    if (event == cv::EVENT_LBUTTONDOWN) {
      if (x >= pos.x && pos.x + size.width >= x && y >= pos.y &&
          pos.y + size.height >= y) {
        func();
      }
    }
  }
};

class InputField : public UIElement {
  bool active;
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

  std::chrono::steady_clock::time_point t_start;

 public:
  bool is_awake;
  InputField(bool n_active, const string n_prompt,
             const string n_default = "enter something here",
             cv::Point n_pos = cv::Point(100, 100),
             cv::Size n_size = cv::Size(100, 100)) {
    prompt = n_prompt;
    pos = n_pos;
    size = n_size;
    char_size = 15;
    ActivateCursor();
    cursor_pos = 0;
    insert_state = false;
    active = n_active;
    is_awake = false;
    entered = n_default;

    text_color = CV_RGB(200, 200, 200);
    cursor_color = CV_RGB(0, 255, 0);
    background_color = CV_RGB(255, 255, 255);
    title_color = CV_RGB(0, 0, 0);

    t_start = std::chrono::high_resolution_clock::now();
  }

  void drawToRect(cv::Mat& mat, cv::Rect target, int face, int thickness,
                  cv::Scalar color, const std::string& str) {
    cv::Size rect = cv::getTextSize(str, face, 1.0, thickness, 0);
    double scalex = (double)target.width / (double)rect.width;
    double scaley = (double)target.height / (double)rect.height;
    double scale = (std::min)(scalex, scaley);
    int marginx =
        scale == scalex
            ? 0
            : (int)((double)target.width * (scalex - scale) / scalex * 0.5);
    int marginy =
        scale == scaley
            ? 0
            : (int)((double)target.height * (scaley - scale) / scaley * 0.5);
    cv::putText(
        mat, str,
        cv::Point(target.x + marginx, target.y + target.height - marginy), face,
        scale, color, thickness, 8, false);
  }

  void drawMonospaceText(cv::Mat& mat, cv::Rect target, int face, int thickness,
                         cv::Scalar color, const std::string& str,
                         int char_size = 15) {
    cv::Size rect = cv::getTextSize(str, face, 1.0, thickness, 0);
    rect.width = static_cast<int>(str.size() * char_size);
    for (int i = 0; i < str.size(); ++i)
      cv::putText(mat, (str[i] + string("")),
                  cv::Point(target.x + char_size * i, target.y + 20), face, 1.5,
                  color, thickness, 8, false);
  }
  void drawCursor(cv::Mat& mat, cv::Point pos, int thickness, cv::Scalar color,
                  const int cursor_pos, int char_size) {
    cv::line(mat, cv::Point(pos.x + char_size * cursor_pos, pos.y + 2),
             cv::Point(pos.x + char_size * cursor_pos,
                       static_cast<int>(pos.y + char_size * 1.5 - 2)),
             color, thickness);
  }
  void drawInsertCursor(cv::Mat& mat, cv::Point pos, int thickness,
                        cv::Scalar color, const int cursor_pos, int char_size) {
    cv::rectangle(
        mat,
        cv::Rect(cv::Point(pos.x + char_size * cursor_pos, pos.y + 2),
                 cv::Point(pos.x + char_size * (cursor_pos + 1), pos.y + 25)),
        color, thickness);
  }
  void ActivateCursor() {
    t_start = std::chrono::high_resolution_clock::now();
    cursor_state = true;
    active = true;
    if (is_awake == false) {
      is_awake = true;
      entered = "";
      cursor_pos = 0;
    }
    text_color = CV_RGB(0, 0, 0);
  }
  void Render() {
    auto t_end = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration<double, std::milli>(t_end - t_start).count() > 200) {
      cursor_state = !cursor_state;  // cursor blink effect
      t_start = t_end;
    }
    
    // draw what user entered
    cv::Rect rect(pos, cv::Size(((int)entered.size() + 1) * char_size + 1,
                                static_cast<int>(char_size * 1.5)));
    drawMonospaceText(*parent_canvas, rect, cv::FONT_HERSHEY_PLAIN, 1,
                      text_color, entered);
    // draw cursor
    if (active) {
      if (cursor_state) {
        if (insert_state)
          drawInsertCursor(*parent_canvas, pos, 2, cursor_color, cursor_pos,
                           char_size);
        else
          drawCursor(*parent_canvas, pos, 2, cursor_color, cursor_pos,
                     char_size);
      }
    }
    // draw prompt
    cv::Rect rect2(pos - cv::Point(0, 32),
                   cv::Size((int)prompt.size() * 16, 16));
    drawToRect(*parent_canvas, rect2, cv::FONT_HERSHEY_PLAIN, 1, title_color,
               prompt);
    cv::rectangle(*parent_canvas, rect, title_color, 1);
  }

  void Update(int key, int x, int y, int event) {
    if (event == cv::EVENT_LBUTTONUP) {
      if (y < pos.y + 20 && y > pos.y - 5 && x > pos.x) {
        if (x < pos.x + entered.size() * char_size + char_size) {
          if (is_awake == false) {
            is_awake = true;
            entered = "";
            cursor_pos = 0;
          } else {
            cursor_pos = (x - pos.x + char_size / 2) / char_size;
            if (cursor_pos > static_cast<int>(entered.size()))
              cursor_pos = static_cast<int>(entered.size());
          }
        }
        ActivateCursor();
      } else {
        if (is_awake) active = false;
      }
    }
    if (active) {
      if (key == 27) {
        entered = "";
        return;
      }
      // ->
      if (key == OPENCV_RIGHT_ARROW_CODE) {
        ActivateCursor();
        if (cursor_pos < entered.size()) cursor_pos++;
      }
      // <-
      if (key == OPENCV_LEFT_ARROW_CODE) {
        ActivateCursor();
        if (cursor_pos > 0) cursor_pos--;
      }
      // backspace
      if (key == 8) {
        ActivateCursor();
        if (cursor_pos > 0) entered.erase(entered.begin() + --cursor_pos);
      }
      // delite
      if (key == 3014656) {
        ActivateCursor();
        if (cursor_pos < entered.size())
          entered.erase(entered.begin() + cursor_pos);
      }
      // enter
      if (key == 13) {
        return;
      }
      // insert
      if (key == 2949120) {
        ActivateCursor();
        insert_state = !insert_state;
      }
      // ASCII symbol entered
      if (key >= 32 && key < 128) {
        ActivateCursor();
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

  string getText() const { return entered; }
};

class ButImage : public UIElement {
 public:
  std::function<void()> func1;
  std::function<void()> func2;
  std::string text1;
  std::string text2;
  int buttonHeight;
  cv::Mat image;
  bool fin = false;
  ButImage(
      cv::Point n_pos = cv::Point(100, 100),
      cv::Size n_size = cv::Size(200, 200),
      std::function<void()> n_func1 =
          []() { printf("%s", "NO FUNCTION FOR BUTTON\n"); },
      std::function<void()> n_func2 =
          []() { printf("%s", "NO FUNCTION FOR BUTTON\n"); },
      cv::Scalar n_color = CV_RGB(0, 0, 0), std::string n_text1 = "Sample text",
      std::string n_text2 = "Sample text", int n_buttonHeight = 20,
      cv::Mat n_image = cv::imread("empty.png"), bool n_fin = false) {
    pos = n_pos;
    size = n_size;
    func1 = n_func1;
    func2 = n_func2;
    color = n_color;
    text1 = n_text1;
    text2 = n_text2;
    buttonHeight = n_buttonHeight;
    image = n_image;
    fin = n_fin;
  }
  void Render() {
    auto button1 =
        Button(pos, cv::Size(size.width, buttonHeight), func1, color, text1);
    button1.SetParentCanvas(parent_canvas);
    button1.Render();

    auto img = Image(image, cv::Point(pos.x, pos.y + buttonHeight),
                     cv::Size(size.width, size.height - buttonHeight * 2));
    img.SetParentCanvas(parent_canvas);
    img.Render();
    if (fin) return;
    auto button2 =
        Button(cv::Point(pos.x, pos.y + size.height - buttonHeight),
               cv::Size(size.width, buttonHeight), func2, color, text2);
    button2.SetParentCanvas(parent_canvas);
    button2.Render();
  }
  void Update(int key, int x, int y, int event) {
    if (event == cv::EVENT_LBUTTONDOWN) {
      if (x >= pos.x && pos.x + size.width >= x && y >= pos.y &&
          pos.y + buttonHeight >= y) {
        func1();
      }
      if (x >= pos.x && pos.x + size.width >= x &&
          y >= pos.y + size.height - buttonHeight && pos.y + size.height >= y) {
        func2();
      }
    }
  }
  void ResetPhoto(cv::Mat n_image) { image = n_image; }
};

class VideoStream : public UIElement {
  cv::VideoCapture cap;

 public:
  VideoStream(std::string file = "", cv::Point n_pos = cv::Point(100, 100),
              cv::Size n_size = cv::Size(100, 100)) {
    pos = n_pos;
    size = n_size;
    cap.open(0);
  }

  void Render() {
    cv::Mat frame;
    cap >> frame;

    if (frame.empty()) return;

    Image i(frame, pos, size);
    i.SetParentCanvas(parent_canvas);
    i.Render();
  }
  void Update(int key, int x, int y, int event) { Render(); }
};