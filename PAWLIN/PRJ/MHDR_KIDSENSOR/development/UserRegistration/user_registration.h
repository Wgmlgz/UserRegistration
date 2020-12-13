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

using namespace pawlin;

Canvas main_canvas;

std::vector<string> input_titles = {"Enter name", "Enter age", "Enter gender"};
std::vector<string> input_default = {"name", "age", "gender"};
std::vector<string> values;

string title_string;

std::vector<InputField*> input_fields;
void CreateStartCanvas();
void CreateConfirmCanvas();
void CreateRegistrationCanvas();
void CreateRegistrationSuccesCanvas();
void CreateRegistrationFailCanvas();
void CreateInputCanvas(std::vector<string>, std::vector<string>);