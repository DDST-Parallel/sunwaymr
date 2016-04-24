#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <fstream>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
using namespace std;

int splitString(const std::string& str, std::vector<std::string>& ret, const std::string& delim)
{
	if (str.empty())
	{
		return 0;
	}

	string tmp;
	string::size_type start = 0U;
	string::size_type end = str.find(delim);
	while (end != std::string::npos)
	{
		tmp = str.substr(start, end - start);
		ret.push_back(tmp);
		start = end + delim.length();
		end = str.find(delim, start);
	}

	tmp = str.substr(start, end);

	if (!tmp.empty())
	{
		ret.push_back(tmp);
		tmp.clear();
	}

	return 0;
}


std::vector<std::string> &splitString(const std::string &s, char delim, std::vector<std::string> &elems) {
    string ch = string(1, delim);
    splitString(s, elems, ch);
    return elems;
}

std::vector<std::string> splitString(const std::string &s, char delim) {
    std::vector<std::string> elems;
    splitString(s, delim, elems);
    return elems;
}

string fileNameFromPath(string const& pathname )
{
	return pathname.substr(pathname.find_last_of("/\\") + 1);
}

int vectorFind(const vector<string>& v, string value){
	int index=-1;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		if(v[i]==value){
			index=i;
			break;
		}
	}
	return index;
}


vector<int> vectorFindAll(const vector<string>& v, string value){
	vector<int>  indexVector;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		if(v[i]==value){
            indexVector.push_back(i);
		}
	}
	if(indexVector.size()==0){
		indexVector.push_back(-1);
	}

	return indexVector;
}

int vectorNonZero(const vector<int>& v){
	int index=-1;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		if(v[i]!=0){
			index=i;
			break;
		}
	}
	return index;
}

vector<int> vectorNonZeroAllIndex(const vector<int>& v){
	vector<int> indexVector;
	int vs=v.size();
	for (int i = 0; i < vs; i++){
		if(v[i]>0){
			indexVector.push_back(i);
		}
	}
	if(indexVector.size()==0){
		indexVector.push_back(-1);
	}
	return indexVector;
}

int vectorNonZeroExcept(const vector<int>& v, const vector<string>& vString, string ss){
	int vs=v.size();
	srand(unsigned(time(NULL)));
	int index=rand()%vs;

	while(v[index]==0 || vString[index]==ss){
		srand(unsigned(time(NULL)));
		index=rand()%vs;
	}
	return index;
}

vector<int> vectorMatchCount(const vector<string>& v1, const vector<string>& v2){
	int vs1=v1.size();
	int vs2=v2.size();

	vector<int> c(vs1,0);
	for (int i = 0; i < vs2; ++i)
	{
		for (int j = 0; j < vs1; ++j)
		{
			if(v2[i]==v1[j]){
				c[j]++;
				break;
			}
		}
	}
	return c;
}

int vectorIntMax(const vector<int>& v){
	int value=*max_element(v.begin(),v.end());
	int index=-1;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		if(v[i]==value){
			index=i;
			break;
		}
	}
	return index;
}

int vectorSum(const vector<int>& v){
	int sum=0;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		sum+=v[i];
	}
	return sum;
}

void vectorFillNegative(vector<int>& v) {
	int vs=v.size();
	for( int i=0;i<vs;i++){
		if (v[i] < 0)
			v[i] = 0;
	}
}

int vectorNonNegativeSum(const vector<int>& v){
	int sum=0;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		if (v[i] >= 0)
			sum+=v[i];
	}
	return sum;
}

vector<int> vectorExpandNonNegativeSum(const vector<int>& v,int total){
	vector<int> uv = v;

	if(uv.size() == 0) uv.push_back(total);
	else {
		int prevSum = vectorNonNegativeSum(v);
		if (total > prevSum) {
			float scale = (float)(total) / prevSum;
			for (unsigned int i=0; i<uv.size(); i++) {
				if (uv[i] >=0 ) {
					uv[i] = ceil(uv[i] * scale);
				}
			}
		}
	}
	return uv;
}


long getCurrentTime()
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}


int randomValue(int start, int end) {
	int ret = start;
	srand(getCurrentTime());
	if (start != end) {
		if (start < end) {
			ret = start + rand() % (end - start);
		} else {
			ret = end + rand() % (start - end);
		}
	}
	return ret;
}

string getLocalHost() {
	string ret = "";

    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);

            string temp = string(addressBuffer);
            string lo = "127.0.0.1";
            if (temp.compare(0, lo.size(), lo)!=0) {
            	ret = temp;
            	break;
            }

        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

    return ret;
}

void mkdirRecursive(const char *dir) {
        char tmp[256];
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp),"%s",dir);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        mkdir(tmp, S_IRWXU);
                        *p = '/';
                }
        mkdir(tmp, S_IRWXU);
}


bool writeFile(string dir, string fileName, string content) {
	mkdirRecursive(dir.c_str());
	ofstream file;
	string fullPath = dir + fileName;
	file.open(fullPath.c_str(), ios::out | ios::trunc);
	if (file.is_open()) {
		file << content;
		file.close();
		return true;
	} else {
		return false;
	}
}

bool readFile(string path, string &content) {
	std::ifstream file(path.c_str(), std::ifstream::in);
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		content = buffer.str();
		file.close();
		return true;
	} else {
		return false;
	}
}

bool readFile(string path, int offset, int length, string &content) {
	std::ifstream file(path.c_str(), std::ifstream::in);
	if (file.is_open()) {
		char *buffer = new char [length];
		file.seekg(offset);
		file.read (buffer,length);

		std::stringstream ss;
		ss << buffer;
		content = ss.str();

		file.close();
		delete []buffer;
		return true;
	} else {
		return false;
	}
}

bool readFileToLines(string path, vector<string> &content) {
	std::ifstream file(path.c_str(), std::ifstream::in);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			content.push_back(line);
		}

		file.close();
		return true;
	} else {
		return false;
	}

}



bool readFileByLineNumber(string path, int offset, int length, string &content) {
	std::ifstream file(path.c_str(), std::ifstream::in);
	if (file.is_open()) {
		int lines_count =0;
		std::string line;
		std::stringstream ss;
		while (std::getline(file , line)) {
			if (lines_count >= offset && lines_count < offset + length) {
				ss << line << endl;
			}
			++lines_count;
			if (lines_count >= offset+length) break;
		}
		content = ss.str();
		file.close();
		return true;
	} else {
		return false;
	}
}

bool getFileLength(string path,  long &size) {
	std::ifstream file(path.c_str(), std::ifstream::ate | std::ifstream::binary);
	if (file.is_open()) {
		size = file.tellg();
		file.close();
		return true;
	} else {
		return false;
	}
}

bool getFileLineNumber(string path,  long &size) {
	std::size_t lines_count =0;
	std::string line;
	std::ifstream file(path.c_str());

	if (file.is_open()) {
		while (std::getline(file , line))
			++lines_count;
		size = lines_count;
		file.close();
		return true;
	} else {
		return false;
	}
}

string num2string(int val) {
	char buffer [33];
	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer, sizeof(buffer), "%d", val);
	return buffer;
}

string num2string(long val) {
	char buffer [33];
	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer, sizeof(buffer), "%ld", val);
	return buffer;
}

// return value: start position of right side delimitation
string::size_type findDelimitationCouplePosition(string s, string delim_l, string delim_r, string::size_type pos) {
	string::size_type pos1 = pos;
	string::size_type pos2 = pos;
	int c = 0, len_l = delim_l.length(), len_r = delim_r.length();
	do {
		pos1 = s.find(delim_l, pos1);
		pos2 = s.find(delim_r, pos2);
		if (pos2 == string::npos) break;
		if (pos1 < pos2) {
			c ++;
			pos1 += len_l;
		}
		else {
			c --;
			if (c <= 0) break;
			pos2 += len_r;
		}
	} while (true);
	return pos2;
}

// example: split "(123(456))(789)" by "(" and ")"
// result: vector{"123(456)", "789"}
vector<string> splitStringByDelimitationCouple(string str, string delim_l, string delim_r) {
	vector<string> vs;
	string::size_type pos1 = 0;
	string::size_type pos2 = 0;
	int len_l = delim_l.length(), len_r = delim_r.length();
	pos2 = findDelimitationCouplePosition(str, delim_l, delim_r, pos1);
	while (pos2 != string::npos) {
		vs.push_back(str.substr(pos1+len_l, pos2-pos1-len_l));
		pos1 = pos2 + len_r;
		pos2 = findDelimitationCouplePosition(str, delim_l, delim_r, pos1);
	}
	return vs;
}

/**
 * Given a directory name, list all direct file names in this directory
 */
 bool listAllFileNames(string dirname, vector<string> &files) {
	DIR *dp;
	struct dirent *dirp;

	if((dp=opendir(dirname.c_str()))==NULL) {
	    //perror("opendir error");
	    return false;
	}

	while((dirp=readdir(dp))!=NULL) {
	    if((strcmp(dirp->d_name,".")==0)||(strcmp(dirp->d_name,"..")==0))
	                        continue;
	    string _file(dirp->d_name);
	    files.push_back(_file);
	}
	closedir(dp);
	return true;
}

 /**
  * Given a directory name, list all direct file names in this directory that contains ...
  */
 bool listAllFileNamesContain(string dirname, vector<string> &files, string contain) {
 	DIR *dp;
 	struct dirent *dirp;

 	if((dp=opendir(dirname.c_str()))==NULL) {
 	    perror("opendir error");
 	    return false;
 	}

 	while((dirp=readdir(dp))!=NULL) {
 	    if((strcmp(dirp->d_name,".")==0)||(strcmp(dirp->d_name,"..")==0))
 	                        continue;
 	    string _file(dirp->d_name);
 	    if(_file.find(contain) < _file.length())
 	    	 files.push_back(_file);
 	}
	closedir(dp);
 	return true;
 }

 unsigned int hash(string str, unsigned int seed = 0) {
 	unsigned int hash = seed;
 	const char *s = str.c_str();
 	while (*s) {
 		hash = hash * 101  +  *s++;
 	}
 	return hash;
 }

#endif /* UTILS_HPP_ */
