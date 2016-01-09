#ifndef XDOG_DATA_H
#define XDOG_DATA_H

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

// max of grayscale img
#define MAX_PIX_VAL 255

// XDOG params
class XDOG_param {
private:
	// standard DOG
	double sigma; // standard deviation of Gaussian
	double k; // factor of standard deviations
	double kernel_size; // size of Gaussian

	// XDOG extension
	double tau; // relative weighting between Gaussians
	double epsilon; // threshold
	double phi; // steepness
public:
	XDOG_param();
	double getSigma() const;
	double getK() const;
	double getKernelSz() const;
	double getTau() const;
	double getEpsilon() const;
	double getPhi() const;

	void setSigma(double v);
	void setKernelSz(double v);
	void setTau(double v);
	void setEpsilon(double v);
	void setPhi(double v);
};

// processing functions
// compute Dx(sigma, k, tau)
cv::Mat& compute_dog(const cv::Mat& src, cv::Mat& dst, const XDOG_param* param);
// thresholding
cv::Mat& threshold_dog(const cv::Mat& src, cv::Mat& dst, const XDOG_param* param);

#endif