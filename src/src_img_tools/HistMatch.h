/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include <opencv2/core.hpp>
#include "Macros.h"

class HistMatch {

private :
	static void do1ChnHist(const cv::Mat& _i, const cv::Mat* mask, double* h, double* cdf);
public :
	static void histMatchRGB(cv::Mat& src, const cv::Mat* src_mask, const cv::Mat& dst, const cv::Mat* dst_mask);
};
