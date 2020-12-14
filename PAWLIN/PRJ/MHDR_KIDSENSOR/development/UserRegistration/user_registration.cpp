#include "stdafx.h"
#include "user_registration.h"


void CreateStartCanvas() {
  main_canvas.ClearCanvas();
  main_canvas.AddUIElement(new Button(cv::Point(1280 / 2 - 100, 720 / 2 - 30 - 50), cv::Size(200, 50), []() {CreateInputCanvas(input_titles, input_default);}, CV_RGB(0, 0, 0), "Start registration"));
  main_canvas.AddUIElement(new Button(cv::Point(1280 / 2 - 100, 720 / 2 + 30 - 50), cv::Size(200, 50), []() {CreateConfirmCanvas();}, CV_RGB(0, 0, 0), "Clear database"));
}
void CreateTextCanvas(string text, cv::Scalar color) {
  main_canvas.ClearCanvas();
  main_canvas.AddUIElement(new Text(text, cv::Point(1280 / 2 - 500, 720 / 2 - 25), cv::Size(1000, 50), color, true, 1, 2));
  main_canvas.AddUIElement(new Button(cv::Point(1280 / 2 - 100, 720 / 2 - 25 + 100 ), cv::Size(200, 50), []() {CreateStartCanvas();}, CV_RGB(0, 0, 0), "Back to main menu"));
}
void CreateConfirmCanvas() {
  main_canvas.ClearCanvas();
  main_canvas.AddUIElement(new Text("Are you sure you want to clear the database?", cv::Point(1280 / 2 - 500, 720 / 2 - 25), cv::Size(1000, 50), CV_RGB(0, 0, 0), true, 1, 2));
  main_canvas.AddUIElement(new Button(cv::Point(1280 / 2 - 100 + 120, 720 / 2 - 50 + 100), cv::Size(200, 50), []() {CreateStartCanvas(); }, CV_RGB(0, 0, 0), "Cancel"));
  main_canvas.AddUIElement(new Button(cv::Point(1280 / 2 - 100 - 120, 720 / 2 - 50 + 100), cv::Size(200, 50), []() {
	  try {
        database.users.clear();
        CreateTextCanvas("Database was successfully cleared");
	  }
	  catch (std::exception error) {
        CreateTextCanvas(error.what(), CV_RGB(100, 0, 0));
	  }
	  
      CreateStartCanvas();
	  }, CV_RGB(200, 0, 0), "Confirm"));
}
void CreateRegistrationCanvas(bool inc, bool start) {
  static VideoStream *vid = nullptr;
  static int pos = 0;
  if (start) vid = new VideoStream("", cv::Point(1280 - 300 - 50, 100), cv::Size(300, 300));
  if (images.size() == 0 || start) {
    images = std::vector<cv::Mat>(6, cv::Mat::zeros(cv::Size(1, 1), 0));
    pos = 0;
  }
  if (inc) {
    ++pos;
  }

  main_canvas.ClearCanvas();
  main_canvas.AddUIElement(vid);
  main_canvas.AddUIElement(new Text(title_string, cv::Point(50, 50), cv::Size(1000, 50), CV_RGB(0, 0, 0), false, 1, 2));
  if(pos >= 5)
    main_canvas.AddUIElement(new Button(cv::Point(1280 - 40 - 420, 720 - 50 - 50), cv::Size(200, 50), []() {CreateRegistrationSuccesCanvas(); }, CV_RGB(0, 0, 0), "Submit"));
  main_canvas.AddUIElement(new Button(cv::Point(1280 - 200 - 50, 720 - 50 - 50), cv::Size(200, 50), []() {CreateStartCanvas(); }, CV_RGB(0, 0, 0), "Cancel"));
  int it = 0;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j, ++it) {
        if (it == pos)
        {
            images.at(pos) = imageProvider->getImage();
            main_canvas.AddUIElement(new ButImage(cv::Point(j * 275 + 50, i * 275 + 80), cv::Size(200, 280), []() {CreateRegistrationCanvas(0, 0); }, []() {CreateRegistrationCanvas(1); }, CV_RGB(0, 0, 0), "Shoot", "Next", 40, images.at(it), pos == 5));
            return;
        }
        main_canvas.AddUIElement(new Image(images.at(it), cv::Point(j * 275 + 50, i * 275 + 120), cv::Size(200, 200)));
    }
  }
}

void CreateRegistrationSuccesCanvas() {
  main_canvas.ClearCanvas();
  //auto cur_img = images.begin();
  int k = 0;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j) {
	  main_canvas.AddUIElement(new Image(images[k],cv::Point(j * 300 + 50, i * 300 + 60), cv::Size(275, 275)));
      ++k;
    }
  }
  main_canvas.AddUIElement(new Text(title_string, cv::Point(50, 40), cv::Size(1000, 50), CV_RGB(0, 0, 0), false, 1, 2));
  main_canvas.AddUIElement(new Text("Was successfully registered!", cv::Point(50, 720 - 30), cv::Size(1000, 50), CV_RGB(0, 0, 0), false, 1, 2));
  main_canvas.AddUIElement(new Button(cv::Point(1280 - 200 - 100, 720 / 2 - 25), cv::Size(200, 50), []() {CreateStartCanvas();}, CV_RGB(0, 0, 0), "Back to main menu"));
  main_canvas.Render();
}
void CreateRegistrationFailCanvas(string error) {
  main_canvas.ClearCanvas();
  main_canvas.AddUIElement(new Text(error, cv::Point(300, 720 / 2 - 200), cv::Size(400, 400), CV_RGB(200, 0, 0), true, 1, 2));
  main_canvas.AddUIElement(new Button(cv::Point(1280 - 200 - 300, 720 / 2 - 25), cv::Size(200, 50), []() {CreateStartCanvas();}, CV_RGB(0, 0, 0), "Back to main menu"));
}
void CreateInputCanvas(std::vector<string> titles = {}, std::vector<string> defaults = {}) {
  main_canvas.ClearCanvas();
  input_fields.clear();
  for (int i = 0; i < titles.size(); ++i) {
    //continue;
    auto tmp_input_field =
        new InputField(titles[i], defaults[i], cv::Point(50, 100 + i * 100));
    input_fields.push_back(tmp_input_field);
    main_canvas.AddUIElement(tmp_input_field);
  }
  main_canvas.AddUIElement(new Button(cv::Point(1280 - 200 - 200 - 50 - 25, 720 - 50 - 50), cv::Size(200, 50), []() {
      for (auto i : input_fields) {
        values.push_back(i->getText());
	  }
	  title_string = string(input_fields[0]->getText() + ", " + input_fields[1]->getText() + ", " + input_fields[2]->getText());
      CreateRegistrationCanvas(0, 1);
	  }, CV_RGB(0, 0, 0), "Submit"));
  main_canvas.AddUIElement(new Button(cv::Point(1280 - 200 - 50, 720 - 50 - 50), cv::Size(200, 50), []() {CreateStartCanvas();}, CV_RGB(0, 0, 0), "Cancel"));
}
void CallBackFunc(int event, int x, int y, int flags, void *userdata) {
  main_canvas.CallBackFunc(event, x, y, flags);
}
int process(const ArgParser &parser) {
  cv::setMouseCallback(main_canvas.window_name, CallBackFunc);
  imageProvider = new IImageProvider;
  CreateStartCanvas();
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

