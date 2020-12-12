// (c) Pawlin Technologies Ltd. 2008
// Pawlin Utility Library
// contains common algorithms useful in different software
// ansi-C/C++ based
// see details in header file

#include "stdafx.h"
#include "pwnutil.h"
#include <stdio.h>
#include <limits>
#include <ctime>
#include <random>
#include <PWNGeneral/PWNExceptionBasic.h>
#include <fstream>


#if defined ODROID
#if defined WIN32 || defined _WIN32
#  ifdef WIN32
#    undef WIN32
#  endif
#  ifdef _WIN32
#    undef _WIN32
#  endif
#endif
#endif

#if !defined WIN32 && !defined _WIN32
#include <libgen.h>
#include "fnmatch.h"
#endif

#ifdef _WIN32
#include "Shlwapi.h"
#pragma comment(lib,"shlwapi.lib")
int fnmatch(const string &wildcard,const string &filename, int) {
	std::wstring wfilename(filename.begin(),filename.end());
	std::wstring wwildcard(wildcard.begin(), wildcard.end());

	return PathMatchSpecW(wfilename.c_str(), wwildcard.c_str()) ? 0 : -1;
}
#endif

uint64_t makeID() {
	time_t currentTime;
	struct tm *localTime;

	time(&currentTime);                    // Get the current time
	localTime = localtime(&currentTime);   // Convert the current time to the local time

	int Hour = localTime->tm_hour;
	int Min = localTime->tm_min;
	int Day = localTime->tm_mday;
	int Mon = localTime->tm_mon;
	int Year = localTime->tm_year;
	return uint64_t(uint64_t(uint64_t(uint64_t(Year * 100 + Mon) * 100 + Day) * 100 + Hour) * 100 + Min) * 1000;
}


int recursiveFileProcess(
	const char *_path,
	const char *wildcard,
	FileCallback *callback,
	void *userargs,
	int verbose,
	bool skiphiddenfiles
) {
	if (skiphiddenfiles) throw("skipping hidden files is not implemented on linux\n");
	string testpath(_path);
	if (testpath[testpath.size() - 1] != '/' && testpath[testpath.size() - 1] != '\\') {
		testpath.push_back('/');
		if (verbose) printf("path \n%s\nwas invalid, adding '/' to the end, now it's \n%s\n", _path, testpath.c_str());
	}

	const char *path = testpath.c_str();
	if (verbose) printf("Processing folder %s...\n", path);
	//string wildcardpath = string(path);//+ string("*.*"); //search for anything including directories
	string curdir = string(".");
	string updir = string("..");
	struct dirent* entry;
	DIR* d_fh = opendir(path);
	if (d_fh == NULL)
		return false;
	while ((entry = readdir(d_fh)) != NULL) {
		if (entry->d_type == DT_DIR) {
			if (verbose)
				puts(entry->d_name);
			string dir = string((const char *)entry->d_name);
			if (dir == curdir || dir == updir) continue;
			dir = string(path) + dir + string("/");
			recursiveFileProcess(dir.c_str(), wildcard, callback, userargs, verbose, skiphiddenfiles);
		}
	}
	rewinddir(d_fh);
	//wildcardpath = string(path) + string(wildcard);
	//if(verbose)puts("Files ..");
	bool result = 1;
	while ((entry = readdir(d_fh)) != NULL) {
		if (entry->d_type != DT_DIR) {
			if (fnmatch(wildcard, entry->d_name, 0) == 0) {
				//if(verbose)puts(entry->d_name);
				string filepath = testpath + string(entry->d_name);
				if (!((*callback)(filepath.c_str(), userargs))) { result = false; break; }// stop processing if callback returns false
			}
		}
	}
	closedir(d_fh);
	return result;
}



void SimpleIntParams::loadFromTextFile(const char *filename) {
	FILE *file = fopen(filename,"rt");
	if(!file) throw(PWNExceptionBasic("SimpleIntParams::loadFromTextFile can't open file"));
	char buf[1024];
	int value = 0;
	while(!feof(file)) {
		int k = fscanf(file,"%s %d",buf,&value);
		if(feof(file) || k!=2) break;
		int_params[buf]=value;
	}
	fclose(file);
}

void SimpleStringParams::loadFromTextFile(const char *filename) {
	FILE *file = fopen(filename,"rt");
	if(!file) throw(PWNExceptionBasic("SimpleIntParams::loadFromTextFile can't open file"));
	char buf[1024];
	char value[1024];
	while(!feof(file)) {
		int k = fscanf(file,"%s %s",buf,value);
		if(feof(file) && k!=2) break;
		this->string_params[buf]=value;
	}
	fclose(file);
}

int SimpleIntParams::getInt(const std::string &name) const {
	std::map <std::string, int>::const_iterator iter = int_params.find(name);
	if(iter==int_params.end()) throw(PWNExceptionBasic("SimpleIntParams::getParam -- no such parameter or check spelling"));
	return iter->second;
}

const std::string &SimpleStringParams::getString(const std::string &name) const {
	std::map <std::string, std::string>::const_iterator iter = string_params.find(name);
	if(iter==string_params.end()) throw(PWNExceptionBasic("SimpleStringParams::getParam -- no such parameter or check spelling"));
	return iter->second;
}

#ifdef _WIN32
	#include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
	#include <sys/time.h>
    #include <dirent.h> 
    #include <fnmatch.h>
   // #include "pwnwinutil.h"
	#include <sys/ioctl.h>
	// this doesn't compile on any OS.....
    //void _get_pgmptr(char** path){
    //     *path=_pgmptr;
    //}
    //
    //void _get_wpgmptr(wchar_t** path){
    //    *path=_wpgmptr;
    //}
#endif


FILE* saveFileToTemp(const char * in_filename){//return pointer to tempfile
	FILE *temp = NULL;
	FILE* fp = fopen(in_filename, "rt");

	//save file
	if (fp != NULL){
		if((temp=tmpfile())!=NULL){
			const int bufsize = 1024*16;
			char buf[bufsize];
			do{
				size_t count_bytes = fread(buf, 1, bufsize, fp);
				fwrite(buf, 1, count_bytes, temp);
			}while (!feof(fp));
		}
		fclose(fp);
	}
	return temp;
}

void  restoreFileFromTemp(FILE* temp, const char * out_filename){

	//restore config.txt
	
	if(temp != NULL){
		FILE* fp = fopen(out_filename, "wt");
	    fseek(temp,0, SEEK_SET);
		const int bufsize = 1024*16;
		char buf[bufsize];
		do{
			size_t count_bytes = fread(buf, 1, bufsize, temp);
			fwrite(buf, 1, count_bytes, fp);
		}while (!feof(temp));
		fclose(temp);
		fclose(fp);
	}
	
}


int writeFileName (const char *filename, void* userargs) {
	FILE* file = (FILE*)userargs;
	fprintf (file,"%s\n", filename);
	return 1;
}


int writeFileNameOnlyInSubDir (const char *filename, void* userargs) {
	FILE* file = (FILE*)userargs;
	size_t size = strlen(filename);
	bool isSub = false;
	for (size_t i = 0; i < size; i++) {
		if (filename[i] == '/') {
			isSub = true;
			break;
		}
	}
	if (isSub) fprintf (file,"%s\n", filename);
	return 1;
}

int kbhit_os(){
	#if defined(_WIN32) || defined(_WIN64)
		return _kbhit();
	#else
		
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting = 0;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
		
		
	#endif
}

int getch_os(){
#if defined (_WIN32) || defined(_WIN64)
    return _getch();
#else
    termios oldt,
    newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
    
#endif
}
#ifdef _MSC_VER
#include "windows.h"
#endif
#ifdef __MINGW32__
#include <chrono>
#include <thread>
#endif

	void delaySeconds(float t) {
#ifndef _WIN32
		__useconds_t delay_u = (__useconds_t)(1000*1000*t);
		usleep(delay_u);
#else
#ifdef __MINGW32__
		//sleep(t*1000*1000); //unresolved problem so far
#else
	Sleep(int(t*1000));
#endif
#endif
	}

long getFileSize(const char* path){
	FILE* fp = fopen(path, "rt");
	if (!fp) return -1;
	fseek(fp, 0L, SEEK_END);
	long sz = ftell(fp);
	fclose(fp);
	return sz;
}

//  from pwnwutil

void FileProcessor::processPath(const char *path,const char *wildcard,bool verbose, bool skiphidden) {
	::recursiveFileProcess(path,wildcard,FileProcessor::fileCallback,this,verbose,skiphidden);
}

int FileProcessor::fileCallback(const char *filename,void *userargs) {
	FileProcessor *_this = (FileProcessor *) userargs;
	return _this->processFile(filename) ? 1 : 0;
}


int onFindImgFile(const char* path, void* params){
	vector<string> & img_files = *(vector<string>*)params;
	img_files.push_back(string(path));
	return 1;
}

void findImgFiles(const string & path, vector<string> & img_files){
	vector<string> ext;
	ext.push_back("*.jpg");
	ext.push_back("*.jpeg");
	ext.push_back("*.tiff");
	ext.push_back("*.tif");
	ext.push_back("*.gif");
	ext.push_back("*.bmp");
	ext.push_back("*.pgm");
	ext.push_back("*.png");


	ext.push_back("*.JPG");
	ext.push_back("*.JPEG");
	ext.push_back("*.TIFF");
	ext.push_back("*.TIF");
	ext.push_back("*.GIF");
	ext.push_back("*.BMP");
	ext.push_back("*.PGM");
	ext.push_back("*.PNG");


	FileCallback* pFileCallback = &onFindImgFile;
	for (unsigned i = 0; i< ext.size(); i++){
		recursiveFileProcess(
			path.c_str(), 
			ext[i].c_str(), 
			pFileCallback, 
			(void *) &img_files,
			false,
			true // skip hidden files
		); 
	}
}
string getFileFromFullPath(const string &fullpath) {
	static vector<char> delims = { '\\','/' };
	vector<string> pieces;
	split(fullpath, delims, pieces);
	return pieces.back();
}
void removeExtensionFromFileName (const string & fname, string &fname_no_ext) {
	fname_no_ext.clear();
	size_t size = fname.size();
	char s[1];
	for (unsigned k = 0 ; k < size; k++	) {
		s[0] = fname.c_str()[k];
		if(s[0] == '.') {
			return;
		}

		if(fname_no_ext.empty()) {
			fname_no_ext = string(&s[0], 1);
		}
		else {
			fname_no_ext += string(&s[0], 1);
		}
	}

	return;		
}

//void FileProcessor::processPath(const char *path, const char *wildcard, bool verbose, bool skiphidden) {
//	::recursiveFileProcess(path, wildcard, FileProcessor::fileCallback, this, verbose, skiphidden);
//}

//int FileProcessor::fileCallback(const char *filename, void *userargs) {
//	FileProcessor *_this = (FileProcessor *)userargs;
//	return _this->processFile(filename) ? 1 : 0;
//}

void FileList::sortByFolders(const vector <string> &folders, const vector<size_t> &indices) const {
	for (size_t n = 0; n<files.size(); n++) {

		string folderName = folders[indices[n]]; //TODO: do a check
		string cmd = "copy " + files[n] + " " + folderName;
		/*	char *buf=new char[cmd.size()];
		memcpy(buf,cmd.c_str(),cmd.size()*sizeof(char));
		changeslash(buf);*/
		changeslash(cmd);
		system(cmd.c_str());
		//	delete [] buf ;
	}


}


bool pwnRandomPass(float probability /*0...1*/){
	static bool isInitialized=false;
	if(isInitialized==false){
		isInitialized=true;
		std::srand((unsigned int)std::time(0));
	}
	float randomValue=(rand()/(float)RAND_MAX);
	if(randomValue<=probability)
		return true;
	else 
		return false;
}

void split(const string &str,char separator,vector<string> &result, bool allow_empty){

	string::const_iterator cur=str.begin();
	string::const_iterator last=str.begin();
	while(cur!=str.end()){
		if((*cur)==separator){
			if(cur!=last || allow_empty){
			
				result.push_back(string(last,cur));
				last=cur+1;
			}
		}
		cur++;
	}
	if(last!=str.end() || allow_empty)
	{
		result.push_back(string(last,str.end()));
	}
}
void split(const string &str,vector<char> separator,vector<string> &result){

	string::const_iterator cur=str.begin();
	string::const_iterator last=str.begin();
	while(cur!=str.end()){
		bool founded=false;
		for(size_t k=0;k<separator.size();k++){
			if((*cur)==separator[k]){
				founded=true;
				break;
			}
		}
		if(founded){
			if(cur!=last){
				result.push_back(string(last,cur));
			}
			last=cur+1;
		}
		cur++;
	}
	if(last!=str.end())
	{
		result.push_back(string(last,str.end()));
	}
}

std::string correct_path(const std::string & path) {
	if (path.empty()) {
		return std::string();
	}
	std::string corrected_path = path;
	if (corrected_path.back() != '/' && corrected_path.back() != '\\') corrected_path += string("/");

	return corrected_path;
}

std::string remove_folder_slash(const std::string & path) {
	if (path.empty()) {
		throw "Can't correct_path\n";
	}

	std::string corrected_path = path;
	if (corrected_path.back() == '/' || corrected_path.back() == '\\') {
		//corrected_path.erase(corrected_path.end());
		corrected_path.pop_back();
	}

	return corrected_path;
}
void create_dir(std::string path) {
	char sbuf[256];
	sprintf(sbuf, "mkdir %s", path.c_str());
	system(sbuf);
}

std::string get_cur_time(bool print) {
	char buff[100];
	time_t now = time(0);
	strftime(buff, 100, "%d_%m_%Y_%H_%M_%S", localtime(&now));
	if(print)printf("%s\n", buff);
	return std::string(buff);

}

std::string exec_command_with_msg(const char* cmd) {
	//std::array<char, 4096> buffer;
	const int buffer_size = 4096;
	char buffer[buffer_size];
	std::string result;
	std::string cmd_str = std::string(cmd);
	cmd_str.append(" 2>&1");
#ifdef __ANDROID__
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd_str.c_str(), "r"), pclose);
#endif
#ifdef _WIN32
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd_str.c_str(), "r"), _pclose);
#endif
#ifdef LINUX
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd_str.c_str(), "r"), pclose);
#endif
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer, buffer_size, pipe.get()) != nullptr) {
		result += buffer;
	}
	return result;
}

std::string exec_command_with_msg_thru_pipe(const char* cmd) {
	//std::array<char, 4096> buffer;
	const int buffer_size = 4096;
	char buffer[buffer_size];
	std::string result;
	std::string cmd_str = std::string(cmd);
	cmd_str.append(" 2>&1");
#ifdef _WIN32
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd_str.c_str(), "r"), _pclose);
#endif
#ifdef LINUX
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd_str.c_str(), "r"), pclose);
#endif
#ifdef __ANDROID__
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd_str.c_str(), "r"), pclose);
#endif
	if (!pipe) {
		std::cout << "Popen failed!" << std::endl;
		return std::string();
	}

	//std::fflush(pipe.get());

	while (fgets(buffer, buffer_size, pipe.get()) != nullptr) {
		result += buffer;
	}
	return result;
}


std::string exec_command_with_msg_thru_file(const char* command) {
	char tmpname[L_tmpnam];
	std::tmpnam(tmpname);
	std::string scommand = command;
	std::string cmd = scommand + " >> " + tmpname;
	std::system(cmd.c_str());
	std::ifstream file(tmpname, std::ios::in);
	std::string result;
	if (file) {
		while (!file.eof()) result.push_back(file.get());
		file.close();
	}
	remove(tmpname);
	return result;
}

std::string ZeroPadNumber(int num, int zeros_num )
{	
	std::string ret = std::to_string(num);
	// Append zero chars
	int str_length = (int) ret.length();
	for (int i = 0; i < zeros_num - str_length; i++) {
		ret = "0" + ret;
	}
		
	return ret;
}
