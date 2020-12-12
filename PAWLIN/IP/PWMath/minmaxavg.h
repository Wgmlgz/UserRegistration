// (c) Pawlin Technologies Ltd 2010
// File: statstics.h / .cpp
// purpose: basic statistical functions
// author: P.V. Skribtsov
// ALL RIGHTS RESERVED

#pragma once

// STL 
#include <algorithm> // for std::max , std::min
#include <vector>
#include <fstream>
#include "math.h"

template <typename T>
inline T median(std::vector<T> &array) {
	std::sort(array.begin(), array.end());
	return array[array.size() / 2];
}
inline int float2int(float x) {
	if (x < 0) return int(x - 0.5f);
	return int(x + 0.5f);
}

struct MinMaxAvg {
public:
	int counter; // to be fixed number of bytes	
	float minv;
	float maxv;
	double sum;
	double sumsq;
	void setZero() {
		counter = 0;
		minv = 0;
		maxv = 0;
		sum = 0;
		sumsq = 0;
	}
	MinMaxAvg() {
		counter = 0;
	}
	void clear() {
		counter = 0;
		minv = 0.0f;
		maxv = 0.0f;
		sum = 0.0f;
		sumsq = 0.0;
	}
	
	inline bool is_init(void) const { return (this->counter > 0); }
	float getMinMaxMiddle() const { return 0.5f*(minv+maxv);}

	void add(const MinMaxAvg & other) {
		if (!other.is_init()) return;
		if (!is_init()) { *this = other; return; }
		
		this->counter += other.counter;
		this->minv = std::min<float>(this->minv, other.minv);
		this->maxv = std::max<float>(this->maxv, other.maxv);
		this->sum += other.sum;
		this->sumsq += other.sumsq;
	}
	
	void take(float v, int n) { // n means how many times this value was inr
		if(counter == 0) {
			minv = v;
			maxv = v;
			sum = v*(float)n;
			sumsq = v*v*(float)n;
		}
		else {
			minv = std::min<float>(minv,v);
			maxv = std::max<float>(maxv,v);
			sum += v*(float)n;
			sumsq += v*v*(float)n;
		}
		counter += n;
	}
	void take(float v) { // n means how many times this value was inr
		if(counter == 0) {
			minv = v;
			maxv = v;
			sum = v;
			sumsq = v*v;
		}
		else {
			minv = std::min<float>(minv,v);
			maxv = std::max<float>(maxv,v);
			sum += v;
			sumsq += v*v;
		}
		counter++;
	}
	float getAvg() const { return float(sum / (double)counter); }
	float getStdev() const { 
		float avg = getAvg();
		
		double v = sumsq / (double)counter - (double)avg*(double)avg;
		if(v<0) return 0;
		return sqrtf((float)v );
	}
	void save(FILE *fp)const{
		
		fprintf(fp,"%d,%f,%f,%lf,%lf\n", counter, minv, maxv, sum, sumsq);
	}
	void load(FILE *fp){
	
		fscanf(fp,"%d,%f,%f,%lf,%lf\n", &counter, &minv, &maxv, &sum, &sumsq);
	}
	void print(bool newline = false) const {
		printf("min %f, max %f, avg %f, stdev %f,count %d%s", minv, maxv, (float)getAvg(), getStdev(),
			this->counter,newline ? "\n" : ""); 
	}
	void print(FILE *file, bool newline = false) const {
		fprintf(file,"min %f, max %f, avg %f, stdev %f,count %d%s", minv, maxv, (float)getAvg(), getStdev(),
			this->counter, newline ? "\n" : "");
	}
};

class MinMaxAvgSafe {
protected:
	MinMaxAvg stats;
	float mean;
	float stdev;
	bool calced;
public:
	MinMaxAvgSafe(const bool &calced = false, const float &mean = 0.0f, const float &stdev = 0.0f) : stats(MinMaxAvg()) {
		this->mean = mean;
		this->stdev = stdev;
		this->calced = calced;
	}

	void take(const float &v) {
		stats.take(v);
	}

	float getMean(const bool &recalc) const {
		if (!calced || recalc) {
			const_cast<float&>(mean) = stats.getAvg();
			return mean;
		}

		return mean;
	}

	float getStdev(const bool &recalc) const {
		if (!calced || recalc) {
			const_cast<float&>(stdev) = stats.getStdev();
			return stdev;
		}

		return stdev;
	}

};