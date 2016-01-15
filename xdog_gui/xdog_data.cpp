#include "xdog_data.h"

// default params
XDOG_param::XDOG_param()
{
	// standard deviation of Gaussian
	sigma = 2;
	// factor of standard deviations
	k = 1.6;
	// size of Gaussian
	kernel_size = 7; 
	// relative weighting between Gaussians
	tau = 1; 
	// threshold
	epsilon = 0; 
	// steepness
	phi = 10; 
}

double XDOG_param::getSigma() const
{
	return sigma;
}

double XDOG_param::getK() const
{
	return k;
}

double XDOG_param::getKernelSz() const
{
	return kernel_size;
}

double XDOG_param::getTau() const
{
	return tau;
}

double XDOG_param::getEpsilon() const
{
	return epsilon;
}

double XDOG_param::getPhi() const
{
	return phi;
}

void XDOG_param::setSigma(double v)
{
	sigma = v;
}

void XDOG_param::setKernelSz(double v)
{
	kernel_size = v;
}

void XDOG_param::setTau(double v)
{
	tau = v;
}

void XDOG_param::setEpsilon(double v)
{
	epsilon = v;
}

void XDOG_param::setPhi(double v)
{
	phi = v;
}

// processing functions
// compute Dx(sigma, k, tau)
cv::Mat& compute_dog(const cv::Mat& src, cv::Mat& dst, const XDOG_param* param)
{
	// compute G(sigma)
	cv::Mat g_sigma = cv::Mat::zeros(src.size(), src.type());
	GaussianBlur(src, g_sigma, cv::Size(param->getKernelSz(), param->getKernelSz()), 
		param->getSigma(), 
		param->getSigma());

	// compute G(k*sigma)
	cv::Mat g_ksigma = cv::Mat::zeros(src.size(), src.type());
	GaussianBlur(src, g_ksigma, cv::Size(param->getKernelSz(), param->getKernelSz()), 
		param->getK()*param->getSigma(), 
		param->getK()*param->getSigma());

	// subtract imgs
	dst = g_sigma - param->getTau()*g_ksigma;

	return dst;
}

// thresholding
cv::Mat& threshold_dog(const cv::Mat& src, cv::Mat& dst, const XDOG_param* param)
{
	int nr = src.rows; // num of rows
	int nl = src.cols*src.channels(); // num of data in each row
	// set value for each pixel
	for(int r=0; r<nr; r++){
		//get pointer to each row
		const float* in_ptr = src.ptr<float>(r);
		float* out_ptr = dst.ptr<float>(r);
		for(int l=0; l<nl; l++){
			if(in_ptr[l] >= param->getEpsilon()){
				// if >= epsilon, set to 1
				out_ptr[l] = 1;
			} 
			else{
				// if < epsilon, set to 1 + tanh(phi * ( Dx(sigma, k, tau) - epsilon ))
				out_ptr[l] = 1 + tanh(param->getPhi() * (in_ptr[l] - param->getEpsilon()));
			}
		}
	}

	return dst;
}