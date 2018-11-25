#pragma once
#define _USE_MATH_DEFINES
#include <set>
#include <iostream>
#include "datatype.h"
#include "filter1d.h"
#include "numericaldiff.h"
#include "utility.h"

class WavePro
{
public:
	WavePro();
	~WavePro();
public:
	inline void setInputData(const std::vector<unsigned char> &data)
	{
		raw_wave_.assign(data.begin(),data.end());
	}
	inline void setInputData(const std::vector<double> &data)
	{
		raw_wave_ = data;
	}
	inline const std::vector<double>& getInputData()
	{
		return raw_wave_;
	}
	inline void setPeakThreshold(double peak_threshold)
	{
		peak_threshold_ = peak_threshold;
	}
	inline void setPulseWidth(double width)
	{
		pulse_width_ = width;
	}
	inline void setSampleSpacing(unsigned long sample_spacing)
	{
		sample_spacing_ = sample_spacing;
	}
	inline double getBackgroundNoise()
	{
		return b_noise_;
	}
	void preprocess(FILT_TYPE type,unsigned int window_size=3,double otherParam = 3.0);
	void initEstimateParam(std::vector<Wave_Param> &param_v);
	void removeBadParam(std::vector<Wave_Param> &param_v);
	void optimizeParamWithLM(std::vector<Wave_Param> &param_v);

private:
	double peak_threshold_;                      //���μ����ֵ
	double pulse_width_;                         //����������
	double b_noise_;                             //��������
	unsigned long sample_spacing_;                //����ʱ����
	std::vector<double> raw_wave_;              //ԭʼ��������
	std::vector<double> final_wave_;            //�����������
};


