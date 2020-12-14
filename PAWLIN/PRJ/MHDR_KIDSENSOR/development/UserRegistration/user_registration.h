#pragma once
#include "stdafx.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <PWNGeneral/ArgParser.h>
#include <PWNGeneral/pwnutil.h>
#include <PWNOpenCvExt/cvmathdrawingnew.h>

#include <vector>

#include "canvas.h"
#include "video_stream.h"
#include "image.h"
#include "rectangle.h"
#include "text.h"
#include "button.h"
#include "input_field.h"
#include "database.h"
#include "but_image.h"

#include "image_provider.h"

using namespace pawlin;

Canvas main_canvas;

std::vector<string> input_titles = {"Enter name", "Enter age", "Enter gender"};
std::vector<string> input_default = {"name", "age", "gender"};
std::vector<string> values;

std::vector<cv::Mat> images;

string title_string;

Database database;

IImageProvider* imageProvider;

std::vector<InputField*> input_fields;
void CreateStartCanvas();
void CreateConfirmCanvas();
void CreateRegistrationCanvas(bool inc = false, bool start = false);
void CreateRegistrationSuccesCanvas();
void CreateTextCanvas(string text, cv::Scalar color = CV_RGB(0, 0, 0));
void CreateRegistrationFailCanvas(string error);
void CreateInputCanvas(std::vector<string>, std::vector<string>);