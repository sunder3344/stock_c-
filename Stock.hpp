#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<iconv.h>
#include <curl/curl.h>

#define OUTLEN 255
using std::string;
using std::vector;

class Stock {
	string no;
public:
	Stock(string stock_no) : no(stock_no) {};
	void show();
	~Stock();
};

class UtilityHelper {
public:
	static string result;
	UtilityHelper();
	~UtilityHelper();
	static void split(const string& s, vector<string>& v, const string& flag);
	static CURLcode curlProtect(const string&, int);
	static size_t receive_data(char* ptr, size_t size, size_t nmemb, void* userdata);
};

class IconvConvert {
public:
	//c++11 feature
	//IconvConvert() = delete;
	//IconvConvert(const IconvConvert&) = delete;
	//IconvConvert& operator=(const IconvConvert&) = delete;

	IconvConvert(const char* from_charset, const char* to_charset) {
		cd = iconv_open(to_charset, from_charset);
	}

	~IconvConvert() {
		iconv_close(cd);
	}

	size_t ConvertCode(char* inbuf, size_t inlen, char* outbuf, size_t outlen) {
		char** pin = &inbuf;
		char** pout = &outbuf;
		memset(outbuf, 0, outlen);
		return iconv(cd, pin, (size_t*)&inlen, pout, (size_t*)&outlen);
	}
private:
	iconv_t cd;
	IconvConvert();
	IconvConvert(const IconvConvert&);
	IconvConvert& operator=(const IconvConvert&);
};
