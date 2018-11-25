#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>
enum FILT_TYPE{GAUSSIAN,MEAN,SG,MID,NON};
class Filter1d
{
public:
	Filter1d() {};
	Filter1d(std::vector<double> data) :data_(data) {};
	~Filter1d() {};
public:
	void setInputData(const std::vector<double> &data) { data_ = data; }
	void filt(std::vector<double> &filt_data,FILT_TYPE type,unsigned int window_size = 3,double other_param = 0.0);

private:
	void createGaussianFilter(unsigned int window_size = 3, double sigma = 4.0);
	void createAverageFilter(unsigned int window_size = 3);
	void createSGFilter(unsigned int window_size = 5, int k = 3);                //k为多项式次数
	void conv(std::vector<double> &filt_data);
	void midFilt(std::vector<double> &filt_data, unsigned int window_size = 3);
private:
	std::vector<double> data_;
	std::vector<double> filter_;          
};