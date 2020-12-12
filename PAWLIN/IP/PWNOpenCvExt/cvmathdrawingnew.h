// (c) Pawlin Technologies Ltd 2019
// File: cvmathdrawingnew.h, .cpp
// Purpose: file for opencv extensions code for mathematics and visualizations
// Author: P.V. Skribtsov
// ALL RIGHTS RESERVED

#pragma once

// OpenCV
#include <opencv2/opencv.hpp>

#include <PWNGeometry/geometricobjects.h>
#include <PWNGeometry/convexpolyintersect.hpp>
#include <PWNImageObj/OvalBase.h>
#include <PWNGeneral/Dataset.h>
#include "cvdrawingcompat.h"

namespace pawlin {
	inline Point3Df ptconv(Point2Df p) { return Point3Df(p.x, p.y, 0); }
	inline Point2Df ptconv(cv::Point2f p) { return Point2Df(p.x, p.y); }
	inline Point2Df ptconv(cv::Point p) { return Point2Df((float)p.x, (float)p.y); }
	inline cv::Point2f ptconv2f(Point2Df p) { return cv::Point2f(p.x, p.y); }
	inline cv::Point ptconv2i(const Point2Df &p) { return cv::Point(float2int(p.x), float2int(p.y)); }
	inline cv::Point convert(const vector<cv::Point2f> &vec, vector<cv::Point> &out) {
		out.resize(vec.size());
		MinMaxAvg statx, staty;
		FOR_ALL(vec, i) {
			out[i] = ptconv2i(ptconv(vec[i]));
			statx.take((float)out[i].x);
			staty.take((float)out[i].y);
		}
		return ptconv2i(Point2Df(statx.getAvg(), staty.getAvg()));
	}
	inline cv::Point convert(const vector<cv::Point2f> &vec, const Box2Df &box, const cv::Size &sz, vector<cv::Point> &out) {
		out.resize(vec.size());
		MinMaxAvg statx, staty;
		FOR_ALL(vec, i) {
			out[i] = ptconv2i(box.box2img(sz.width, sz.height, ptconv(vec[i])));
			statx.take((float)out[i].x);
			staty.take((float)out[i].y);
		}
		return ptconv2i(Point2Df(statx.getAvg(), staty.getAvg()));
	}

	inline void convert(vector<vector<cv::Point2f>> &vec, const Box2Df &box, const cv::Size &sz, vector<vector<cv::Point>> &out) {
		out.resize(vec.size());
		FOR_ALL(vec, i) convert(vec[i], box, sz, out[i]);
	}

	//void drawOvalBase(const OvalBase &oval, cv::Scalar color, int width, float scaleX, float scaleY, cv::Mat &img, bool drawAxis = true);

#define OPENCV_RIGHT_ARROW_CODE 0x270000
#define OPENCV_LEFT_ARROW_CODE 0x250000
#define OPENCV_RIGHT_ARROW_CODE_TERMINAL_LINUX 0xFF53
#define OPENCV_LEFT_ARROW_CODE_TERMINAL_LINUX 0xFF51
#define OPENCV_LEFT_ARROW_CODE_XMING 0x10FF51
#define OPENCV_RIGHT_ARROW_CODE_XMING 0x10FF53
#define OPENCV_UP_ARROW_CODE_XMING 0x10FF52
#define OPENCV_DOWN_ARROW_CODE_XMING 0x10FF54

#define OPENCV_SPECIAL_KEY(k) ((k & 0x20FF00) != 0)

	class PolyModel1D {
		FloatVector coefs;
	public:
		static void testPoly();
		const FloatVector &getCoefs() const { return coefs; }
		PolyModel1D() {}
		PolyModel1D(const Dataset &data, size_t N) : coefs(N + 1, 0) {
			cv::Mat A((int)data.size(), (int)N + 1, CV_32F);
			cv::Mat B((int)data.size(), 1, CV_32F);
			FOR_ALL(data.rows, i) {
				for (size_t n = 0; n <= N; n++) A.at<float>((int)i, (int)n) = powf(data.rows[i].data[0], (float)(N - n));
				B.at<float>((int)i) = data.rows[i].data[1];
			}
			cv::Mat res;
			cv::solve(A, B, res, cv::DECOMP_SVD);
			memcpy(&coefs.data[0], res.data, sizeof(float)*coefs.size());
			//coefs.print(10);
		}

		float compute(float x) const {
			float m = 1.0f;
			float sum = 0;
			FOR_ALL(coefs, i) {
				sum += m*coefs[coefs.size() - 1 - i];
				m *= x;
			}
			return sum;
		}

		float computeD(float x) const {
			if(coefs.size() == 4) return coefs[2] + 2 * coefs[1] * x + 3 * coefs[0] * x*x;
			if (coefs.size() == 3) return coefs[1] + 2 * coefs[0] * x ;
			if (coefs.size() == 2) return coefs[0];
			throw std::runtime_error("not valid call for models with higher degree > 4");
			return 0;
		}

		float computeD2(float x) const {
			if (coefs.size() == 4) return 2 * coefs[1] + 3 * 2 * coefs[0] * x;
			if (coefs.size() == 3) return 2 * coefs[0];
			if (coefs.size() == 2) return 0;

			throw std::runtime_error("not valid call for models with higher degree > 4");
		}
		struct Regularizer {
			vector <float> sampleWeights; // if not specified for an index, assuming it's one
			Dataset pinPoints; // additional points added in the end of the est - 3 columns [x][y][weight]
			float noiseratio;
			Regularizer(float stress_first, float regx, float regy, float regw) : noiseratio(0) {
				sampleWeights.push_back(stress_first);
				FloatVector row;
				row.push_back(regx);
				row.push_back(regy);
				row.push_back(regw);
				pinPoints.push_back(row);
			}
			Regularizer(float stress_first) : noiseratio(0) {
				sampleWeights.push_back(stress_first);
			}

			Regularizer() : noiseratio(0) {}
		};
		PolyModel1D(const Dataset &data, size_t xColumn, size_t yColumn, size_t N, const Regularizer &reg, Dataset &debug) : coefs(N + 1, 0) {
			size_t R = reg.pinPoints.size(); // number of additional points
			cv::Mat A((int)(data.size() + R), (int)N + 1, CV_32F);
			cv::Mat B((int)(data.size() + R), 1, CV_32F);
			cv::Mat Aclean((int)data.size(), (int)N + 1, CV_32F); // without gain for debug checks
			FOR_ALL(data.rows, i) {
				float v = data.rows[i].data[xColumn];
				float noise = 2.0f*(rnd() - 0.5f)*v*reg.noiseratio;
				float gain = (i < reg.sampleWeights.size()) ? reg.sampleWeights[i] : 1.0f;
				for (size_t n = 0; n <= N; n++) {
					float coef = powf(v + noise, float(N - n));
					A.at<float>((int)i, (int)n) = gain*coef;
					Aclean.at<float>((int)i, (int)n) = coef;
				}
				B.at<float>((int)i) = data.rows[i].data[yColumn] * gain;
			}
			// set intercept knowlege
			const int lastrow = (int)data.size();
			for (size_t i = 0; i < R; i++) {
				const float gain = reg.pinPoints[i][2]; // importance of intercept constraint
				for (size_t n = 0; n <= N; n++) {
					float v = reg.pinPoints[i][0];
					A.at<float>(lastrow + (int)i, (int)n) = gain*powf(v, float(N - n));
				}
				B.at<float>(lastrow + (int)i) = reg.pinPoints[i][1];
			}
			cv::Mat res;
			cv::solve(A, B, res, cv::DECOMP_SVD);
			memcpy(&coefs.data[0], res.data, sizeof(float)*coefs.size());
			cv::Mat trend = Aclean*res;
			FOR_ALL(data.rows, i) {
				FloatVector row;
				row.data.push_back(trend.at<float>((int)i)); // model output after training
				row.data.push_back(data.rows[i].data[yColumn]); // target values
				debug.rows.push_back(row);
			}
			//coefs.print(10);
		}


	};

	class CursorHandler {
		int cursor;
	public:
		void run(const std::string &winname, const cv::Mat &background);
		enum Event {POS, CLICK, KEY};
		static void onMouse(int event, int x, int y, int flags, void *param);
		virtual void onColumn(int x, Event event, int param) = 0;
	};



	class CvInteractWindowBase {
	protected:
		string winname;
		cv::Size canv_size;
		int toggles[256];
		struct Selection {
			int lastSelected;
			Selection() : lastSelected(-1) {} // by default nothing is selected
		};
		Selection selection;
		int canv_type;

	public:
		CvInteractWindowBase(const std::string &win, cv::Size canv_size, int canv_type) :
			winname(win),
			canv_size(canv_size),
			canv_type(canv_type)
		{
			memset(toggles, 0, sizeof(toggles));
			cv::namedWindow(winname.c_str(),cv::WINDOW_AUTOSIZE);
			cv::moveWindow(winname.c_str(), 50, 50);
			cv::setMouseCallback(winname.c_str(), mouse_callback, this);
		}

		string name() const {
			return winname;
		}

		virtual void show(cv::Mat &canvas) const {
		}

		static void track_callback(int pos, void *params);
		static void mouse_callback(int event, int x, int y, int flags, void *params);
		virtual void updateSliders() {};
/*
enum  	cv::MouseEventFlags {
cv::EVENT_FLAG_LBUTTON = 1,
cv::EVENT_FLAG_RBUTTON = 2,
cv::EVENT_FLAG_MBUTTON = 4,
cv::EVENT_FLAG_CTRLKEY = 8,
cv::EVENT_FLAG_SHIFTKEY = 16,
cv::EVENT_FLAG_ALTKEY = 32
}
Mouse Event Flags see cv::MouseCallback. More...

enum  	cv::MouseEventTypes {
cv::EVENT_MOUSEMOVE = 0,
cv::EVENT_LBUTTONDOWN = 1,
cv::EVENT_RBUTTONDOWN = 2,
cv::EVENT_MBUTTONDOWN = 3,
cv::EVENT_LBUTTONUP = 4,
cv::EVENT_RBUTTONUP = 5,
cv::EVENT_MBUTTONUP = 6,
cv::EVENT_LBUTTONDBLCLK = 7,
cv::EVENT_RBUTTONDBLCLK = 8,
cv::EVENT_MBUTTONDBLCLK = 9,
cv::EVENT_MOUSEWHEEL = 10,
cv::EVENT_MOUSEHWHEEL = 11
}*/
		virtual void updateClick(int x, int y, int event, int mouseflags) {};
		virtual ~CvInteractWindowBase() {
			printf("~CvInteractWindowBase()..");
			cv::destroyWindow(winname.c_str());
			printf("done\n");
		}
	};


	// class for ROI selection by mouse by A. Dolgopolov
	// usage example:
	//
	//	SelectionROIWindow select_roi("SELECT PSF_ROI", src_img);
	//	select_roi.run();
	//	deblur_image_manual_psf(src_img, argv[1], select_roi.getROI());

	class SelectionROIWindow :public CvInteractWindowBase {
	protected:
		bool roi_capture; // roi capture process started
		bool changed_roi; // roi has been changed
		cv::Point g_pt1, g_pt2;
		cv::Mat m_src_im;
		cv::Scalar framecolor;
	public:
		SelectionROIWindow(const std::string &win, const cv::Mat& src_im, cv::Scalar framecolor = CV_RGB(0, 255, 0)) 
			: CvInteractWindowBase(win, src_im.size(), src_im.type()), framecolor(framecolor)
		{
			m_src_im = src_im;
			
			roi_capture = false;
			changed_roi = false;

			g_pt1.x = 0;
			g_pt1.y = 0;

			g_pt2.x = m_src_im.cols - 1;
			g_pt2.y = m_src_im.rows - 1;
		}
		void setBackground(const cv::Mat &mat) { m_src_im = mat.clone(); }
		void setColor(cv::Scalar color) { framecolor = color; }
		virtual void updateSliders() {};

		// implements drag-n-drop behavoir
		virtual void updateClick(int x, int y, int event, int mouseflags) {

			switch (event)
			{
			case cv::EVENT_MOUSEMOVE: {
				if (mouseflags == cv::EVENT_FLAG_LBUTTON) {
					g_pt2.x = x;
					g_pt2.y = y;
				}
				break;
			}
			case cv::EVENT_LBUTTONDOWN:
			{

				g_pt1.x = x;
				g_pt1.y = y;
				g_pt2.x = x;
				g_pt2.y = y;
				roi_capture = true;
				break;
			}
			case cv::EVENT_LBUTTONUP:
			{
				g_pt2.x = x;
				g_pt2.y = y;
				roi_capture = false;
				changed_roi = true;
				done();
				break;
			}
			default:;
			}

		};
		virtual void done() {}
		virtual void show(cv::Mat &canvas) const override {
			m_src_im.copyTo(canvas);
			cv::rectangle(canvas, g_pt1, g_pt2, framecolor, 2);
		}

		virtual cv::Rect getROI() const {
			cv::Point pt1, pt2;
			pt1.x = std::min<int>(g_pt1.x, g_pt2.x);
			pt1.y = std::min<int>(g_pt1.y, g_pt2.y);
			pt2.x = std::max<int>(g_pt1.x, g_pt2.x);
			pt2.y = std::max<int>(g_pt1.y, g_pt2.y);
			cv::Rect selected_roi;
			selected_roi.x = pt1.x;
			selected_roi.y = pt1.y;
			selected_roi.width = pt2.x - pt1.x + 1;
			selected_roi.height = pt2.y - pt1.y + 1;
			return selected_roi;
		}

		int run(char additional_exit_key = 'e') {
			bool done = false;
			printf("Press 'Esc' key for exit from selection roi mode.\n");
			cv::Mat canvas;
			changed_roi = false;
			int k = 0;
			while (!done) {
				show(canvas);
				cv::imshow(winname, canvas);
				k = cv::waitKeyEx(30);
				//printf("%X\n", k);
				if (!OPENCV_SPECIAL_KEY(k)) k = k & 0xFF;

#ifdef WINDOWS
				//проверка что крест не был нажат
				HWND Hide2 = FindWindowA(NULL, (LPCSTR)winname.c_str());
				if (key(k) || k == additional_exit_key || Hide2 == NULL) done = true;
#else
				if (key(k) || k == additional_exit_key) done = true;
#endif
			}
			return k;
		}

		virtual ~SelectionROIWindow() {
		}

		virtual bool key(int k) { // return true if done
			if (k == 27) return true;
			return false;
		}

	};

	class MultiRectSelectionWindow : public SelectionROIWindow {
	public:
		struct LabelRect {
			cv::Rect rect;
			int label;
		};
	protected:

		vector < LabelRect> array;
		vector <string> labels;
		vector <cv::Scalar> colors;
		int current_label;
	public:
		MultiRectSelectionWindow(
			const string &win, 
			const cv::Mat &src_im, 
			const vector <string> &labels, 
			const vector <cv::Scalar> &colors
		) :
			SelectionROIWindow(win,src_im,CV_RGB(128,128,128)), 
			labels(labels),
			colors(colors),
			current_label(0)
		{
			g_pt1 = g_pt2 = cv::Point(0, 0);
			framecolor = colors[current_label];
			if (labels.empty() || labels.size() != colors.size()) 
				throw std::runtime_error("MultiRectSelectionWindow - empty labels array now allowed and colors & labels must have same size!");
			FOR_ALL_IF(labels, l, labels[l].empty()) throw std::runtime_error("MultiRectSelectionWindow - zero length labels not allowed!");
		}
		void setArray(const vector < LabelRect> &_array) { array = _array; }
		const vector < LabelRect> & getArray() const { return array; }
		static void test();
		void update_label() {
			if (current_label < 0) current_label = 0;
			if (current_label >= (int)labels.size()) current_label = (int)labels.size() - 1;
			framecolor = colors[current_label];
		}
		virtual void updateClick(int x, int y, int event, int mouseflags) override {
			if (event == cv::EVENT_MOUSEWHEEL) {
				if (mouseflags > 0) current_label--;
				else current_label++;
				update_label();
				return;
			}
			if(event==cv::EVENT_RBUTTONUP) {
				// remove last rectangle
				if (array.empty()) return;
				array.resize(array.size() - 1);
				if (array.empty()) {
					g_pt1 = g_pt2 = cv::Point(0, 0);
					return;
				}
				g_pt1 = array.back().rect.tl();
				g_pt2 = array.back().rect.br();
			}
			else SelectionROIWindow::updateClick(x, y, event, mouseflags);
		}
		virtual void done() override {
			array.push_back(LabelRect({ getROI(),current_label }));
			g_pt1 = g_pt2 = cv::Point(0, 0);
		}
		virtual void show(cv::Mat &canvas) const override {
			SelectionROIWindow::show(canvas);
			FOR_ALL(array, i) {
				cv::rectangle(canvas, array[i].rect, colors[array[i].label], 2);
				cv::putText(canvas, labels[array[i].label], array[i].rect.tl(), 
					cv::FONT_HERSHEY_PLAIN,
					1, CV_RGB(0, 0, 255)
				);
			}
			// show current label
			string msg = "[" + labels[current_label] + "]";
			cv::putText(canvas, msg, cv::Point(4, 16),
				cv::FONT_HERSHEY_PLAIN,
				1, colors[current_label]// CV_RGB(0, 0, 255)
			);
		}
		virtual bool key(int k) override { // return true if done
			if (k == -1) return false;
			if (k == 27) return true;
			if (k == OPENCV_RIGHT_ARROW_CODE || k == OPENCV_RIGHT_ARROW_CODE_TERMINAL_LINUX || k == OPENCV_RIGHT_ARROW_CODE_XMING) 
				current_label++;
			if (k == OPENCV_LEFT_ARROW_CODE || k == OPENCV_LEFT_ARROW_CODE_TERMINAL_LINUX || k == OPENCV_LEFT_ARROW_CODE_XMING) 
				current_label--;
			int key = 0;  if ((k & 0xFF00) == 0) key = k & 0xFF;
			FOR_ALL(labels, l) {
				if (std::tolower(labels[l][0]) == key || labels[l][0]==key) current_label = (int)l;
			}
			update_label();
			return false;
		}

	};

	void imshowWithResize(string windowName, const cv::Mat &img, int sizeMul, int cvinterpolation = cv::INTER_LINEAR);
	//#endif

	// differs from base class by fixed width & height
	class SelectionSolidROIWindow :public SelectionROIWindow {
		cv::Size size;
		vector <int> exitKeys;
		string message;
	public:
		void setSize(cv::Size sz) { size = sz; }
		void setMessage(string msg) { message = msg; }
		void setExitKeys(const vector<int> &keys) { exitKeys = keys; }
		void addExitKey(int k) { exitKeys.push_back(k); }
		virtual bool key(int k) override {
			return std::find(exitKeys.begin(), exitKeys.end(), k) != exitKeys.end();
		}
		virtual void show(cv::Mat &canvas) const override;
		virtual void updateSliders() {};
		SelectionSolidROIWindow(const std::string &win, const cv::Mat &background, cv::Size sz, const string &message,
			cv::Scalar framecolor = cv::Scalar(200, 20, 140))
			: size(sz), message(message),
			SelectionROIWindow(win, background,framecolor)
		{
			addExitKey(27); // ESC
		}
		virtual cv::Rect getROI() const override;

		// implement "stick-to-the-mouse" behavoir
		virtual void updateClick(int x, int y, int event, int mouseflags) {

			switch (event)
			{
			case cv::EVENT_MOUSEMOVE: {
				if (x+size.width <= m_src_im.cols && y+size.height <= m_src_im.rows) {
					g_pt1.x = x;
					g_pt1.y = y;
					g_pt2.x = x + size.width;
					g_pt2.y = y + size.height;
				}
				break;
			}

			case cv::EVENT_LBUTTONDOWN:
			{
				g_pt1.x = x;
				g_pt1.y = y;
				roi_capture = true;
				break;
			}
			default:;
			}

		};
	};
	using cv::Rect2f;
	using cv::Point2f;
	class Subdiv2Df : public cv::Subdiv2D {
	public:
		Subdiv2Df::Subdiv2Df(Rect2f rect)
		{
			validGeometry = false;
			freeQEdge = 0;
			freePoint = 0;
			recentEdge = 0;

			initDelaunay(rect);
		}
		void insert(const vector<Point2Df> &points) {
			FOR_ALL(points, i) cv::Subdiv2D::insert(ptconv2f(points[i]));
		}
		void insert(const vector<Point2f> &points) {
			cv::Subdiv2D::insert(points);
		}

		void initDelaunay(Rect2f rect)
		{
			//CV_INSTRUMENT_REGION();

			float big_coord = 3.f * MAX(rect.width, rect.height);
			float rx = rect.x;
			float ry = rect.y;

			vtx.clear();
			qedges.clear();

			recentEdge = 0;
			validGeometry = false;

			topLeft = Point2f(rx, ry);
			bottomRight = Point2f(rx + rect.width, ry + rect.height);

			Point2f ppA(rx + big_coord, ry);
			Point2f ppB(rx, ry + big_coord);
			Point2f ppC(rx - big_coord, ry - big_coord);

			vtx.push_back(Vertex());
			qedges.push_back(QuadEdge());

			freeQEdge = 0;
			freePoint = 0;

			int pA = newPoint(ppA, false);
			int pB = newPoint(ppB, false);
			int pC = newPoint(ppC, false);

			int edge_AB = newEdge();
			int edge_BC = newEdge();
			int edge_CA = newEdge();

			setEdgePoints(edge_AB, pA, pB);
			setEdgePoints(edge_BC, pB, pC);
			setEdgePoints(edge_CA, pC, pA);

			splice(edge_AB, symEdge(edge_CA));
			splice(edge_BC, symEdge(edge_AB));
			splice(edge_CA, symEdge(edge_BC));

			recentEdge = edge_AB;
		}

	};

	class VoronoiInfo {
		Subdiv2Df subdiv;
		vector<Polygon > facets_clipped;
		vector<Point2f> centers;
		vector<vector<size_t> > neighbours;
	public:
		VoronoiInfo(const cv::Size &canvsize, const vector<cv::Point2f> &points ) :
			subdiv(cv::Rect(cv::Point(0, 0), canvsize))
		{
			subdiv.insert(points);
			vector<vector<Point2f> > facets;
			subdiv.getVoronoiFacetList(vector<int>(), facets, centers);
			// build connections
			std::map<std::pair<int, int>, std::set<size_t> > connections;
			facets_clipped.resize(facets.size());
			pawlin::Polygon rectpoly(canvsize.width, canvsize.height);
			FOR_ALL(facets, i) {
				// convert facets to integer & cross with screen rectangle
				pawlin::Polygon ipoly;
				convert(facets[i], ipoly);
				pawlin::intersectPolygon(rectpoly, ipoly, facets_clipped[i]);

				FOR_ALL(facets_clipped[i], j) {
					cv::Point p = facets_clipped[i][j];
					connections[std::make_pair(p.x, p.y)].insert(i);
				}
			}
			neighbours.resize(centers.size());
			for (auto &points : connections) {
				for (auto &links_i : points.second) {
					for (auto &links_j : points.second) {
						if (links_i != links_j) neighbours[links_i].push_back(links_j);
					}
				}
			}
		}
		void draw_links(cv::Mat &canvas, cv::Scalar color = 0, int thickness = 1) const {
			FOR_ALL(facets_clipped, i) {
				FOR_ALL(neighbours[i], n) {
					size_t index = neighbours[i][n];
					cv::line(canvas, centers[i], centers[index], CV_RGB(80, 80, 80), 1, cv::LINE_AA);
				}
			}
		}
		void draw_grid(cv::Mat &canvas, cv::Scalar color = 0, int thickness = 1) const {
			FOR_ALL(facets_clipped, i) {
				polylines(canvas, facets_clipped[i], true, color, thickness, cv::LINE_AA, 0);

			}
		}
		void drawGridIndex(cv::Mat &index32S) const {
			FOR_ALL(facets_clipped, i) {
				cv::fillConvexPoly(index32S, facets_clipped[i], cv::Scalar((double)i));
			}
		}

	};


	class TouchedObjectsSeparator {
		double distsq(const cv::Point &a, const cv::Point &b) const {
			auto diff = a - b;
			return diff.ddot(diff);
		}
		double loop(const vector<double> &dist, int index) const {
			if (index < 0) return loop(dist, index + (int)dist.size());
			if (index >= (int)dist.size()) return loop(dist, index - (int)dist.size());
			return dist[index];
		}
		void findMinimums(const vector<double> &_dist, vector<int> &minimums) const {
			enum State { INIT, NONE, FALL, RISE, PLATO };
			State state = INIT;
			int counterFall = 0;
			int counterRise = 0;
			int counterPlato = 0;
			const int maxPlato = 2;
			const int checkDeep = 10;
			int extrPos = 0;
			double lastDist = 0;
			vector<double> dist(_dist.size()); // smooth input array
			for (int i = 0; i < (int)dist.size(); i++) {
				double avg = loop(_dist, i - 1);
				avg += loop(_dist, i);
				avg += loop(_dist, i + 1);
				dist[i] = avg / 3.0;
			}
			for (int i = -checkDeep; i < (int)dist.size() + checkDeep; i++) {
				double v = loop(dist, i);
				switch (state) {
				case INIT:
					state = NONE;
					break;

				case NONE:
					if (v < lastDist) {
						state = FALL;
						counterFall = 1;
					}
					break;

				case FALL:
					if (v < lastDist) {
						counterFall++;
						break;
					}
					if (counterFall < checkDeep) {
						state = NONE;
						break;
					}
					extrPos = i;
					if (v == lastDist) {
						state = PLATO;
						counterPlato = 1;
						break;
					}
					if (v > lastDist) {
						state = RISE;
						counterRise = 1;
						break;
					}
					break;

				case PLATO:
					if (v == lastDist) {
						counterPlato++;
						if (counterPlato > maxPlato) state = NONE;
						break;
					}
					if (v > lastDist) {
						state = RISE;
						counterRise = 1;
						break;
					}
					break;

				case RISE:
					if (v > lastDist) {
						counterRise++;
						if (counterRise >= checkDeep) {
							//printf("FALL %d, PLATO %d, RISE %d Pos %d\n",
							//	counterFall,
							//	counterPlato,
							//	counterRise, extrPos);
							if (extrPos < 0) extrPos += (int)dist.size();
							if (extrPos >= (int)dist.size()) extrPos -= (int)dist.size();
							minimums.push_back(extrPos);
							state = NONE;
						}
						break;
					}
					state = NONE;
					break;
				}
				lastDist = v;
			}
		}
		void breakContour(const vector<cv::Point> &contour, int weakThreshold, cv::Mat &img) const {
			const double weakSq = weakThreshold*double(weakThreshold);
			std::vector<std::pair<int, int>> brokers;
			//#define DEBUGMARKER
			FOR_ALL(contour, i) {
#ifdef DEBUGMARKER
				cv::Mat canvas = img.clone();
				cv::circle(canvas, contour[i], 2, cv::Scalar(150, 150, 150), -1, cv::LINE_AA);
				cv::imshow("cursor", canvas);
				if (i < 506) continue;
				printf("i=%zu\n", i);
				int key = cv::waitKey(0);
				if (key == ' ') continue;
#endif
				vector<double> dist(contour.size());
				FOR_ALL(contour, j) { // this will be 'double' kill but will simplify logic below of finding minimums
					dist[j] = distsq(contour[i], contour[j]);
				}
				vector<int> minimums;
				findMinimums(dist, minimums);
#ifdef DEBUGMARKER
				FOR_ALL(minimums, m) {
					int index = minimums[m];
					int idiff = std::abs((int)(i - index)) % (int)contour.size();
					if (idiff <= 2) continue;
					cv::circle(canvas, contour[index], 2, cv::Scalar(200, 200, 200), -1, cv::LINE_AA);
					printf("distance %f\n", dist[index]);
				}
				cv::imshow("cursor", canvas);
				cv::waitKey(0);
#endif
				FOR_ALL(minimums, m) {
					int index = minimums[m];
					int idiff = std::abs((int)(i - index)) % (int)contour.size();
					if (idiff <= 2) continue;
					if (dist[index] < weakSq && index != i) {
						// check that line passes only along object values
						cv::LineIterator it(img, contour[i], contour[index], 4);
						if (it.count <= 1) continue;
						bool wrong = false;
						for (int i = 0; i < it.count; i++, ++it) {
							unsigned char v = *(unsigned char *)*it;
							if (v == 0) {
								wrong = true;
								break;
							}
						}
						if (wrong) continue;
						// break here
						//printf("break %d-%d\n", (int) i, index);
						brokers.push_back(std::make_pair((int)i, index));

						//cv::line(img, contour[i], contour[index], cv::Scalar(100, 100, 100), 1, cv::LINE_8); // if past point is not included, it's okay we will fill it with zero by 'double' kill as it will be starting point for second case
						//cv::circle(img, contour[index], 2, cv::Scalar(150, 150, 150), -1, cv::LINE_AA);
					}
				}
			}
			const double toleranceSq = 3 * 3;
			FOR_ALL(brokers, i) {
				cv::Point p1s = contour[brokers[i].first];
				cv::Point p1e = contour[brokers[i].second];
				for (size_t j = i + 1; j < brokers.size(); j++) {
					cv::Point p2s = contour[brokers[j].first];
					cv::Point p2e = contour[brokers[j].second];
					if (distsq(p1s, p2e)<toleranceSq && distsq(p1e, p2s)<toleranceSq)
						cv::line(img,
							//p1s,p1e,
							0.5*(p1s + p2e), 0.5*(p1e + p2s),
							cv::Scalar(0, 0, 0), 1, cv::LINE_4); // if past point is not included, it's okay we will fill it with zero by 'double' kill as it will be starting point for second case

				}
			}

			//cv::imshow("img", img);
			//cv::waitKey(0);
		}
		public:
		void trySeparate(const cv::Mat &img, int weakThreshold, vector<cv::Mat> &out, const double minArea = 10) const {
			// build contours
			vector<vector<cv::Point> > contours0, contours1;
			vector<cv::Vec4i> hierarchy0, hierarchy1;
			findContours(img, contours0, hierarchy0, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);
			if (contours0.size() == 0)
				return;
			cv::Mat copy = img.clone();
			for (int idx = 0; idx >= 0; idx = hierarchy0[idx][0])
			{
				const vector<cv::Point>& c = contours0[idx];
				double area = fabs(contourArea(cv::Mat(c)));
				if (area < minArea) continue;
				breakContour(c, weakThreshold, copy);
			}
			findContours(copy, contours1, hierarchy1, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
			for (int idx = 0; idx >= 0; idx = hierarchy1[idx][0])
			{
				const vector<cv::Point>& c = contours1[idx];
				double area = fabs(contourArea(cv::Mat(c)));
				if (area < minArea) continue;
				cv::Mat single = cv::Mat::zeros(img.size(), img.type());
				cv::drawContours(single, contours1, idx, CV_RGB(255, 255, 255), cv::FILLED, cv::LINE_4);
				out.push_back(single);
			}
		}
	};

	class ThresholdSelector : public pawlin::CvInteractWindowBase {
		int mint, maxt;
		const int sliderDashes = 512;
		double minv, maxv;
		cv::Mat image;
		float scale;
	protected:
		cv::Mat mask;
	public:
		ThresholdSelector(const string &winname, const cv::Mat &image, float scale = 1.0f) :
			pawlin::CvInteractWindowBase(winname, image.size(), image.type()),
			image(image.clone()),
			scale(scale)
		{
			mint = 0, maxt = sliderDashes;
			cv::minMaxLoc(image, &minv, &maxv);
			pawlin::debugImg(winname, image, scale, 1, true);
			cv::createTrackbar("mincut", winname, &mint, sliderDashes);
			cv::createTrackbar("maxcut", winname, &maxt, sliderDashes);
		}
		double cut(int v) const {
			return minv + double(maxv - minv)*v / (double)sliderDashes;
		}
		cv::Mat generate() const {
			cv::Mat res;
			cv::threshold(image, res, cut(maxt), 0, cv::THRESH_TRUNC);
			cv::threshold(res - cut(mint), res, 0, 0, cv::THRESH_TOZERO);
			res += cut(mint);
			return res;
		}
		double run(const cv::Mat &_back) {
			cv::Mat back;
			if (_back.channels() == 3) back = _back;
			if (_back.channels() == 1) cv::cvtColor(_back, back, cv::COLOR_GRAY2BGR);
			while (true) {
				cv::Mat imagemod = generate();
				cv::normalize(imagemod, imagemod, 1, 0, cv::NORM_MINMAX);
				imagemod.convertTo(imagemod, CV_8U, 255);
				cv::cvtColor(imagemod, imagemod, cv::COLOR_GRAY2BGR);
				cv::Mat blend = back.empty() ? imagemod : (imagemod + back)*0.5;
				compute();
				draw(blend);
				int key = pawlin::debugImg(winname, blend, scale, 30, true);
				if (key == 27) return thresh();
			}
		}
		double thresh() const {
			return 0.5*cut(mint) + 0.5*cut(maxt);
		}
		virtual void compute() {
			mask = image > thresh();
		}
		virtual void draw(cv::Mat &canvas) const {
			string text = 
				"min:" + float2str(cut(mint), 1) + "  max:" + float2str(cut(maxt), 1) + 
				" thresh:" + float2str(thresh(), 1);
			cv::putText(
				canvas, 
				text, 
				cv::Point(30, 30), 
				cv::FONT_HERSHEY_PLAIN, 1.0 / scale, CV_RGB(255, 0, 0),int(1+1.0/scale+0.5));
			pawlin::debugImg("mask", mask, scale, 1);
		}
	};

};