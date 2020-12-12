#include "stdafx.h"
#include "button.h"
#include "text.h"

Button::Button(cv::Point n_pos, cv::Size n_size, std::function <void()> n_func, cv::Scalar n_color, std::string n_text)
{
	type = "Button";
	pos = n_pos;
	size = n_size;
	func = n_func;
	color = n_color;
	text = n_text;
}

void Button::Render() 
{
	auto border = RectangleLine(pos, size, color);
	border.SetParentCanvas(parent_canvas);
	border.Render();

	auto txt = Text(text, pos, size, color, true);
	txt.SetParentCanvas(parent_canvas);
	txt.Render();
}

void Button::Update(int key, int x, int y, int event)
{
	if (event == cv::EVENT_LBUTTONDOWN) {
		if (x >= pos.x && pos.x + size.width >= x && y >= pos.y && pos.y + size.height >= y)
		{
			func();
		}
	}
}