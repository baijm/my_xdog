//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "imatrix.h"
#include "ETF.h"
#include "fdog.h"
#include "myvec.h"


int main(int argc, char *argv[]){

    //通过opencv载入图片
	char* sample = "D:\\projects\\my_xdog\\xdog_gui\\image\\dog.jpg";
	cv::Mat src_mat = cv::imread(sample);
	cv::cvtColor(src_mat, src_mat, CV_BGR2GRAY);
	cv::Mat res_mat = cv::Mat::zeros(src_mat.size(), src_mat.type());
	cv::namedWindow("gray");
	cv::imshow("gray", src_mat);
	cvWaitKey(0);

    //img初始化
	int w, h;
	w = src_mat.cols; 
	h = src_mat.rows;
    imatrix img(h,w); // int array

    for (int i=0;i<h;i++)
        for (int j=0;j<w;j++){
			img.p[i][j]= src_mat.at<uchar>(i, j);
        }

//     We assume that you have loaded your input image into an imatrix named "img"

    int image_x = img.getRow();
    int image_y = img.getCol();

    //构建flow
    ETF e;
    e.init(image_x, image_y);
    e.set(img); // get flows from input image
//    e.set2(img); // get flows from flow map(二次流)
    e.Smooth(4, 2);

    //基于flow，做DOG
    double tao = 0.99;
    double thres = 0.7;
    GetFDoG(img, e, 1.0, 3.0, tao);
    GrayThresholding(img, thres);

    //显示结果
    for (int i=0;i<h;i++){
        for (int j=0;j<w;j++){
            int color=img.p[i][j];
			res_mat.at<uchar>(i, j) = color;
            //cvSet2D(gray_image,i,j,cvScalar(color,color,color,0));
        }
	}
	cv::namedWindow("result");
	cv::imshow("result", res_mat);
    //cvShowImage("result",gray_image);
    cvWaitKey(0);
    return 0;
}

