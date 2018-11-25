#include "utility.h"

void transParam(const std::vector<Wave_Param> &std_v,Eigen::VectorXd &eigen_v)
{
	for(size_t i = 0; i < std_v.size(); ++i){
		eigen_v[3*i] = std_v[i].a;
		eigen_v[3*i+1] = std_v[i].b;
		eigen_v[3*i+2] = std_v[i].sigma;
	}
}

void addParam(const std::vector<Wave_Param> &raw_v,const Eigen::VectorXd &delta_p,std::vector<Wave_Param> &new_v)
{
	for(size_t i = 0; i < raw_v.size(); ++i){
		new_v[i].a = raw_v[i].a + delta_p[i*3];
		new_v[i].b = raw_v[i].b + delta_p[i*3+1];
		new_v[i].sigma = raw_v[i].sigma + delta_p[i*3+2];
	}
}

void updateParam(const Eigen::VectorXd &delta_p,std::vector<Wave_Param> &param_v)
{
	for(size_t i = 0; i < param_v.size(); ++i){
		param_v[i].a += delta_p[i*3];
		param_v[i].b += delta_p[i*3 + 1];
		param_v[i].sigma += delta_p[i*3 + 2];
	}
}

void computeJacob(const int datasize,const std::vector<Wave_Param> &param_v,Eigen::MatrixXd &jacob)
{
	for(int i = 0; i < datasize; ++i){
		for(size_t j = 0; j < param_v.size(); ++j){
			jacob(i,j*3) = exp(-((double)i - param_v[j].b) *((double)i - param_v[j].b)/ (2* param_v[j].sigma * param_v[j].sigma));
			jacob(i,j*3+1) = param_v[j].a * exp(-((double)i - param_v[j].b) * ((double)i - param_v[j].b)/(2* param_v[j].sigma * param_v[j].sigma))*
				(double(i) - param_v[j].b)/(param_v[j].sigma * param_v[j].sigma);
			jacob(i,j*3+2) = param_v[j].a * exp(-((double)i - param_v[j].b) * ((double)i - param_v[j].b)/(2* param_v[j].sigma * param_v[j].sigma))*
				(double(i) - param_v[j].b) * (double(i) - param_v[j].b) / (param_v[j].sigma * param_v[j].sigma * param_v[j].sigma);
		}
	}
}

void computeDisError(const std::vector<double> &data,const std::vector<Wave_Param> &param_v,Eigen::VectorXd &distance)
{
	std::vector<double> y(data.size(),0);
	for(size_t i = 0; i < data.size(); ++i){
		for(size_t j = 0; j < param_v.size(); ++j){
			y[i] += param_v[j].a * exp(-(i - param_v[j].b) * (i - param_v[j].b)/(2* param_v[j].sigma * param_v[j].sigma));
		}
		distance[i] = abs(y[i] - data[i]);
	}
}

bool compareWavePos(Wave_Param a,Wave_Param b)
{
	return a.b < b.b;
}