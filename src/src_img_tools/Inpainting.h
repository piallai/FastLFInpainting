/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "ocv.h"


class Inpainting {
public:

	/*! Pixels inpainted are pixels valued non 0 in _inpaint_mask.*/
	template <class Timg>
	static void inpaintTelea(Timg& _image, const ocv::Tmask& _inpaint_mask);
};

#include <opencv2/videostab/inpainting.hpp>
template <class Timg>
void Inpainting::inpaintTelea(Timg& _image, const ocv::Tmask& _inpaint_mask) {

	cv::Mat_< cv::Vec<uchar, Timg::value_type::channels> > convert_image;

	std::pair<double, double> scaling = ocv::convertTo(_image, convert_image, true);
	cv::inpaint(convert_image, _inpaint_mask, convert_image, 1, cv::INPAINT_TELEA);
	ocv::convertTo(convert_image, _image, false);
	ocv::rescale_inv(_image, scaling);

}


