#ifndef DATATYPE_H
#define DATATYPE_H
#include <Eigen/Dense>

struct EIGEN_ALIGN16 Wave_Param
{
	union 
	{
		struct  
		{
			double a, b, sigma;
		};
		double data[4];
	};

	Wave_Param() :a(0.0), b(0.0), sigma(0.0)
	{
		data[3] = 0.0;
	}
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


#endif