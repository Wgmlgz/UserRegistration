#include "stdafx.h"
#include "user_registration.h"




void CreateStartCanvas() {
  main_canvas.ClearCanvas();
  main_canvas.AddUIElement(new Button(cv::Point(1280 / 2 - 100, 720 / 2 - 30 - 50), cv::Size(200, 50), []() {CreateInputCanvas();}, CV_RGB(0, 0, 0), "Start registration"));
  main_canvas.AddUIElement(new Button(cv::Point(1280 / 2 - 100, 720 / 2 + 30 - 50), cv::Size(200, 50), []() {CreateConfirmCanvas();}, CV_RGB(0, 0, 0), "Clear database"));
  main_canvas.Run();
}
void CreateConfirmCanvas() {
  main_canvas.ClearCanvas();
  main_canvas.AddUIElement(new Button(cv::Point(1280 / 2 - 100 - 120, 720 / 2 - 50), cv::Size(200, 50), []() {printf("%s", "HI GAY!!!!\n"); }, CV_RGB(200, 0, 0), "Confirm"));
  main_canvas.AddUIElement(new Button(cv::Point(1280 / 2 - 100 + 120, 720 / 2 - 50), cv::Size(200, 50), []() {CreateStartCanvas(); }, CV_RGB(0, 0, 0), "Cancel"));
  main_canvas.Run();
}
void CreateRegistrationCanvas() {
  main_canvas.ClearCanvas();
  main_canvas.AddUIElement(new Text("Wgmlgz, 54, attack heleckopter", cv::Point(50, 50), cv::Size(1000, 50), CV_RGB(0, 0, 0), false, 1, 2));
  main_canvas.AddUIElement(new VideoStream("Novogodniy_dudos.avi", cv::Point(1280 - 300 - 50, 100), cv::Size(300, 300)));
  main_canvas.AddUIElement(new Button(cv::Point(1280 - 200 - 50, 720 - 50 - 50), cv::Size(200, 50), []() {CreateStartCanvas(); }, CV_RGB(0, 0, 0), "Cancel"));
  main_canvas.Run();
}
void CreateRegistrationSuccesCanvas(//std::vector<cv::Mat> images = nullptr
) {
  main_canvas.ClearCanvas();
  //auto cur_img = images.begin();
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j) {
		main_canvas.AddUIElement(new Image(cv::imread("da.png"),cv::Point(j * 300 + 50, i * 300 + 60), cv::Size(275, 275)));
    }
  }
  main_canvas.AddUIElement(new Text("Wgmlgz, 54, attack heleckopter", cv::Point(50, 40), cv::Size(1000, 50), CV_RGB(0, 0, 0), false, 1, 2));
  main_canvas.AddUIElement(new Text("Was successfully registered!", cv::Point(50, 720 - 30), cv::Size(1000, 50), CV_RGB(0, 0, 0), false, 1, 2));
  main_canvas.AddUIElement(new Button(cv::Point(1280 - 200 - 100, 720 / 2 - 25), cv::Size(200, 50), []() {CreateStartCanvas();}, CV_RGB(0, 0, 0), "Back to main menu"));
  main_canvas.Render();
  main_canvas.Run(0);
}
void CreateRegistrationFailCanvas(string error) {
  main_canvas.ClearCanvas();
  main_canvas.AddUIElement(new Text(error, cv::Point(300, 720 / 2 - 200), cv::Size(400, 400), CV_RGB(200, 0, 0), true, 1, 2));
  main_canvas.AddUIElement(new Button(cv::Point(1280 - 200 - 300, 720 / 2 - 25), cv::Size(200, 50), []() {CreateStartCanvas();}, CV_RGB(0, 0, 0), "Back to main menu"));
  main_canvas.Run();
}
void CreateInputCanvas() {
  main_canvas.ClearCanvas();
  main_canvas.AddUIElement(new Button(cv::Point(1280 - 200 - 200 - 50 - 25, 720 - 50 - 50), cv::Size(200, 50), []() {CreateRegistrationCanvas();}, CV_RGB(0, 0, 0), "Submit"));
  main_canvas.AddUIElement(new Button(cv::Point(1280 - 200 - 50, 720 - 50 - 50), cv::Size(200, 50), []() {CreateStartCanvas();}, CV_RGB(0, 0, 0), "Cancel"));
  
  
  main_canvas.Run();
}
int process(const ArgParser &parser) {
  //main_canvas.AddUIElement(new VideoStream("Novogodniy_dudos.avi", cv::Point(200, 100)));
  //main_canvas.AddUIElement(new Image(cv::imread("da.png")));
  //main_canvas.AddUIElement(new RectangleLine(cv::Point(100, 300), cv::Size(100, 100)));
  //main_canvas.AddUIElement(new Button(cv::Point(200, 400), cv::Size(200, 100), []() {printf("%s", "HI GAY!!!!\n"); }, CV_RGB(0, 0, 0)));
  //main_canvas.AddUIElement(new Text("8=====)"));
  CreateRegistrationCanvas();
  CreateStartCanvas();

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

