// (c) Pawlin Technologies Ltd.
// http://www.pawlin.ru 
// 
// File: Point3Df.cpp
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
#include "stdafx.h"
#include "Point3Df.h"

//#include <PWNGeometry/Eigen/Geometry>
Point3Df::Point3Df() {x = 0; y = 0; z = 0;}

Point3Df::Point3Df(float d, float b, float c) {x = d; y = b; z = c;}

Point3Df::Point3Df(const Point3Df &other) {x = other.x; y = other.y; z = other.z;}

Point3Df::Point3Df(const Point3Df &p1,const Point3Df &p2) { x = p1.y*p2.z - p1.z*p2.y; y = p1.z*p2.x - p1.x*p2.z; z = p1.x*p2.y - p1.y*p2.x;}



float Point3Df::product(const Point3Df &other) const { return (float)(other.x*x+other.y*y+other.z*z);}

Point3Df Point3Df::vecproduct(const Point3Df &other) const {return Point3Df(*this,other);}



Point3Df& Point3Df::operator = (const Point3Df &other) {this->x = other.x; this->y = other.y; this->z = other.z; return *this;}


void Point3Df::print() const {printf("X: %.6f  Y: %.6f  Z: %.6f\n", x, y, z);}

void Point3Df::takeMin(const Point3Df &other) {
	x = std::min<float>(x,other.x);
	y = std::min<float>(y,other.y);
	z = std::min<float>(z,other.z);

}

void Point3Df::takeMax(const Point3Df &other) {

}

const bool Point3Df::isZero() const {return (x==0 && y==0 && z==0);}

const bool Point3Df::equals(const Point3Df &other) const { return ( (x == other.x) && (y == other.y) && (z == other.z) ); }

void Point3Df::floor(void) {x = floorf(x); y = floorf(y); z = floorf(z);}

void Point3Df::round(void) {x = floorf(x + 0.5f); y = floorf(y + 0.5f); z = floorf(z + 0.5f);}

bool Point3Df::operator < (const Point3Df &other) const { // for sorting, mapping purposes
	if(x > other.x) return false;
	if(x < other.x) return true;
	if(y > other.y) return false;
	if(y < other.y) return true;
	if(z >= other.z) return false;
	return true;
}

Point3Df Point3Df::operator + (const Point3Df &other) const {	Point3Df res = *this;	res.add(other);	return res;	}

Point3Df Point3Df::operator + (const float k) const { Point3Df res = *this;	res.add(k);	return res; }

Point3Df Point3Df::operator - (const Point3Df &other) const {	Point3Df res = *this;	res.minus(other);	return res;	}

Point3Df Point3Df::operator * (const float k) const {	return this->multiplied(k);	}

Point3Df Point3Df::operator / (const float k) const { 
	Point3Df res = *this;
	res.x /= k;
	res.y /= k;
	res.z /= k;
	return res;
}

Point3Df perComponentMax(const Point3Df &a,const Point3Df &b){
	Point3Df result;

	return result;
}
Point3Df perComponentMin(const Point3Df &a,const Point3Df &b){
	Point3Df result;

	return result;
}

