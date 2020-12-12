#pragma once
#include "stdafx.h"
#include "ui_element.h"
#include "button.h"
#include "image.h"
#include "rectangle.h"

class ButImage : public UIElement {
public:
	std::function <void()> func1;
	std::function <void()> func2;
	std::string text1;
	std::string text2;
	int buttonHeight;
	cv::Mat image;
	ButImage(cv::Point n_pos = cv::Point(100, 100),
			 cv::Size n_size = cv::Size(200, 200),
			 std::function <void()> n_func1 = []() {printf("%s", "NO FUNCTION FOR BUTTON\n"); },
			 std::function <void()> n_func2 = []() {printf("%s", "NO FUNCTION FOR BUTTON\n"); },
			 cv::Scalar n_color = CV_RGB(0, 0, 0),
			 std::string n_text1 = "Sample text",
			 std::string n_text2 = "Sample text",
			 int buttonHeight = 20,
			 cv::Mat n_image = cv::imread("empty.png"));
    void Render();
	void Update(int key, int x, int y, int event);
	void ResetPhoto(cv::Mat n_image);
};