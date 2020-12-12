// (C) Pawlin Technologies Ltd. 2006
// Purpose: command line program (main) arguments parser

#include "stdafx.h"
#include <PWNGeneral/ArgParser.h>
#include <PWNGeneral/pwnutil.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream> 

/*#ifndef LINUX
#ifndef MSC_VER
#define _stricmp strcasecmp
#define _strnicmp strncmp
#endif
#endif*/

/*#ifdef LINUX 

#include <string>
#define strcasecmp _stricmp
//#define _strnicmp strncmp
#define strncasecmp _strnicmp 
#endif

int strcasecmp(const char* str1, const char* str2, bool sensitive = true)
{
	if (sensitive)
		return strcmp(str1,str2);
	return _stricmp(str1, str2);
}
int strncasecmp(const char* str1, const char* str2, size_t max_count, bool sensitive = true)
{
	if (sensitive)

		return strncmp(str1,str2,max_count);


	return _strnicmp(str1, str2, max_count);
}*/



#ifndef WIN32
	#include <strings.h>
#endif
#include <PWNGeneral/ArgParser.h>
int strcasecmp_os(const char* str1, const char* str2, bool sensitive = true)
{
	if (sensitive)
		return strcmp(str1,str2);
	#ifdef WIN32
		return _stricmp(str1, str2);
	#else
	   return strcasecmp(str1, str2);
	#endif
}
int strncasecmp_os(const char* str1, const char* str2, size_t max_count, bool sensitive = true)
{
	if (sensitive)
		return strncmp(str1,str2,max_count);
	#ifdef WIN32
		return _strnicmp(str1, str2, max_count);
	#else
		return strncasecmp(str1, str2, max_count);
	#endif
}



bool ArgParser::queryFlag(const char* flag) const {
	
	for (int i = 1; i < arg_num; i++)
		if (!strcasecmp_os(args[i],flag,sensitive)) return true;

	return false;
}

void ArgParser::queryAll(std::map<std::string,std::string> &params) const { // fills all param=value pairs
	for(int i = 1; i < arg_num; i++) {
		vector <std::string> pair;
		std::string arg = this->args[i];
		split(arg.begin(),arg.end(),'=',pair);
		if(pair.size()==2) params[pair.front()] = pair.back();
		if(pair.size()==1) params[pair.front()] = string("");
	}
}

bool ArgParser::queryString(const char* paramName, std::string &value) const { //returns true if the parameter exists
	char* valPos = 0;

	for (int i = 1; i < arg_num; i++) 
		if (!strncasecmp_os(args[i],paramName, strlen(paramName),sensitive)) {
			valPos = strchr(args[i], '=');
			valPos++;
			value = valPos;
			return true;
		}

	return false;
}

bool ArgParser::queryDouble(const char* paramName, double &value) const { //returns true if the parameter exists
	const char* valPos = 0;

	for (int i = 1; i < arg_num; i++) {
		unsigned pLen = (unsigned) strlen(paramName);
		if (!strncasecmp_os(args[i],paramName, pLen, sensitive)) {
			valPos = strchr(args[i], '=');
			valPos++;
			value = atof(valPos);
			if(value == 0.0 && valPos[0]!='0') {printf("wrong format for double value\n"); throw("wrong format for double value");}
			return true;
		}
	}

	return false;
}

bool ArgParser::queryFloat(const char* paramName, float &value) const { //returns true if the parameter exists
	const char* valPos = 0;

	for (int i = 1; i < arg_num; i++) {
		unsigned pLen = (unsigned) strlen(paramName);
		if (!strncasecmp_os(args[i],paramName, pLen, sensitive)) {
			valPos = strchr(args[i], '=');
			valPos++;
			double dvalue;
			dvalue = atof(valPos);
			value = (float) dvalue;
			if(value == 0.0 && valPos[0]!='0') {printf("wrong format for float value\n"); throw("wrong format for float value");}
			return true;
		}
	}

	return false;
}
bool ArgParser::queryFilename(const char* paramName, std::string &value) const { //returns true if the parameter exists

	char* valPos = 0;

	for (int i = 1; i < arg_num; i++) 
		if (!strncasecmp_os(args[i],paramName, strlen(paramName),sensitive)) {
			valPos = strchr(args[i], '=');
			valPos++;
			value = valPos;
			FILE *stream = fopen(valPos,"rt");
			if(!stream) {
				printf("can't open file %s\n",valPos);
				throw(paramName);
			}
			fclose(stream);
			return true;
		}

	return false;
}

bool ArgParser::readFilelist(const std::string &filelist, std::vector <std::string> &inpFileNames) {
	FILE* readFile = fopen(filelist.data(),"r");
	if(!readFile) {
		printf("Can't open filelist %s\n", filelist.data());
		return false;
	}
	std::string str;
	char buf[1000];
	while( fscanf(readFile, "%s", buf) == 1 ) {
		str = buf;
		inpFileNames.push_back(str);
	}
	fclose(readFile);
	return true;
}

void ArgParser::setCaseSensitive(bool flag)
{
	sensitive = flag;
}

void ArgParser::saveToFile(FILE *fp)const {
	fprintf(fp,"PWN ArgParser dump\n");
	if(sensitive) 
		fprintf(fp,"sensitive=1\n");
	else
		fprintf(fp,"sensitive=0\n");
	fprintf(fp,"arg_num=%d\n",arg_num);
	for(int k=0;k<arg_num;k++){
		fprintf(fp,"%s\n",args[k]);
	}
}

void  ArgParser::loadFromFile(FILE *fp) {
	char buf[2048];
	if(fscanf(fp,"PWN ArgParser dump\n")==EOF)
		return;
/*	if(!strcasecmp(buf,"PWN ArgParser dump"))
		throw "Wrong args params";*/
	int sensitive_int;
	fscanf(fp,"sensitive=%d\n",&sensitive_int);
	if(sensitive_int==0)
		sensitive=false;
	else
		sensitive=true;
	
	vector<string> tmp_args;
	int k = 0;
	while (!feof(fp)) {
		fgets(buf, 2048, fp);
		if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = 0;
		tmp_args.push_back(std::string(buf));
		//args[k] = new char[strlen(buf) + 1];
		//strcpy(args[k], buf);
		k++;
	}
	arg_num = (int)tmp_args.size();
	fscanf(fp, "arg_num=%d\n", &arg_num);

	args = new char*[arg_num];
	for(int k=0;k<arg_num;k++){
		//fgets(buf, 2048,fp);
		//if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = 0;
		args[k]=new char[strlen(tmp_args[k].c_str())+1];
		strcpy(args[k], tmp_args[k].c_str());
	}
	//for(int k=0;k<arg_num;k++){
	//	fgets(buf, 2048,fp);
	//	if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = 0;
	//	args[k]=new char[strlen(buf)+1];
	//	strcpy(args[k],buf);
	//}
}

void  ArgParser::loadFromBatFile(const std::string &filePath ) {
	//char buf[2048];
	vector<string> tmp_args;
	std::string tmpstr;
	std::ifstream file(filePath.c_str());
	std::getline(file, tmpstr);
	char *token = strtok(&tmpstr[0], " ");

	// Keep printing tokens while one of the 
	// delimiters present in str[]. 
	while (token != NULL)
	{
		tmp_args.push_back(token);
		token = strtok(NULL, " ");
	}

	arg_num = (int)tmp_args.size();
	//fscanf(fp, "arg_num=%d\n", &arg_num);

	args = new char*[arg_num];
	for (int k = 0; k < arg_num; k++) {
		args[k] = new char[strlen(tmp_args[k].c_str()) + 1];
		strcpy(args[k], tmp_args[k].c_str());
	}
}


void  ArgParser::dumpToMap(std::map <std::string,std::string> &dump)const{
	for(int k=0;k<arg_num;k++){
		string str(args[k]);
		size_t pos;
		if((pos=str.find_last_of("="))!=string::npos){
			string paramName=str.substr(0,pos-1);
			string paramValue=str.substr(pos,str.size()-pos);
			dump[paramName]=paramValue;
		}else{
			dump["param"+int2str(k)]=args[k];

			dump["param"+int2str(k)]=args[k];

		}

	}

}