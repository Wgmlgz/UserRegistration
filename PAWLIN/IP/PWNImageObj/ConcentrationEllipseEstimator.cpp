#include "stdafx.h"


#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include "ConcentrationEllipseEstimator.h"




void ConcentrationEllipseEstimator::estimateEllipseShapeParam(float cov_xx, float cov_xy, float cov_yy, float & thetta, float & rx, float & ry, float size_coef) {
	if ( (cov_xx < 0.0f) || (cov_yy < 0.0f) ) throw "Incorrect covariance\n";
	if (size_coef <= 0.0f) throw "Incorrect size_coef\n";
	
	//---DO NOT MODIFY THE ANGLE SIGN (!) (angle will be in typical, not image coordinate system)
	thetta = 0.5f*atan2f(-2.0f*cov_xy, cov_xx - cov_yy);//---define HyperOval_Creator::get_e_points behaviour and drawing beh and getAngleInImageCS / setAngle beh
	float sint = sinf(thetta);
	float cost = cosf(thetta);
	
	//---size_coef sigma ellipse
	rx = size_coef*sqrtf( fabsf(cov_xx*sint*sint + 2.0f*cov_xy*sint*cost + cov_yy*cost*cost) );
	ry = size_coef*sqrtf( fabsf(cov_xx*cost*cost - 2.0f*cov_xy*sint*cost + cov_yy*sint*sint) );
}

bool ConcentrationEllipseEstimator::estimate_ellipse(const CovMatrix & image_points_cov_matrix, OvalBase & ellipse, float size_coef) {
	if (image_points_cov_matrix.is_defined() == false) throw "Cov matrix is not defined!\n";
		
	ellipse.return_to_def();
		
	if( (image_points_cov_matrix.cov_xx <= float(1e-6)) && (image_points_cov_matrix.cov_yy <= float(1e-6)) ) return false;
		
	const float rad2deg_coef = float(180.0/M_PI);
		
	float thetta, rx, ry;		
	estimateEllipseShapeParam(image_points_cov_matrix.cov_xx, 
								image_points_cov_matrix.cov_xy, image_points_cov_matrix.cov_yy, thetta, rx, ry, size_coef);
		
	ellipse.x_image = image_points_cov_matrix.av_x;		
	ellipse.x = ellipse.x_image;
	ellipse.y_image = image_points_cov_matrix.av_y;	
	ellipse.y = ellipse.y_image;
	ellipse.angle = thetta*rad2deg_coef;					
	ellipse.sx = 2.0f*rx;
	ellipse.sy = 2.0f*ry;	
	
	return true;
}


bool CovMatrixHandling::covMatrix2Rect(const ConcentrationEllipseEstimator::CovMatrix &cov_matrix,Rect2Df &rect, float scale) {
	if (cov_matrix.is_defined() == false) 
		return false;
	float sigma_y = sqrtf(cov_matrix.cov_yy), sigma_x = sqrtf(cov_matrix.cov_xx);
	Point2Df  pa((sigma_y > 1.e-6f) ? (scale * cov_matrix.cov_xy / sigma_y) : 0, scale * sigma_y),		
		      pb(scale * sigma_x,(sigma_x > 1.e-6f) ? (scale * cov_matrix.cov_xy / sigma_x) : 0),
			  pc(cov_matrix.av_x, cov_matrix.av_y);

	vector<Point2Df> p4(4);
	p4[0] = pc + pa;
	p4[1] = pc - pa ;
	p4[2] = pc + pb;
	p4[3] = pc - pb;
	rect = Rect2Df(p4);
	return true;
}
