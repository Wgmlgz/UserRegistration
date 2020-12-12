// (c) Pawlin Technologies Ltd. 2008
// Pawlin Utility Library
// contains common algorithms useful in different software
// ansi-C/C++ based
// Author: Pavel Skribtsov, Alex Dolgopolov, Sergey Zagoruiko
// Version: 1.0
// Modify date: 10-JUN-08

#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <algorithm>
//#include "pwnwinutil.h" // don't include this here - it mixes cross platform and windows specific code!
//#ifndef WIN32
//#endif
#include <stdio.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>

// sorry guys, this macro is by lazy skribtsov
// (I decided to introduce it after 20 years of programming in C++)
// ...what's interesting  
// ... new C++ standard has now similar new sytax for such cycle!!! .. something like "for(auto i : array)" , so this means I am in the trend :-)
#define FOR_ALL(vname,i) for(size_t i = 0; i < vname.size(); i++)
#define FOR_ALL_IF(vname,xx,condition) for(size_t xx = 0; xx < vname.size(); xx++) if(condition)
inline const char *yesno(bool v) { return v ? "YES" : "NO"; }

template <typename Tz>
const char *endcomma(const Tz &array, size_t n) { return n + 1 == array.size() ? "\n" : ","; }
inline float rnd() { return rand() / (float)RAND_MAX; }
#define escape_ascii 27
#ifndef WIN32
	#define _cls printf("%c[2J",escape_ascii)
	#define set_cursor(row,col) printf("%c[%d;%dH",escape_ascii,row,col)
#else
	#define _cls system("cls")
	#define set_cursor(row,col) {COORD pos = {col, row}; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);}
#endif


using std::string;
using std::vector;
int getch_os();
void delaySeconds(float t) ;
int kbhit_os();

#ifdef _MSC_VER
	#pragma warning(disable : 4996)
	#define finite_check _finite
	//#include <math.h>
	//#include <cmath>

	//#pragma warning(disable : 4996)
	//#define finite_check _finite
	//#define finite_check std::isfinite
#else
#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE
#endif
#ifndef __USE_ISOC99
#define __USE_ISOC99
#endif

#include <math.h>

//#ifndef finite_check
	#include <cmath>
	#define finite_check std::isfinite
//#else
//	#define finite_check isfinite
//#endif
#endif 



#ifdef _MSC_VER
typedef unsigned __int64 uint64_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
#endif

#ifndef MAX_PATH
#ifdef  PATH_MAX
#define MAX_PATH PATH_MAX
#else
#define MAX_PATH 260
#endif
#endif

#if (defined(WIN32) || defined(_WIN64)) && !defined __MINGW32__
#include <Windows.h>
#include <PWNAudio/dirent.h>
#endif
#ifndef WIN32
#ifndef STM32
#include "dirent.h"
#endif
#endif


inline void save(FILE *file, const string &str) {
		uint64_t isize = str.size();
		fwrite(&isize,sizeof(isize),1,file);
		if(isize!=0)
			fwrite(str.c_str(),sizeof(char), (size_t) isize,file);
}
inline void load(FILE *file, std::string &array) {
		uint64_t isize = 0;
		fread(&isize,sizeof(isize),1,file);
		array.resize((size_t)isize);
		if(isize!=0)
			fread(&(array[0]),sizeof(char), (size_t)isize,file);
}

inline void save(FILE *file, const vector <string> &array) {
	uint64_t isize = array.size();
	fwrite(&isize, sizeof(isize), 1, file);
	FOR_ALL(array, i) save(file, array[i]);
}

inline void load(FILE *file, vector <string> &array) {
	uint64_t isize = 0;
	fread(&isize, sizeof(isize), 1, file);
	array.resize((size_t) isize);
	FOR_ALL(array, i) load(file, array[i]);
}



template <class _T> 
class MapFileSaver { // required _T to have .save / load (file) methods
public:
	static void save(FILE *file, const std::map <std::string,_T> &array) {
		uint64_t isize = array.size();
		fwrite(&isize,sizeof(isize),1,file);
		for(typename std::map <std::string,_T>::const_iterator iter = array.begin(); iter != array.end(); ++iter) {
			::save(file,iter->first);
			iter->second.save(file);
		}
	}
	static void load(FILE *file, std::map <std::string,_T> &array) {
		uint64_t isize = 0;
		fread(&isize,sizeof(isize),1,file);
		for(size_t i = 0; i < isize; i++) {
			std::string str;
			::load(file,str);
			_T obj;
			obj.load(file);
			array[str] = obj;
		}
	}
};
template <class _T> 
class FileSaver {
public:

	static void save(FILE *file, const std::vector <_T> &array) {
			uint64_t isize = array.size();
			fwrite(&isize,sizeof(isize),1,file);
			if(isize!=0)
				fwrite(&array.front(),sizeof(_T), (size_t) isize,file);
	}
	static void load(FILE *file, std::vector <_T> &array) {
			uint64_t isize = 0;
			fread(&isize,sizeof(isize),1,file);
			array.resize((size_t)isize);
			if(isize!=0)
				fread(&array.front(),sizeof(_T), (size_t)isize,file);
	}

	static void save(FILE *file, const std::vector < std::vector <_T > > &graph) {
		uint64_t size = graph.size();
		fwrite(&size,sizeof(size),1,file);
		for(size_t i = 0; i < size; i++) save(file,graph[i]);
	}
	static void load(FILE *file, std::vector < std::vector < _T > > &graph) {
		uint64_t size = 0;
		fread(&size,sizeof(size),1,file);
		graph.resize((size_t)size);
		for(size_t i = 0; i < size; i++) load(file,graph[i]);
	}

	static void save(const char * filename, const std::vector <_T> &array)
	{
		FILE * file = NULL;
		file = fopen( filename, "wb");
		if ( NULL == file)
		{
			throw "Couldn't open file for writing";
		}
		save(file, array);

		fclose(file);
	}

	static void load(const char * filename, std::vector <_T> &array) {
		FILE * file = NULL;
		file = fopen( filename, "rb" );
		if ( NULL == file )
		{
			throw "Couldn't open file for reading";
		}
		load(file, array);

		fclose(file);
	}

	static void save(const char * filename, const std::vector < std::vector <_T > > &graph) {
		FILE * file = NULL;
		file = fopen( filename, "wb");
		if ( NULL == file)
		{
			throw "Couldn't open file for writing";
		}
		save(file, graph);

		fclose(file);
	}
	static void load(const char * filename, std::vector < std::vector < _T > > &graph) {
		FILE * file = NULL;
		file = fopen( filename, "rb" );
		if ( NULL == file )
		{
			throw "Couldn't open file for reading";
		}
		load(file, graph);		

		fclose(file);
	}
};

template <class _T> 
class FileSaverUsingSaveLoad {
public:
	static void save(FILE *file, const std::vector<_T> &array) {
		uint64_t size = array.size();
		fwrite(&size,sizeof(size),1,file);
		for(size_t i = 0; i < array.size(); i++) array[i].save(file);
	}

	static void load(FILE *file, std::vector<_T> &array) {
		uint64_t size = 0;
		fread(&size,sizeof(size),1,file);
		array.resize((size_t) size);
		for(size_t i = 0; i < size; i++) array[i].load(file);
	}

	static void save(const char * filename, const std::vector <_T> &array)
	{
		FILE * file = NULL;
		file = fopen( filename, "wb");
		if ( NULL == file)
		{
			throw "Couldn't open file for writing";
		}
		save(file, array);

		fclose(file);
	}

	static void load(const char * filename, std::vector <_T> &array) {
		FILE * file = NULL;
		file = fopen( filename, "rb" );
		if ( NULL == file )
		{
			throw "Couldn't open file for reading";
		}
		load(file, array);

		fclose(file);
	}

};
int writeFileName (const char *filename, void* userargs);

FILE* saveFileToTemp(const char * in_filename);//return pointer to tempfile

int writeFileNameOnlyInSubDir (const char *filename, void* userargs);

void  restoreFileFromTemp(FILE* temp, const char * out_filename);

inline void pwnSleep( int ms ) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
uint64_t makeID();

//return -1, if file not exist, or can't open
long getFileSize(const char* path);

class SimpleIntParams {
protected:
	std::map <std::string, int> int_params;
public:
	int getInt(const std::string &name) const;
	void loadFromTextFile(const char *filename);
};

class SimpleStringParams {
protected:
	std::map <std::string, std::string> string_params;
public:
	const std::string &getString(const std::string &name) const;
	int getIntValue(const std::string &name) const { return atoi(getString(name).c_str());}
	float getFloatValue(const std::string &name) const { return (float)atof(getString(name).c_str());}
	void loadFromTextFile(const char *filename);
};
inline const char *bool2str(bool b) {
	static const char *yes = "YES";
	static const char *no = "NO";
	return b ? yes : no;
}
inline std::string int2str(int64_t id) {
	char buf[64];
	sprintf(buf, "%lld", id); 
	return buf;
}
inline std::string int2str(int id) {
	return std::to_string(id);
}

inline std::string float2str(float v, const int digits=6) {
	char buf[64];
	sprintf(buf, "%.*f",digits, v); 
	return buf;
}

inline std::string float2str(double v, const int digits = 6) {
	char buf[128];
	sprintf(buf, "%.*lf", digits, v);
	return buf;
}
inline std::string smart_float2str(float v, const int digits=6) {
	int iv = int(v);
	if((float)iv==v) return int2str(iv);
	return float2str(v,digits);
}
inline std::string smart_float2str(double v, const int digits = 6) {
	int iv = int(v);
	if ((double)iv == v) return int2str(iv);
	return float2str(v, digits);
}
inline std::string int2str(::size_t id) {
	return std::to_string(id);
}
#define FixedNameMaxSize 256
struct FixedName {
	char name[FixedNameMaxSize];
	std::string getName() const {return name;}
	void setName(const std::string &str) {
		if(str.size()+1>FixedNameMaxSize) throw("too long name");
		strcpy(name,str.c_str());
	}
};

//---Macros Equivalents
template<typename T>
inline T sqr(const T & in) {
	return in*in;
}

struct BestElement {
	::size_t index;
	float metric;
	BestElement(::size_t i, float m) : index(i), metric(m) {}
	BestElement() {index = 0; metric = 0;}
	bool operator < (const BestElement &other) const
	{
		return metric < other.metric;
	}
};

template<typename T>
struct Range {
	T boundary_left;
	T boundary_right;

	Range(void) { }	
	Range(const T & boundary_left, const T & boundary_right) : boundary_left(boundary_left), boundary_right(boundary_right) {		
		if ( !(this->boundary_left <= this->boundary_right) ) throw "Incorrect boundaries for the range\n";
	}


	inline bool contain(const T & quantity) const { return ((quantity >= this->boundary_left) && (quantity <= this->boundary_right)); }

	inline T size(void) const { return this->boundary_right - this->boundary_left; }

};
typedef int FileCallback(const char *filename, void *userargs);

inline void addslashtopath(string &testpath) {
	if (testpath[testpath.size() - 1] != '\\' && testpath[testpath.size() - 1] != '/')
		testpath.push_back('\\');
}

inline void changeslash(string &str) {
	for (size_t k = 0; k<str.size(); k++) {
		if (str[k] == '/')str[k] = '\\';
	}
}
inline void inversechangeslash(string &str) {
	for (size_t k = 0; k<str.size(); k++) {
		if (str[k] == '\\')str[k] = '/';
	}
}
inline void changeslash(char *ptr) {
	while (*ptr) {
		if (*ptr == '/') *ptr = '\\';
		ptr++;
	}
}


class FileProcessor {
public:
	FileProcessor() {}
	virtual ~FileProcessor() {}
	void processPath(const char *path, const char *wildcard, bool verbose, bool skiphidden = false);
protected:
	virtual bool processFile(const char *filename) = 0; // return true if continue
private:
	static int fileCallback(const char *filename, void *userargs);
};

class FileList : public FileProcessor {
protected:
	vector <string> files;
public:
	FileList() {
	}
	void setFiles(const vector <string> & new_files) {
		files = new_files;
	}
	FileList(const string &path, const string &wildcard, bool random = false) {
		if (path.empty() || wildcard.empty()) return;
		FileProcessor::processPath(path.c_str(), wildcard.c_str(), false);
	/*	if (random)
			random_shuffle(files.begin(), files.end());*/
	}
	void sortByFolders(const vector <string> &folders, const vector<size_t> &indices) const;
	virtual bool processFile(const char *filename) {
		files.push_back(filename);
		return true;
	}
	size_t size() const {	return files.size();	}
	const vector <string> &getFiles() const { return files; }
	const string& operator [](size_t index) const { return files[index]; }
};

// this function was simply WRONG, I spent two ! night hours trying to figure out why my project didn't work!! what a shame!!!
// THIS IS WHAT IT WAS
// GAME: FIND A FEW(!) ERRORS:
//
//template <typename InputIt, typename ValueType, typename ContainerType>
//void split(InputIt begin, InputIt end, const ValueType &delimiter, std::vector<ContainerType> &out)
//{
//	out.clear();
//
//	ContainerType current_part;
//	while(begin != end)
//	{
//		if(*begin == delimiter)
//		{
//			if(!current_part.empty()) out.push_back(current_part);
//			continue;
//		}
//
//		current_part.push_back(*begin);
//		begin++;
//	}
//}

template <typename InputIt, typename ValueType, typename ContainerType>
void split(InputIt begin, InputIt end, const ValueType &delimiter, std::vector<ContainerType> &out)
{
	out.clear();

	ContainerType current_part;
	for(;begin != end;begin++)
	{
		if(*begin == delimiter) {
			if(!current_part.empty()) {
				out.push_back(current_part);
				current_part.clear();
			}
		}
		else
			current_part.push_back(*begin);
	}
	if(!current_part.empty()) out.push_back(current_part);
}
void split(const string &str,char separator,vector<string> &result, bool allow_empty = false);

void split(const string &str,vector<char> separator,vector<string> &result);



#if !defined(_WIN32) && !defined(__ANDROID__  ) && !defined(STM32)
#include <mcheck.h>
#include <iostream>
inline void checkMemory(const void * adr, const char * location)
{
	mcheck_status status_res = mprobe((void*)adr);

	if(status_res == MCHECK_OK)
		return;
	std::cout<<"Consistency at "<<location<<": ";
	switch(status_res)
	{
	case MCHECK_DISABLED:
		std::cout<<"MCHECK_DISABLED";
		break;
	case MCHECK_OK:
		std::cout<<"MCHECK_OK";
		break;
	case MCHECK_HEAD:
		std::cout<<"MCHECK_HEAD";
		break;
	case MCHECK_TAIL:
		std::cout<<"MCHECK_TAIL";
		break;
	case MCHECK_FREE:
		std::cout<<"MCHECK_FREE";
		break;
	default:
		std::cout<<"MCHECK_UNKNOWN";
		break;
	}
	std::cout<<std::endl;
}
#endif

bool pwnRandomPass(float probability /*0...1*/);

void findImgFiles(const string & path, vector<string> & img_files);

int onFindImgFile(const char* path, void* params);

string getFileFromFullPath(const string &fullpath);
void removeExtensionFromFileName(const string & fname, string &fname_no_ext);

std::string correct_path(const std::string & path);
std::string remove_folder_slash(const std::string & path);
void create_dir(std::string path);

std::string get_cur_time(bool print = true);

std::string exec_command_with_msg_thru_pipe(const char* cmd);
std::string exec_command_with_msg_thru_file(const char* command);
std::string ZeroPadNumber(int num, int zeros_num = 7);

template <class T>
class RingBuffer {
	vector<T> storage;
	size_t head; // pointer to the end of queue
	size_t effective_size;
public:
	RingBuffer(size_t maxsize) : storage(maxsize) {
		effective_size = 0;
		head = 0;
	}
	bool push_back(const T &cf) {
		storage[head] = cf; // CamFrame assign operator should be called! -> no memory reallocation
		head++;
		head = head % storage.size();

		//std::cout << "head after push back: " << head << "\n";
		if (effective_size < storage.size()) {
			effective_size++;
			return true;
			//std::cout << "effective_size after push back: " << effective_size << "\n";
		}
		return false;
	}
	bool pop_front(T &frame) { // most old frame arrived
		if (effective_size == 0) return false;
		frame = front();
		effective_size--;
		return true;
	}
	bool pop() {
		//std::cout << "pop\n";
		if (effective_size == 0) return false;
		effective_size--;
		//std::cout << "effective_size = " << effective_size << "\n";
		return true;
	}
	size_t size() const { return effective_size; }
	bool empty() const { return size() == 0; }
	const T& front() const {
		if (effective_size == 0) throw std::runtime_error("empty CamFrameRingBuffer front() request!");
		size_t wherefrom_index = (head + storage.size() - effective_size) % storage.size();
		//std::cout << "front() wherefrom_index" << wherefrom_index << "\n";
		return storage[wherefrom_index];
	}

	void get_vector(std::vector<T>& v)
	{
		v.clear();
		for (size_t i = 0; i < effective_size; i++)
		{
			size_t wherefrom_index = (head + storage.size() - effective_size + i) % storage.size();
			v.push_back(storage[wherefrom_index]);
		}
		return;
	}

};