// File: ArgParser.h
// Purpose: parser of console (stdio) program arguments
// Authors: S. Korobkova, P. Skribtsov
// Date: Jan 2009
// (C) PAWLIN TECHNOLOGIES LTD. ALL RIGHTS RESERVED

#pragma once

#include <vector>
#include <string>
#include <map>

//#include <tchar.h>

class ArgParser {
	int arg_num;
	char** args;
	bool sensitive;

public:
	ArgParser(){arg_num=0; args=NULL;}
	ArgParser(int argc, char* argv[], bool set_sensitive = true) : arg_num(argc), args(argv),sensitive(set_sensitive) {};
	void queryAll(std::map<std::string,std::string> &params) const; // fills all param=value pairs
	bool queryFlag(const char* flag) const;
	bool queryString(const char* paramName, std::string &value) const;
	bool queryDouble(const char* paramName, double &value) const;
	bool queryFloat(const char* paramName, float &value) const;
	bool querySize_t(const char *paramName, size_t &value) const {
		double d = 0;
		if(!queryDouble(paramName,d)) return false;
		value = (size_t) d;
		return true;
	}
	bool queryInt(const char *paramName, int &value) const {
		double d = 0;
		if(!queryDouble(paramName,d)) return false;
		value = (int) d;
		return true;
	}
	bool queryFilename(const char* paramName, std::string &value) const;
	void setCaseSensitive(bool flag = true);
	static bool readFilelist(const std::string &filelist, std::vector <std::string> &files);
	void saveToFile(FILE *fp)const ;
	void loadFromFile(FILE *fp) ;
	void loadFromBatFile(const std::string &filePath);
	void dumpToMap(std::map <std::string,std::string> &dump)const;
};