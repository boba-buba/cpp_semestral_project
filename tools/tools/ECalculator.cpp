#include "ECalculator.h"

vd MACalculator::calculateMA()
{
	vd ma;
	double sum = 0;
	for (std::size_t i = 0; i < prices_.size(); ++i)
	{
		sum += prices_[i];
		if (i >= numPeriods_)
		{
			ma.push_back(sum / numPeriods_);
			sum -= prices_[i - numPeriods_];
		}
	}
	return ma;
}

vd MACalculator::calculateEMA()
{
	vd ema;
	double sum = 0;
	double multiplier = 2.0 / (numPeriods_ + 1);

	for (std::size_t i = 0; i < prices_.size(); ++i)
	{
		sum += prices_[i];
		if (i == numPeriods_)
		{
			ema.push_back(sum / numPeriods_);
			sum -= prices_[i - numPeriods_];
		}
		else if (i > numPeriods_)
		{
			double val = (1 - multiplier) * ema.back() + multiplier * prices_[i];
			ema.push_back(val);
		}
	}
	return ema;
}

MACalculator &MACalculator::operator=(const MACalculator& ma)
{
	if (this != &ma)
	{
		numPeriods_ = ma.numPeriods_;
		prices_ = ma.prices_;
	}
	return *this;
}


double VolatilityCalculator::rangeVolatility()
{
	if (prices_.size() < 2)
	{
		return 0;
	}
	double min = prices_[0];
	double max = min;
	for (std::size_t i = 0; i < prices_.size(); ++i)
	{
		min = prices_[i] < min ? prices_[i] : min;
		max = prices_[i] > max ? prices_[i] : max;
	}
	return max - min;
}

double VolatilityCalculator::avgDailyRange()
{
	std::size_t n = prices_.size();
	if (n < 2) return 0;

	double previous = prices_[0];
	double sum = 0;
	for (std::size_t i = 0; i < n; ++i)
	{
		double range = abs(prices_[i] - previous);
		sum += range;
	}
	return sum / n - 1;
}

VolatilityCalculator& VolatilityCalculator::operator=(const VolatilityCalculator& v)
{
	if (&v != this)
	{
		prices_ = v.prices_;
	}
	return *this;
}


double CorrelationCalculator::correlation()
{
	double sum = 0;
	double meanA = mean(this->pricesA_);
	double meanB = mean(this->pricesB_);
	std::size_t minimum = std::min(this->pricesB_.size(), this->pricesA_.size());

	for (std::size_t i = 0; i < minimum; ++i)
	{
		auto val = (this->pricesA_[i] - meanA) * (this->pricesB_[i] - meanB);
		sum += val;
	}
	vd intervalA{ this->pricesA_.begin(), this->pricesA_.begin() + minimum - 1 };
	vd intervalB{ this->pricesB_.begin(), this->pricesB_.begin() + minimum - 1 };
	double stdDevA = stdDev(intervalA);//interval
	double stdDevB = stdDev(intervalB);//interval 0 ... minimum
	sum /= (stdDevA * stdDevB);
	return sum / (minimum - 1);
}

CorrelationCalculator& CorrelationCalculator::operator= (const CorrelationCalculator& c)
{
	if (this != &c)
	{
		pricesA_ = c.pricesA_;
		pricesB_ = c.pricesB_;
	}
	return *this;
}
