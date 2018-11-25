#include "wavepro.h"
WavePro::WavePro():peak_threshold_(5.0),pulse_width_(4.0),sample_spacing_(1000)
{

}
WavePro::~WavePro()
{

}
void WavePro::preprocess(FILT_TYPE type,unsigned int window_size/* =3 */,double otherParam /* = 3.0 */)
{

	Filter1d filter(raw_wave_);
	std::vector<double> smooth_wave;
	filter.filt(smooth_wave,type,window_size,otherParam);

	double noise_background = 0.0;

	size_t data_size = raw_wave_.size();

	for(size_t i = data_size-15; i < data_size; ++i)
		noise_background += smooth_wave[i];
	noise_background = noise_background/15;

	for(size_t i = 0; i < data_size; ++i){
		smooth_wave[i] -= noise_background;
		if(smooth_wave[i] < 0)
			smooth_wave[i] = 0.0;
	}
	b_noise_ = noise_background;
	final_wave_ = smooth_wave;
}

void WavePro::initEstimateParam(std::vector<Wave_Param> &param_v)
{
	std::vector<double> data = final_wave_;
	int data_size = data.size();
	double sigma(0.0),sigma_l(0.0),sigma_r(0.0);
	int inflection_l(0),inflection_r(0);
	std::vector<double>::iterator max_iter = max_element(data.begin(),data.end());
	double maxa = *max_iter;

	Wave_Param cur_param;
	std::vector<double> diffdata;
	std::vector<int> posivec;
	std::vector<int> obvious_superpos;
	std::vector<int> non_superpos;	
	std::vector<Wave_Param> tmp_paramvec;
	std::vector<Wave_Param> substract_paramvec;

	NumericalDiff numdiff;
	while(maxa > peak_threshold_){

		//寻找明显波峰位置
		for(int i = 2; i < data_size - 2; ++i){
			if(data[i] > peak_threshold_ && data[i] > data[i-1] && data[i-1] > data[i-2] &&
			   data[i] > data[i+1] && data[i+1] > data[i+2])
			   posivec.push_back(i);
		}
		//若未找到明显波峰，将最大值位置作为波峰初始位置
		if(posivec.empty()){
			int pos = max_iter - data.begin();
			posivec.push_back(pos);
		}
		//五点二阶数值差分
		numdiff.setData(data);
		numdiff.fivePt_secOrder_diff(diffdata);
		const double super_threshold = 4*pulse_width_;

		//划分明显叠加波和非明显叠加波（弱叠加波和独立波）
		if(posivec.size() == 1){
			non_superpos.push_back(posivec[0]);
		} else if(posivec.size() == 2){
			if(abs(posivec[0] - posivec[1]) < super_threshold){
				obvious_superpos.push_back(posivec[0]);
				obvious_superpos.push_back(posivec[1]);
			} else {
				non_superpos.push_back(posivec[0]);
				non_superpos.push_back(posivec[1]);
			}
		} else{
			for(size_t i = 0; i < posivec.size(); ++i){
				if(i == 0){
					if(abs(posivec[i+1] - posivec[i]) < super_threshold)
						obvious_superpos.push_back(posivec[i]);
					 else 
						non_superpos.push_back(posivec[i]);
				} else if (i == posivec.size() - 1){
					if(abs(posivec[i-1] - posivec[i]) < super_threshold)
						obvious_superpos.push_back(posivec[i]);
					else
						non_superpos.push_back(posivec[i]);
				} else{
					if(abs(posivec[i] - posivec[i-1]) < super_threshold || abs(posivec[i] - posivec[i+1]) < super_threshold)
						obvious_superpos.push_back(posivec[i]);
					 else
						non_superpos.push_back(posivec[i]);
				}
			}
		}
		//求明显叠加波的波形参数
		for(size_t i = 0; i < obvious_superpos.size(); ++i){
			int pos = obvious_superpos[i];
			for(int j = pos - 1; j > 0; --j){
				if(diffdata[j] >= 0){
					inflection_l = j;
					break;
				}
			}

			for(int j = pos + 1; j < data_size; ++j){
				if(diffdata[j] >= 0){
					inflection_r = j;
					break;
				}
			}

			if(inflection_l == 0){
				cur_param.a = data[pos];
				cur_param.b = pos;
				cur_param.sigma = inflection_r - pos;
			}else if(inflection_r == 0){
				cur_param.a = data[pos];
				cur_param.b = pos;
				cur_param.sigma = pos - inflection_l;
			}else{
				cur_param.b = (inflection_l + inflection_r)/2.0;
				cur_param.sigma = (inflection_r - inflection_l)/2.0;
				cur_param.a = data[(unsigned int)cur_param.b];
			}
			tmp_paramvec.push_back(cur_param);
		}

		if(tmp_paramvec.size() > 0){
			sort(tmp_paramvec.begin(),tmp_paramvec.end(),compareWavePos);
			cur_param = tmp_paramvec[0];
			if(cur_param.b < 2*pulse_width_)
				cur_param.sigma = pulse_width_;
			param_v.push_back(cur_param);
			substract_paramvec.push_back(cur_param);
		}


		//求非明显叠加波的波形参数

		const double coeff_inver = 1/sqrt(2*log(2.0));
		for(size_t i = 0; i < non_superpos.size(); ++i){

			double local_max_half = data[non_superpos[i]]/2;
			for(int j = non_superpos[i] - 1; j > 0; --j){
				if(data[j+1] >= local_max_half && data[j] <= local_max_half){
					sigma_l = abs(j - non_superpos[i])*coeff_inver;
					break;
				}
			}
			
			for(int j = non_superpos[i]+1; j < data_size; ++j){
				if(data[j-1] >= local_max_half && data[j] <= local_max_half){
					sigma_r = abs(j - non_superpos[i])*coeff_inver;
					break;
				}
			}

			if(sigma_l <= sigma_r)
				sigma = sigma_l;
			else
				sigma = sigma_r;
			if(non_superpos[i] < 2*pulse_width_)
				sigma = pulse_width_;
			cur_param.b = non_superpos[i];
			cur_param.a = data[(unsigned int)cur_param.b];		
			cur_param.sigma = sigma;

			param_v.push_back(cur_param);
			substract_paramvec.push_back(cur_param);
		}

		//减去当前波形分量
		for(size_t k = 0; k < substract_paramvec.size(); ++k){
			for(int i = 0; i < data_size; ++i){
				data[i] -= substract_paramvec[k].a*exp(-(double)(i - substract_paramvec[k].b) * (double)(i - substract_paramvec[k].b)
					        /(2*substract_paramvec[k].sigma * substract_paramvec[k].sigma));
				if(data[i] < 0)
					data[i] = 0;
			}
		}

		//求取新的最大值
		max_iter = max_element(data.begin(),data.end());
		maxa = *max_iter;

		diffdata.clear();
		tmp_paramvec.clear();
		posivec.clear();
		obvious_superpos.clear();
		non_superpos.clear();
		substract_paramvec.clear();
	}
}

void WavePro::removeBadParam(std::vector<Wave_Param> &param_v)
{

	int  param_size = static_cast<int>(param_v.size());
	std::vector<bool> flags(param_size, false);

	double max_a = 0.0;
	for (const auto& p : param_v)
		max_a = std::max(max_a, p.a);
	for (int i = 0; i < param_size; ++i)
	{
		for (int j = i + 1; j < param_size; ++j)
		{
			if (fabs(param_v[i].b - param_v[j].b) < 2* pulse_width_)
			{
				if (param_v[i].a < param_v[j].a / 2)
					param_v[i].a = 0;
				if (param_v[j].a < param_v[i].a / 2)
					param_v[j].a = 0;
			}
		}
	}

	for (auto &p :param_v)
	{
		if (p.a < 0.5*max_a && (p.sigma < 0.4 * pulse_width_ || p.sigma > 2 * pulse_width_))
			p.a = 0;
		if (p.a < 0.25*max_a)
			p.a = 0;
	}
	param_v.erase(std::remove_if(param_v.begin(), param_v.end(), [](const Wave_Param& p) {return p.a == 0; }), param_v.end());
}

void WavePro::optimizeParamWithLM(std::vector<Wave_Param> &param_v)
{
	if (param_v.empty())
		return;
	std::cout << "initial parameter: " << std::endl;
	for(size_t i = 0; i < param_v.size(); ++i)
	{
		std::cout << param_v[i].a << " " << param_v[i].b << " " << param_v[i].sigma << std::endl;
	}

	const std::vector<double> &data = final_wave_;
	int n = data.size();
	int m = param_v.size();
	int c = m*3;

	Eigen::MatrixXd jcob(n, c);                          //雅克比矩阵

	Eigen::VectorXd deltap;
	Eigen::VectorXd dis(n);                              //拟合差向量 x-f(p)
	Eigen::VectorXd p(c);

	int k = 0;
	int v = 2;
	bool stop = false;
	double e1(1E-15),e2(1E-15),e3(1E-15),t(1E-3);
	computeDisError(data,param_v,dis);
	computeJacob(n,param_v,jcob);


	Eigen::VectorXd g = jcob.transpose()*dis;
	Eigen::MatrixXd A = jcob.transpose()*jcob;

	stop = g.lpNorm<Eigen::Infinity>() <= e1;
	double u = t * A.diagonal().maxCoeff();
	Eigen::MatrixXd I = Eigen::MatrixXd::Identity(c,c);

	while(!stop && k < 100){
		++k;
		double row = 0;

		deltap = (A + u * I).jacobiSvd(Eigen::ComputeThinU |Eigen::ComputeThinV).solve(g);
		transParam(param_v,p);
		if(deltap.norm() <= e2 * p.norm())
			stop = true;
		else{
			std::vector<Wave_Param> pnew(param_v.size());
			addParam(param_v,deltap,pnew);

			Eigen::VectorXd disnew(n);
			computeDisError(data,pnew,disnew);
			double tmp = deltap.transpose() * (u * deltap + g);

			row = (dis.squaredNorm() - disnew.squaredNorm()) / (deltap.transpose() * (u * deltap + g));
			if(row > 0){
				updateParam(deltap,param_v);
				computeDisError(data,param_v,dis);
				computeJacob(n,param_v,jcob);

				A = jcob.transpose()*jcob;
				g = jcob.transpose()*dis;
				if(g.lpNorm<Eigen::Infinity>() <= e1 || dis.squaredNorm()  <= e3)
					stop = true;
				u = u * std::max(double(1)/3 , 1-(2*row -1)*(2*row -1)*(2*row -1));
				v = 2;
			}else{
				u *= v;
				v *= 2;
			}
		}
	}

	std::cout << "final parameter: " << std::endl;
	for(size_t i = 0; i < param_v.size(); ++i)
	{
		std::cout << param_v[i].a << " " << param_v[i].b << " " << param_v[i].sigma << std::endl;
	}

}