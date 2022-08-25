#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include "Stock.hpp"
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::setw;
using std::stringstream;

size_t UtilityHelper::receive_data(char* ptr, size_t size, size_t nmemb, void *userdata) {
	result = ptr;
	return size * nmemb;
}

CURLcode UtilityHelper::curlProtect(const string& url, int timeout) {
	CURL* curl = curl_easy_init();
	CURLcode code;
	struct curl_slist* head = NULL;
	head = curl_slist_append(head, "Referer:https://finance.sina.com.cn");

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, UtilityHelper::receive_data);
	//curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&strResponse);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, head);
	code = curl_easy_perform(curl);
	curl_slist_free_all(head);
	curl_easy_cleanup(curl);
	return code;
}

void UtilityHelper::split(const string& s, vector<string>& v, const string& flag) {
	string::size_type pos1, pos2;
	pos2 = s.find(flag);
	pos1 = 0;
	while (string::npos != pos2) {
		v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + flag.size();
		pos2 = s.find(flag, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

void Stock::show() {
	string urlHead = "https://hq.sinajs.cn/list=";
	string url = urlHead + no;
	string res;
	UtilityHelper::curlProtect(url, 3);
	vector<string> v;
	//先分割字符串
	string sep = "\"";
	string::size_type loc = UtilityHelper::result.find(sep);
	string str = UtilityHelper::result.substr(loc + 1, UtilityHelper::result.length());

	//转换格式
	IconvConvert icc("gbk", "utf-8");
	char outString[512] = {0};
	char* str_input = const_cast<char*>(str.c_str());
	int ret = icc.ConvertCode(str_input, strlen(str_input), outString, 512);
	string out = (string)outString;

	UtilityHelper::split(out, v, ",");

	stringstream s1(v[1]);
	double init_price;
	s1 >> init_price;

	stringstream s2(v[2]);
	double yesterday_price;
	s2 >> yesterday_price;

	stringstream s3(v[3]);
	double current_price;
	s3 >> current_price;

	stringstream s4(v[4]);
	double top_price;
	s4 >> top_price;

	stringstream s5(v[5]);
	double end_price;
	s5 >> end_price;

	stringstream s8(v[8]);
	long deal_num;
	s8 >> deal_num;

	stringstream s9(v[9]);
	double deal_amount;
	s9 >> deal_amount;

	double surplus = current_price - yesterday_price;
	double rate = surplus * 100 / yesterday_price;
	deal_num = deal_num / 100;
	deal_amount = deal_amount / 10000;

	int len = v[0].length();
	cout.setf(std::ios::left);
	if (len == 12) {
		printf("%-17s|%-13.2f|%-13.2f|%-13.2f|%-13.2f|%-16.2f|%-16.2f|%-16d|%-14.2f|\n", v[0].c_str(), current_price,
					   	rate, yesterday_price, init_price, top_price, end_price, deal_num, deal_amount);
	} else {
		printf("%-16s|%-13.2f|%-13.2f|%-13.2f|%-13.2f|%-16.2f|%-16.2f|%-16d|%-14.2f|\n", v[0].c_str(), current_price,
						rate, yesterday_price, init_price, top_price, end_price, deal_num, deal_amount);
	}
}

string UtilityHelper::result = "";


int main() {
	//show title
	cout.setf(std::ios::left);
	printf("%-15s|%-15s|%-15s|%-15s|%-15s|%-20s|%-20s|%-20s|%-20s|\n", "名称", "当前", 
					"涨幅(%)", "昨收", "今开", "当日最高", "当日最低", "成交数(手)", "成交金额(万元)");

	//read config file
	std::ifstream file("config.ini");
	if (!file.is_open()) {
		std::cerr << "Open config file failed!";
		return -1;
	}
	string code;
	while (getline(file, code)) {
		Stock *stk = new Stock(code);
		stk->show();
	}
	file.close();
	
	/*string url = "http://hq.sinajs.cn/list=sh688189";
	string res;
	UtilityHelper::curlProtect(url, 3);
	vector<string> v;

	//先分割字符串
	string sep = "\"";
	string::size_type loc = UtilityHelper::result.find(sep);
	string str = UtilityHelper::result.substr(loc + 1, UtilityHelper::result.length());

	//转换格式
	IconvConvert icc("gbk", "utf-8");
	char outString[512] = {0};
	char* str_input = const_cast<char*>(str.c_str());
	int ret = icc.ConvertCode(str_input, strlen(str_input), outString, 512);
	string out = (string)outString;
	//cout << out << endl;

	UtilityHelper::split(out, v, ",");
	//for (vector<string>::size_type i = 0; i != v.size(); ++i)
	//	cout << v[i] << endl;
	Stock* st = new Stock("688189");
	st->show(v);*/
	return 0;
}
