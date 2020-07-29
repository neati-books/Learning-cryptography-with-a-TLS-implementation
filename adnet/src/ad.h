#pragma once
#include<chrono>
#include<database/mysqldata.h>
#include<thread>
#include<tcpip/website.h>
#include<random>
#include<set>
#define INTERVAL 60

class Ad : public WebSite
{
protected:
	SqlQuery sq;
	void process();

	void click_ad();
	std::string request_ad();
	
private:
	void all_the_database_job(), insert_increment();
	bool check_token(std::string token);
	std::string new_token();

	std::set<std::string> token_, prev_token_;
	std::map<std::string, int> view_increase_, click_increase_, view_induce_, click_induce_;
	std::random_device rd_;
	std::thread th_;
};


