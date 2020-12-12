#include "stdafx.h"
#include "video_stream.h"

VideoStream::VideoStream(std::string file, cv::Point n_pos, cv::Size n_size) {
  type = "VideoStream";
  pos = n_pos;
  size = n_size;
  if (file == "") {
    cap.open(0);
  } else {
    cap.open(file);
  }
}

void VideoStream::Render() {
  cv::Mat frame;
  cap >> frame;

  Image i(frame, pos, size);
  i.SetParentCanvas(parent_canvas);
  
  i.Render();
}
