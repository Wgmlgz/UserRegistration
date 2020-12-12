#pragma once
//#include "stdafx.h"
#include <string>
#include <vector>
#include <time.h>
#include <ctime>
#undef UNICODE

#if defined(WIN32) && defined(__MINGW32__)
#define USE_LINUX

#else
	#if defined(WIN32) || defined(_WIN64)
		#define USE_WIN
	#else
		#define USE_LINUX
	#endif
#endif

#ifndef USE_WIN
	#include <sys/time.h>
	#include <stdio.h>
#include <unistd.h>
#else
#ifndef NOMINMAX
	#define NOMINMAX
#endif
	#include <windows.h>
#endif

#ifndef timersub
#define timersub(a, b, result)                                                \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                          \
    if ((result)->tv_usec < 0) {                                              \
      --(result)->tv_sec;                                                     \
      (result)->tv_usec += 1000000;                                           \
    }                                                                         \
  } while (0)
#endif



#include <time.h>
//#define WIN32_LEAN_AND_MEAN

//#define LARGE_INTEGER long long
//#endif

class CDuration
{
protected:

#ifdef WIN32
	LARGE_INTEGER m_liStart;
	LARGE_INTEGER m_liStop;
	LONGLONG m_exclusion;
	LARGE_INTEGER m_exStart;
	LARGE_INTEGER m_exStop;
	LONGLONG m_llFrequency;
	LONGLONG m_llCorrection;
#else
	struct timeval m_liStart;
        struct timeval m_liStop;
	struct timeval m_exStart;
        struct timeval m_exStop;
	double m_exclusion;
#endif
	


public:
	CDuration(void);

	void Start(void);
	void Stop(void);
	inline void Pause() {
		#ifdef USE_WIN
			QueryPerformanceCounter(&m_exStart);
		#else
			gettimeofday(&m_exStart, 0);
		#endif
			//printf("exstart %li ", m_exStart.QuadPart);
	}
	inline void Resume() {
		#ifdef USE_WIN
			QueryPerformanceCounter(&m_exStop);
			//printf("exstop %li ", m_exStop.QuadPart);
			m_exclusion += m_exStart.QuadPart - m_exStop.QuadPart; // m_exlusion will be negative, it should be added in getDuration
		#else
			gettimeofday(&m_exStop, 0);
			struct timeval delta;
			timersub(&m_exStop, &m_exStart,&delta);
			m_exclusion -= (double)(delta.tv_sec*1000000+delta.tv_usec);
		#endif
	}

	double GetDuration(void) const;
    double GetCurDuration(void) const;
	float getDurationSec() { 
		Stop();
		return float(GetDuration()/1000000.0); 
	}

	// measures time interval from the time of calling Start
	bool timer_ms(int milliseconds) {
		Stop(); // we can "stop" it multiple times as GetDuration measures time between start and last stop
		int timepassed_ms = int(GetDuration() / 1000.0 + 0.5);
		if(timepassed_ms < milliseconds && timepassed_ms >= 0) return false; // wait
		Start();
		return true; // ready for next interval measurement
	}
};

class CDurationTimer : public CDuration {
	int period;
public:
	CDurationTimer(int period) : period(period) {}
	bool pulse() { return timer_ms(period); }
};

inline CDuration::CDuration(void)
{
#ifdef USE_WIN

	LARGE_INTEGER liFrequency;

	QueryPerformanceFrequency(&liFrequency);
	m_llFrequency = liFrequency.QuadPart;
	// Calibration
	Start();
	Stop();

	m_llCorrection = m_liStop.QuadPart-m_liStart.QuadPart;
#else
	Start(); // initialize start time-- important for timer purposes
#endif
}

inline void CDuration::Start(void)
{
	// Ensure we will not be interrupted by any other thread for a while
	#ifndef USE_WIN
		#ifndef __MINGW32__
	 			sleep(0);
		#endif
	 	m_exclusion = 0;
	 	gettimeofday(&m_liStart, 0);
	#else
	 	Sleep(0);
        	m_exclusion = 0;
	 	QueryPerformanceCounter(&m_liStart);
	#endif
	
	//printf("...profiler start... %li\n",m_liStart);
}

inline void CDuration::Stop(void)
{
	#ifndef USE_WIN
		gettimeofday(&m_liStop, 0);
	#else
		QueryPerformanceCounter(&m_liStop);
	#endif
	//printf("...profiler stop... %li\n",m_liStop);
}


inline double CDuration::GetCurDuration(void) const
{
#ifdef WIN32
	LARGE_INTEGER liStop;
#else
	struct timeval liStop;
#endif
#ifndef WIN32
	gettimeofday(&liStop, 0);
#else
	QueryPerformanceCounter(&liStop);
#endif


#ifdef WIN32
	return (double)(liStop.QuadPart - m_liStart.QuadPart - m_llCorrection + m_exclusion)*1000000.0 / m_llFrequency;
#else
	struct timeval delta;
	timersub(&liStop, &m_liStart, &delta);
	return (double)(delta.tv_sec * 1000000 + delta.tv_usec) + m_exclusion;
#endif
}
inline double CDuration::GetDuration(void) const
{
	#ifdef USE_WIN
		return (double)(m_liStop.QuadPart-m_liStart.QuadPart-m_llCorrection + m_exclusion)*1000000.0 / m_llFrequency;
	#else
		struct timeval delta;
		timersub(&m_liStop, &m_liStart,&delta);
		return (double)(delta.tv_sec*1000000+delta.tv_usec)+m_exclusion;
	#endif
}


class Profiler : public CDuration {
	std::vector <std::string> names;
	std::vector <double> durations;
	double forgetfactor;
	int pcounter;
public:
	Profiler(double f = 0.5) : forgetfactor(f),pcounter(0) {};
	~Profiler() {};

	void startSequence() {Start();pcounter=0;}
	void markPoint(const char *name) { 
		Stop();
		pcounter++; 
		if(pcounter > (int)names.size()) {
			names.push_back(name);
			durations.push_back(GetDuration());
		}
		else {
			durations[pcounter-1] = (1-forgetfactor)*durations[pcounter-1] + forgetfactor*GetDuration();
		}
		Start();
	}
	const char *getPointName(int i) const { return names[i].data(); }
	double getPointDuration(const char *name) const {
		for(unsigned int i = 0; i < names.size(); i++)
			if(names[i] == name) return durations[i]/1000; //milliseconds
		return 0; //error
	}
	double getPointDuration(int i) const { return durations[i]/1000; } // milliseconds
	const char *formatDurationByIndex(char *buf, int i, int digits = 4 ) const {
		sprintf(buf,"%0*.1lf",digits, getPointDuration(i));
		return buf;
	}
	const char *formatDuration(char *buf, const char *name, int digits = 4) const {
		sprintf(buf,"%0*.1lf",digits, getPointDuration(name));
		return buf;
	}
	const char *formatDuration(char *buf, double d, int digits = 4) const {
		sprintf(buf,"%0*.1lf",digits, d);
		return buf;
	}
	int getPointsCount() const { return (int) names.size(); }
	void print() const {
		for(int i = 0; i < getPointsCount(); i++) 	printf("%s takes %.2f ms\n",getPointName(i),getPointDuration(i));
		
	}
	void tablePrint(FILE *file) const {
		for(int i = 0; i < getPointsCount(); i++) 
			fprintf(file,"%f\t",getPointDuration(i));
	}
	void tablePrintHeader(FILE *file) const {
		for(int i = 0; i < getPointsCount(); i++) 
			fprintf(file,"%s\t",getPointName(i));
	}
	void print_full() const {
		for(int i = 0; i < getPointsCount(); i++) 
			printf("%s takes %f ms\n",getPointName(i),getPointDuration(i));
	}

	void print_full(FILE *file) const {
		for(int i = 0; i < getPointsCount(); i++) 
			fprintf(file, "%s takes %f ms\n",getPointName(i),getPointDuration(i));
	}

	void print_time() const {
#ifdef WIN32
		char tmpbuf[128];
#ifndef __MINGW32__
		/* Display operating system-style date and time. */
		_strtime( tmpbuf );
#else
		time_t t = time(NULL);
		struct tm *tmp = localtime(&t);
		strftime(tmpbuf,128,"%c",tmp);
#endif
		printf( "OS time:\t%s\n", tmpbuf );
#else
	throw("not implemented");
#endif
	}
	void print_time(FILE *file) const {
#ifdef WIN32
		char tmpbuf[128];
#ifndef __MINGW32__
		/* Display operating system-style date and time. */
		_strtime( tmpbuf );
#else
		time_t t = time(NULL);
		struct tm *tmp = localtime(&t);
		strftime(tmpbuf,128,"%c",tmp);
#endif

		fprintf(file, "%s\t", tmpbuf );
#else
		std::time_t t = std::time(nullptr);
		char mbstr[256];
		std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&t));
		fprintf(file,"%s\n", mbstr);
#endif
	}	
};

struct LogFile {
	FILE *file;
	bool headerPrinted;
	bool appending;
	LogFile(const char *filename, bool append = false) {
		appending = append;
		file = fopen(filename,append ? "at+" : "wt");
		headerPrinted = false;
		if(appending) return;
		Profiler temp; fprintf(file,"Log created at ");
		temp.print_time(file);
		fprintf(file,"\n");
	}
	~LogFile() {
		if(!appending) {
			Profiler temp; fprintf(file,"Log destroyed at ");
			temp.print_time(file);
			fprintf(file,"\n");
		}
		fclose(file);
	}
	void log(const Profiler &prof, bool logsystime = true) const {
		if(!headerPrinted) {
			prof.tablePrintHeader(file);
			if(logsystime) fprintf(file,"OS Time\t");
			fprintf(file,"\n");
			(bool &)headerPrinted = true;
		}
		prof.tablePrint(file);
		if(logsystime) prof.print_time(file);
		fprintf(file,"\n");
		fflush(file);
	}

	void log_full(const Profiler &prof) const {
		prof.print_full(file);
		fflush(file);
	}
};
