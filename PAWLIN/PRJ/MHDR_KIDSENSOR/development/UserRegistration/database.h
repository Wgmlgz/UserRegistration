#pragma once
#include <PWNGeneral/ArgParser.h>
#include <PWNGeneral/pwnutil.h>
#include <PWNOpenCvExt/cvmathdrawingnew.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <vector>

#include "stdafx.h"

class User {
 public:
  string name, age, gender;
  std::vector<cv::Mat> images;
  User(string n_name, string n_age, string n_gender,
       std::vector<cv::Mat> n_images) {
    name = n_name;
    age = n_age;
    gender = n_gender;
    images = n_images;
  }
};

class Database {
 public:
  std::vector<User> users;
};