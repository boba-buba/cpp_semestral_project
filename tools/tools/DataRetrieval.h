#pragma once
#ifndef DataRetrieval_h
#define DataRetrieval_h
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "GumboParser.h"
#include "curl/curl.h"


struct MemoryStruct {
	char* memory = nullptr;
	size_t size = 0;
};


using vvs = std::vector<std::vector<std::string>>;
class Data
{
protected:	
	const char* urlHistory_ = "";
	const char* urlAnalysis_ = "";
	void ExtractHTMLString(const char*& url);
	int GetHistoricalData();
	std::vector<double> ClosePrices; // AdjClose from the web
	std::vector<std::string> Dates;
	std::vector<double> ReturnsStandard; //daily returns of the SPY
	std::size_t pos = 0;double actualEPS = 0;double estimateEPS = 0;
	std::string ReleaseDate = "";

public:
	MemoryStruct chunk;
	Data(const char* url1, const char* url2) :urlHistory_(url1), urlAnalysis_(url2) {};
	virtual ~Data() {};
	virtual int GetData()=0;
	virtual void ShowData() = 0;
	virtual void CalculateAbnormalReturns(const std::vector<double>&) = 0;
	virtual const std::vector<std::string>& GetDates() const = 0;
	virtual const std::vector<double>& GetTimeSeries() const = 0;
	virtual const std::vector<double>& GetReturns() const = 0;
	virtual const std::vector<double>& GetAbnormalReturns() const = 0;
	virtual const std::string& getEPSDate() const = 0;
};


class Stock : public Data
{
protected:
	vvs eps_;
	std::vector<double> Returns; //daily returns of the company
	std::vector<double> AbnormalReturns; //average abnormal return
	std::size_t interval = 0;
public:
	Stock(const char* t, const char* t2) :Data(t, t2){}
	virtual ~Stock() {}
	virtual int GetData();
	virtual void ShowData();	
	virtual void CalculateAbnormalReturns(const std::vector<double>& spyReturns);
	virtual const std::vector<std::string>&  GetDates() const{ return Dates; }
	virtual const std::vector<double>& GetTimeSeries() const { return ClosePrices; }
	virtual const std::vector<double>& GetReturns() const { return Returns; }
	virtual const std::vector<double>& GetAbnormalReturns() const { return AbnormalReturns; }
	void CalculateReturns();
	void GetEPSData();
	virtual const std::string& getEPSDate() const { return ReleaseDate; }	
};


class Standard :public Data //default SPY
{
	
public:
	Standard(const char* t, const char* t2 = "") :Data(t, t2) {}
	virtual ~Standard() {}	
	virtual int GetData();
	virtual void ShowData() {};
	virtual void CalculateAbnormalReturns(const std::vector<double>&) {};
	virtual const std::vector<std::string>& GetDates() const { return Dates; }
	virtual const std::vector<double>&  GetTimeSeries() const { return ClosePrices; }
	virtual const std::vector<double>& GetReturns() const { return ReturnsStandard; }
	virtual const std::vector<double>& GetAbnormalReturns() const { return ReturnsStandard; };
	virtual const std::string& getEPSDate() const { return ""; }
	void CalculateReturns();
};



#endif // !DataRetrieval_h
