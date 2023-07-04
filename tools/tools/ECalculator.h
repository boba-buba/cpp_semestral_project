#pragma once
#ifndef ECalculator_h
#include <vector>
#include <iostream>
#include <cmath>

using vd = std::vector<double>;
class Calculator {
public:
	vd prices_;
	virtual ~Calculator() {};
	virtual double mean(const vd&) = 0;
	virtual double stdDev(const vd&) = 0;
};

class MACalculator : public Calculator
{
public:
	MACalculator(std::size_t numPeriods, const vd& p) :numPeriods_(numPeriods), prices_(p) {};
	MACalculator(const MACalculator& ma) :numPeriods_(ma.numPeriods_), prices_(ma.prices_) {};
	MACalculator& operator=(const MACalculator&);
	virtual double mean(const vd&) {return 0;}
	virtual double stdDev(const vd&) {return 0;}
	vd calculateMA(); 
	vd calculateEMA(); 
private:
	std::size_t numPeriods_ = 5;
	vd prices_;
};

class VolatilityCalculator : public Calculator
{
public:
	VolatilityCalculator(const vd& p):prices_(p) {};
	VolatilityCalculator(const VolatilityCalculator& v) :prices_(v.prices_) {};
	VolatilityCalculator& operator=(const VolatilityCalculator& v);

	virtual double mean(const vd& p)
	{
		double sum = 0;
		for (std::size_t i = 0; i < p.size(); ++i)
		{
			sum += p[i];
		}
		return sum / p.size();
	}
	virtual double stdDev(const vd& p)
	{
		double m = mean(p);
		double sum = 0;
		for (std::size_t i = 0; i < p.size(); ++i)
		{
			double val = p[i] - m;
			sum += val * val;
		}
		return sqrt(sum / (p.size() - 1));
	}

	double rangeVolatility();
	double avgDailyRange();
private:
	vd prices_;

};
class CorrelationCalculator : public Calculator
{
public:
	CorrelationCalculator(const vd& a, const vd& b):pricesA_(a), pricesB_(b) {};
	CorrelationCalculator(const CorrelationCalculator& c) :pricesA_(c.pricesA_), pricesB_(c.pricesB_) {};
	CorrelationCalculator& operator= (const CorrelationCalculator& c);
	virtual double mean(const vd& p) 
	{ 
		double sum = 0;
		for (std::size_t i = 0; i < p.size(); ++i)
		{
			sum += p[i];
		}
		return sum / p.size();	
	}

	virtual double stdDev(const vd& prices)
	{
		double m = mean(prices);
		double sum = 0;
		for (std::size_t i = 0; i < prices.size(); ++i)
		{
			double val = prices[i] - m;
			sum += val * val;
		}
		return sqrt(sum / (prices.size() - 1));
	}

	double correlation();
private:
	vd pricesA_;
	vd pricesB_;
};


#endif // !ECalculator_h
