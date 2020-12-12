// (c) Pawlin Technologies Ltd 2019
// File: cvmathdrawingnew.h, .cpp
// Purpose: file for opencv extensions code for mathematics and visualizations
// Author: P.V. Skribtsov
// ALL RIGHTS RESERVED

#pragma once

// OpenCV
#include <opencv2/opencv.hpp>

#include <PWNGeometry/geometricobjects.h>
#include <PWNImageObj/OvalBase.h>
#include <PWNGeneral/Dataset.h>

namespace pawlin {
	inline cv::Point median2d(const vector<cv::Point> &in) {
		vector<int> array1(in.size());
		vector<int> array2(in.size());
		FOR_ALL(in, i) array1[i] = in[i].x, array2[i] = in[i].y;
		sort(array1.begin(), array1.end());
		sort(array2.begin(), array2.end());
		return cv::Point(
			array1[array1.size() / 2],
			array2[array2.size() / 2]);
	}

	inline int debugImg(const string &name, const cv::Mat &_img, double scale = 0.5, int delay = 0, bool normalize = true) {
		//return;
		cv::Mat img;
		cv::resize(_img, img, cv::Size(), scale, scale, scale < 1.0 ? cv::INTER_AREA : 1);
		if (normalize) cv::normalize(img, img, (_img.type() == CV_8U || _img.type() == CV_8UC3) ? 255 : 1, 0, cv::NORM_MINMAX);
//		else img = _img.clone();
		cv::imshow(name, img);
		if(delay>=0) return cv::waitKeyEx(delay);
		return -1;
	}

	inline void drawGrid(cv::Mat &lap1, int stepx, int stepy, cv::Scalar color, int thickness = 1, int lineType = 8) {
		for (int i = 0; i < lap1.rows; i += stepy) {
			cv::line(lap1, cv::Point(0, i), cv::Point(lap1.cols, i), color,thickness,lineType );
		}
		for (int i = 0; i < lap1.cols; i += stepx) {
			cv::line(lap1, cv::Point(i, 0), cv::Point(i, lap1.rows), color, thickness, lineType);
		}
	}

	inline void drawOvalBase(cv::Mat &canvas, const OvalBase &oval, cv::Scalar color, bool useOvalImageCoords = true, int thickness = 1, int linetype = 8) {
		cv::RotatedRect rrect(cv::Point2f(useOvalImageCoords ? oval.x_image : oval.x, useOvalImageCoords ? oval.y_image : oval.y),
			cv::Size2f(oval.sx, oval.sy), 90 - oval.angle);
		cv::ellipse(canvas, rrect, color, thickness,linetype);
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

	inline cv::Point point(const Point2Df &pt) 
	{
		return cv::Point(float2int(pt.x), float2int(pt.y));
	}

	class ImageCache {
		string basename;
		vector<int> params;
		enum Mode { UNKNOWN, LOAD, SAVE };
		Mode getMode() const {
			if (ifs.is_open()) return LOAD;
			if (ofs.is_open()) return SAVE;
			return UNKNOWN;
		}
		std::ifstream ifs;
		std::ofstream ofs;
		//! Read cv::Mat from binary
		cv::Mat readMat;
		void readMatBinary(std::ifstream& ifs, cv::Mat& in_mat)
		{
			int rows, cols, type;
			ifs.read((char*)(&rows), sizeof(int));
			if (rows == 0) {
				return;
			}
			ifs.read((char*)(&cols), sizeof(int));
			ifs.read((char*)(&type), sizeof(int));

			if (readMat.empty()) {
				readMat.create(rows, cols, type);
			}
			ifs.read((char*)(readMat.data), readMat.elemSize() * readMat.total());
			in_mat = readMat;
		}
		void writeMatBinary(std::ofstream& ofs, const cv::Mat& out_mat)
		{
			if (out_mat.empty()) {
				int s = 0;
				ofs.write((const char*)(&s), sizeof(int));
				return;
			}
			int type = out_mat.type();
			ofs.write((const char*)(&out_mat.rows), sizeof(int));
			ofs.write((const char*)(&out_mat.cols), sizeof(int));
			ofs.write((const char*)(&type), sizeof(int));
			ofs.write((const char*)(out_mat.data), out_mat.elemSize() * out_mat.total());
		}
	public:
		string getFilename() const {
			string name = basename;
			FOR_ALL(params, i) name += "_" + int2str(params[i]);
			name += ".bin";
			return name;
		}
		ImageCache(const string &basename, const vector<int> &params) : basename(basename),params(params) {}
		~ImageCache() {
			if (ifs.is_open()) ifs.close();
			if (ofs.is_open()) ofs.close();
		}
		string tmpName() const { return getFilename() + "_tmp"; }
		void complete() {
			if (getMode() == SAVE) {
				ofs.close();
#ifdef LINUX
				string util = "rn";
#else
				string util = "rename";
#endif
				string cmd = util + " " + tmpName() + " " + getFilename();
				system(cmd.c_str());
			}
		}
		bool load(const cv::Size &sz, cv::Mat &out) {
			if (ofs.is_open()) return false;
			if (ifs.is_open()) {
				readMatBinary(ifs, out);
				return true;
			}
			else {
				ifs.open(getFilename(), std::ios::binary);
				if (!ifs.is_open()) {
					printf("can't open cache file %s for reading, going to save mode", getFilename().c_str());
					return false;
				}
				readMatBinary(ifs, out);
				if (out.size() == sz) return true;

				// otherwise don't match - switch to save mode
				ifs.close();
				return false;
			}
		}
		void save(const cv::Mat &in) {
			if (getMode() == LOAD) throw std::runtime_error("save attempt upon successful load not permitted");
			if (getMode() == UNKNOWN) {
				ofs.open(tmpName(), std::ios::binary);
			}
			writeMatBinary(ofs, in);
		}
	};

	class AveragingQueue {
		std::deque<cv::Mat> queue;
		cv::Mat running_sum;
		size_t maxsize;
	public:
		AveragingQueue(size_t maxsize) : maxsize(maxsize) {}
		size_t current_size() const {
			return queue.size();
		}
		size_t max_size() const { return maxsize; }
		bool filled() const {
			return queue.size() == maxsize;
		}
		bool push_frame(const cv::Mat &frame) { // true if fully filled
			queue.push_back(frame);
			if (running_sum.empty()) running_sum = frame.clone();
			else running_sum += frame;
			if (queue.size() > maxsize) {
				running_sum -= queue.front();
				queue.pop_front();
				return true;
			}
			return false;
		}
		cv::Mat getAvg() const {
			float k = 1.0f / (float)queue.size();
			cv::Mat res = running_sum * k;
			return res.clone();
		}

		cv::Mat getSum() const { return running_sum; }
		cv::Mat getFirst() const {
			return queue.front();
		}
		cv::Mat getLast() const {
			return queue.back();
		}
		cv::Mat getMiddle() const {
			return queue[queue.size() / 2];
		}
	};


};