/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "HistMatch.h"
#include "ocv.h"

using namespace cv;
using namespace std;

/* Match Red, Green and Blue histograms of 'dst' to that of 'src', according to both masks.
Based on: http://www.morethantechnical.com/2010/01/28/recoloring-via-histogram-matching-with-opencv-w-code/
Modified by Shervin Emami so it can also pass NULL for the masks if you want to process the whole image.*/
void HistMatch::histMatchRGB(cv::Mat& dst, const cv::Mat* dst_mask, const cv::Mat& src, const cv::Mat* src_mask) {

	vector<Mat> chns;
	cv::split(dst, chns);
	vector<Mat> chns1;
	cv::split(src, chns1);
	cv::Mat dst_hist = cv::Mat::zeros(1, 256, CV_64FC1);
	cv::Mat src_hist = cv::Mat::zeros(1, 256, CV_64FC1);
	cv::Mat dst_cdf = cv::Mat::zeros(1, 256, CV_64FC1);
	cv::Mat src_cdf = cv::Mat::zeros(1, 256, CV_64FC1);
	cv::Mat Mv(1, 256, CV_8UC1);
	uchar* M = Mv.ptr();

	for (int i = 0; i<3; i++) {
		dst_hist.setTo(Scalar(0));
		src_hist.setTo(Scalar(0));
		dst_cdf.setTo(Scalar(0));
		dst_cdf.setTo(Scalar(0));

		double* _dst_cdf = (double*)dst_cdf.ptr();
		double* _src_cdf = (double*)src_cdf.ptr();
		double* _dst_hist = (double*)dst_hist.ptr();
		double* _src_hist = (double*)src_hist.ptr();

		do1ChnHist(chns[i], dst_mask, _dst_hist, _dst_cdf);
		do1ChnHist(chns1[i], src_mask, _src_hist, _src_cdf);

		uchar last = 0;
		double const HISTMATCH_EPSILON = 0.000001;

		for (int j = 0; j<dst_cdf.cols; j++) {
			double F1j = _dst_cdf[j];

			for (uchar k = last; k<src_cdf.cols; k++) {
				double F2k = _src_cdf[k];
				// Note: Two tests were combined into one for efficiency, by Shervin Emami, Apr 24th 2011.
				//if (abs(F2k - F1j) F1j) {
				if (F2k > F1j - HISTMATCH_EPSILON) {
					M[j] = k;
					last = k;
					break;
				}
			}
		}

		cv::Mat lut(1, 256, CV_8UC1, M);
		cv::LUT(chns[i], lut, chns[i]);
		
	}

	cv::Mat res;
	cv::merge(chns, res);

	res.copyTo(dst);
}

// Compute histogram and CDF for an image with mask
void HistMatch::do1ChnHist(const cv::Mat& _i, const cv::Mat* mask, double* h, double* cdf) {
	cv::Mat _t = _i.reshape(1, 1);

	// Get the histogram with or without the mask
	if (mask) {
		cv::Mat _tm;
		mask->copyTo(_tm);
		_tm = _tm.reshape(1, 1);
		for (int p = 0; p<_t.cols; p++) {
			uchar m = _tm.at<uchar>(0, p);
			if (m > 0) {	// Mask value
				uchar c;
				c = _t.at<uchar>(0, p);	// Image value
				h[c] += 1.0;	// Histogram
			}
		}
	} else {
		for (int p = 0; p<_t.cols; p++) {
			uchar c;
			c = _t.at<uchar>(0, p);	// Image value
			h[c] += 1.0;	// Histogram
		}
	}

	//normalize hist to a max value of 1.0
	cv::Mat _tmp(1, 256, CV_64FC1, h);
	double minVal, maxVal;
	cv::minMaxLoc(_tmp, &minVal, &maxVal);
	_tmp = _tmp / maxVal;

	// Calculate the Cumulative Distribution Function
	cdf[0] = h[0];
	for (int j = 1; j<256; j++) {
		cdf[j] = cdf[j - 1] + h[j];
	}

	//normalize CDF to a max value of 1.0
	_tmp.data = (uchar*)cdf;	// Array of doubles, but gets a byte pointer.
	cv::minMaxLoc(_tmp, &minVal, &maxVal);
	_tmp = _tmp / maxVal;
}
