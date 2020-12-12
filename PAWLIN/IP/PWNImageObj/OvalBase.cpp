#include "stdafx.h"

#include "OvalBase.h"
#include <PWMath/angles.h>
#include <PWNImageObj/ConcentrationEllipseEstimator.h>


using namespace std;


void OvalBase::get_extreme_points(Point2Df e_points[4], bool use_xy, float scale) const {
	float half_min_axis = scale*0.5f*getMinAxis();
	float half_max_axis = scale*0.5f*getMaxAxis();	
	float rad_ang_1 = M_PIf*getAngleInImageCS() / 180.0f;

	Point2Df start;
	if (use_xy) {
		start.x = this->x;
		start.y = this->y;
	}
	else {
		start.x = this->x_image;
		start.y = this->y_image;
	}

	Point2Df gv_1(cosf(rad_ang_1), sinf(rad_ang_1));
	Point2Df gv_2(gv_1.y, -gv_1.x);

	Ray2Df ray_1(start, gv_1);
	Ray2Df ray_2(start, gv_2);

	e_points[0] = ray_1.getPoint(half_max_axis);
	e_points[1] = ray_1.getPoint(-half_max_axis);
	e_points[2] = ray_2.getPoint(half_min_axis);
	e_points[3] = ray_2.getPoint(-half_min_axis);	
}

float OvalBase::getPerimeter() const {
	float t_1 = 1.5f*(this->sx + this->sy);
	float t_2 = 2.5f*this->sx*this->sy;
	float t_3 = 0.75f*(sqr(this->sx) + sqr(this->sy));
	float p_1 = fabsf( 3.141592f*(t_1 - sqrtf(t_2 + t_3)) );

	////---test
	//float h = sqr(this->sx - this->sy)/sqr(this->sx + this->sy);
	//float t_11 = 10.0f + sqrtf(4.0f - 3.0f*h);
	//float t_22 = 1.0f + 3*h/t_11;
	//float t_33 = 0.5f*3.141592f*(this->sx + this->sy);
	//float p_2 = t_33*t_22;

	//printf("\n\n%.3f-%.3f\n\n", p_1, p_2);
	////---test


	return p_1;
}

OvalBase::Basis::Basis(const OvalBase &o, float scale)  {
	c = Point2Df(o.x*scale,o.y*scale);
	float d1 = o.getMaxAxis()*scale;
	float d2 = o.getMinAxis()*scale;
	float angle = normalizeAngle(o.angle);
	if(angle > 270+45.0f || angle < 45.0f) angle -= 180.0f;
	else 
		if(angle > 90+45.0f && angle > 180+ 45.0f) angle -= 180.0f;
	float angrad = angle*M_PIf/180.0f;
	e1.x = cosf(angrad);
	e1.y = -sinf(angrad);
	e2.x = -e1.y;
	e2.y = e1.x;
	// check nearest angle to upper

	e1.multiply(0.5f*d1);
	e2.multiply(0.5f*d2);
	updateCoefs();
}

Rect2Df OvalBase::get_image_rect(float r_angle ,float scale) const{
	ConcentrationEllipseEstimator::CovMatrix covMatrix;

	estimateEllipseCovParam(covMatrix.cov_xx,covMatrix.cov_xy,covMatrix.cov_yy, r_angle, 4 * scale);
	covMatrix.av_x = this->x_image;
	covMatrix.av_y = this->y_image;

	Rect2Df rect;
	CovMatrixHandling::covMatrix2Rect(covMatrix,rect);
	return rect;
};

