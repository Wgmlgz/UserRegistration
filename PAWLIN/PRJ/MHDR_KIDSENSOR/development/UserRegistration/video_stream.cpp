#include "stdafx.h"
#include "video_stream.h"

VideoStream::VideoStream(std::string file, cv::Point n_pos, cv::Size n_size) {
  pos = n_pos;
  size = n_size;
  cap.open(0);
}

void VideoStream::Update(int key, int x, int y, int event) {
  Render();
}

void VideoStream::Render() {
  cv::Mat frame;
  cap >> frame;

  if (frame.empty()) return;

  Image i(frame, pos, size);
  i.SetParentCanvas(parent_canvas);
  i.Render();
}
