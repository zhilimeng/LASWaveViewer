#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <Eigen\Dense>

#include <datatype.h>

void transParam(const std::vector<Wave_Param> &std_v,Eigen::VectorXd &eigen_v);
void addParam(const std::vector<Wave_Param> &raw_v,const Eigen::VectorXd &delta_p,std::vector<Wave_Param> &new_v);
void updateParam(const Eigen::VectorXd &delta_p,std::vector<Wave_Param> &param_v);
void computeJacob(const int datasize,const std::vector<Wave_Param> &param_v,Eigen::MatrixXd &jacob);
void computeDisError(const std::vector<double> &data,const std::vector<Wave_Param> &param_v,Eigen::VectorXd &distance);

bool compareWavePos(Wave_Param a,Wave_Param b);