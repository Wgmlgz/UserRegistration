#pragma once
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <PWNGeometry/geometricobjects.h>
#include <PWNGeneral/pwnutil.h>

#define M_PIf 3.14159265f
#ifndef M_PI 
#define M_PI 3.141592653589793238462643383279502884
#endif


class OvalBase {
public:
	struct IsConcentricParam {
		float max_x_diff;
		float max_y_diff;
		float max_angle_diff;
		float max_ratio_diff;

		IsConcentricParam(float max_x_diff = 2.0f, float max_y_diff = 2.0f, float max_angle_diff = 2.0f, float max_ratio_diff = 0.2f) :
			max_x_diff(max_x_diff), max_y_diff(max_y_diff), max_angle_diff(max_angle_diff), max_ratio_diff(max_ratio_diff) { } 
	};

public:
	//---can be different from x_image(y_image) by shift only
	float x;
	float y;	
	//---in standard image coordinate system
	float x_image;
	float y_image;

	float angle;//---in typical, not image coordinate system (degrees)
	float sx;
	float sy;	
	int id;
	int index;

#ifdef USE_AUX_VAL_IN_OVALBASE
	float aux_val;
	float aux_val2;
	float aux_val3;
	float aux_val4;
#endif

public:
	OvalBase(void) {
		return_to_def();
	}

	OvalBase(FILE * file) {
		fread(this, sizeof(OvalBase), 1, file);		
	}

	inline void return_to_def(void) {
		x = 0.0f;
		y = 0.0f;
		x_image = -1.0f;
		y_image = -1.0f;
		angle = 0.0f;
		sx = 0.0f;
		sy = 0.0f;
		id = -1;
		index = -1;
#ifdef USE_AUX_VAL_IN_OVALBASE
		aux_val = 0.0f;
#endif
	}


	inline float getAngleInImageCS(void) const { return -1.0f*this->angle; }
	inline void setAngle(float deg_angle_in_imageCS) { this->angle = -1.0f*deg_angle_in_imageCS; }


	inline float getMinAxis(void) const {
		return std::min<float>(this->sx, this->sy);
	}
	inline float getMaxAxis(void) const {
		return std::max<float>(this->sx, this->sy); 	
	}
	inline float getAverageAxis(void) const {
		return 0.5f*(this->sx + this->sy);
	}


	inline float getRatio() const {//---define matching beh and histogram view
		float sum = sx+sy;
		if (sum == 0.0f) throw "Can't get ratio\n";	
		return 2.0f*fabsf(sx-sy)/(sum);
	}		
	//---[0; 1]
	inline float getRoundness(void) const {
		const float min_axis = getMinAxis(); 
		const float max_axis = getMaxAxis();
		if (max_axis == 0.0f) throw "Can't get roundness\n";

		return min_axis/max_axis;
	}
	inline float getTallness(void) const {
		const float min_axis = getMinAxis();
		const float max_axis = getMaxAxis();
		if (min_axis == 0.0f) throw "Can't get tallness\n";

		return max_axis / min_axis;
	}

	//---[0; 1]
	inline float getEccentricity(void) const {
		float min_axis = getMinAxis(); 
		float max_axis = getMaxAxis();
		if (max_axis == 0.0f) throw "Can't get eccentricity\n";

		return sqrtf(1.0f - sqr(min_axis / max_axis));
	}

	//---
	float getPerimeter(void) const;
	inline float getArea(void) const { return float(0.25 * M_PI) * sx * sy; }

	//---if use_xy == false, x_image / y_image will be used
	void get_extreme_points(Point2Df e_points[4], bool use_xy = true, float scale = 1.f) const;

	inline void normalize() {
		if(sx <= sy) {
			//printf("+,%.3f\n",angle);
			return;
		}
		//printf("-\n");
		float t = sx;
		sx = sy;
		sy = t;
		angle += 90.0f;
	}
	/*inline float computeDx() const { return std::max<float>(fabsf(sy/2*cosf(angle/180.0f*3.1415f)),fabsf(sx/2*sinf(angle/180.0f*3.1415f)));}
	inline float computeDy() const { return std::max<float>(fabsf(sx/2*cosf(angle/180.0f*3.1415f)),fabsf(sy/2*sinf(angle/180.0f*3.1415f)));}*/

	inline float computeDx() const { 
		float half_min_axis = 0.5f*getMinAxis();
		float half_max_axis = 0.5f*getMaxAxis();
		float ang = float(M_PI / 180) * getAngleInImageCS();

		return std::max<float>(fabsf(half_max_axis*cosf(ang)), fabsf(half_min_axis*sinf(ang)));
	}

	inline float computeDy() const { 
		float half_min_axis = 0.5f*getMinAxis();
		float half_max_axis = 0.5f*getMaxAxis();
		float ang = float(M_PI / 180) * getAngleInImageCS();

		return std::max<float>(fabsf(half_min_axis*cosf(ang)), fabsf(half_max_axis*sinf(ang)));
	}

	//---type == 'W'ork / 'I'mage
	inline Point2Df getCenter(char type = 'W') const { 
		if ( (type != 'W') && (type != 'I') ) throw "Incorrect type for getCenter\n";
		if (type == 'W')
			return Point2Df(this->x, this->y);
		else
			return Point2Df(this->x_image, this->y_image);
	}
	
	inline Point2Df getImageCenter(void) const { return Point2Df(this->x_image, this->y_image); }

	inline Point2Df getCenterFast() const { 
		return Point2Df(this->x, this->y);
	}

	//---type == 'W'ork / 'I'mage
	inline Rect2Df getRect(char type) const {
		if ( (type != 'W') && (type != 'I') ) throw "Incorrect type for rect\n";

		float dx = computeDx();
		float dy = computeDy();

		Rect2Df rect1;
		const float _x_ = ( (type == 'W')? x : x_image );
		const float _y_ = ( (type == 'W')? y : y_image );
		rect1.left =	_x_ - dx;
		rect1.top =		_y_ + dy;
		rect1.right =	_x_ + dx;
		rect1.bottom =	_y_ - dy;
		return rect1;
	}

	//--- correct vs getRect
	Rect2Df get_image_rect(float r_angle = 0 , float scale = 1.f) const;

	cv::RotatedRect get_image_rect_rotate_by_angle(float r_angle = 0 , bool width_greater_height = false,float scale = 1.f) const{
		Rect2Df rect =  get_image_rect(r_angle,scale);		
		Point2Df c = rect.getCenter();
		cv::Size2f sz(rect.width(),rect.height());
		if(width_greater_height)
			if(sz.width < sz.height) {

				float tmp = sz.width;
				sz.width  = sz.height;
				sz.height = tmp;

				if(r_angle < 0)
					r_angle += 90;
				else  
					r_angle -= 90;
			}			
			return cv::RotatedRect(cv::Point2f(c.x,c.y), sz, r_angle);
	}

	//---normalize() ok?
	inline cv::RotatedRect get_image_rotatedRect(void) const {
		return cv::RotatedRect(cv::Point2f(this->x_image, this->y_image), cv::Size2f(getMaxAxis(), getMinAxis()), getAngleInImageCS());
	}

	inline float getSize() const {
		float dx = computeDx();
		float dy = computeDy();
		return 2.0f*sqrtf(dx*dx + dy*dy);
	}

	//---must be in the same coordinate system
	inline bool isConcentric(const OvalBase & other, const IsConcentricParam * param = NULL) const { 
		IsConcentricParam d_param;		
		const IsConcentricParam * _param_ = (param ? param : &d_param);

		if (fabsf(x - other.x) > _param_-> max_x_diff) return false;
		if (fabsf(y - other.y) > _param_-> max_y_diff) return false;		
		if (fabsf(angle - other.angle) > _param_-> max_angle_diff) return false;		
		float r = getRatio();

		float orr = other.getRatio();
		if (fabsf(r - orr) > _param_-> max_ratio_diff) return false;
		return true;
	}

	void estimateEllipseCovParam(double &cov_xx, double &cov_xy, double &cov_yy, const double r_angle = 0,const double size_coef = 4) const {
		double dx =  sx / size_coef, dy = sy / size_coef;
		dx *= dx; dy *= dy;
		double t = (M_PI/180) * (angle + r_angle);
		double s2 = sin(t);
		s2 *= s2;
		double c2 = 1 - s2;

		cov_xx = s2 * dx +  c2 * dy;
		cov_yy = c2 * dx +  s2 * dy;
		cov_xy = 0.5 * sin(2 * t) * (dx - dy);
	}

	void estimateEllipseCovParam(float &cov_xx, float &cov_xy, float &cov_yy,const float r_angle = 0, const float size_coef = 4) const {
		double d_cov_xx, d_cov_xy, d_cov_yy;
		estimateEllipseCovParam(d_cov_xx, d_cov_xy, d_cov_yy, r_angle, size_coef);
		cov_xx = (float)d_cov_xx;
		cov_xy = (float)d_cov_xy;
		cov_yy = (float)d_cov_yy;		  
	}

	

	//---PavelS section
public:
	struct Basis : public Basis2Df {

		Basis(const OvalBase &o, float scale = 1.0f);
		Basis(const Basis2Df &basis) : Basis2Df(basis) {}
		OvalBase lcs2wcs(const OvalBase &other) const {
			Basis ovalAsBasis(other);
			Point2Df p=ocs2wcs(ovalAsBasis.c);
			return OvalBase(Basis2Df(
				p,
				ocs2wcs(ovalAsBasis.c+ovalAsBasis.e1)-p,
				ocs2wcs(ovalAsBasis.c+ovalAsBasis.e2)-p)
				);
		}
		OvalBase wcs2lcs(const OvalBase &other) const {
			Basis ovalAsBasis(other);
			Point2Df p=wcs2ocs(ovalAsBasis.c);
			return OvalBase(Basis2Df(
				p,
				wcs2ocs(ovalAsBasis.c+ovalAsBasis.e1)-p,
				wcs2ocs(ovalAsBasis.c+ovalAsBasis.e2)-p)
				);
		}
	};
	OvalBase(const Basis2Df &basis) {
		return_to_def();
		x = basis.c.x;
		y = basis.c.y;
		sx = basis.e1.norm()*2.0f;
		sy = basis.e2.norm()*2.0f;
		this->angle = atan2f(-basis.e1.y,basis.e1.x);
	}

	OvalBase (const Point2Df &c, const Point2Df &v1, const Point2Df &v2) {
		x = c.x;
		y = c.y;

		Point2Df dim1 = v1 - c;
		Point2Df dim2 = v2 - c;

		sx = 2.0f*std::min<float>(dim1.norm(), dim2.norm());
		sy = 2.0f*std::max<float>(dim1.norm(), dim2.norm());

		angle = dim1.norm() > dim2.norm() ? atan2f(dim1.y, dim1.x) : atan2f(dim2.y, dim2.x);
		angle = float(180.0/M_PI) * angle;
		normalize();
		angle *= -1.0f; //transforming to typical (not image) coordinate system
	}


public:
	// note this one does not change x_image, y_image
	void scale(float k) {
		x*=k;
		y*=k;
		sx*=k;
		sy*=k;
	}
	void expand(float k) {
		sx*=k;
		sy*=k;
	}
	bool contains2(float x, float y) const {
		Basis b(*this);
		Point2Df p = b.wcs2ocs(Point2Df(x,y));
		return p.norm() <= 1.0f;
	}
	bool contains2(const Point2Df &p) const { return contains2(p.x, p.y); }
	template <class T>
	bool contains(const T &other) const {
		float h = x; 
		float k = y; 
		float rx = this->getMaxAxis(); 
		float ry = this->getMinAxis();
		float eq = (other.x - h) * (other.x - h) / ( rx * rx) + (other.y - h) * (other.y - h) / (ry * ry);
		return eq <= 1.f;
	}



};


