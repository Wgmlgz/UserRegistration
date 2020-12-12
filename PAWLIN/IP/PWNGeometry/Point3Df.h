// (c) Pawlin Technologies Ltd.
// http://www.pawlin.ru 
// 
// File: Point3Df.h
// Creation date: 01.12.2015
// Purpose: basic 3D point structure
// Author: Pavel Skribtsov
// 
// ALL RIGHTS RESERVED. USAGE OF THIS FILE FOR ANY REASON 
// WITHOUT WRITTEN PERMISSION FROM 
// PAWLIN TECHNOLOGIES LTD IS PROHIBITED 
// ANY VIOLATION OF THE COPYRIGHT AND PATENT LAW WILL BE PROSECUTED 
// FOR ADDITIONAL INFORMATION REFER http://www.pawlin.ru 
// ---------------------------------------------- 

#pragma once
#include <stdio.h>
#include <cmath>
#include <float.h>
#include <stdlib.h>
#include <PWMath/minmaxavg.h>
#include <stdint.h>
#include <vector>


#ifndef STM32DRONE

#include <ostream>
#include <iomanip>
#endif
struct pwn_double3 {
	double p[3];
	double & operator [] (int i) { return p[i]; }
	const double & operator [] (int i) const { return p[i]; }
	double sqnorm() const { return p[0]*p[0]+p[1]*p[1]+p[2]*p[2];}
	pwn_double3 operator - (const pwn_double3 &pt) const { pwn_double3 r; r[0] = p[0] - pt[0];r[1] = p[1] - pt[1];r[2] = p[2] - pt[2]; return r;}
};

struct Point3Df {
	float x;
	float y;
	float z;
    //float padding;
	Point3Df ();
	Point3Df (float d, float b, float c);
	//Point3Df (const CvPoint3D32f &other) {x = (float)other.x; y = (float)other.y; z = (float)other.z;}
	Point3Df (const Point3Df &other);
	//Point3Df (const Point3d &other) {x = (float)other.x; y = (float)other.y; z = (float)other.z;}
	Point3Df(const Point3Df &p1,const Point3Df &p2);
	void push_into(std::vector<float> &v) const { v.push_back(x); v.push_back(y); v.push_back(z); }
	inline float getX() const { return x;}

	inline float getY() const{ return y;}

	inline float getZ() const { return z;}

	inline void add(const Point3Df &other) {x+=other.x; y+=other.y; z+=other.z;}
	inline void add(float k) { x += k; y += k; z += k; }

	inline void addWeighted(const Point3Df &other, float w) {x+=w*other.x; y+=w*other.y; z+=w*other.z;}

	inline void minus(const Point3Df &other) {x-=other.x; y-=other.y; z-=other.z;}

	inline void negate() { x*=-1.0f; y*=-1.0f; z*=-1.0f;}

	inline void multiply(float k) { x=(float)(x*k); y=(float)(y*k); z=(float)(z*k);}

	inline void multiply(const Point3Df & other) {x *= other.x; y *= other.y; z *= other.z;}	
	inline Point3Df multiplied(const Point3Df & other) const {Point3Df temp = *this; temp.multiply(other); return temp;}	
	float product(const Point3Df &other) const;

	Point3Df vecproduct(const Point3Df &other) const;
	inline float norm() const { return sqrtf(x*x+y*y+z*z);}
	inline float maxabs() const { return std::max<float>(fabsf(x), std::max<float>(fabsf(y), fabsf(z))); }
	inline float minabs() const { return std::min<float>(fabsf(x), std::min<float>(fabsf(y), fabsf(z))); }
	inline float normsqare() const { return x*x+y*y+z*z;}

	inline float normate() { float k = norm(); if(k == 0) return 0; float invK = 1.0f/k; this->multiply(invK); return k;}
	inline Point3Df normalized() const { Point3Df p(x,y,z); p.normate(); return p; }

	inline float distance(const Point3Df &other) const {Point3Df d(other); d.minus(*this); return d.norm();}

	inline float distancexy(const Point3Df &other) const {Point3Df d(other); d.minus(*this); d.z = 0.0f; return d.norm();}

	inline Point3Df multiplied(float k) const { return Point3Df((float)(x*k),(float)(y*k),(float)(z*k));}

	inline Point3Df negative() {return Point3Df(-1.0f*x,-1.0f*y,-1.0f*z);}

	inline Point3Df getNormal(const Point3Df &p1,const Point3Df &p2) const { Point3Df t1(p1); Point3Df t2(p2); t1.minus(*this); t2.minus(*this); return Point3Df(t1,t2);}

	inline bool operator == (const Point3Df &other) const {return x==other.x && y==other.y && z==other.z;}

	inline bool operator != (const Point3Df &other) const {return x!=other.x && y!=other.y && z!=other.z;}

	inline float operator [] (const int i) const {
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		throw ("Index is out of bound! Index must take values: 0 - x coord, 1 - y coord, 2 - z coord\n");
	};

	inline float & operator [] (const int i) {
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		throw ("Index is out of bound! Index must take values: 0 - x coord, 1 - y coord, 2 - z coord\n");
	};

	Point3Df& operator = (const Point3Df &other);
	inline float sqdist(const Point3Df &other) const {return float(other.x-x)*float(other.x-x) + float(other.y-y)*float(other.y-y)+float(other.z-z)*float(other.z-z);}
	float sqdist(const pwn_double3 &other) const;
	void print() const;  //---SinDM
	void takeMin(const Point3Df &other);
	void takeMax(const Point3Df &other);
	const bool isZero() const; 
	const bool equals(const Point3Df &other) const;
	void floor(void); //---SinDM
	void round(void); //---SinDM
	bool operator < (const Point3Df &other) const;
	Point3Df operator -() const { return this->multiplied(-1.0f); }
	Point3Df operator + (const Point3Df &other) const;
	Point3Df operator + (float k) const;
	Point3Df operator - (const Point3Df &other) const;
	Point3Df operator * (const float k) const;
	Point3Df operator / (const float k) const;

#if !STM32DRONE && !STM32DRONE_V3  && !__MINGW32__ && !LINUX
	void operator<<( std::ostream &stream){
		stream<<std::setprecision(8)<<x<<" "<<std::setprecision(8)<<y<<" "<<std::setprecision(8)<<z;
	};
	friend std::ostream &operator<<( std::ostream &output, 
                                       const Point3Df &p )
      { 
		 output<<p.getX()<<" "<<p.getY()<<" "<<p.getZ();
         return output;            
      }
	friend std::istream &operator>>( std::istream &input, 
                                        Point3Df &p )
      { 
		
		  input >> p.x >> p.y >> p.z;
         return input;            
      }
#endif

};

Point3Df perComponentMax(const Point3Df &a,const Point3Df &b);
Point3Df perComponentMin(const Point3Df &a,const Point3Df &b);

class MinMaxAvgPoint3Df {
public:
	MinMaxAvg x;
	MinMaxAvg y;
	MinMaxAvg z;
	void print() const {
		printf("X:\n");
		x.print(true);
		printf("Y:\n");
		y.print(true);
		printf("Z:\n");
		z.print(true);
	}
	void take(const Point3Df &pt) {
		x.take(pt.x);
		y.take(pt.y);
		z.take(pt.z);
	}
	void take(const Point3Df &pt, int n) {
		x.take(pt.x,n);
		y.take(pt.y,n);
		z.take(pt.z,n);
	}
	Point3Df getMin() const {
		return Point3Df(x.minv, y.minv, z.minv);
	}
	Point3Df getMax() const {
		return Point3Df(x.maxv, y.maxv, z.maxv);
	}
	Point3Df getStdev() const {
		return Point3Df(
			x.getStdev(),
			y.getStdev(),
			z.getStdev());
	}
	Point3Df getAvg() const {
		return Point3Df(
			x.getAvg(),
			y.getAvg(),
			z.getAvg());
	}	
	Point3Df getMinMaxMiddle() const {
		return Point3Df(
			x.getMinMaxMiddle(),
			y.getMinMaxMiddle(),
			z.getMinMaxMiddle());
	}
};

class MinMaxAvgPoint3DfExt : public MinMaxAvgPoint3Df {
public:
	MinMaxAvg diff;
	Point3Df last;
	void take(const Point3Df &p) {
		
		if (x.counter) {
//			diff.take(p.x*last.x*3);
			diff.take(p.product(last));
//			diff.take((p-last).maxabs());
		}
		MinMaxAvgPoint3Df::take(p);
		last = p;
	}
};

struct Point3DfTime : public Point3Df
{
	uint64_t time;
	Point3DfTime():time(0) {}
	Point3DfTime(const Point3Df &p, uint64_t ts) : Point3Df(p), time(ts) {}
};

struct Point3DfTemp : public Point3Df
{
	float temp;
	Point3DfTemp() :temp(0) {}
	Point3DfTemp(const Point3Df &p, float temp) : Point3Df(p), temp(temp) {}
};

struct Point3DfColor : public Point3Df
{
	Point3Df color;
	Point3DfColor() {}
	Point3DfColor(const Point3Df &p, const Point3Df &color) : Point3Df(p), color(color) {}
};

struct Point3DfTimeTemp : public Point3Df
{
	uint64_t time;
	float temp;
	Point3DfTimeTemp() :time(0), temp(0) {}
	Point3DfTimeTemp(const Point3Df &p, uint64_t ts, float temp) : Point3Df(p), time(ts), temp(temp) {}
};