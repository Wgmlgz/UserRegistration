#include "stdafx.h"
#include "text.h"
#include "ui_element.h"

Text::Text(string txt,
           cv::Point n_pos,
           cv::Size n_size, cv::Scalar n_color, bool n_autosize_mode,
           int n_face, double n_scale) {
  text = txt;
  autosize_mode = n_autosize_mode;
  face = n_face;
  scale = n_scale;
  pos = n_pos;
  size = n_size;
  color = n_color;
}

void Text::Render() {
    if (autosize_mode) {
        cv::Size rect = cv::getTextSize(text, face, 1.0, 1, 0);
        double scalex = (double)size.width / (double)rect.width;
        double scaley = (double)size.height / (double)rect.height;
        double scale = (std::min)(scalex, scaley);
        scale *= 0.8;
        int marginx = scale == scalex ? 0 : (int)((double)size.width * (scalex - scale) / scalex * 0.5);
        int marginy = scale == scaley ? 0 : (int)((double)size.height * (scaley - scale) / scaley * 0.5);

        cv::putText(*parent_canvas, text, cv::Point(pos.x + marginx, pos.y + size.height - marginy), face, scale, color, scale, 8, false);
    }
    else {
        cv::putText(*parent_canvas, text, pos, face, scale, color, scale);
    }
}
void Text::Update() {}