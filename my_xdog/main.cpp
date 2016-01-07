#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <cmath>
#include <iostream>

using namespace std;
using namespace cv;

int MAX_PIX_VAL = 255;

struct XDOG_param {
	// standard DOG
	double sigma; // standard deviation of Gaussian
	double k; // factor of standard deviations
	double kernel_size; // size of Gaussian

	// XDOG extension
	double tau; // relative weighting between Gaussians
	double epsilon; // threshold
	double phi; // steepness
};

XDOG_param param;
Mat src; Mat dog; Mat dog_th;
char window_name[] = "XDOG demo";

// compute Dx(sigma, k, tau)
Mat& compute_dog(const Mat& src, Mat& dst, const XDOG_param& param);
// thresholding
Mat& threshold(const Mat& src, Mat& dst, const XDOG_param& param);

// function main
 int main( int argc, char** argv )
 {
   namedWindow(window_name, WINDOW_AUTOSIZE);
   src = imread("lena.jpg", 0);
   // convert to double [0, 1]
   src.convertTo(src, CV_32F);
   src /= MAX_PIX_VAL;

   dog = Mat::zeros(src.size(), src.type());
   dog_th = Mat::zeros(src.size(), src.type());

   // initialize XDOG params
   param.sigma = 5; // standard deviation of Gaussian
   param.k = 1.6; // factor of standard deviations
   param.kernel_size = 15; // size of Gaussian
   param.tau = 1; // relative weighting between Gaussians
   param.epsilon = 0; // threshold
   param.phi = 100; // steepness

   // display src img
   imshow(window_name, src);
   waitKey(500);

   // compute DOG
   compute_dog(src, dog, param);
   imshow(window_name, dog);
   waitKey(1000);

   // threshold
   threshold(dog, dog_th, param);
   imshow(window_name, dog_th);
   waitKey();

   return 0;
 }

// compute Dx(sigma, k, tau)
Mat& compute_dog(const Mat& src, Mat& dst, const XDOG_param& param)
{
	// compute G(sigma)
	Mat g_sigma = Mat::zeros(src.size(), src.type());
	GaussianBlur(src, g_sigma, Size(param.kernel_size, param.kernel_size), param.sigma, param.sigma);

	// compute G(k*sigma)
	Mat g_ksigma = Mat::zeros(src.size(), src.type());
	GaussianBlur(src, g_ksigma, Size(param.kernel_size, param.kernel_size), param.k*param.sigma, param.k*param.sigma);

	// subtract imgs
	dst = g_sigma - param.tau*g_ksigma;

	return dst;
}

// thresholding
Mat& threshold(const Mat& src, Mat& dst, const XDOG_param& param)
{
	cout << "inside threshold :" << endl;
	int nr = src.rows; // num of rows
	int nl = src.cols*src.channels(); // num of data in each row
	// set value for each pixel
	for(int r=0; r<nr; r++){
		//get pointer to each row
		const float* in_ptr = src.ptr<float>(r);
		float* out_ptr = dst.ptr<float>(r);
		for(int l=0; l<nl; l++){
			if(in_ptr[l] >= param.epsilon){
				// if >= epsilon, set to 1
				out_ptr[l] = 1;
			} 
			else{
				// if < epsilon, set to 1 + tanh(phi * ( Dx(sigma, k, tau) - epsilon ))
				out_ptr[l] = 1 + tanh(param.phi * (in_ptr[l] - param.epsilon));
			}
		}
	}

	return dst;
}