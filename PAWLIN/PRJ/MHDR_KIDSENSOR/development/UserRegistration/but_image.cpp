#include "stdafx.h"
#include "but_image.h"
#include "button.h"
#include "text.h"

ButImage::ButImage(cv::Point n_pos, cv::Size n_size,
                   std::function<void()> n_func1, std::function<void()> n_func2,
                   cv::Scalar n_color, std::string n_text1, std::string n_text2,
                   int n_buttonHeight, cv::Mat n_image, bool n_fin) 

void ButImage::Render() 

void ButImage::Update(int key, int x, int y, int event) 

void ButImage::ResetPhoto(cv::Mat n_image)