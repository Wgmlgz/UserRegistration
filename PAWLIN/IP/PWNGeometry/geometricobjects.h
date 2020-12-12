// (c) Pawlin Technologies Ltd 
// File: geometricobjects.h
// Date: 15-AUG-2008
// Authors: Svetlana Korobkova, Pavel Skribtsov
// Purpose: geometric objects description classes

// ALL RIGHTS RESERVED, USAGE WITHOUT WRITTEN PERMISSION FROM PAWLIN TECHNOLOGIES LTD. is prohibited
#pragma once

#include <stdio.h>
#include <vector>
#include <cmath>
#include <float.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#ifdef WIN32
#include <tuple>
#include <Windows.h>
#endif
//#include <xutility>
using std::map;
struct Sphere;
struct Point3Df;
#include "Point3Df.h"
#include <PWNGeneral/pwnutil.h>

struct ISphereSearchable {
	enum IntersectionType {separated=0, intersection=1, within=2}; // within means the sphere is within the object
	virtual IntersectionType intersects(const Sphere &s, bool inclusive) const = 0;
	virtual bool contains(const Point3Df &p, bool inclusive) const = 0;
};


//#define double3 pwn_double3

//class PointsSet;
//struct PlaneInfo;


//---To discriminate between pwn and cv
namespace pwn {
	struct Point3d {
		long x;
		long y;
		long z;
		Point3d () {x = 0; y = 0; z = 0;}
		Point3d (long d, long b, long c) { x = d; y = b; z = c;}
		//Point3d (const CvPoint3D32f &other) {x = (long)other.x; y = (long)other.y; z = (long)other.z;}
		Point3d (const Point3d &other) {x = other.x; y = other.y; z = other.z;}
		bool operator == (const Point3d &other) const {return x==other.x && y==other.y && z==other.z;}
		Point3d& operator = (const Point3d &other) {this->x = other.x; this->y = other.y; this->z = other.z; return *this;}
		double sqdist(const Point3d &other) const {return double(other.x-x)*double(other.x-x) + double(other.y-y)*double(other.y-y)+double(other.z-z)*double(other.z-z);}
		double sqdist(const pwn_double3 &other) const;
		void takeMin(const Point3d &other) {
			x = std::min<long>(x,other.x);
			y = std::min<long>(y,other.y);
			z = std::min<long>(z,other.z);
		}
		void takeMax(const Point3d &other) {
			x = std::max<long>(x,other.x);
			y = std::max<long>(y,other.y);
			z = std::max<long>(z,other.z);
		}

	};
};

struct Point3Dkd {
	double x;
	double y;
	double z;
	Point3Dkd () {x = 0; y = 0; z = 0;}
	Point3Dkd (double d, double b, double c) {x = d; y = b; z = c;}
	//Point3Dkd (const CvPoint3D32f &other) {x = (double)other.x; y = (double)other.y; z = (double)other.z;}
	Point3Dkd (const Point3Dkd &other) {x = other.x; y = other.y; z = other.z;}
	Point3Dkd (const pwn::Point3d &other) {x = (double)other.x; y = (double)other.y; z = (double)other.z;}
	Point3Dkd(Point3Dkd &p1,Point3Dkd &p2) { x = p1.y*p2.z - p1.z*p2.y; y = p1.z*p2.x - p1.x*p2.z; z = p1.x*p2.y - p1.y*p2.x;}
	double getX() { return x;}
	double getY() { return y;}
	double getZ() { return z;}
	void add(const Point3Dkd &other) {x+=other.x; y+=other.y; z+=other.z;}
	void minus(const Point3Dkd &other) {x-=other.x; y-=other.y; z-=other.z;}
	void negate() { x*=-1; y*=-1; z*=-1;}
	void multiply(float k) { x=(double)(x*k); y=(double)(y*k); z=(double)(z*k);}
	float product(const Point3Dkd &other) const { return (float)(other.x*x+other.y*y+other.z*z);}
	float norm() { return (float)sqrt(x*x+y*y+z*z);}
	float normate() { float k = norm(); float invK = 1/k; this->multiply(invK); return k;}
	float distance(const Point3Dkd &other) const {Point3Dkd d(other); d.minus(*this); return d.norm();}
	Point3Dkd multiplied( double k) const { return Point3Dkd((double)(x*k),(double)(y*k),(double)(z*k));}
	Point3Dkd negative() {return Point3Dkd(-1*x,-1*y,-1*z);}
	Point3Dkd getNormal(const Point3Dkd &p1,const Point3Dkd &p2) { Point3Dkd t1(p1); Point3Dkd t2(p2); t1.minus(*this); t2.minus(*this); return Point3Dkd(t1,t2);}
	bool operator == (const Point3Dkd &other) const {return x==other.x && y==other.y && z==other.z;}
	Point3Dkd& operator = (const Point3Dkd &other) {this->x = other.x; this->y = other.y; this->z = other.z; return *this;}
	double sqdist(const Point3Dkd &other) const {return double(other.x-x)*double(other.x-x) + double(other.y-y)*double(other.y-y)+double(other.z-z)*double(other.z-z);}
	double sqdist(const pwn_double3 &other) const;

	Point3Dkd operator + (const Point3Dkd &other) const {	Point3Dkd res = *this;	res.add(other);	return res;	}
	Point3Dkd operator - (const Point3Dkd &other) const {	Point3Dkd res = *this;	res.minus(other);	return res;	}
	Point3Dkd operator * (const double k) const {	return this->multiplied(k);	}
};



struct Point2Df {
	float x;
	float y;
	Point2Df () {x = 0; y = 0;}
	Point2Df (float d, float b) {x = d; y = b;}
	Point2Df (const Point2Df &other) {x = other.x; y = other.y;}
	Point2Df ( const Point3Df &other ) { x = other.x, y = other.y; } // truncating z
	//Point2Df(const Point2Df &p1,const Point2Df &p2) { x = p1.y*p2.z - p1.z*p2.y; y = p1.z*p2.x - p1.x*p2.z; z = p1.x*p2.y - p1.y*p2.x;}
	inline float getX() const { return x;}
	inline float getY() const { return y;}	
	inline void get_point(float point[2]) const { point[0] = this->x; point[1] = this->y; }
	inline void add(const Point2Df &other) {x+=other.x; y+=other.y;}
	inline void addWeighted(const Point2Df &other, float w) {x+=w*other.x; y+=w*other.y;}
	inline void minus(const Point2Df &other) {x-=other.x; y-=other.y;}
	inline void negate() {x*=-1.0f; y*=-1.0f;}
	inline void multiply(float k) { x=(float)(x*k); y=(float)(y*k);}
	inline void multiply(const Point2Df & other) {x *= other.x; y *= other.y;}  //---SinDM
	float product(const Point2Df &other) const { return (float)(other.x*x+other.y*y);}

	//Point2Df vecproduct(const Point2Df &other) const {return Point2Df(*this,other);}
	inline float norm() const { return sqrtf(sqnorm());}
	inline float sqnorm() const { return x*x + y*y; }
	inline float normate() { float k = norm(); float invK = 1.0f/k; this->multiply(invK); return k;}
	inline float distance(const Point2Df &other) const {Point2Df d(other); d.minus(*this); return d.norm();}
	inline float distancex (const Point2Df &other) const {return abs(other.x - x);}
	inline float distancey (const Point2Df &other) const {return abs(other.y - y);}
	inline Point2Df multiplied(float k) const { return Point2Df((float)(x*k),(float)(y*k));}
	inline Point2Df multiplied(const Point2Df & other) const { return Point2Df((float)(x*other.x), (float)(y*other.y)); }
	inline Point2Df negative() const {return Point2Df(-1.0f*x,-1.0f*y);}
	//inline Point2Df getNormal(const Point2Df &p1,const Point2Df &p2) { Point2Df t1(p1); Point2Df t2(p2); t1.minus(*this); t2.minus(*this); return Point2Df(t1,t2);}
	
	inline void set(float x, float y) { this->x = x; this->y = y; }
	
	inline bool operator == (const Point2Df &other) const {return x==other.x && y==other.y;}


	Point2Df& operator = (const Point2Df &other) {this->x = other.x; this->y = other.y; return *this;}
	bool operator <= (const Point2Df &other) const {return ((x<=other.x) && (y<=other.y));}	
	bool operator >= (const Point2Df &other) const {return ((x>=other.x) && (y>=other.y));}
	
	inline float sqdist(const Point2Df &other) const {return float(other.x-x)*float(other.x-x) + float(other.y-y)*float(other.y-y);}
	//float sqdist(const double2 &other) const;
	void print() const {printf("X: %.3f  Y: %.3f \n", x, y);}  //---SinDM
	void takeMin(const Point2Df &other) {
		x = std::min<float>(x,other.x);
		y = std::min<float>(y,other.y);
	}
	void takeMax(const Point2Df &other) {
		x = std::max<float>(x,other.x);
		y = std::max<float>(y,other.y);
	}
	void floor(void) {x = floorf(x); y = floorf(y);} //---SinDM
	void round(void) {x = floorf(x + 0.5f); y = floorf(y + 0.5f);} //---SinDM
	void ceil(void) {x = ceilf(x); y = ceilf(y);}
	Point2Df operator + (const Point2Df &other) const {	Point2Df res = *this;	res.add(other);	return res;	}
	Point2Df operator - (const Point2Df &other) const {	Point2Df res = *this;	res.minus(other);	return res;	}
	Point2Df operator * (const float k) const {	return this->multiplied(k);	}


	friend std::ostream &operator<<( std::ostream &output, 
                                       const Point2Df &p )
      { 
		 output<<p.getX()<<" "<<p.getY();
         return output;            
      }
};

struct MeanPoint2Df {
	Point2Df sum;
	size_t count;
	MeanPoint2Df() : sum(0,0), count(0) {}
	void add(const Point2Df &p) {sum.add(p); count++;}
	Point2Df getMean() const { return count ? sum.multiplied(1.0f / count) : sum;}
};

struct GeneralizedBasis2Df {
	virtual Point2Df wcs2ocs(const Point2Df &wp) const = 0;
	virtual Point2Df ocs2wcs(const Point2Df &op) const = 0;
	virtual ~GeneralizedBasis2Df() {}
};
struct Basis2Df : public GeneralizedBasis2Df {
	float e1m, e2m;
	Point2Df c,e1,e2;

	Basis2Df() : e1m(1.0f),e2m(1.0f),c(0,0),e1(1.0f,0),e2(0,1.0f) {};
	Basis2Df(float width,float height) :
		e1m(1.0f/(width*width)),e2m(1.0f/(height*height)),c(0,0),e1(width,0),e2(0,height) {};
	Basis2Df(const Point2Df &c, const Point2Df &e1, const Point2Df &e2) : c(c), e1(e1),e2(e2) {
		updateCoefs();
	}
	void scale(float k) {
		c.multiply(k);
		e1.multiply(k);
		e2.multiply(k);
		updateCoefs();
	}
	Point2Df wcs2ocs(const Point2Df &wp) const ;
	Point2Df ocs2wcs(const Point2Df &op) const ;
	float getRatio() const { return e1.norm() / e2.norm(); }
	float getScale() const { return (e1+e2).multiplied(0.5f).norm(); }
	void setVectors(const Point2Df &a1, const Point2Df &a2) {
		e1 = a1, e2 = a2;
		updateCoefs();
	}
	void normalize() {
		e1.normate();
		e2.normate();
		updateCoefs();
	}
	void updateCoefs() {
		e1m = 1.0f / e1.product(e1);
		e2m = 1.0f / e2.product(e2);
	}
};

struct PolarBasis2Df : public GeneralizedBasis2Df {
	float er; // what is this for ? as far as i see no method uses this member
	Point2Df c;

	PolarBasis2Df() : er(1.0f),c(0,0) {};
	PolarBasis2Df(float cx,float cy) : c(cx,cy) {
			
	}

	Point2Df wcs2ocs(const Point2Df &wp) const ;
	Point2Df ocs2wcs(const Point2Df &op) const ;	
};

struct PolarBasis2Df_ref_angle : public  PolarBasis2Df {
	Point2Df ref_center;
	bool verbose;
	
	// :BUG: ref_center here is ready by itself - maybe Point2Df() was supposed
	PolarBasis2Df_ref_angle() : PolarBasis2Df(), ref_center(0,0), verbose(false) {};
	PolarBasis2Df_ref_angle(float cx,float cy, bool verbose = false) : PolarBasis2Df(cx, cy), ref_center(0.0f, 0.0f), verbose(verbose) {}
	PolarBasis2Df_ref_angle(float cx,float cy, Point2Df ref_center,  bool verbose = false) : PolarBasis2Df(cx, cy), ref_center(ref_center), verbose(verbose) {}

	Point2Df wcs2ocs(const Point2Df &wp) const ;
	//Point2Df ocs2wcs(const Point2Df &op) const override;	
};


inline const float determinant( const Point2Df &a, const Point2Df &b ) { return ( a.x * b.y - a.y * b.x ); }
inline const float determinant( const Point3Df &a, const Point3Df &b, const Point3Df &c )
{ return ( a.x * ( b.y * c.z - b.z * c.y ) - b.x * ( a.y * c.z - a.z * c.y ) + c.x * ( a.y * b.z - a.z * b.y ) ); }

bool ZcomparePoints3Df(const Point3Df &a, const Point3Df &b); // {return a.z < b.z; }



float ray_t(const Point3Df &a, const Point3Df &b, const Point3Df &o);
inline float rayProject(const Point3Df &a, const Point3Df &b, const Point3Df &o) {return ray_t(a,b,o);}//warning!!! careful with the name of this function
inline Point3Df readPoint3Df(const string &str) {
	vector<string> vals;
	static const vector<char> sep = { ',' , ' '  };
	split(str, sep, vals);
	if (vals.size() != 3) throw std::runtime_error("readPoint3Df wrong count of values during init");
	return Point3Df(
		(float)atof(vals[0].c_str()),
		(float)atof(vals[1].c_str()),
		(float)atof(vals[2].c_str())
		);
}

inline Point2Df readPoint2Df(const string &str) {
	vector<string> vals;
	static const vector<char> sep = { ',' , ' ' };
	split(str, sep, vals);
	if (vals.size() != 2) throw std::runtime_error("readPoint2Df wrong count of values during init");
	return Point2Df(
		(float)atof(vals[0].c_str()),
		(float)atof(vals[1].c_str())
	);
}

struct Box2Df {
	Point2Df boxmin;
	Point2Df boxmax;
	Box2Df() {}
	Box2Df(const Point2Df &a, const Point2Df &b) {
		boxmin.x = std::min<float>(a.x, b.x);
		boxmin.y = std::min<float>(a.y, b.y);

		boxmax.x = std::max<float>(a.x, b.x);
		boxmax.y = std::max<float>(a.y, b.y);
	}
	Point2Df size() const { return boxmax - boxmin; }
	float aspect() const { auto sz = size();  return sz.x / sz.y; }
	void normalize() {
		Box2Df temp(boxmin, boxmax);
		*this = temp;
	}
	inline Point2Df genPoint(float tx, float ty) const {
		Point2Df dir = boxmax - boxmin;
		return boxmin + Point2Df(dir.x*tx, dir.y*ty);
	}
	inline Point3Df p3(const Point2Df &p, float z) const { return Point3Df(p.x, p.y, z); }
	void genContour3Df(vector<Point3Df> &out, float z = 0) const {
		Point2Df p[5];
		p[0] = genPoint(0, 0);
		p[1] = genPoint(1, 0);
		p[2] = genPoint(1, 1);
		p[3] = genPoint(0, 1);
		p[4] = p[0];
		for (int i = 0; i < 5; i++) out.push_back(p3(p[i], z));
	}
	void generateGrid(int stepx, int stepy, vector<Point2Df> &grid) const {
		float dx = 1.0f / (stepx + 1);
		float dy = 1.0f / (stepy + 1);
		for (int i = 1; i < stepx; i++) {
			for (int j = 1; j < stepy; j++) {
				grid.push_back(genPoint(dx*i, dy*j));
			}
		}
	}
	Point2Df box2img(int cols, int rows, const Point2Df &pt) const {
		Point2Df res = pt;
		res.x -= boxmin.x;
		res.y -= boxmin.y;
		auto sz = boxmax - boxmin;
		res.x *= cols/sz.x;
		res.y = rows-1-res.y*rows/sz.y;
		return res;
	}

	Point2Df img2box(int cols, int rows, const Point2Df &pt) const {
		Point2Df res = Point2Df(pt.x,rows-1-pt.y);
		auto sz = boxmax - boxmin;
		res.x *= sz.x/cols;
		res.y *= sz.y/rows;
		return res + boxmin;
	}
};

struct Box3Df {
	Point3Df boxmin;
	Point3Df boxmax;
	Box3Df() {}
	Box3Df(const Box2Df &box2d, float minz, float maxz) {
		boxmin = Point3Df(box2d.boxmin.x, box2d.boxmin.y, minz);
		boxmax = Point3Df(box2d.boxmax.x, box2d.boxmax.y, maxz);
	}
	Box3Df(const Point3Df &a, const Point3Df &b) {
		boxmin.x = std::min<float>(a.x, b.x);
		boxmin.y = std::min<float>(a.y, b.y);
		boxmin.z = std::min<float>(a.z, b.z);

		boxmax.x = std::max<float>(a.x, b.x);
		boxmax.y = std::max<float>(a.y, b.y);
		boxmax.z = std::max<float>(a.z, b.z);
	}
	void normalize() {
		Box3Df temp(boxmin, boxmax);
		*this = temp;
	}
	void genABCDEFGH(vector<Point3Df> &ver) const {
		ver.push_back(Point3Df(boxmin.x, boxmin.y, boxmin.z));
		ver.push_back(Point3Df(boxmax.x, boxmin.y, boxmin.z));
		ver.push_back(Point3Df(boxmax.x, boxmax.y, boxmin.z));
		ver.push_back(Point3Df(boxmin.x, boxmax.y, boxmin.z));
		ver.push_back(Point3Df(boxmin.x, boxmin.y, boxmax.z));
		ver.push_back(Point3Df(boxmax.x, boxmin.y, boxmax.z));
		ver.push_back(Point3Df(boxmax.x, boxmax.y, boxmax.z));
		ver.push_back(Point3Df(boxmin.x, boxmax.y, boxmax.z));
	}
};

struct Ray3Df {
	Ray3Df(const Point3Df &a, const Point3Df &b) : a(a), b(b) {}
	Ray3Df() {}
	Point3Df a;
	Point3Df b;
	bool boxIntersection(const Point3Df &boxmin, const Point3Df &boxmax, float &tmin, float &tmax) const;
	Point3Df project(const Point3Df &o) const {
		Point3Df temp(b);
		temp.multiply(ray_t(a,b,o));
		temp.add(a);
		return temp;
	}
	Point3Df getPoint(float t) const {
		Point3Df ret(a);
		Point3Df bl(b);
		bl.multiply(t);
		ret.add(bl);

		return ret;
	}
	float distance(const Point3Df &o) const { return project(o).distance(o);}
	// returns distance to the point, sets end of the ray to the nearest point
	float rayProject(const Point3Df &o) const {
		return ray_t(a,b,o);
	}
	Point3Df getNearest(const Point3Df & point) {
		float t = ray_t(a,b,point);
		return getPoint(t);
	}

	float getRayProj(const Point3Df& o) const {
		throw std::runtime_error("never use this function, as b is a direction vector, not a second point!!!!\n");
		Point3Df v(o);
		v.minus(a);
		Point3Df v1(b);
		v1.minus(a);
		const float result = v.product(v1) / sqrtf(v1.product(v1));
		return result;
	}

	Point3Df getPointOnRay(float t) const {
		throw std::runtime_error("never use this function, as b is a direction vector, not a second point!!!!\n");
		Point3Df ret(a);
		Point3Df bl(b - a);
		bl.normate();
		bl.multiply(t);
		ret.add(bl);
		return ret;
	}

	bool computePlaneIntersection(const Point3Df &n, float d, float &t) const // computes ray parameter of intersection of this ray with plane (x,n)+d = 0
	{
		//t = [-d - (a,n)] / (b,n)
		float tmp = (a.product(n) - d)*(-1.0f);
		float tmp2 = b.product(n);
		if (tmp2 == 0) return 0; // no interaction

		t = tmp / tmp2;
		return 1;
	}


	// the method for retrieving parameters of cross
	void rayCross( const Ray3Df &other, float *thisRayParameter, float *otherRayParameter ) const
	{
		if (otherRayParameter == NULL) return;
		else
		{ 
			float b1b1 = getDotProduct( b, b );
			float b2b2 = getDotProduct( other.b, other.b );
			float b1b2 = getDotProduct( b, other.b );
			if( b2b2*b1b1 == b1b2*b1b2 ) return;
			float k = b1b2 / b1b1;					
           			 Point3Df temp1(b);			
         			   temp1.multiply(k);			
         			   temp1.minus(other.b);			
         			   Point3Df temp2(other.a);					
         			   temp2.minus(a);					
         			   *otherRayParameter = getDotProduct(temp1, temp2);
			*otherRayParameter /= b2b2 - k * b1b2;
         			   if (thisRayParameter != NULL)            
          			      *thisRayParameter = k *(*otherRayParameter) + getDotProduct(b, temp2)/b1b1;  
		}


		/*r2.t = cvDotProduct(
			cvMinus(k,r1.b,r2.b),
			cvMinus(r2.a,r1.a)
			);
		r2.t /= b2b2 - k * b1b2;
		
		r1.t = k * r2.t + cvDotProduct(
			r1.b,
			cvMinus(r2.a,r1.a)
			)/b1b1;*/
		//return sqrtf(cvSquareDistance(r1.getPoint(), r2.getPoint()));
	}

	// method for dot product
	float getDotProduct( const Point3Df &first, const Point3Df &second ) const
	{
		const float product = first.x*second.x + first.y*second.y + first.z*second.z;
		return product;
	}

	//method for finding cross point
    Point3Df getCrossPoint(const Ray3Df &other)
    {
        float my_t = 1;
        float other_t = 1;
        rayCross(other, &my_t, &other_t);
        //my_point
        Point3Df my_point(b);
        my_point.multiply(my_t);
        my_point.add(a);
        //other point
        Point3Df other_point(other.b);      
        other_point.multiply(other_t);        
        other_point.add(other.a);
        //cross point
        Point3Df cross_point(my_point);
        cross_point.add(other_point);
        cross_point.multiply(0.5f);
        return cross_point;
    }

    //calculate "a" and "b" ray-parameters in other system of coordinates
    void getRayParamsInOtherSOC(const std::vector<float> & rot_matrix, 
                                const Point3Df & trans_vector, 
                                Point3Df & new_a, Point3Df & new_b)
    {
        //---new_a
        new_a = mat2Point(rot_matrix, a);
        new_a.add(trans_vector);
        //---new_b
        Point3Df temp(a);
        temp.add(b);        
        new_b = mat2Point(rot_matrix, temp);
        new_b.add(trans_vector);
        new_b.minus(new_a);
    }

    void convertRayIntoOtherSOC(const std::vector<float> & rot_matrix,
                                const Point3Df & trans_vector)
    {
        Point3Df new_a;
        Point3Df new_b;
        getRayParamsInOtherSOC(rot_matrix, trans_vector, new_a, new_b);
        a = new_a;
        b = new_b;
    }


    //(3x3) x (3x1)
    Point3Df mat2Point(const std::vector<float> & matrix, const Point3Df & point)
    {
        float x = matrix[0]*point.x + matrix[1]*point.y + matrix[2]*point.z;        
        float y = matrix[3]*point.x + matrix[4]*point.y + matrix[5]*point.z;        
        float z = matrix[6]*point.x + matrix[7]*point.y + matrix[8]*point.z;
        Point3Df result(x, y, z);
        return result;
    }
};

float ray_t(const Point2Df &a, const Point2Df &b, const Point2Df &o);
struct Ray2Df
{
	Point2Df start;
	Point2Df guidingVector;
	const Point2Df end() const 
	{ 
		Point2Df endPoint = start;
		endPoint.add( guidingVector );
		return endPoint;
	}

	Ray2Df() {}
	Ray2Df( const Point2Df &startInput, const Point2Df &guidingVectorInput ) : start( startInput ), guidingVector(guidingVectorInput) {}
	Ray2Df( const Ray2Df &other ) { start = other.start; guidingVector = other.guidingVector; }
	Ray2Df( const Ray3Df &other ) { start = (Point2Df)(other.a); guidingVector = (Point2Df)(other.b) ; }
	Point2Df project(const Point2Df &other) const { 
		Point2Df temp(guidingVector);
		temp.multiply(ray_t(start,guidingVector,other ));
		temp.add(start);
		return temp;
	}
	Point2Df getPoint(float t) const {
		Point2Df ret(start);
		Point2Df bl(guidingVector);
		bl.multiply(t);
		ret.add(bl);

		return ret;
	}
	float angleCos( const Ray2Df &other ) const
	{ return guidingVector.product( other.guidingVector ) / ( guidingVector.norm() * other.guidingVector.norm() ); }
	float horizonAngleCos() const
	{ return guidingVector.x / ( guidingVector.norm() ); }

	float distance(const Point2Df &other) const { return project(other).distance(other);}
	// returns distance to the point, sets end of the ray to the nearest point
	float rayProject(const Point2Df &other) const {
		return ray_t(start,guidingVector,other);
	}
	Point2Df getNearest(const Point2Df & point) {
		float t = ray_t( start, guidingVector, point);
		return getPoint(t);
	}
	const bool isGuidingCollinear( const Ray2Df &other, const float precisionInput )
	{
		return ( fabs( guidingVector.x * other.guidingVector.y - guidingVector.y * other.guidingVector.x ) < precisionInput );
	}

	const int getCrossParameters( const Ray2Df &other, float &thisParameterOutput, float &otherParameterOutput ) const
	{
		thisParameterOutput = 0;
		otherParameterOutput = 0;

		const Point2Df startDifferenceVector = Point2Df( ( other.start.x - start.x ), ( other.start.y - start.y ) );
		const float detA = determinant( guidingVector, other.guidingVector.negative() );
		const float detA1 = determinant( startDifferenceVector, other.guidingVector.negative() );
		const float detA2 = determinant( guidingVector, startDifferenceVector );

		if( 0 == detA )
		{
			if( ( 0 == detA1 ) && ( 0 == detA2 ) ) // segments on one line
			{
				if( start == other.start ) // equal start points
				{
					return -3;
				}

				// add code here for accurate parameters definition
				return -2;
			}
			else // on parallel lines
			{
				return -1;
			}
		}

		thisParameterOutput = detA1 / detA;
		otherParameterOutput = detA2 / detA;
		return 0;
	}

	// the method for retrieving parameters of skew lines
	void raySkew( const Ray2Df &other, float *thisRayParameter, float *otherRayParameter ) const
	{
		if (otherRayParameter == NULL) return;
		else
		{ 
			float b1b1 = getDotProduct( guidingVector, guidingVector );
			float b2b2 = getDotProduct( other.guidingVector, other.guidingVector );
			float b1b2 = getDotProduct( guidingVector, other.guidingVector );
			if( b2b2*b1b1 == b1b2*b1b2 ) return;
			float k = b1b2 / b1b1;					
           			 Point2Df temp1(guidingVector);			
         			   temp1.multiply(k);			
         			   temp1.minus(other.guidingVector);			
         			   Point2Df temp2(other.start);					
         			   temp2.minus(start);					
         			   *otherRayParameter = getDotProduct(temp1, temp2);
			*otherRayParameter /= b2b2 - k * b1b2;
         			   if (thisRayParameter != NULL)            
          			      *thisRayParameter = k *(*otherRayParameter) + getDotProduct(guidingVector, temp2)/b1b1;  
		}


		/*r2.t = cvDotProduct(
			cvMinus(k,r1.b,r2.b),
			cvMinus(r2.a,r1.a)
			);
		r2.t /= b2b2 - k * b1b2;
		
		r1.t = k * r2.t + cvDotProduct(
			r1.b,
			cvMinus(r2.a,r1.a)
			)/b1b1;*/
		//return sqrtf(cvSquareDistance(r1.getPoint(), r2.getPoint()));
	}

	// method for dot product
	float getDotProduct( const Point2Df &first, const Point2Df &second ) const
	{
		const float product = first.x*second.x + first.y*second.y;
		return product;
	}

	//method for finding cross point
    Point2Df getSkewLinesNearestPoint(const Ray2Df &other)
    {
        float my_t = 1;
        float other_t = 1;
        raySkew(other, &my_t, &other_t);
        //my_point
        Point2Df my_point( guidingVector );
        my_point.multiply(my_t);
        my_point.add( start );
        //other point
        Point2Df other_point(other.guidingVector);      
        other_point.multiply(other_t);        
        other_point.add(other.start );
        //cross point
        Point2Df cross_point(my_point);
        cross_point.add(other_point);
        cross_point.multiply(0.5f);
        return cross_point;
    }
};

struct Segment3Df
{
	Point3Df start;
	Point3Df end;
	void add(const Point3Df &shift) {start.add(shift); end.add(shift); }

	Segment3Df() { start = Point3Df(0,0,0); end = Point3Df(0,0,0); }
	Segment3Df( const Point3Df &startInput, const Point3Df &endInput ) :
	start(startInput), end(endInput) {};
	Ray3Df convertToRay() const
	{
		Point3Df collinearVector = end;
		collinearVector.minus( start );
		Ray3Df resultingRay = Ray3Df( start, collinearVector );
		return resultingRay;
	}
};

struct Segment2Df { // not considering z coordinate
	Point2Df start;
	Point2Df end; // these don't need to be one less then the other

	Segment2Df() { start = Point2Df(0,0); end = Point2Df(0,0); }

	Segment2Df(const Point2Df &s, const Point2Df &e) : start(s), end(e) {};
	Segment2Df(const Segment2Df &other) { start = other.start; end = other.end;};
	Segment2Df(const Segment3Df &other) { start = ( Point2Df )other.start; end = ( Point2Df )other.end;}; // note: z coordinate is truncated!

	const float calculateLength() const { return start.distance( end ); }
	Ray2Df convertToRay() const
	{
		Point2Df collinearVector = end;
		collinearVector.minus( start );
		Ray2Df resultingRay = Ray2Df( start, collinearVector );
		return resultingRay;
	}

	bool contains(const Point2Df &pt) const {
		bool resultx = false, resulty = false;
		if (start.x <= pt.x && pt.x <= end.x) resultx = true;
		if (start.x >= pt.x && pt.x >= end.x) resultx = true;
		if (start.y <= pt.y && pt.y <= end.y) resulty = true;
		if (start.y >= pt.y && pt.y >= end.y) resulty = true;

		return resultx && resulty;
	};

	bool containsEps(const Point2Df &pt) const {
		float epsilon = 0.0001f;
		bool resultx = false, resulty = false;
		if (pt.x - start.x > 0 && pt.x - start.x < epsilon && end.x - pt.x > 0 &&  end.x - pt.x < epsilon) resultx = true;
		if (pt.x - start.x < 0 && pt.x - start.x > -epsilon && end.x - pt.x < 0 &&  end.x - pt.x > -epsilon) resultx = true;
		if (pt.y - start.y > 0 && pt.y - start.y < epsilon && end.y - pt.y > 0 &&  end.y - pt.y < epsilon) resulty = true;
		if (pt.y - start.y < 0 && pt.y - start.y > -epsilon && end.y - pt.y < 0 &&  end.y - pt.y > -epsilon) resulty = true;

		return resultx && resulty;
	};

	bool intersects(const Segment2Df &other, Point2Df &pInters) const {
		float epsilon = 0.0f; //0.0001f;
		Point2Df intersPt; intersPt.x = 0.0f; intersPt.y = 0.0f;
		float A = end.x - start.x;				float B = end.y - start.y;
		float C = other.end.x - other.start.x;	float D = other.end.y - other.start.y;

		if (fabs(C*B - A*D) <= epsilon) return false;
	
		if (fabs(B) <= epsilon) {
			intersPt.y = start.y; 
			intersPt.x = C / D *(start.y - other.start.y) + other.start.x;
		}
		else {
			intersPt.y = (start.x - other.start.x)*B*D + other.start.y*C*B - start.y*A*D;
			intersPt.y /= C*B - A*D;
			intersPt.x = start.x + (intersPt.y - start.y)*A / B;
		}

		pInters = intersPt;
		return contains(intersPt) && other.contains(intersPt);
	};

	bool intersects(const Segment2Df &other) const {
		Point2Df pInters;
		return intersects(other, pInters);
	};
};

struct Normal {
	Point3Df base;
	Point3Df normal;
	float error;

	Normal() {};
	Normal(const Normal &other) {base = other.base; normal = other.normal; error =  other.error;};
	Normal(const Point3Df &b, const Point3Df &n, float e) {base = b; normal = n; error = e;};
	void normalize() { normal.normate();};
	void stretch(float k) {normal.multiply(k);};
};

extern int debugcounter_intersects;
extern int debugcounter_contains;
struct Sphere : public ISphereSearchable {
	Point3Df center;
	float R;
	bool contains(const Sphere &other) const { return sqrt(center.sqdist(other.center)) + other.R < R; }
	//bool contains(const Point3Df &p, bool inclusive) const { 
	//	return inclusive ? center.sqdist(p) <= R*R : center.sqdist(p) < R*R; 
	//}
	bool operator ==(const Sphere &other) const { return R==other.R && center.sqdist(other.center) < 1.0; }
	bool contains(const Point3Df &p, bool inclusive) const { 
		//debugcounter_contains ++;
		return inclusive ?	center.sqdist(p) <= R*R :
							center.sqdist(p) < R*R; 
	}
	IntersectionType intersects(const Sphere &other, bool inclusive) const {
		//debugcounter_intersects++;
		float m = (R - other.R);
		float d = center.sqdist(other.center);
		if(inclusive	&& (m > 0 && d < m*m)) return ISphereSearchable::within;
		if(!inclusive	&& (m > 0 && d <=  m*m)) return ISphereSearchable::within;
		float r = (R + other.R);
		if(inclusive	&& (d <= r*r)) return  ISphereSearchable::intersection;
		if(!inclusive	&& (d <  r*r)) return  ISphereSearchable::intersection;
		return  ISphereSearchable::separated;
	}
	void save(FILE *file) const {
		fwrite(&this->center,sizeof(Point3Df),1,file);
		fwrite(&this->R,sizeof(float),1,file);
		//fwrite(this,sizeof(Sphere),1,file);
	}
	void load(FILE *file) {
		fread(&this->center,sizeof(Point3Df),1,file);
		fread(&this->R,sizeof(float),1,file);
		//fread(this,sizeof(Sphere),1,file);
	}

};

struct ZCylinder {
	double x;
	double y;
	double R;
	ZCylinder() {};
	ZCylinder(const Sphere &s) : x(double(s.center.x)), y(double(s.center.y)),R(s.R) {};
	ZCylinder(const pwn::Point3d &p, double RR) : x(double(p.x)), y(double(p.y)),R(RR) {};
	ZCylinder(const Point3Df &p, double RR) : x(double(p.x)), y(double(p.y)),R(RR) {};
	bool contains(const Sphere &other) const { return sqrt((x-other.center.x)*(x-other.center.x) + (y-other.center.y)*(y-other.center.y))+other.R <= R;}
	bool contains(const pwn::Point3d &other) const { return (x-other.x)*(x-other.x) + (y-other.y)*(y-other.y) <= R*R;}
	bool contains(const Point3Df &other) const { return (x-other.x)*(x-other.x) + (y-other.y)*(y-other.y) <= R*R;}
	inline bool intersects(const Sphere &other) const { return (x-other.center.x)*(x-other.center.x) + (y-other.center.y)*(y-other.center.y)<= (other.R + R)*(other.R + R);}
};
struct ZCylinderf : public ISphereSearchable {
	float x;
	float y;
	float R;
	ZCylinderf() {};
	ZCylinderf(const Sphere &s) : x(float(s.center.x)), y(float(s.center.y)),R(s.R) {};
	ZCylinderf(const pwn::Point3d &p, float RR) : x(float(p.x)), y(float(p.y)),R(RR) {};
	ZCylinderf(const Point3Df &p, float RR) : x(float(p.x)), y(float(p.y)),R(RR) {};
	bool contains(const Sphere &other) const { return sqrt((x-other.center.x)*(x-other.center.x) + (y-other.center.y)*(y-other.center.y))+other.R <= R;}
	bool contains(const pwn::Point3d &other) const { return (x-other.x)*(x-other.x) + (y-other.y)*(y-other.y) <= R*R;}
	bool contains(const Point3Df &other) const { return (x-other.x)*(x-other.x) + (y-other.y)*(y-other.y) <= R*R;}
	bool contains(const Point3Df &other, bool inclusive) const { 
		return inclusive ? (x-other.x)*(x-other.x) + (y-other.y)*(y-other.y) <= R*R :
		(x-other.x)*(x-other.x) + (y-other.y)*(y-other.y) < R*R;
	}
	IntersectionType intersects(const Sphere &other, bool inclusive) const {
		float m = (R - other.R);
		float d = (x-other.center.x)*(x-other.center.x) + (y-other.center.y)*(y-other.center.y);
		if(inclusive	&& (m > 0 && d < m*m)) return ISphereSearchable::within;
		if(!inclusive	&& (m > 0 && d <=  m*m)) return ISphereSearchable::within;
		float r = (R + other.R);
		if(inclusive	&& (d <= r*r)) return  ISphereSearchable::intersection;
		if(!inclusive	&& (d <  r*r)) return  ISphereSearchable::intersection;
		return  ISphereSearchable::separated;
	}
};

struct Rect2Df {
	float top; // max y
	float bottom; //min y
	float left;
	float right;

	Rect2Df() : top(0), bottom(0), left(0), right(0) {};
	Rect2Df(float t, float b, float l, float r) : top(t), bottom(b), left(l), right(r) {};
	Rect2Df(const Rect2Df & other) : top(other.top), bottom(other.bottom), left(other.left), right(other.right) {  }  

	template <class T>
	Rect2Df( const T &lb, const T &rt ) : top(float(rt.y)), bottom(float(lb.y)), left(float(lb.x)), right(float(rt.x)){}

	template <class T>
	Rect2Df(const std::vector <T> & set) {
		if (set.empty()) throw ("couldn't init rectangle\n");
		const T *ptr = &set[0];
		const size_t setsize = set.size();
		left = float(ptr->x), right = float(ptr->x);
		bottom = float(ptr->y), top = float(ptr->y);
		for (size_t i = 1; i < setsize; i++) {
			const float x = float(ptr[i].x);
			const float y = float(ptr[i].y);
			if (x < left) left = x;
			if (x > right) right = x;
			if (y < bottom) bottom = y;
			if (y > top) top = y;		
		}
	}
	void set(float val) {
		left = val;		
		top = val; 
		right = val;
		bottom = val; 		 
	}
	void shift(float x, float y) {
		left += x; right += x;
		top += y; bottom += y;
	}

	void scale(float s){
		left *= s; right *= s;
		top *= s; bottom *= s;
	}

	inline float sqr(float x) const { return x*x; }
	float sqdistance(const Rect2Df &other) const {
		float distance;
		Point2Df thisCenter, otherCenter;
		thisCenter = this->getCenter();
		otherCenter = other.getCenter();
		distance = sqr(thisCenter.x -		otherCenter.x) + 
			sqr(thisCenter.y -		otherCenter.y) + 
			sqr(bottom - top -	(other.bottom - other.top)) + 
			sqr(right -	left -	(other.right - other.left));
		return distance;
	}

	void expand(float border) {
		top += border;
		bottom -=border;
		left -= border;
		right += border;
	}
	void expandto(float size_fraction) {
		if (size_fraction < -1.0f) throw "Wrong size_fraction\n";
		float coeff = 1.0f + size_fraction;
		Point2Df src_center = getCenter();
		shift(-src_center.x, -src_center.y);
		scale(coeff);
		shift(src_center.x, src_center.y);
	}
	void expandto(const Rect2Df &other) {
		top = std::max<float>(top,other.top);
		bottom = std::min<float>(bottom,other.bottom);
		left = std::min<float>(left, other.left);
		right = std::max<float>(right, other.right);
	}

	float diagonal() const {return sqrtf(width()*width() + height()*height());}

//1. Check if box A is completely to the left of box B (a.left <= b.right). If so, then quit the process: they don't intersect.
//2. Otherwise, check if it's completely to the right  (a.right >= b.left). If so, quit the process.
//3. If not, check if it's completely above box     B  (a.bottom >= b.top). If so, quit.
//4. If not, check if it's completely beneath box B    (a.top <= b.bottom). If so, quit.
//If not, then the boxes intersect.
	bool intersects(const Rect2Df &b) const {
		if (left > b.right) return false;
		if (right < b.left) return false;
		if (bottom > b.top) return false;
		if (top < b.bottom) return false;
		return true;
	}
	bool intersects( const Segment2Df &line ) const {
		std::vector<Point2Df> vertices;
		createPolygon(vertices);
		if( contains(line.start) )	return true;
		if( Segment2Df( vertices[0], vertices[1] ).intersects(line) )	return true;
		if( Segment2Df( vertices[1], vertices[2] ).intersects(line) )	return true;
		if( Segment2Df( vertices[2], vertices[3] ).intersects(line) )	return true;
		if( Segment2Df( vertices[3], vertices[0] ).intersects(line) )	return true;
		return false;
	}
	template <class T>
	bool contains(const T &other) const {
		float xStart = left; 
		float xEnd = right; 
		float yStart = bottom; 
		float yEnd = top;
		return other.x >= xStart && other.x <= xEnd && other.y >= yStart && other.y <= yEnd;;
	}
	
	inline bool contains(const Rect2Df & other) const {
		Point2Df other_vertices[4];
		other.get_vertices(other_vertices);

		for (int i = 0; i<4; i ++) {
			if ( contains( other_vertices[i] ) == false ) return false;		
		}

		return true;
	}
	
	float width() const {return right - left;}
	float height() const { return top- bottom;} // note top > bottom
	float area() const {return width()*height();}	
	float aspect() const {
		float w = width();
		float h = height();
		if (h) return w/h;		
		throw("Incorrect rect aspect\n");	
	}
	void print() const {
		printf("RoughRect:\n");
		printf("top  = %f; bottom = %f\n", top, bottom);
		printf("left = %f; right  = %f\n", left, right);
	}

	void print_to_file(const char* fname) const {
		FILE* file = fopen(fname, "at+");
		fprintf(file,"RoughRect:\n");
		fprintf(file,"top  = %f; bottom = %f\n", top, bottom);
		fprintf(file,"left = %f; right  = %f\n", left, right);
		fclose(file);
	}

	Point2Df getCenter() const { return Point2Df( (left+right)*0.5f, (bottom+top)*0.5f ); }

	inline void get_vertices(Point2Df vertices[4]) const {
		vertices[0] = Point2Df(this->left, this->bottom);
		vertices[1] = Point2Df(this->left, this->top);
		vertices[2] = Point2Df(this->right, this->top);
		vertices[3] = Point2Df(this->right, this->bottom);	
	}

	void createPolygon( std::vector<Point2Df> &vertices ) const {
		vertices.resize(4);
		vertices[0] = Point2Df( left, bottom );
		vertices[1] = Point2Df( left, top );
		vertices[2] = Point2Df( right, top );
		vertices[3] = Point2Df( right, bottom );
	}

	inline bool operator == (const Rect2Df &other) const {
		bool top_ok = std::fabs(top - other.top) < FLT_EPSILON;
		bool bot_ok = std::fabs(bottom - other.bottom) < FLT_EPSILON;
		bool left_ok = std::fabs(left - other.left) < FLT_EPSILON;
		bool right_ok = std::fabs(right - other.right) < FLT_EPSILON;

		return top_ok && bot_ok && left_ok && right_ok;
	}

	inline bool operator != (const Rect2Df &other) const {
		return !(*this==other);
	}
	friend std::ostream &operator<<( std::ostream &output, 
                                       const Rect2Df &p )
      { 
		  output<<p.top<<" "<<p.bottom<<" "<<p.left<<" "<<p.right;
         return output;            
      }
};

//a*x + b*y + d = c*z
float planeDistance(const Point3Df &p, float *abcd); 
//a*x + b*y + d = c*z
float sgnplaneDistance(const Point3Df &p, float *abcd);

// note: equation ax + by + cz + d = 0;
const Point3Df planesCrossPoint( const float *abcd1, const float *abcd2 );
const Point3Df planesCrossVector( const float *abcd1, const float *abcd2 );
const Ray3Df planesCross( const float *abcd1, const float *abcd2 );

bool checkPointsInLine(const std::vector <Point3Df> &p, float threshold);
Point3Df createOrtoBasis(const Point3Df &p0, const Point3Df &p1);
Point3Df findAngles(const Point3Df &p1, const Point3Df &p2); //returns angles in radians between vector projections on respective planes: x - OYZ, y - OXZ, z - OXY
//Point3Df findRotationAngles(const Point3Df &p);

float rndf();
Point3Df planeProject(const Point3Df &normal, float d, const Point3Df &p); // this is for plane (x,n) - d = 0
void randomizePoints(const std::vector <Point3Df> &input, std::vector <Point3Df> &output) ;

class PointsCloud {
protected:
	std::vector<Point3Df> points;
	std::vector<size_t> segments;
public:
	PointsCloud() {}
	PointsCloud( std::vector<Point3Df> points, std::vector<size_t> segments ) : points(points),segments(segments) {}
	PointsCloud( const std::vector<Point3Df> &_points) : points(_points),segments(1,points.size()) {} // represent single segment cloud
	~PointsCloud() {}
	void loadBinTrain( const char *filename , bool strict = true);
	void saveBinTrain( const char *filename ) const;
	const std::vector<Point3Df> &getPoints() const { return points; }
	const std::vector<size_t> &getSegments() const {	return segments;}
	void setPoints( const std::vector<Point3Df> &pointsin ) {	points = pointsin;	}
	void setSegments( const std::vector<size_t> &segmentsin )	{	segments = segmentsin;	}
};

struct Parallelepipedf {
	Point2Df x, y, z;	// for each x,y,z: first dim is A, second dim is (B-A)
	Parallelepipedf( const std::vector<Point3Df> &points )	{
		std::vector<Point3Df>::const_iterator it=points.begin();
		x.x = (*it).x;	x.y = (*it).x;
		y.x = (*it).y;	y.y = (*it).y;
		z.x = (*it).z;	z.y = (*it).z;
		for( it=points.begin(); it!=points.end(); ++it )
		{
			if( (*it).x < x.x )	x.x = (*it).x;
			if( (*it).x > x.y )	x.y = (*it).x;
			if( (*it).y < y.x )	y.x = (*it).y;
			if( (*it).y > y.y )	y.y = (*it).y;
			if( (*it).z < z.x )	z.x = (*it).z;
			if( (*it).z > z.y )	z.y = (*it).z;
		}
		x.y -= x.x;
		y.y -= y.x;
		z.y -= z.x;
	}
	Parallelepipedf() : x(Point2Df(0,1)), y(Point2Df(0,1)), z(Point2Df(0,1)) {}
	Parallelepipedf( const Point2Df &x, const Point2Df &y, const Point2Df &z) : x(x), y(y), z(z) { check(); }
	Parallelepipedf( const Rect2Df &dims, const Point2Df z ) : 
		x( Point2Df(dims.left,dims.width()) ), 
		y( Point2Df(dims.bottom,dims.height()) ),
		z( Point2Df(z.x, z.y-z.x) ){ check(); };
	Rect2Df getBase() const {	return Rect2Df( y.x+y.y, y.x, x.x, x.x+x.y );	};
	inline float transformToX( const float xin ) const {	return (xin-x.x)/x.y;	}
	inline float transformToY( const float yin ) const {	return (yin-y.x)/y.y;	}
	inline float transformToZ( const float zin ) const {	return (zin-z.x)/z.y;	}
	inline float transformBackX( const float xin ) const {	return xin*x.y+x.x;	}
	inline float transformBackY( const float yin ) const {	return yin*y.y+y.x;	}
	inline float transformBackZ( const float zin ) const {	return zin*z.y+z.x;	}
	void transformToXY( const float xin, const float yin, float *v ) const {	v[0]=transformToX(xin);	v[1]=transformToY(yin);	}
	inline void check() const {
		if( x.y==0 || y.y==0 || z.y==0 )
			throw("Parallelepipedf zero width check failed!");
	}
	Point3Df transformToXYZ( const Point3Df &p ) const {
		return Point3Df( transformToX(p.x), transformToY(p.y), transformToZ(p.z) );	
	}
	Point3Df transformPlaneTo( const float wx, const float wy, const float d ) const {
		Point3Df out;
		out.x = wx*x.y/z.y;
		out.y = wy*y.y/z.y;
		out.z = (d - z.x + wx*x.x + wy*y.x)/z.y;
		return out;
	}
	Point3Df transformPlaneBack( const float wx, const float wy, const float d ) const {
		Point3Df out;
		out.x = wx/x.y/z.y;
		out.y = wy/y.y/z.y;
		out.z = d/z.y + z.x - out.x*x.x - out.y*y.x;
		return out;
	}
	static Point2Df findZDims( const std::vector<Point3Df> &points )	{
		if( points.empty() )
			throw("Parallelepipedf::findZDims called with empty point cloud!");
		float zmax=points[0].z, zmin=points[0].z;
		for( std::vector<Point3Df>::const_iterator it=points.begin(); it!=points.end(); ++it )	{
			if( (*it).z < zmin ) zmin = (*it).z;
			if( (*it).z > zmax ) zmax = (*it).z;
		}
		return Point2Df( zmin, zmax );
	}
};

class PolygonScaling {
public:
	PolygonScaling() {}
	virtual  ~PolygonScaling() {}

	template< typename T >
	static void scale(const std::vector < T >& in, std::vector < T >& out, float scaleVal) {
		out.clear();
		const size_t points_count = in.size();
		if(points_count == 0) {
			return;
		}

		T center; 
		center.x = 0; 
		center.y = 0;
		for(size_t i = 0; i < points_count; ++i) {
			const T& p = in[i];
			center.x += p.x;
			center.y += p.y;
		}
		center.x /= points_count;
		center.y /= points_count;

		out = in;
		for(size_t i = 0; i < points_count; ++i) {
			T& p = out[i];
			p.x -= center.x;
			p.y -= center.y;
		}
		for(size_t i = 0; i < points_count; ++i) {
			T& p = out[i];
			p.x *= scaleVal;
			p.y *= scaleVal;
		}
		for(size_t i = 0; i < points_count; ++i) {
			T& p = out[i];
			p.x += center.x;
			p.y += center.y;
		}
	}
};

std::pair<Point3Df,Point3Df> getMinMax(std::vector<Point3Df> &points);


