// (c) Pawlin Technologies Ltd 
// File: geometricobjects.cpp
// Date: 15-AUG-2008
// Authors: Svetlana Korobkova, Pavel Skribtsov
// Purpose: geometric objects operations realization

// ALL RIGHTS RESERVED, USAGE WITHOUT WRITTEN PERMISSION FROM PAWLIN TECHNOLOGIES LTD. is prohibited

#include "stdafx.h"

#include "geometricobjects.h"
#include <iostream>
#include <stdint.h>

#ifdef __linux__
#include <stdlib.h>
#endif
//#include <PWNGeneral/pwnutil.h>


Point2Df Basis2Df::wcs2ocs(const Point2Df &wp) const {
	Point2Df d = wp - c;
	return Point2Df(d.product(e1)*e1m,d.product(e2)*e2m);
}
Point2Df Basis2Df::ocs2wcs(const Point2Df &op) const {
	return c + (e1*op.x+e2*op.y);
}

Point2Df PolarBasis2Df::wcs2ocs(const Point2Df &wp) const {
	Point2Df d = wp - c;
	float angle = atan2f(d.y,d.x);//takes into account signs of x and y already
	float r = sqrtf(d.x*d.x + d.y*d.y);
	return Point2Df(r,angle);
}
Point2Df PolarBasis2Df::ocs2wcs(const Point2Df &op) const {
	float r =  op.x;
	float angle = op.y;
	return c + Point2Df(r*cosf(angle), r*sinf(angle));
}

Point2Df PolarBasis2Df_ref_angle::wcs2ocs(const Point2Df &wp) const {
	Point2Df d = wp - c;
	float angle = atan2f(d.y,d.x);//takes into account signs of x and y already
	float r = sqrtf(d.x*d.x + d.y*d.y);

	Point2Df ref_d = ref_center - c;
	float ref_angle = atan2f(ref_d.y, ref_d.x);
	//if(verbose) printf("ref_angle = %f\n", ref_angle);
	
	float pi = 3.1459265f;
	
	/*float delta_a = 0.0f;
	float delta_r = 0.0f;
	delta_a =  min(pi - fabs(angle), fabs(angle));
	delta_r =  min(pi - fabs(ref_angle), fabs(ref_angle));*/

	//if (ref_angle > 0) {
	
	float angle_o = angle;
	if (angle_o < 0) {
		float delta = pi - fabs(angle_o);
		angle_o =  pi + delta;
	}

	float ref_angle_o = ref_angle;
	if (ref_angle_o < 0) {
		float delta = pi - fabs(ref_angle_o);
		ref_angle_o =  pi + delta;
	}

	float diff = angle_o - ref_angle_o;
	float comp_val = std::min<float>(fabsf(diff), 2.0f*pi - fabsf(diff) );		
	if(diff < 0) comp_val *= -1.0f;	

	diff = comp_val;

	float sign = 1.0f;

	if ( (0 < ref_angle) && (ref_angle < pi/2.0f) && (0 > angle) && (angle > -pi/2.0f) ) {
		sign = -1.0f;
	}

	if ( (0 < angle) && (angle < pi/2.0f) && (0 > ref_angle) && (ref_angle > -pi/2.0f) ) {
		sign = -1.0f;
	}

	diff *= sign;
	
	return Point2Df(r,diff);	
}

double pwn::Point3d::sqdist(const pwn_double3 &other) const {
	return double(other[0]-x)*double(other[0]-x) + double(other[1]-y)*double(other[1]-y)+double(other[2]-z)*double(other[2]-z);
}

bool ZcomparePoints3Df(const Point3Df &a, const Point3Df &b) {return a.z < b.z; }

float ray_t(const Point2Df &a, const Point2Df &b, const Point2Df &o)
{
	Point2Df v(o);
	v.minus(a);
	return v.product(b)/b.product(b);
}

float ray_t(const Point3Df &a, const Point3Df &b, const Point3Df &o) {
	Point3Df v(o);
	v.minus(a);
	return v.product(b) / b.product(b);
}

//a*x + b*y + c + d*z = 0
float planeDistance(const Point3Df &p, float *abcd) { 
	float nom = fabs(abcd[0]*p.x + abcd[1]*p.y + abcd[3]*p.z + abcd[2]); 
	float denom = sqrt(abcd[0]*abcd[0] + abcd[1]*abcd[1] + abcd[3]*abcd[3]);
	return nom/denom;
}

//a*x + b*y + c + d*z = 0
float sgnplaneDistance(const Point3Df &p, float *abcd) {
	float nom = -abcd[0]*p.x - abcd[1]*p.y - abcd[3]*p.z - abcd[2]; 
	float denom = sqrt(abcd[0]*abcd[0] + abcd[1]*abcd[1] + abcd[3]*abcd[3]);
	return nom/denom;
}

// note: equation ax + by + cz + d = 0;
// point where z = 0
const Point3Df planesCrossPoint( const float *abcd1, const float *abcd2 )
{
	const float detAB = abcd1[0]*abcd2[1] - abcd1[1]*abcd2[0];
	if( 0 == detAB )
	{
		throw "Badly defined planes!";
	}

	const float detAD = abcd1[0]*abcd2[3] - abcd1[3]*abcd2[0];
	const float detBD = abcd1[1]*abcd2[3] - abcd1[3]*abcd2[1];

	const float x = detAD / detAB;
	const float y = detBD / detAB;
	Point3Df crossPoint( x, y, 0.0f );
	return crossPoint;
}

// note: equation ax + by + cz + d = 0;
const Point3Df planesCrossVector( const float *abcd1, const float *abcd2 )
{
	return Point3Df ( Point3Df( abcd1[0], abcd1[1], abcd1[2] ), Point3Df( abcd2[0], abcd2[1], abcd2[2] ) );
}

// note: equation ax + by + cz + d = 0;
const Ray3Df planesCross( const float *abcd1, const float *abcd2 )
{
	const Point3Df crossPointZeroZ = planesCrossPoint( abcd1, abcd2 );
	const Point3Df collinearVector = planesCrossVector( abcd1, abcd2 );
	return Ray3Df( crossPointZeroZ, collinearVector );
}

bool boolVecSum (std::vector <bool> &inp) {
	for (unsigned i = 0; i < inp.size(); i++) {
		if (!inp[i]) return false;
	}

	return true;
}

float maxVecValue(std::vector <float> &inp) {
	if (inp.size() == 0) return 0.0f;
	float maxv = inp[0];
	for (unsigned i = 1; i < inp.size(); i++) {
		if (maxv < inp[i]) maxv = inp[i];
	}

	return maxv;
}

// this is for plane (x,n) - d = 0
Point3Df planeProject(const Point3Df &normal, float d, const Point3Df &p) {
	float pn = p.product(normal);
	float nn = normal.product(normal);
	if (nn ==0.0f) throw ("planeProject : zero normal");
	Point3Df k(p);
	k.add(normal.multiplied((d-pn)/nn));
	return k;
}

float rndf() { 
	return (float)rand()/float(RAND_MAX);
}

void randomizePoints(const std::vector <Point3Df> &input, std::vector <Point3Df> &output) {
	output.clear();
	output = input;
	unsigned size = (unsigned)input.size();
	for (unsigned i = 0; i < size; i++){
		unsigned num1 = rand() % size; // random first index
		Point3Df tmpP;
		tmpP = output[i];
		output[i] = output[num1];
		output[num1] = tmpP;
	}
}

bool checkPointsInLine(const std::vector <Point3Df> &p, float threshold) {
	Point3Df a,b,c;
	a = p[0]; b = p[1];
	b.minus(a); b.normate();
	for (unsigned i = 2; i < p.size(); i++) {
		c = p[i];
		c.minus(a); c.normate();
		Point3Df m(b,c);
		float d = m.norm();
		if (d > threshold) return false;
	}
	return true;
}

Point3Df createOrtoBasis(const Point3Df &p0, const Point3Df &p1) {
	Point3Df a(p1),z(0.0f, 0.0f, 1.0f);
	a.minus(p0);
	Point3Df pnt(a,z);
	if(pnt.product(pnt)==0) { // two points are vertical
		float n = a.norm();
		pnt.x = n;
		pnt.y = n; // form XY-plane
	}
	pnt.add(p0);
	return pnt;
}

Point3Df findAngles(const Point3Df &p1, const Point3Df &p2) {
	Point3Df angles(0.0f, 0.0f, 0.0f);

	float p1OXYmod = sqrtf(p1.x*p1.x + p1.y*p1.y); 
	float p1OXZmod = sqrtf(p1.x*p1.x + p1.z*p1.z); 
	float p1OYZmod = sqrtf(p1.y*p1.y + p1.z*p1.z); 

	float p2OXYmod = sqrtf(p2.x*p2.x + p2.y*p2.y); 
	float p2OXZmod = sqrtf(p2.x*p2.x + p2.z*p2.z); 
	float p2OYZmod = sqrtf(p2.y*p2.y + p2.z*p2.z); 

	//OXY proj = rotation around Z
	float cosA = (p1OXYmod > 0 && p2OXYmod > 0) ? ( (p1.x*p2.x + p1.y*p2.y) / (p1OXYmod*p2OXYmod) ) : 1.0f;
	//OXZ proj = rotation around Y
	float cosB = (p1OXZmod > 0 && p2OXZmod > 0) ? ( (p1.x*p2.x + p1.z*p2.z) / (p1OXZmod*p2OXZmod) ) : 1.0f;
	//OYZ proj = rotation around X
	float cosC = (p1OYZmod > 0 && p2OYZmod > 0) ? ( (p1.y*p2.y + p1.z*p2.z) / (p1OYZmod*p2OYZmod) ) : 1.0f;

	angles.x = acos(cosC);
	angles.y = acos(cosB);
	angles.z = acos(cosA);

	float pi = 3.1459265f;
	Point3Df angleGrad(0.0f, 0.0f, 0.0f);
	angleGrad.x = angles.x*180.0f/pi;
	angleGrad.y = angles.y*180.0f/pi;
	angleGrad.z = angles.z*180.0f/pi;
	
	printf("Angles: x = %f, y = %f, z = %f\n", angleGrad.x, angleGrad.y, angleGrad.z);

	return angles;
}

struct Point3Dfp : public Point3Df {
	float padding;
};
void PointsCloud::loadBinTrain(const char *filename,bool strict) {
	FILE *infile = fopen(filename,"rb");
	if(infile==NULL) throw("PointsCloud::loadBinTrain can't open file");
	while(!feof(infile)) {
		uint64_t count = 0;
		size_t read = fread(&count,sizeof(count),1,infile);
		if(feof(infile)) break;
		if(read!=1) throw("PointsCloud::loadBinTrain read structure error whie reading count");
		segments.push_back((size_t)count);
		size_t writeindex = points.size();
		points.resize(points.size()+(size_t)count);
		//if(sizeof(Point3Df)!=16) {
		//	std::vector <Point3Dfp> temp;
		//	temp.resize((size_t)count);
		//	read = fread(&temp.front(),sizeof(Point3Dfp),(size_t)count,infile);
		//	for(size_t i = 0; i < count; i++) points[writeindex+i] = temp[i];
		//}
		//else {
			Point3Df *ptr = &(points[writeindex]);
			read = fread(ptr,sizeof(Point3Df),(size_t)count,infile);
		//}
		if(read!=count && strict) throw("PointsCloud::loadBinTrain read structure error whie reading count");
		if(read!=count) {points.resize(points.size()-(size_t)count+read);break;} // in any case	
	}
	fclose(infile);
};

void PointsCloud::saveBinTrain( const char *filename ) const {
	FILE * dumpFile = NULL;
	dumpFile = fopen( filename, "wb");
	size_t j=0;
	for( size_t i=0; i<segments.size(); i++ )
	{
		uint64_t pointsToDump = (uint64_t)segments[i];
		fwrite(&pointsToDump, sizeof(pointsToDump), 1, dumpFile);
		fwrite(&(points[j]), sizeof(Point3Df), (size_t)pointsToDump, dumpFile);
		j += segments[i];
	}
	fclose( dumpFile );
}


//template< typename T >
//void PolygonScaling::scale(const std::vector < T >& in, std::vector < T >& out, float scaleVal) {
//	out.clear();
//	const size_t points_count = in.size();
//	if(points_count == 0) {
//		return;
//	}
//
//	T center; 
//	center.x = 0; 
//	center.y = 0;
//	for(size_t i = 0; i < points_count; ++i) {
//		const T& p = in[i];
//		center.x += p.x;
//		center.y += p.y;
//	}
//	center.x /= points_count;
//	center.y /= points_count;
//
//	out = in;
//	for(size_t i = 0; i < points_count; ++i) {
//		T& p = out[i];
//		p.x -= center.x;
//		p.y -= center.y;
//	}
//	for(size_t i = 0; i < points_count; ++i) {
//		T& p = out[i];
//		p.x *= scaleVal;
//		p.y *= scaleVal;
//	}
//	for(size_t i = 0; i < points_count; ++i) {
//		T& p = out[i];
//		p.x += center.x;
//		p.y += center.y;
//	}
//}


std::pair<Point3Df,Point3Df> getMinMax(std::vector<Point3Df> &points)  {
	Point3Df maxPoint(FLT_MIN,FLT_MIN,FLT_MIN);
	Point3Df minPoint(FLT_MAX,FLT_MAX,FLT_MAX);
	for(size_t k=0;k<points.size();k++){
		maxPoint.x = std::max<float>(points[k].x,maxPoint.x);
		maxPoint.y = std::max<float>(points[k].y,maxPoint.y);
		maxPoint.z = std::max<float>(points[k].z,maxPoint.z);

		minPoint.x = std::min<float>(points[k].x,minPoint.x);
		minPoint.y = std::min<float>(points[k].y,minPoint.y);
		minPoint.z = std::min<float>(points[k].z,minPoint.z);
	}
	return std::pair<Point3Df,Point3Df> (minPoint,maxPoint);
};


/*
* Ray class, for use with the optimized ray-box intersection test
* described in:
*
*      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
*      "An Efficient and Robust Ray-Box Intersection Algorithm"
*      Journal of graphics tools, 10(1):49-54, 2005
*
*/

class Ray {
public:
	Ray() { }
	Ray(Point3Df o, Point3Df d) {
		origin = o;
		direction = d;
		inv_direction = Point3Df(1 / d.x, 1 / d.y, 1 / d.z);
		sign[0] = (inv_direction.x < 0);
		sign[1] = (inv_direction.y < 0);
		sign[2] = (inv_direction.z < 0);
	}
	Ray(const Ray &r) {
		origin = r.origin;
		direction = r.direction;
		inv_direction = r.inv_direction;
		sign[0] = r.sign[0]; sign[1] = r.sign[1]; sign[2] = r.sign[2];
	}

	Point3Df origin;
	Point3Df direction;
	Point3Df inv_direction;
	int sign[3];
};

/*
* Ray-box intersection using IEEE numerical properties to ensure that the
* test is both robust and efficient, as described in:
*
*      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
*      "An Efficient and Robust Ray-Box Intersection Algorithm"
*      Journal of graphics tools, 10(1):49-54, 2005
*
*/
class Box {
public:
	Box() { }
	Box(const Point3Df &min, const Point3Df &max) {
		parameters[0] = min;
		parameters[1] = max;
	}
	// (t0, t1) is the interval for valid hits
	bool intersect(const Ray &, float &t0, float &t1) const;

	// corners
	Point3Df parameters[2];
};
bool Box::intersect(const Ray &r, float &t0, float &t1) const {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (parameters[r.sign[0]].x - r.origin.x) * r.inv_direction.x;
	tmax = (parameters[1 - r.sign[0]].x - r.origin.x) * r.inv_direction.x;
	tymin = (parameters[r.sign[1]].y - r.origin.y) * r.inv_direction.y;
	tymax = (parameters[1 - r.sign[1]].y - r.origin.y) * r.inv_direction.y;
	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	tzmin = (parameters[r.sign[2]].z - r.origin.z) * r.inv_direction.z;
	tzmax = (parameters[1 - r.sign[2]].z - r.origin.z) * r.inv_direction.z;
	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	t0 = tmin;
	t1 = tmax;
	return true;
}


bool Ray3Df::boxIntersection(const Point3Df & boxmin, const Point3Df & boxmax, float & tmin, float & tmax) const
{
	Box box(boxmin, boxmax);
	Ray ray(a, b);
	return box.intersect(ray,tmin,tmax);
}
