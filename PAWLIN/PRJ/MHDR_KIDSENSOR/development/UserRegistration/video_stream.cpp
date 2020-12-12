#include "stdafx.h"
#include "video_stream.h"

VideoStream::VideoStream(std::string file) {
  if (file == "") {
    cap.open("Novogodniy_dudos.avi");
  } else {
    cap.open(file);
  }
}

void VideoStream::Render() {
  cv::Mat frame;
  cap >> frame;
  printf("dsd");
  frame.copyTo(*parent_canvas);
  //parent_canvas->
  //*parent_canvas = frame;
  //cv::imshow("this is you, smile! :)", frame);
}
