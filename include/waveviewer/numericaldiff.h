#pragma once
#include <vector>
class NumericalDiff
{
public:
	NumericalDiff(){}
	~NumericalDiff(){}
public:
	void setData(const std::vector<double> &data) { data_ = data; }
	void fivePt_firstOrder_diff(std::vector<double> &diff);
	void fivePt_secOrder_diff(std::vector<double> &diff);

private:
	double fivePt_firstOrder_diff(size_t i);           //return the first-order derivative of data_[i]
	double fivePt_secOrder_diff(size_t i);             //return the seconde-order derivative of data_[i]
private:
	 std::vector<double> data_;
};