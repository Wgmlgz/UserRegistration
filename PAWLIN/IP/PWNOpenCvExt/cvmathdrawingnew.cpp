// (c) Pawlin Technologies Ltd 2019
// File: cvmathdrawingnew.h, .cpp
// Purpose: file for opencv extensions code for mathematics and visualizations
// Author: P.V. Skribtsov
// ALL RIGHTS RESERVED
#include "stdafx.h"
#include "cvmathdrawingnew.h"
#include <PWNImageObj/ConcentrationEllipseEstimator.h>
#include <PWNProfiler/duration.h>
namespace pawlin {

	void pawlin::PolyModel1D::testPoly()
	{
		Dataset data;
		const float vel = 0.5f;
		const float acc = 0.02f;
		const float maxt = 1.0f;
		const int samples = 10;
		for (int i = 0; i < samples; i++) {
			float t = maxt*i / (samples - 1);
			FloatVector row;
			row.push_back(t);
			row.push_back(100.0f + vel*t + acc*t*t / 2.0f);
			data.push_back(row);
			printf("time %f, pos %f\n", t, row.data.back());
		}
		Profiler prof;
		prof.startSequence();
		pawlin::PolyModel1D model(data, 2);
		prof.markPoint("compute poly");
		prof.print();
		model.getCoefs().print();
		for (int i = 0; i < samples; i++) {
			float t = maxt*i / (samples - 1);
			printf("t = %f \t pos %f\t", t, model.compute(t));
			printf("vel %f\t", model.computeD(t));
			printf("acc %f\n", model.computeD2(t));
		}
	}
	//void stitch(const vector <cv::Mat> &imgs, const string &result_name = "result.jpg") {
	//	bool try_use_gpu = false;
	//	bool divide_images = false;
	//	cv::Stitcher::Mode mode = cv::Stitcher::SCANS;
	//	cv::Mat pano;
	//	cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(mode, try_use_gpu);
	//	cv::Stitcher::Status status = stitcher->stitch(imgs, pano);
	//	cv::imwrite(result_name, pano);
	//	printf("status %d\n", status);
	//}

//	void drawOvalBase(const OvalBase &oval, cv::Scalar color, int width, float scaleX, float scaleY, cv::Mat &img, bool drawAxis) {
//		cv::RotatedRect box;
//		
//		box.center = cv::Point2f(oval.x, oval.y);
//		box.angle = oval.angle + 90.0f; // need to add 90.0f if oval was normalized for correct display...
//#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 1
//										// do nothing
//#else
//		box.angle *= -1.0f;
//#endif
//		box.size = cv::Size2f(oval.sx*scaleX, oval.sy*scaleY);
//		cv::ellipse(img, box, color, width, cv::LINE_AA);
//		cv::Point c((int)oval.x, (int)oval.y);
//		cv::Point e1(point(OvalBase::Basis(oval).e1));
//		cv::Point e2(point(OvalBase::Basis(oval).e2));
//		if (drawAxis) {
//			cv::arrowedLine(img, c, c + e1, color);
//			cv::arrowedLine(img, c, c + e2, color);
//		}
//	}

	void CursorHandler::run(const std::string &winname, const cv::Mat & background)
	{
		cursor = background.cols / 2;
		cv::namedWindow(winname);
		cv::setMouseCallback(winname, onMouse, this);
		for (;;) {
			cv::Mat canvas = background.clone();
			int i = cursor;

			cv::line(canvas, cv::Point(i, 0), cv::Point(i, background.rows), CV_RGB(255, 255, 255), 1); // draw cursor
			cv::imshow(winname, canvas);

			int k = cv::waitKeyEx(10);

			if (k == OPENCV_RIGHT_ARROW_CODE) cursor = (cursor + 1) % background.cols;
			if (k == OPENCV_LEFT_ARROW_CODE) cursor = (cursor - 1) % background.cols;
			if (cursor < 0) cursor = 0;
			char key = 0; if((k&0xFF00)==0) key= k & 255;
			if (key <= 0) continue;
			if (key == 27) break;
			onColumn(cursor, Event::KEY, k);
		}
	}

	void CursorHandler::onMouse(int event, int x, int y, int flags, void * param)
	{
		if (event == cv::MouseEventTypes::EVENT_LBUTTONDOWN) {
			CursorHandler *_this = (CursorHandler *)param;
			_this->cursor = x;
			_this->onColumn(x, CursorHandler::Event::CLICK, flags);
		}
	}



	void CvInteractWindowBase::track_callback(int pos, void *params) {
		CvInteractWindowBase *_this = (CvInteractWindowBase *)params;
		_this->updateSliders();
	}

	void CvInteractWindowBase::mouse_callback(int event, int x, int y, int flags, void *params) {
		CvInteractWindowBase *_this = (CvInteractWindowBase *)params;
		_this->updateClick(x, y, event, flags);
	}

	void imshowWithResize(string windowName, const cv::Mat &img, int sizeMul, int cvinterpolation) {
		cv::Mat resized;
		cv::resize(img, resized, cv::Size(img.cols*sizeMul, img.rows*sizeMul), 0.0, 0.0, cvinterpolation);
		//cv::normalize(resized,resized,0.0,1.0,NORM_MINMAX);
		cv::imshow(windowName, resized);
	}

	void SelectionSolidROIWindow::show(cv::Mat &canvas) const {
		SelectionROIWindow::show(canvas);
		cv::putText(canvas, message, cv::Point(g_pt1.x + 10, g_pt1.y + 20), cv::FONT_HERSHEY_PLAIN, 1, framecolor, 1);
	}

	cv::Rect SelectionSolidROIWindow::getROI() const {
		return cv::Rect(g_pt1, size);
	}

	void MultiRectSelectionWindow::test()
	{
		cv::Mat temp(480, 640, CV_8UC3);
		temp = CV_RGB(100, 150, 250);
		vector <string> labels = { "GOOD","MID","BAD" };
		vector <cv::Scalar> colors = { CV_RGB(0,255,0), CV_RGB(200,150,0), CV_RGB(255,0,0) };
		MultiRectSelectionWindow obj("MultiRectSelectionWindow", temp, labels, colors);
		obj.run();
	}

}

