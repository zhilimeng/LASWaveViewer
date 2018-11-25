#include "numericaldiff.h"

double NumericalDiff::fivePt_firstOrder_diff(size_t i)
{
	size_t data_size = data_.size();
	if(0 == i)
		return (-25*data_[0] + 48*data_[1] - 36*data_[2] + 16*data_[3] - 3*data_[4])/12;
	else if(1 == i)
		return (-3*data_[0] - 10*data_[1] + 18*data_[2] - 6*data_[3] + data_[4]) / 12;
	else if(data_size-2 == i)
		return (-data_[i-3] + 6*data_[i-2] -18*data_[i-1] + 10*data_[i] + 3*data_[i+1]) / 12;
	else if(data_size -1 == i)
		return (-3*data_[i-4] - 16*data_[i-3] + 36*data_[i-2] - 48*data_[i-1] + 25*data_[i]) / 12;
	else
		return (data_[i-2] - 8*data_[i-1] + 8*data_[i+1] - data_[i+2]) / 12;
}
double NumericalDiff::fivePt_secOrder_diff(size_t i)
{
	size_t data_size = data_.size();
	if(0 == i)
		return (35*data_[0] - 104*data_[1] + 114*data_[2] - 56*data_[3] + 11*data_[4]) / 12;
	else if(1 == i)
		return (11*data_[0] - 20*data_[1] + 6*data_[2] + 4*data_[3] - data_[4]) / 12;
	else if(data_size - 2 == i)
		return (-data_[i-3] + 4*data_[i-2] + 6*data_[i-1] - 20*data_[i] + 11*data_[i+1]) / 12;
	else if(data_size - 1 == i)
		return (11*data_[i-4] - 56*data_[i-3] + 114*data_[i-2] - 104*data_[i-1] + 35*data_[i]) / 12;
	else
		return (-data_[i-2] + 16*data_[i-1] - 30*data_[i] + 16*data_[i+1] - data_[i+2]) / 12;

}
void NumericalDiff::fivePt_firstOrder_diff(std::vector<double> &diff)
{
	double diffx;
	for(size_t i = 0; i < data_.size(); ++i){
		diffx = fivePt_firstOrder_diff(i);
		diff.push_back(diffx);
	}
}

void NumericalDiff::fivePt_secOrder_diff(std::vector<double> &diff)
{
	double diffx;
	for(size_t i = 0; i < data_.size(); ++i){
		diffx = fivePt_secOrder_diff(i);
		diff.push_back(diffx);
	}
}