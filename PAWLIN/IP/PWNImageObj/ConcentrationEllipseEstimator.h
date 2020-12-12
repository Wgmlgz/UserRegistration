#pragma once

#include <math.h>
#include <PWNImageObj/OvalBase.h>
#include <PWNGeneral/pwnutil.h>




class ConcentrationEllipseEstimator {
public:
	struct CovMatrix {
		float av_x;
		float av_y;
		float cov_xx;
		float cov_xy;
		float cov_yy;
		
		CovMatrix(void) { return_to_def(); }
		
		
		inline bool is_defined(void) const {
			if ( (this->cov_xx < 0.0f) || (this->cov_yy < 0.0f) ) return false;
			return true;
		}

		void init(double sum_x, double sum_y, double sum_xx, double sum_xy, double sum_yy, size_t count) {
			if (count < 1) throw "Can't init covariance matrix\n";
			init(sum_x, sum_y, sum_xx, sum_xy, sum_yy, double(count));
		}

		void init(double sum_x, double sum_y, double sum_xx, double sum_xy, double sum_yy, double weight_sum) {
			if (weight_sum < 0) throw "Can't init covariance matrix\n";
			
			double avx, avy;
			avx = sum_x/weight_sum;
			avy = sum_y/weight_sum;	
			
			this->av_x = float(avx);
			this->av_y = float(avy);
			this->cov_xx = float( abs(sum_xx/weight_sum - sqr(avx)) );
			this->cov_xy = float( sum_xy/weight_sum - avx*avy );	
			this->cov_yy = float( abs(sum_yy/weight_sum - sqr(avy)) );	
		}

		void init(const float matrix[4], const Point2Df & center) {
			this->av_x = center.x;
			this->av_y = center.y;
			this->cov_xx = matrix[0];
			this->cov_xy = matrix[1];
			this->cov_yy = matrix[3];
			if (!is_defined()) throw "Can't init covariance matrix\n";
		}

		inline void get_matrix(float matrix[4]) const {
			matrix[0] = this->cov_xx; matrix[1] = this->cov_xy;	
			matrix[2] = this->cov_xy; matrix[3] = this->cov_yy;		
		}
		
		inline void get_center(Point2Df & center) const {
			center.x = this->av_x;
			center.y = this->av_y;
		}

		inline void return_to_def(void) {
			this->av_x = 0.0f; this->av_y = 0.0f;
			this->cov_xx = -1.0f; this->cov_xy = 0.0f; this->cov_yy = -1.0f;
		}
	};

public:
	template<typename Point2DType>
	static void estimateCovMatrix(const Point2DType * points, unsigned int count, CovMatrix & cov_matrix) {
		if (count < 1) throw "Can't estimate covariance matrix\n";

		double sum_x, sum_y, sum_xx, sum_xy, sum_yy;
		sum_x = sum_y = sum_xx = sum_xy = sum_yy = 0.0;
	
		for (unsigned int i = 0; i<count; i ++) {
			const Point2DType & point = points[i];
			
			double x = double( point.x );
			double y = double( point.y );
			
			sum_x += x;
			sum_y += y;
			sum_xx += x*x;
			sum_xy += x*y;
			sum_yy += y*y;	
		}		
		cov_matrix.init(sum_x, sum_y, sum_xx, sum_xy, sum_yy, (size_t)count);				
	}

	template<typename Point2DType>
	static void estimateCovMatrixWeighted(const Point2DType * points,const float * weights, unsigned int count, CovMatrix & cov_matrix) {
		if (count < 1) throw "Can't estimate covariance matrix\n";

		double sum_x, sum_y, sum_xx, sum_xy, sum_yy, sum_weights;
		sum_x = sum_y = sum_xx = sum_xy = sum_yy = sum_weights = 0.0;
	
		for (unsigned int i = 0; i<count; i ++) {
			const Point2DType & point = points[i];
			
			double x = double( point.x );
			double y = double( point.y );
			
			sum_x += x * weights[i];
			sum_y += y * weights[i];
			sum_xx += x*x * weights[i];
			sum_xy += x*y * weights[i];
			sum_yy += y*y * weights[i];
			sum_weights += weights[i];
		}		
		cov_matrix.init(sum_x, sum_y, sum_xx, sum_xy, sum_yy, sum_weights);				
	}
	
	template<typename Point2DType>
	static void estimateCovMatrix(const std::vector <Point2DType> & points, CovMatrix & cov_matrix) {
		if (points.empty()) throw "Can't estimate covariance matrix\n";		
		estimateCovMatrix(&points[0], unsigned( points.size() ), cov_matrix);
	}

	//---thetta will be in typical, not image coordinate system (!)
	//---rx/ry will be scaled by size_coef
	static void estimateEllipseShapeParam(float cov_xx, float cov_xy, float cov_yy, float & thetta, float & rx, float & ry, float size_coef = 2.0f);
	
	//---image_points - in standard image coordinate system
	template<typename Point2DType>
	static bool estimate_ellipse(const std::vector<Point2DType> & image_points, OvalBase & ellipse, float size_coef = 2.0f) {
		CovMatrix cov_matrix;		
		estimateCovMatrix(image_points, cov_matrix);		
		
		if ( estimate_ellipse(cov_matrix, ellipse, size_coef) == false ) { ellipse.return_to_def(); return false; }
		
		return true;
	}

	//---image_points - in standard image coordinate system
	static bool estimate_ellipse(const CovMatrix & image_points_cov_matrix, OvalBase & ellipse, float size_coef = 2.0f);

};

class CovMatrixHandling {
public:
	static bool covMatrix2Rect(const ConcentrationEllipseEstimator::CovMatrix &cov_matrix,Rect2Df &rect, float scale = 2.0f); 
	
};

inline void makeOval(const Point2Df *points, size_t count, OvalBase &out, const float * weights = NULL) {
	ConcentrationEllipseEstimator::CovMatrix covMatrix;

	if (weights == NULL)
		ConcentrationEllipseEstimator::estimateCovMatrix(points, (unsigned int)(count), covMatrix);
	else
		ConcentrationEllipseEstimator::estimateCovMatrixWeighted(points, weights, (unsigned int)(count), covMatrix);

	out.x_image = covMatrix.av_x;
	out.x = covMatrix.av_x;
	out.y_image = covMatrix.av_y;
	out.y = covMatrix.av_y;

	float thetta, rx, ry;
	ConcentrationEllipseEstimator::estimateEllipseShapeParam(covMatrix.cov_xx, covMatrix.cov_xy, covMatrix.cov_yy, thetta, rx, ry);

	out.angle = 180.0f*thetta / 3.14159265f;
	out.sx = 2.0f*rx + 1.0f;
	out.sy = 2.0f*ry + 1.0f;
}

inline void makeOval(const OvalBase &o1, const OvalBase &o2, OvalBase &out) {
	Point2Df p[8];
	o1.get_extreme_points(p);
	o2.get_extreme_points(p + 4);
	return makeOval(p, 8, out);
}