/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "Contour.h"
#include "ocv_convert.h"

void Contour::get_segmentation(const cv::Mat& _labels, Tcontour& _contour) {

	cv::Mat_<cv::Vec1f> gradient;
	cv::Mat gradient_x, gradient_y;
	cv::Mat labels_tmp;
	_labels.convertTo(labels_tmp, CV_32FC1);
	cv::Sobel(labels_tmp, gradient_x, labels_tmp.type(), 1, 0, 1, 1, 0);
	cv::Sobel(labels_tmp, gradient_y, labels_tmp.type(), 0, 1, 1, 1, 0);

	gradient_x = cv::abs(gradient_x);
	gradient_y = cv::abs(gradient_y);

	cv::add(gradient_x, gradient_y, gradient);

	for (cv::MatIterator_<cv::Vec1f> it = gradient.begin(); it != gradient.end(); ++it) {

		/*! If gradient is not null, then sets mask_value.*/
		if ((*it)[0] != 0) {
			(*it)[0] = ocv::mask_value;// 1;
		}
	}

	ocv::convertTo(gradient, _contour);
}
