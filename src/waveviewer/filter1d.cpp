#include "filter1d.h"

void Filter1d::createGaussianFilter(unsigned int window_size /* = 3 */, double sigma /* = 4.0 */)
{
	int half_size = (window_size - 1)/2;
	std::vector<double> gaussian_filter(window_size);
	for(int i = -half_size; i <= half_size; ++i)
		gaussian_filter[half_size+i] = exp(-i*i/(2*sigma*sigma));
	
	double sumg = accumulate(gaussian_filter.begin(),gaussian_filter.end(),0.0);

	for(size_t i = 0; i < gaussian_filter.size(); ++i)
		gaussian_filter[i] /= sumg;

	filter_.assign(gaussian_filter.begin(),gaussian_filter.end());
}

void Filter1d::createAverageFilter(unsigned int window_size /* = 3 */)
{
	std::vector<double> aver_filter(window_size);
	for(size_t i = 0; i < window_size; ++i)
		aver_filter[i] = 1.0/window_size;
	filter_.assign(aver_filter.begin(),aver_filter.end());
}

void Filter1d::createSGFilter(unsigned int window_size /* = 5 */, int k /* = 3 */)
{
	std::vector<double> sg_filter(window_size);
	double normalisation(0.0);
	if(3 == k || 2 == k){
		if(5 == window_size){
			normalisation = 35.0;
			sg_filter[0] = -3.0/normalisation;
			sg_filter[1] = 12/normalisation;
			sg_filter[2] = 17/normalisation;
			sg_filter[3] = 12/normalisation;
			sg_filter[4] = -3/normalisation;
		}else if(7 == window_size){
			normalisation = 21.0;
			sg_filter[0] = -2.0/normalisation;
			sg_filter[1] = 3.0/normalisation;
			sg_filter[2] = 6.0/normalisation;
			sg_filter[3] = 7.0/normalisation;
			sg_filter[4] = 6.0/normalisation;
			sg_filter[5] = 3.0/normalisation;
			sg_filter[6] = -2.0/normalisation;
		}
	}
	filter_.assign(sg_filter.begin(),sg_filter.end());
}

void Filter1d::conv(std::vector<double> &filt_data)
{
	filt_data = data_;
	size_t window_size = filter_.size();
	size_t half_size = (window_size-1)/2;

	for(size_t i = half_size; i < data_.size()-half_size; ++i){
		double s = 0.0;
		for(size_t j = 0; j < window_size; ++j)
			s += filter_[j]*data_[i-half_size+j];
		filt_data[i] = s;
	}
}

void Filter1d::midFilt(std::vector<double> &filt_data, unsigned int window_size /* = 3 */)
{
	filt_data = data_;
	std::vector<double> tmp(window_size);
	size_t data_size = data_.size();
	size_t half_size = (window_size-1)/2;
	for(size_t i = half_size; i < data_size - half_size; ++i){
		for(size_t j = 0; j < window_size; ++j)
			tmp[j] = data_[i-half_size+j];
		sort(tmp.begin(),tmp.end());
		filt_data[i] = tmp[half_size];
	}
}

void Filter1d::filt(std::vector<double> &filt_data,FILT_TYPE type,unsigned int window_size /* = 3 */,double other_param /* = 0.0 */)
{
	switch(type)
	{
	case GAUSSIAN:
		{
			double sigma = other_param;
			createGaussianFilter(window_size,sigma);
			conv(filt_data);
			break;
		}
	case MEAN:
		{
			createAverageFilter(window_size);
			conv(filt_data);
			break;
		}

	case SG:
		{
			int k = static_cast<int>(other_param);
			createSGFilter(window_size,k);
			conv(filt_data);
			break;
		}

	case MID:
		midFilt(filt_data,window_size);
	default:
		filt_data = data_;
		break;
	}
}