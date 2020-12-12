// File: Dataset.h / cpp
// Purpose: floating point table
// Author: Pavel Skribtsov
// Date: 25-12-08
// Version 1.0
// (C) PAWLIN TECHNOLOGIES LTD. ALL RIGHTS RESERVED

#pragma once

// STL
#include <algorithm>
#include <map>
#include <set>

// IP
#include <PWNGeneral/pwnutil.h>
#include <PWMath2_0/SimpleMatrix.h>
#include <float.h>
#include <vector>
#include <string>
#include <math.h>
#include <malloc.h>
#include <sstream>


#ifdef WIN32
#define DATASET_SSE
#endif
//#define FORCE_AVG_SIZE_TO__PROTOSIZE

#ifdef DATASET_SSE
#include <xmmintrin.h>
#endif
#ifdef __MINGW32__
#ifndef _aligned_malloc
#define _aligned_malloc __mingw_aligned_malloc
#define _aligned_free __mingw_aligned_free
#endif
#endif

#ifdef __APPLE__
inline void* _aligned_malloc( int allocsize, int align )   {return malloc( allocsize );}
inline void _aligned_free( void* ptr ) {free(ptr);}
#endif

#ifdef __ANDROID__
inline void* _aligned_malloc(int allocsize, int align) { return malloc(allocsize); }
inline void _aligned_free(void* ptr) { free(ptr); }
#endif

#if defined( __linux__ ) && !defined(__ANDROID__)
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <iosfwd>
// Linux aligh 16 is not implemented


inline void* _aligned_malloc( int allocsize, int align )   {return aligned_alloc(align,allocsize);}
inline void _aligned_free( void* ptr ) {free(ptr);}





#endif

#ifndef _WIN32
#define ALIGN_TYPE
#else
#define ALIGN_TYPE __declspec(align(16))
#endif

//#define _USE_ALIGNED_VECTOR


template<typename T>
bool string_to_number(const std::string& numberAsString, T &val) {
    std::stringstream stream(numberAsString);
    stream >> val;
    if(stream.fail()) {
        return false;
    }
    return true;
}


template <typename T, std::size_t N = 16>
class AlignmentAllocator {
public:
  typedef T value_type;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  typedef T * pointer;
  typedef const T * const_pointer;

  typedef T & reference;
  typedef const T & const_reference;

  public:
  inline AlignmentAllocator () throw () { }

  template <typename T2>
  inline AlignmentAllocator (const AlignmentAllocator<T2, N> &) throw () { }

  inline ~AlignmentAllocator () throw () { }

  inline pointer adress (reference r) {
    return &r;
  }

  inline const_pointer adress (const_reference r) const {
    return &r;
  }

  inline pointer allocate (size_type n) {
     return (pointer)_aligned_malloc(n*sizeof(value_type), N);
  }

  inline void deallocate (pointer p, size_type) {
    _aligned_free (p);
  }

  inline void construct (pointer p, const value_type & wert) {
     new (p) value_type (wert);
  }

  inline void destroy (pointer p) {
    p->~value_type ();
  }

  inline size_type max_size () const throw () {
    return size_type (-1) / sizeof (value_type);
  }

  template <typename T2>
  struct rebind {
    typedef AlignmentAllocator<T2, N> other;
  };

  bool operator!=(const AlignmentAllocator<T,N>& other) const  {
    return !(*this == other);
  }

  // Returns true if and only if storage allocated from *this
  // can be deallocated from other, and vice versa.
  // Always returns true for stateless allocators.
  bool operator==(const AlignmentAllocator<T,N>& other) const {
    return true;
  }
};

template <typename T, std::size_t Alignment>
class aligned_allocator
{
	public:
 
		// The following will be the same for virtually all allocators.
		typedef T * pointer;
		typedef const T * const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;
		typedef std::size_t size_type;
		//typedef ptrdiff_t difference_type;
 
		T * address(T& r) const
		{
			return &r;
		}
 
		const T * address(const T& s) const
		{
			return &s;
		}
 
		std::size_t max_size() const
		{
			// The following has been carefully written to be independent of
			// the definition of size_t and to avoid signed/unsigned warnings.
			return (static_cast<std::size_t>(0) - static_cast<std::size_t>(1)) / sizeof(T);
		}
 
 
		// The following must be the same for all allocators.
		template <typename U>
		struct rebind
		{
			typedef aligned_allocator<U, Alignment> other;
		} ;
 
		bool operator!=(const aligned_allocator& other) const
		{
			return !(*this == other);
		}
 
		void construct(T * const p, const T& t) const
		{
			void * const pv = static_cast<void *>(p);
 
			new (pv) T(t);
		}
 
		void destroy(T * const p) const
		{
			p->~T();
		}
 
		// Returns true if and only if storage allocated from *this
		// can be deallocated from other, and vice versa.
		// Always returns true for stateless allocators.
		bool operator==(const aligned_allocator& other) const
		{
			return true;
		}
 
 
		// Default constructor, copy constructor, rebinding constructor, and destructor.
		// Empty for stateless allocators.
		aligned_allocator() { }
 
		aligned_allocator(const aligned_allocator&) { }
 
		template <typename U> aligned_allocator(const aligned_allocator<U, Alignment>&) { }
 
		~aligned_allocator() { }
 
		
		// The following will be different for each allocator.
		T * allocate(const std::size_t n) const
		{
			// The return value of allocate(0) is unspecified.
			// Mallocator returns NULL in order to avoid depending
			// on malloc(0)'s implementation-defined behavior
			// (the implementation can define malloc(0) to return NULL,
			// in which case the bad_alloc check below would fire).
			// All allocators can return NULL in this case.
			if (n == 0) {
				return NULL;
			}
 
			// All allocators should contain an integer overflow check.
			// The Standardization Committee recommends that std::length_error
			// be thrown in the case of integer overflow.
			if (n > max_size())
			{
#ifdef __MINGW32__
				throw ("aligned_allocator<T>::allocate() - Integer overflow.");/*using Eigen*/
#else
				throw std::length_error("aligned_allocator<T>::allocate() - Integer overflow.");
#endif

			}
 
			// Mallocator wraps malloc().
			void * const pv = _aligned_malloc(n * sizeof(T), int(Alignment));
 
			// Allocators should throw std::bad_alloc in the case of memory allocation failure.
			if (pv == NULL)
			{
				throw std::bad_alloc();
			}
 
			return static_cast<T *>(pv);
		}
 
		void deallocate(T * const p, const std::size_t n) const
		{
			_aligned_free(p);
		}
 
 
		// The following will be the same for all allocators that ignore hints.
		template <typename U>
		T * allocate(const std::size_t n, const U * /* const hint */) const
		{
			return allocate(n);
		}
 
 
		// Allocators are not required to be assignable, so
		// all allocators should have a private unimplemented
		// assignment operator. Note that this will trigger the
		// off-by-default (enabled under /Wall) warning C4626
		// "assignment operator could not be generated because a
		// base class assignment operator is inaccessible" within
		// the STL headers, but that warning is useless.
	private:
		aligned_allocator& operator=(const aligned_allocator&);
};

using ::std::vector;

struct FloatVector {
    #ifdef _USE_ALIGNED_VECTOR
	typedef std::vector <float, aligned_allocator<float, 16> > aligned_vector_float;
    #else
       typedef std::vector <float> aligned_vector_float;
    #endif
	inline void push_back(float v) { data.push_back(v); }
	aligned_vector_float data;
	float &operator[] (size_t i) {return data[i];}
	const float &operator[] (size_t i) const { return data[i];}
	operator aligned_vector_float & () {return data;}
	FloatVector *newArrayOfClones(size_t size) const { 
		FloatVector *array = new FloatVector[size];
		for(size_t i = 0; i < size; i++) array[i] = *this;
		return array;
	}
	void toDouble(vector<double> &out) const {
		out.resize(data.size());
		FOR_ALL(data, i) out[i] = (double)data[i];
	}
	void join(const FloatVector &other);
	virtual ~FloatVector() {assert_aligned();}
	inline vector <float> unaligned_data() const {
		vector <float> d(data.size());
		memcpy(&d.front(),&data.front(),sizeof(float)*data.size());
		return d;
	}
	static FloatVector *newArray(size_t size, size_t dims, float initial_value); // was introduced to use with CycleBufferGeneric template class
	void assert_aligned() { 
#ifdef _USE_ALIGNED_VECTOR
		if(data.empty()) return;
		size_t addr = (size_t) &data.front(); 
		if (addr & 15) {
			throw (std::runtime_error("should have been aligned"));
		}
#endif
  
	}
	FloatVector() {}
	FloatVector(size_t size, float initial_value) : data(size,initial_value) { assert_aligned();}
	FloatVector(const std::vector<float> & initial_data) {
		data.resize(initial_data.size()); assert_aligned();
		memcpy(&data.front(),&initial_data.front(),sizeof(float)*data.size());
	}
	FloatVector(const std::vector<double> & initial_data) {
		data.resize(initial_data.size()); assert_aligned();
		FOR_ALL(data, i) data[i] = (float)initial_data[i];
	}
	FloatVector(const float* first_elem, const float* last_elem) : data(first_elem, last_elem) { throw "Check your code for this constructor consistency\n"; }
	FloatVector(const float* ptr, size_t size) {
		data.resize(size); assert_aligned();
		memcpy(&data.front(),ptr,sizeof(float)*size);
	}
	void operator += (const FloatVector &other) { add(other);}
	FloatVector operator - (const FloatVector &other) const {
		FloatVector result(*this);
		result.subs(other);
		return result;
	}
	FloatVector operator + (const FloatVector &other) const {
		FloatVector result(*this);
		result.add(other);
		return result;
	}
	// demultiplexes double sized float vector into two 
	void deinterlace(FloatVector &even, FloatVector &odd) const {
		size_t sz = data.size();
		if(sz&1) throw(std::runtime_error("odd float vectors can not be deinterlaced"));
		size_t half = sz/2;
		even.data.resize(half);
		odd.data.resize(half);
		const float *df = &(data[0]);
		float *ef = &(even.data[0]);
		float *of = &(odd.data[0]);
		for(size_t s = 0; s < sz; s+=2) {
			*ef = df[s];
			*of = df[s+1];
			ef++;of++;
		}
	}
	// multiplexes 2 float vectors
	void interlace(const FloatVector &even, const FloatVector &odd) {
		size_t sz = even.data.size();
		if(sz!=odd.data.size()) throw std::runtime_error("different sized float vectors can not be interlaced");
		size_t dsize = sz*2;
		data.resize(dsize);
		float *df = &(data[0]);
		const float *ef = &(even.data[0]);
		const float *of = &(odd.data[0]);
		for(size_t s = 0; s < dsize; s+=2) {
			df[s]  = *ef;
			df[s+1]= *of;
			ef++;of++;
		}
	}

	// not inclusive the element with index end!
	void init(const vector <FloatVector> &source, size_t start, size_t end) {
		size_t sz = source.size();
		if(sz==0) return;
		size_t newsize = 0;
		const FloatVector *srcptr = &source.front();
		for(size_t i = start; i < end; i++) newsize+= srcptr[i].data.size();
		data.resize(newsize);
		float *df = &(data[0]);
		for(size_t i = start; i < end; i++) {
			size_t elements = srcptr[i].data.size();
			memcpy(df,&(srcptr[i].data.front()),sizeof(float)*elements);
			df+=elements;
		}
	}

	void init(const vector <FloatVector> &source) {
		init(source,0,source.size()); 
	}
	void decompose(size_t sz, vector <FloatVector> &out) const {
		size_t thissize = data.size();
		if(thissize % sz) throw std::runtime_error("FloatVector - attempt to decompose on the wrong sized vectors or vector size is wrong");
		size_t n = thissize / sz;
		out.resize(n);
		const float *df = &(data[0]);
		for(size_t i = 0; i < n; i++) {
			out[i].data.resize(sz);
			memcpy(&(out[i].data.front()),df,sizeof(float)*sz);
			df+=sz;
		}
	}

	void operator -= (const FloatVector &other) { subs(other); }
	void operator /= (unsigned int value) { mul(1.0f / (float)value); }
	FloatVector operator / (unsigned int value) const { FloatVector temp = *this; temp /= value; return temp; }
	void print(int digits = 4, bool nextline = true) const {
		size_t size = data.size();
		for(size_t s = 0; s < size; s++) printf("%.*f ",digits, data[s]);
		if(nextline) printf("\n");
	}
	void print_to_file(FILE* file, int digits = 4, bool nextline = true) const {
		size_t size = data.size();
		for(size_t s = 0; s < size; s++) fprintf(file, "%.*f ",digits, data[s]);
		if(nextline) fprintf(file, "\n");
	}
	inline void clear(void) {this->data.clear();}
	inline void add(const FloatVector &other) {
		float *df = &(data[0]);
		const float *of = &(other.data[0]);
		size_t size = data.size();
		for(size_t s = 0; s < size; s++) df[s] += of[s];
	}
	inline void addUniformNoise(float magnitude) {
		float noise;
		float *df = &(data[0]);
		size_t size = data.size();
		for (size_t s = 0; s < size; s++) {
			noise = (rnd()*2.0f - 1.0f)*magnitude;
			df[s] += noise;
		}
	}
	inline void addWeighted(const FloatVector &other, float w) {
		float *df = &(data[0]);
		const float *of = &(other.data[0]);
		size_t size = data.size();
		for(size_t s = 0; s < size; s++) df[s] += of[s]*w;
	}
	inline void subs(const FloatVector &other) {
		float *df = &(data[0]);
		const float *sf = &(other.data[0]);
		size_t size = data.size();
		for(size_t s = 0; s < size; s++) df[s] -= sf[s];
	}
	inline void subs(const FloatVector &other, float mul) {
		float *df = &(data[0]);
		const float *sf = &(other.data[0]);
		size_t size = data.size();
		for(size_t s = 0; s < size; s++) df[s] = (df[s]-sf[s])*mul;
	}
	inline void subs(const FloatVector &other, const FloatVector &mul) {
		float *df = &(data[0]);
		const float *sf = &(other.data[0]);
		const float *ms = &(mul.data[0]);
		size_t size = std::min<size_t>(data.size(),other.data.size());
		for(size_t s = 0; s < size; s++) df[s] = (df[s]-sf[s])*ms[s];
	}
	inline void square() {
		float *df = &(data[0]);
		size_t size = data.size();
		for(size_t s = 0; s < size; s++) df[s] *= df[s];
	}
	inline void mul(float m) {
		float *df = &(data[0]);
		size_t size = data.size();
		for(size_t s = 0; s < size; s++) df[s] *= m;
	}
	inline void component_mul(const FloatVector &other) {
		float *df = &(data[0]);
		const float *sf = &(other.data[0]);
		size_t size = data.size();
		for(size_t s = 0; s < size; s++) df[s] *= sf[s];
	}
	inline void component_div(const FloatVector &other) {
		float *df = &(data[0]);
		const float *sf = &(other.data[0]);
		size_t size = data.size();
		for(size_t s = 0; s < size; s++) df[s] /= sf[s];
	}
	inline void component_mul(const FloatVector &other, float regularization) {
		float *df = &(data[0]);
		const float *sf = &(other.data[0]);
		size_t size = data.size();
		for(unsigned s = 0; s < size; s++) {
			df[s] *= sf[s];
			if(df[s] < regularization) df[s] = regularization;
		}
	}
	inline void component_mul_sumnorm1(const FloatVector &other, float regularization) {
		float *df = &(data[0]);
		const float *sf = &(other.data[0]);
		size_t size = data.size();
		float sum = 0;
		for(unsigned s = 0; s < size; s++) {
			df[s] *= sf[s];
			if(df[s] < regularization) df[s] = regularization;
			sum += df[s];
		}
		float k = 1.0f / sum;
		for(unsigned s = 0; s < size; s++) df[s] *= k;
	}
	inline float dotproduct(const FloatVector &other) const {
		const float *df = &(data[0]);
		const float *sf = &(other.data[0]);
		size_t size = data.size();
		float sum = 0;
		for(unsigned s = 0; s < size; s++) sum += df[s] * sf[s];
		return sum;
	}
	float sqdiff(const FloatVector &other) const;
	inline float maxabsdiff(const FloatVector &other) const {
		const float *df = &(data[0]);
		const float *of = &(other.data[0]);
		size_t size = data.size();
		float sum = 0,dif;
		for(size_t s = 0; s < size; s++) {
			dif = fabsf(df[s]-of[s]);
			if(dif>sum) sum = dif;
		}
		return sum;
	}
	inline float sqdiff(const FloatVector &other, size_t sz) const {
		const float *df = &(data[0]);
		const float *of = &(other.data[0]);
		size_t size = std::min<size_t>(data.size(),sz);
		float sum = 0,dif;
		for(size_t s = 0; s < size; s++) {
			dif = df[s]-of[s];
			sum += dif*dif;
		}
		return sum;
	}
	inline float components_sum() const {
		float sum = 0;
		for(unsigned s = 0; s < data.size(); s++) sum += data[s];
		return sum;
	}
	inline bool hasZeroElement() const {
		for(unsigned s = 0; s < data.size(); s++) if(data[s]==0) return true;
		return false;
	}
	inline size_t countNonZeroElements() const {
		size_t count = 0;
		for(unsigned s = 0; s < data.size(); s++) if(data[s]!=0) count++;
		return count;
	}
	inline bool finite() const {
		for(unsigned s = 0; s < data.size(); s++) if(!finite_check((double)data[s])) return false;
		return true;
	}
	inline bool hasZeroElement(float tolerance) const {
		for(unsigned s = 0; s < data.size(); s++) if(fabs(data[s])<tolerance) return true;
		return false;
	}
	inline float norm() const { return sqrtf(dotproduct(*this));}
	void normalize(const float eps = FLT_MIN);
	inline float sqnorm() const { return dotproduct(*this);}
	inline void resize(size_t size){data.resize(size);}                                      //avdol01
	inline void resize(size_t size, float initial_value){data.resize(size,  initial_value);} //avdol01
	inline void delElem(unsigned idx){if (idx < data.size()) data.erase(data.begin()+idx);}  //avdol01
	void zero() {	memset(&(data.front()),0,sizeof(float)*data.size());}
	void boolean(vector<bool> &out) const {
		const float *df = &(data[0]);
		size_t sz = this->data.size();
		out.resize(sz);
		for(size_t i = 0; i < sz; i++) out[i] = df[i] > 0.5f ? true : false;
	}
	size_t size() const { return data.size();}
	void integrate(FloatVector &out) const {
		const float *df = &(data[0]);
		size_t sz = this->data.size();
		out.resize(sz);
		out[0] = df[0];
		for (size_t i = 1; i < sz; i++) out[i] = out[i-1] + df[i];
	}
	float interpolate(float x) const { // x between 0 and 1
		int sx = int(x);
		float dx = x - (float)sx;
		size_t sz = this->data.size();
		size_t index0 = size_t(x*sz);
		size_t index1 = index0 + 1;
		if (index0 >= sz) index0 = sz-1;
		if (index1 >= sz) index1 = sz-1;
		float v0 = data[index0];
		float v1 = data[index1];
		return v0 + (v1 - v0) * dx;
	}
	void resample(size_t n) {
		float k = (float)n / (float)data.size();
		FloatVector integral;
		integrate(integral);
		data.resize(n);
		for (size_t i = 0; i < n; i++) {
			float p0 = integral.interpolate((i+0) / float(n));
			float p1 = integral.interpolate((i+1) / float(n));
			data[i] = (p1 - p0)*k;
		}
	}
	inline float median() const
	{
		auto temp = data;
		size_t n = temp.size() / 2;
		std::nth_element(temp.begin(), temp.begin() + n, temp.end());
		return temp[n];
	}
};

typedef std::vector <FloatVector> FloatMatrix;
struct VectorMatrix {
	struct MSize {
		size_t cols, rows;
		size_t offset;
	};
	std::vector<MSize> elements;
	std::vector<float> data;
	VectorMatrix() {}
	VectorMatrix(const std::vector<FloatMatrix> &a) { init(a) ; }
	void init( const std::vector<FloatMatrix> &a );
	float *getMatrix(size_t e) {return &data[elements[e].offset];}
	float *getRow(size_t e, size_t r) { return &data[elements[e].offset + elements[e].cols*r];}
	const float *getMatrix(size_t e) const {return &data[elements[e].offset];}
	const float *getRow(size_t e, size_t r) const { return &data[elements[e].offset + elements[e].cols*r];}
	bool operator==(const VectorMatrix &other) const {
		if(elements.size()!=other.elements.size()) 
			return false;
		if(data.size() != other.data.size()) 
			return false;
		for(size_t i = 0; i < elements.size(); i++)
		{
			const VectorMatrix::MSize & currentElement = elements[i];
			const VectorMatrix::MSize & otherElement = other.elements[i];
			if(currentElement.cols != otherElement.cols)
				return false;
			if(currentElement.offset != otherElement.offset)
				return false;
			if(currentElement.rows != otherElement.rows)
				return false;
		}
		for(size_t j = 0; j < data.size(); j++)
		{
			if(data[j] != other.data[j])
				return false;
		}
		return true;
	}
};

struct DatasetStatistics {
	FloatVector setmin;
	FloatVector setmax;
	FloatVector setavg;
	FloatVector setsigma;
	DatasetStatistics(){}
	DatasetStatistics(size_t ndim){
		setDim(ndim);
	}
	void setDim(size_t ndim)
	{
		setmin.data.resize(ndim);
		setmax.data.resize(ndim);
		setavg.data.resize(ndim);
		setsigma.data.resize(ndim);
		M2.data.resize(ndim);
		clear();
	}
	void clear(){
		size_t ndim = setmin.data.size();
		for (size_t i = 0; i< ndim; i++){
			setmin.data[i] = FLT_MAX;
			setmax.data[i] = -FLT_MAX;
			setavg.data[i] = 0.0f;
			setsigma.data[i] = 0.0f;
			M2.data[i] = 0.0f;
		
		}
		N = 0.0f;
	}
	void add_data(const FloatVector & x){
		size_t ndim = setmin.data.size();
		if (N == 0.0f){
			for (size_t i = 0; i< ndim; i++){
				setmin.data[i] = x.data[i];
				setmax.data[i] = x.data[i];
			}
		}
		N += 1.0f;
		for (size_t i = 0; i< ndim; i++){
			setmin.data[i] = std::min<float>(x.data[i], setmin.data[i]);
			setmax.data[i] = std::max<float>(x.data[i], setmax.data[i]);
			float delta = x.data[i]-setavg.data[i];
			setavg.data[i] += delta/N;
			M2.data[i] += delta*(x.data[i]-setavg.data[i]);
		}
	}
	void update_sigma(){
		size_t ndim = setmin.data.size();
		for (size_t i = 0; i< ndim; i++){
			setsigma.data[i] = sqrtf(M2.data[i]/(N-1.0f));
		}
	}
private:
	float N;
	FloatVector M2;
};
class Dataset;
class SimpleNormalization{
public:
	SimpleNormalization(){}
	FloatVector avg;
	FloatVector scales;
	inline void normalize(FloatVector &x) const {
		//size_t size = (unsigned) std::min<size_t>(x.data.size(), avg.data.size());
		x.subs(avg,scales);
		//for(size_t i = 0; i < size; i++) x.data[i] = (x.data[i] - avg.data[i]) * scales.data[i];
	}
	inline void normalize(float *x, size_t size) const {
		const float *avgptr = &avg.data[0];
		const float *sclptr = &scales.data[0];
		for(size_t i = 0; i < size; i++) x[i] = (x[i] - avgptr[i]) * sclptr[i];
	}
	inline void denormalize(FloatVector &x) const {
		x.component_div(scales);
		x.add(avg);
	}
	
	inline void denormalize(float & x, size_t column) const {
      column = (size_t) std::min<size_t>(column, avg.data.size()-1);
      x = x/scales.data[column]+avg.data[column];
   }
	inline void normalize(FloatVector &x, size_t maxcolumns) const {
		size_t size = (size_t) std::min<size_t>(avg.data.size(), std::min<size_t>(x.data.size(), maxcolumns));
		for(size_t i = 0; i < size; i++) x.data[i] = (x.data[i] - avg.data[i]) * scales.data[i];
	}
	static const char *fileheader;
	void save(FILE *file) const {
		fprintf(file,"%s\n",fileheader);
		fprintf(file,"variables:%d\n",(unsigned int)avg.data.size());
		for(size_t i = 0; i < avg.data.size(); i++) fprintf(file,"%.12f\t%.12f\n",avg[i],scales[i]);
	}		
	void load(FILE *file) {
		char buf[1024];
		fgets(buf,1024,file);
		//printf("reading header:%s\n",buf);
		if(strstr(buf,fileheader)==NULL) throw("Wrong normalization header\n");
		int sizei = 0;
		fscanf(file,"variables:%d\n",&sizei);
		size_t size = (size_t)sizei;
		if(size==0) throw("wrong normalization format\n");
		avg.data.resize(size);
		scales.data.resize(size);
		for(size_t i = 0; i < avg.data.size(); i++) fscanf(file,"%f\t%f\n",&avg[i],&scales[i]);
	}

	void normalize(Dataset &sequence) const;
	void normalize(Dataset &sequence,size_t dims) const;

	inline void clear_data(void) { this->avg.clear(); this->scales.clear(); }
};


class SimpleMatrixSimpleNormalization: public SimpleNormalization {
public:
	SimpleMatrixSimpleNormalization() {} // empty constructor for later loading from stream
	
	SimpleMatrixSimpleNormalization(const SimpleMatrix &matrix, 
		size_t startIndex, size_t rowCount, bool doubleRange = true,
		int maxcolumns = -1); // by default normalization maps to range [-1 ; 1], otherwise [-0.5; 0.5]
	SimpleMatrixSimpleNormalization(const SimpleMatrix &matrix, size_t startIndex, size_t rowCount, FloatVector &setmin, FloatVector &setmax, bool doubleRange = true,
		int maxcolumns = -1); // by default normalization maps to range [-1 ; 1], otherwise [-0.5; 0.5]
	void computeMatrixMinMaxAverage(const SimpleMatrix &matrix,FloatVector &setmin, 
							   FloatVector &setmax, 
							   FloatVector &setavg,
								size_t startIndex, 
								size_t rowCount,
								int maxcolumns ) const;


};
class BooleanMask : public std::vector<bool> {
	std::vector<char> cache;
public:
	static const char *fileheader;
	BooleanMask(size_t n, bool value) { resize(n,value);}
	BooleanMask(unsigned n) { resize(n);}
	BooleanMask() {}
	/*BooleanMask& operator = (const vector<bool>& mask){
		this->clear();
		for (unsigned i = 0; i< this->size(); i++)
			this->push_back(mask[i]);
		return *this;
	}*/
	void save(FILE *file) const {
		fprintf(file,"%s\n",fileheader);
		fprintf(file,"variables:%u\n",(unsigned int)size());
		for(size_t i = 0; i < size(); i++) fprintf(file,"%zu\t%u\n",i,this->at(i) ? 1 : 0);
	}
		
	void load(FILE *file) {
		char buf[1024];
		fgets(buf,1024,file);
		//printf("reading header:%s\n",buf);
		const bool bufCorrespondsToTheHeader = (NULL != strstr(buf,fileheader) );
		if(!bufCorrespondsToTheHeader) throw("Wrong BooleanMask header\n");
		int sizei = 0;
		fscanf(file,"variables:%d\n",&sizei);
		size_t size = (size_t)sizei;
		if(size==0) {
			clear();
			return;
		}
		reserve(size);
		size_t index; bool p;
		for(size_t i = 0; i <size; i++) {
			size_t pTmp;
			fscanf(file,"%zu\t%zu\n",&index,&pTmp);
			if (pTmp) p = true; else p = false;
			//printf("%d: scaned index = %d, p = %d\n", i, index, p);
			push_back(p);
			if(index!=i) {
				//printf("Corrupted BooleanMask - index = %d, i = %d\n", index, i);
				throw("Corrupted BooleanMask!\n");
			}
		}
		updateCache();
	}
	void print() {
		const int width = 14;
		for(int i = 0; i < width; i++) printf("---+");
		printf("\n");
		for(size_t start = 0; start < size(); start+= width) {
			for(size_t h = 0; h < 2; h++) {
				for(size_t i = start; i < std::min<size_t>(start+width,size()); i++) 
					if(h==0) printf("%3u|",(unsigned int)i);
					else printf("%s", this->at(i) ? "YES|" : "   |");
				printf("\n");
				for(size_t i = start; i < std::min<size_t>(start+width,size()); i++) printf("---+");
				printf("\n");
			}
		}
	}

	void updateCache()
	{
		cache.clear();

		const std::vector<bool> &mask = *this;
		size_t fullSize = mask.size();
		for(size_t featIdx = 0 ; featIdx < fullSize; featIdx++ )
		{
			if(mask[featIdx])
				cache.push_back(1);
			else
				cache.push_back(0);
		}
	}

	void apply(const std::vector<float> &full, std::vector<float> &masked)
	{
		if(full.size() != cache.size())
			throw("Corrupted BooleanMask Cache!\n");
		
		masked.clear();
		masked.reserve(this->size());

		const size_t fullSize = full.size();
		const float* fullPtr = &full.front();
		const char * maskPtr = &cache.front();
		for(size_t featIdx = 0 ; featIdx < fullSize; featIdx++)
		{
			if(maskPtr[featIdx])
				masked.push_back(fullPtr[featIdx]);
		}
	}
};
class DatasetSimpleNormalization;
class Dataset {
public:
	size_t size() const { return rows.size(); }
	bool empty() const {return rows.empty();}
	Dataset () {}
	Dataset(std::vector <FloatVector>& data) : rows(data) {}
	Dataset (const Dataset &other, const std::vector<bool> &columnsMask);
	std::vector <FloatVector> rows;
	const FloatVector &operator [](size_t index) const { return rows[index]; }
	FloatVector &operator [](size_t index) { return rows[index]; }
	inline void push_back(const FloatVector &row) {rows.push_back(row);}
	void makeVariants(std::vector <std::map<float, size_t> *> &variants) const;
	void makeVariants(std::vector <std::map<float, size_t> > &variants) const;
	void makeVariants(std::vector <std::set<float> > &variants) const;
	void computeMinMaxAvg(FloatVector &setmin, FloatVector &setmax, FloatVector &setavg,
		size_t startIndex, 
		size_t rowCount, int maxcolumns = -1) const;
	void computeMinMaxAvg(FloatVector &setmin, FloatVector &setmax, FloatVector &setavg,	
		const Dataset& cut_off_min_max,
		size_t startIndex, 
		size_t rowCount, int maxcolumns = -1) const;
	void join_cols(const Dataset &other) {
		FOR_ALL(rows, i) rows[i].data.insert(rows[i].data.end(), other.rows[i].data.begin(), other.rows[i].data.end());
	}
	void add_data(const Dataset & other) { this->rows.insert(this->rows.end(), other.rows.begin(), other.rows.end()); }
	void normalize(const DatasetSimpleNormalization &norm);
	void denormalize(const DatasetSimpleNormalization &norm);
	inline size_t columns() const { return rows[0].data.size(); }
	inline size_t getRowsCount() const {return  rows.size(); }
	inline size_t getCols() const { return columns(); }
	inline size_t getRows() const { return getRowsCount(); }
	inline float getCell(size_t i, size_t j) const { return rows[i].data[j]; }
	inline float *getRow(size_t i) {return &(rows[i].data[0]);}
	inline const float *getRow(size_t i) const {return &(rows[i].data[0]);}
	bool saveCSV(const char *filename, bool useComma = true, const vector<string>&header = vector<string>()) const {return saveCSVmode(filename,"wt",useComma,header);}
	bool saveCSVmode(const char *filename, const std::string &mode = std::string("wt"), bool useComma = true, const vector<string>&header = vector<string>()) const;
	bool saveBIN(const char *filename) const;
	bool loadBIN(const char *filename);
	bool loadCSV(const char *filename, bool useComma = true);
	bool saveCSV(FILE* fp, bool useComma = true) const;
	bool saveBIN(FILE *fp) const ;//for multi Datasets at one same file
	bool loadBIN(FILE *fp);//for multi Datasets at one same file
	void getColumn(size_t index, FloatVector &out) const {
		out.resize(rows.size());
		for (size_t i = 0; i < rows.size(); i++) out[i] = rows[i][index];
	}

	inline void clear(void) { this->rows.clear(); }

	BestElement nearest(const FloatVector &x, bool useabs) const { // full uncompressed distance
		if(rows.empty()) throw(std::runtime_error("Dataset::nearest- no records"));
		const FloatVector *rowptr = &(rows.front());
		size_t count = rows.size();
		size_t dim = x.data.size();
		float bestdistance = useabs ? rowptr->maxabsdiff(x) : rowptr->sqdiff(x,dim);
		size_t bestindex = 0;
		for(size_t i = 1; i < count; i++) {
			float dist = useabs ? rowptr[i].maxabsdiff(x) : rowptr[i].sqdiff(x,dim);
			if(dist < bestdistance) bestdistance = dist, bestindex = i;
		}
		return BestElement(bestindex,useabs ? bestdistance : sqrtf(bestdistance/(float)dim));
	}

	// deprecated
	//operator std::vector < std::vector < float > > ()
	//{
	//	throw("change this operator to createVector function and by the way it shuold be const");

	//	return output;
	//}
	void addNoise(float noise) {  //uniform noise is added in the range [-1;+1]*addNoise
		FOR_ALL(rows,i) {
			FOR_ALL(rows[i].data,j) {
				float v = (float)rand() / float(RAND_MAX);
				v -= 0.5f;
				v*=2.0f*noise;
				rows[i].data[j]+=v;
			}
		}
	}

	void transpose(Dataset & other) const {
		other.rows.resize(this->getCols());		
		for (size_t i = 0; i < this->getRows(); ++i) {
			for (size_t j = 0; j < this->getCols(); ++j) {
				other.rows[j].data.push_back(this->rows[i].data[j]);
			}
		}
	}

	// checks that all values in col2 are driven by the values in col1 (mutually unique correspondence)
	bool checkCorrelation(size_t col1, size_t col2, std::map <float, std::set <float> > &check) const {
		FOR_ALL(rows, r) {
			float v1 = rows[r].data[col1];
			float v2 = rows[r].data[col2];
			check[v1].insert(v2);
			if (check[v1].size() > 1) return false;
		}
		return true;
	}
	
};

class DatasetSimpleNormalization: public SimpleNormalization {
public:
	DatasetSimpleNormalization() {} // empty constructor for later loading from stream
	DatasetSimpleNormalization(const Dataset &data, bool doubleRange = true) {
		init(data,0,data.rows.size(), doubleRange); 
	}
	DatasetSimpleNormalization(
		const Dataset &data, 
		size_t startIndex, 
		size_t rowCount, 
		bool doubleRange = true,// by default normalization maps to range [-1 ; 1], otherwise [-0.5; 0.5]
		int maxcolumns = -1) 
	{
		init(data,startIndex,rowCount,doubleRange,maxcolumns);
	} 
	void init(const Dataset &data, size_t startIndex, size_t rowCount, bool doubleRange = true,
		int maxcolumns = -1); // by default normalization maps to range [-1 ; 1], otherwise [-0.5; 0.5]
	DatasetSimpleNormalization(const Dataset &data, size_t startIndex, size_t rowCount, FloatVector &setmin, FloatVector &setmax, bool doubleRange = true,
		int maxcolumns = -1); // by default normalization maps to range [-1 ; 1], otherwise [-0.5; 0.5]
};

#ifdef USE_OPTIMIZED_STRINGVECTOR
class StringVector {
	vector<char> storage;
	vector<uint16_t> ends;
	static size_t maxline;
public:
	size_t size() const { return ends.size(); }
	StringVector() { storage.reserve(maxline); }
	inline void toVector(vector<string> &out) const {
		out.clear();
		for (size_t i = 0; i < ends.size(); i++) out.push_back(get(i));
	}
	~StringVector() {}
	void clear() {
		storage.clear();
		ends = vector<uint16_t>();
	}
	inline void fromVector(const vector<string> &in) {
		clear();
		size_t size = 0;
		FOR_ALL(in, i) {
			size += in[i].size()+1;
			if (size > 0xFFFF) throw std::runtime_error("too long cell in StringTable");
			ends.push_back((uint16_t)size);
		}
		storage.resize(size);
		updateMaxline();
		FOR_ALL(in, i) memcpy(&storage[0] + computeOffset(i), in[i].c_str(), in[i].size()+1);
	}
	StringVector(const vector<string> &in) { fromVector(in);}
	void operator = (const vector<string> &in) {	fromVector(in);	}
	size_t computeOffset(size_t index) const {return (index == 0) ? 0 : ends[index - 1];}
	inline size_t size_of(size_t index) const {
		if (index >= ends.size()) throw std::runtime_error("StringVector index out of bound");
		size_t offset = computeOffset(index);
		size_t size = ends[index] - offset;
		return size - 1; // excluding null-terminated char, which is always present
	}
	inline const string get(size_t index) const {
		if (index >= ends.size()) throw std::runtime_error("StringVector index out of bound");
		size_t offset = computeOffset(index);
		size_t size = ends[index] - offset;
		string result(&storage[0] + offset,size-1); // excluding 0 terminator char
		return result;
	}
	bool empty() const { return ends.empty(); }
	void push_back_slow(const std::string &str) {
		vector<string> temp;
		toVector(temp);
		temp.push_back(str);
		fromVector(temp);
	}
	void push_back(const std::string &str) {
		size_t endsback = ends.empty() ? 0 : ends.back();
		size_t end = endsback + str.size() + 1;
		if (end > 0xFFFF) throw std::runtime_error("StringVector push_back - too long string row");
		storage.resize(end);
		updateMaxline();
		strcpy(&storage[0] + endsback, str.c_str());
		ends.push_back((uint16_t)end);
	}	
	struct Ref {
		StringVector &parent;
		size_t index;
		Ref(StringVector &parent, size_t index) : parent(parent), index(index) {}
		
		size_t size() const { return parent.size_of(index); }
		bool empty() const { return size() == 0; }
		void operator = (const string &value) {
			vector<string> temp;
			parent.toVector(temp);
			temp[index] = value;
			parent.fromVector(temp);
		}
		operator const std::string () const { return parent.get(index); }
		const char *c_str() const { 
			const char *result = &parent.storage[0] + parent.computeOffset(index);
			return result; 
		}
		void push_back(char c) { 
			if (index + 1 == parent.ends.size()) {
				parent.storage.back() = c;
				parent.storage.push_back(0);
			}
			else {
				parent.storage.insert(parent.storage.begin() + parent.ends[index], c);
			}
			parent.updateMaxline();
			parent.ends[index]++;
		}
	};
	inline void updateMaxline() {
		//if (storage.size() > maxline) maxline = storage.size();
	}
	inline const string front() const { return get(0); }
	inline Ref back() { return Ref(*this, ends.size() - 1); }
	inline string back() const {return get(ends.size() - 1); }
	inline operator vector<string>() { vector<string> temp; toVector(temp); return temp; }
	inline Ref front() { return Ref(*this, 0); }
	inline Ref end() { return Ref(*this, ends.size() - 1); }
	const string operator[] (size_t index) const { return get(index); }
	Ref operator[] (size_t index) { return Ref(*this, index); }
};
#else
typedef std::vector<std::string> StringVector;
#endif
class StringTable {
public:
	StringTable() {}
	StringTable(const std::vector <StringVector>& _rows) : rows(_rows) {}
	size_t size() const { return rows.size(); }
	std::vector <StringVector> rows;
	StringVector & operator[](size_t index) { return rows[index]; }
	const StringVector & operator[](size_t index) const { return rows[index]; }
	void fillDataset(Dataset &dataset,size_t start=0, size_t size=0) const {
		if(size==0) size = rows.size();
		dataset.rows.resize(size,FloatVector(rows.front().size(),0));
		for(size_t i = 0; i < size; i++) {
			const StringVector &row = rows[start+i];
			FOR_ALL(row,j) dataset.rows[i].data[j] = (float) atof(row[j].c_str());
		}
	}
	void save(const char* fname, char delim = '\t', bool noRET = true, const vector<string> &header = vector<string>()) const {
		FILE* fp = fopen(fname, "wt");
		if(fp==NULL) {
			printf("Output file: %s\n", fname);
              			throw std::runtime_error("StringTable can't open file for writting");
		}
		for(size_t i = 0; i < header.size(); i++) {
			fprintf(fp,"%s",header[i].c_str());
			if(i+1<header.size()) fputc(delim,fp); // no need to delim before end of line, otherwise extra emty column is created
		}
		if(header.size()) fprintf(fp, "\n");

		for (size_t i = 0; i< rows.size(); i++){
			for (size_t j = 0; j< rows[i].size(); j++){
				string str = rows[i][j];
				if(noRET) std::replace(str.begin(),str.end(),'\n',' ');
				bool use_quotes = false;
				if(strchr(str.c_str(),delim) || strchr(str.c_str(),'\n')) use_quotes = true; //throw std::runtime_error("delimiter was found within StringTable cell");
				if(use_quotes) fputc('\"',fp);
				fprintf(fp, "%s", str.c_str());
				if(use_quotes) fputc('\"',fp);
				if(j+1<rows[i].size()) fputc(delim,fp); // no need to delim before end of line, otherwise extra emty column is created
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
};

class NamedTable {
protected:
	vector <string> vars;
	vector <int> vargroup;
	vector <string> row_names;
public:
	const vector<string> &getNames() const { return vars;}
	const vector<int> &getGroups() const { return vargroup;}
	const vector<string> &getRowNames() const{ return row_names;}

	void setNames(const vector<string>& names) { this->vars = names; }
	void setGroups(const vector<int>& groups) { this->vargroup = groups; }
	void setRowNames(const vector<string>& rowNames) { this->row_names = rowNames; }
};

class NamedDataset : public NamedTable {
protected:
	Dataset data;
public:
	const Dataset &getData() const { return data;}
	Dataset &getData() { return data;}
	void setData(const Dataset& data) { this->data = data; }
	bool saveCSV(const char *filename,  const std::string &mode = std::string("wt"), bool useComma = true) const 
	{
		FILE *fp = fopen(filename,mode.c_str());
		if(!fp)  {
			return false;
		}
		for(size_t i = 0; i < row_names.size(); i++) {	
			if(useComma) {
				fprintf(fp,"%s,",row_names[i].c_str());	
			}
			else {
				fprintf(fp,"%s;",row_names[i].c_str());
			}
		}
		fprintf(fp,"\n");
		data.saveCSV(fp, false);
		fclose(fp);
		return true;
	}
	
};

class NamedStringTable : public NamedTable {
protected:
	StringTable data;
public:
	NamedStringTable() {}
	NamedStringTable(const NamedDataset& nd);
	const StringTable &getData() const { return data;}
	StringTable &getData() { return data;}
	void setData(const StringTable& data) { this->data = data; }
};

    
struct Float2DArray {
	float *ptr;
	Float2DArray(const Float2DArray &other) {
		ptr = NULL;
		init(other.sn,other.sk);
		memcpy(ptr,other.ptr,alloc_size);
	}
	void operator = (const Float2DArray &other) {
		_aligned_free(ptr);
		init(other.sn,other.sk);
		if(alloc_size) memcpy(ptr,other.ptr,alloc_size);
	}
	int sn, sk;
	int skwidth;
	int alloc_size;
	Float2DArray() {ptr = NULL; sn = sk = skwidth = alloc_size = 0;}
	Float2DArray(int n, int k) { ptr = NULL;init(n,k); }
	void init(int n, int k) {
		if(ptr) _aligned_free(ptr);
		int rem = k%4;
		skwidth = k;
		if(rem>0) skwidth += 4-rem;
		sn = n; sk = k;
		alloc_size = int(sizeof(float))*n*skwidth;
		ptr = (float *) _aligned_malloc(alloc_size,16);
	}
	inline float & value(int i, int j) { return ptr[i*skwidth + j]; }
	inline const float & value(int i, int j) const { return ptr[i*skwidth + j]; }
	inline float * row(int i) { return ptr+i*skwidth; }
	inline const float * row(int i) const { return ptr + i*skwidth; }
	~Float2DArray() {
		_aligned_free(ptr);
	}
};
