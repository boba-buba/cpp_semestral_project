#pragma once
#ifndef Utilities_h
#include <iostream>
#include <chrono>
#include <sstream>
#include <string>
#include <map>


namespace Time {

	std::map<std::string, std::string> months = {
		{"Jan", "01"}, {"Feb", "02"}, {"Mar", "03"}, {"Apr", "04"}, {"May", "05"}, {"Jun", "06"},
		{"Jul", "07"}, {"Aug", "08"}, {"Sep", "09"}, {"Oct", "10"}, {"Nov", "11"}, {"Dec", "12"}
	};

	std::string ToStandardDateFormat(std::string& d)
	{
		std::string month = d.substr(0, 3);
		std::string day = d.substr(4, 2);
		std::string year = d.substr(8);
		return months.at(month) + "/" + day + "/" + year;
	}
	/*
	using Days = std::chrono::year_month_day;

	Days ParseDate(std::string& date)
	{
		Days days = Days{};
		std::istringstream stream(date);
		stream.exceptions(std::ios_base::badbit | std::ios_base::failbit);
		std::chrono::from_stream(stream, "%m/%d/%Y", days);
		return days;
	}

	int Difference2Dates(Days& d1, Days& d2)
	{
		auto diff = std::chrono::sys_days(d2) - std::chrono::sys_days(d1);
		int d = std::abs(diff.count());
		return d;
	}*/
};

#endif // !Utilities_h
