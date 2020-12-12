// https://github.com/abreheret/polygon-intersection
#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <opencv2/opencv.hpp>
#include <vector>
namespace pawlin {
	float distPoint(cv::Point p1, cv::Point p2);
	float distPoint(cv::Point2f p1, cv::Point2f p2);
	bool segementIntersection(cv::Point p0_seg0, cv::Point p1_seg0, cv::Point p0_seg1, cv::Point p1_seg1, cv::Point * intersection);
	bool segementIntersection(cv::Point2f p0_seg0, cv::Point2f p1_seg0, cv::Point2f p0_seg1, cv::Point2f p1_seg1, cv::Point2f * intersection);

	bool pointInPolygon(cv::Point p, const cv::Point * points, int n);
	bool pointInPolygon(cv::Point2f p, const cv::Point2f * points, int n);


	struct Polygon : public std::vector<cv::Point> {
		Polygon(const vector<cv::Point> &other) {
			std::vector<cv::Point> &base = *this;
			base=other;
			pointsOrdered();
		}
		Polygon(int width, int height) {
			push_back(cv::Point(0, 0));
			push_back(cv::Point(width, 0));
			push_back(cv::Point(width, height));
			push_back(cv::Point(0, height));
			push_back(cv::Point(0, 0));
			pointsOrdered();
		}
		Polygon(int n_ = 0) { assert(n_ >= 0); resize(n_); }
		virtual ~Polygon() {}
		const cv::Point* pt() const { return &((*this)[0]); }
		const int n() const { return (int)size(); }
		void add(const cv::Point &p) { push_back(p); }
		cv::Point getCenter() const;
		void pointsOrdered();
		float area() const;
		bool pointIsInPolygon(cv::Point p) const;
	};


void intersectPolygon( const cv::Point * poly0, int n0,const cv::Point * poly1,int n1, Polygon & inter ) ;
void intersectPolygon( const Polygon & poly0, const Polygon & poly1, Polygon & inter ) ;
void intersectPolygonSHPC(const Polygon * sub,const Polygon* clip,Polygon* res) ;
void intersectPolygonSHPC(const Polygon & sub,const Polygon& clip,Polygon& res) ;
};
#endif //