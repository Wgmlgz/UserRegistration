#include "stdafx.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <PWNGeneral/ArgParser.h>
#include <PWNGeneral/pwnutil.h>
#include <PWNOpenCvExt/cvmathdrawingnew.h>

#include "canvas.h"
#include "video_stream.h"
#include "image.h"
#include "rectangle.h"
#include "text.h"
#include "button.h"

using namespace pawlin;
class TextEdit : public CvInteractWindowBase {
	string prompt;
	string entered;
	cv::Mat canvas;
	cv::Point where;

    int char_size;
    int cursor_pos;
    bool cursor_state;
    bool insert_state;

    // colors
    cv::Scalar text_color;
    cv::Scalar cursor_color;
    cv::Scalar background_color;
    cv::Scalar title_color;
 public:
	TextEdit(const string &prompt) : prompt(prompt), CvInteractWindowBase("Enter string", cv::Size(512, 512), CV_8UC3) {
		canvas = cv::Mat::zeros(canv_size, canv_type);
		where = cv::Point(100, 100);

        char_size = 15;
        cursor_state = false;
        cursor_pos = 0;
        insert_state = false;

        // colors
        text_color       = CV_RGB(120, 120, 120);
        cursor_color     = CV_RGB(0  , 255, 0  );
        background_color = CV_RGB(255, 255, 255);
        title_color      = CV_RGB(33 , 33 , 33 );
	}
	void drawToRect(cv::Mat & mat, cv::Rect target, int face, int thickness, cv::Scalar color, const std::string & str)
	{
		cv::Size rect = cv::getTextSize(str, face, 1.0, thickness, 0);
		double scalex = (double)target.width / (double)rect.width;
		double scaley = (double)target.height / (double)rect.height;
		double scale = (std::min)(scalex, scaley);
		int marginx = scale == scalex ? 0 : (int)((double)target.width * (scalex - scale) / scalex * 0.5);
		int marginy = scale == scaley ? 0 : (int)((double)target.height * (scaley - scale) / scaley * 0.5);
		cv::putText(mat, str, cv::Point(target.x + marginx, target.y + target.height - marginy), face, scale, color, thickness, 8, false);
	}
    void drawMonospaceText(cv::Mat &mat, cv::Rect target, int face, int thickness, cv::Scalar color, const std::string &str, int char_size = 15)
	{
        cv::Size rect = cv::getTextSize(str, face, 1.0, thickness, 0);
        rect.width = static_cast<int>(str.size() * char_size);
        for (int i = 0; i < str.size(); ++i)
            cv::putText(mat, (str[i] + string("")), cv::Point(target.x + char_size * i, target.y + 20), face, 1.5, color, thickness, 8, false);
    }
    void drawCursor(cv::Mat &mat, cv::Point pos, int thickness, cv::Scalar color, const int cursor_pos, int char_size)
	{
      cv::line(mat, cv::Point(pos.x + char_size * cursor_pos, pos.y + 2),
                    cv::Point(pos.x + char_size * cursor_pos, static_cast<int>(pos.y + char_size * 1.5 - 2)),
          color, thickness);
    }
    void drawInsertCursor(cv::Mat &mat, cv::Point pos, int thickness, cv::Scalar color, const int cursor_pos, int char_size) {
        cv::rectangle(mat, cv::Rect(cv::Point(pos.x + char_size * cursor_pos, pos.y + 2),
                           cv::Point(pos.x + char_size * (cursor_pos + 1), pos.y + 25)),
            color, thickness);
    }
	void run() {
        for (;;) {
            cursor_state = !cursor_state;  // cursor blink effect
			canvas = background_color; // fill with nice color

			// draw what user entered
            cv::Rect rect(where, cv::Size(((int)entered.size() + 1) * char_size + 1, static_cast<int>(char_size * 1.5)));
            drawMonospaceText(canvas, rect, cv::FONT_HERSHEY_PLAIN, 1, text_color, entered);

			// draw cursor
            if (cursor_state) {
                 if (insert_state)
                     drawInsertCursor(canvas, where, 1, cursor_color, cursor_pos, char_size);
                 else
                     drawCursor(canvas, where, 1, cursor_color, cursor_pos, char_size);
            }

			// draw prompt
			cv::Rect rect2(where-cv::Point(0,32), cv::Size((int)prompt.size() * 16, 16));
			drawToRect(canvas, rect2, cv::FONT_HERSHEY_PLAIN, 1, title_color, prompt);

            // key events
			int key = pawlin::debugImg(winname, canvas, 1, 200, false);
			// ESC button pressed
			if (key == 27) {
                entered = "";
                return;
            }
            // ->
            if (key == OPENCV_RIGHT_ARROW_CODE){ 
                cursor_state = false;
				if (cursor_pos < entered.size())cursor_pos++;
            }
            // <-
			if (key == OPENCV_LEFT_ARROW_CODE) { 
                cursor_state = false;
                if (cursor_pos > 0) cursor_pos--;
            } 
			// backspace
            if (key == 8) { 
                cursor_state = false;
                if (cursor_pos > 0)
                    entered.erase(entered.begin() + --cursor_pos);
            }	
			// delite
            if (key == 3014656) { 
                cursor_state = false;
                if (cursor_pos < entered.size()) entered.erase(entered.begin() + cursor_pos);
            }
			// enter
			if (key == 13) {
                return;
            }
            // insert
            if (key == 2949120) {
                cursor_state = false;
                insert_state = !insert_state;
            }
            // ASCII symbol entered
			if (key >= 32 && key < 128) {  
                cursor_state = false;
                if (insert_state) {
                  if (cursor_pos == entered.size())
                    entered.push_back((char)key);
                  else
                    entered[cursor_pos] = (char)key;
                } else {
                  entered.insert(cursor_pos, 1, (char)key);
                }
                cursor_pos++;
            }
		}
	}
	virtual void updateClick(int x, int y, int event, int flags) override {
        // change cursor position with mouse click
        if (event == cv::EVENT_LBUTTONDOWN) {
            if (y < where.y + 20 && y > where.y - 5) {
                if (x > where.x&& x < where.x + entered.size() * char_size + char_size) {
                    cursor_pos = (x - where.x + char_size / 2) / char_size;
                    if (cursor_pos > static_cast<int>(entered.size())) cursor_pos = static_cast<int>(entered.size());
                    cursor_state = false;
                }
            }
        }
	}
	string getText() const { return entered; }
};

int process(const ArgParser &parser) {
  Canvas main_canvas;
  main_canvas.AddUIElement(new VideoStream("Novogodniy_dudos.avi", cv::Point(200, 100)));
  main_canvas.AddUIElement(new Image(cv::imread("da.png")));
  main_canvas.AddUIElement(new RectangleLine(cv::Point(100, 300), cv::Size(100, 100)));
  main_canvas.AddUIElement(new Button(cv::Point(200, 400), cv::Size(200, 100), []() {printf("%s", "HI GAY!!!!\n"); }, CV_RGB(0, 0, 0)));
  main_canvas.AddUIElement(new Text("8=====)"));
  main_canvas.Run();
  return 0;
}

int main(int argc, char *argv[])
{
	try {
		ArgParser parser(argc, argv);
		return process(parser);
	}
	catch (std::exception e) {
		printf("Exception occured: %s\n", e.what());
		getch_os();
	}
    return 0;
}

