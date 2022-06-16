/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "ocv_minmax.h"

void ocv::minMaxLoc(const cv::Mat& _image, double* _min, double* _max, const ocv::Tmask _mask) {

	cv::Point min_loc, max_loc;
	minMaxLoc(_image, _min, _max, &min_loc, &max_loc, _mask);
}

void ocv::minMaxLoc(const cv::Mat& _image, double* _min, double* _max, cv::Point* _min_loc, cv::Point* _max_loc, const ocv::Tmask _mask) {

	unsigned int min_channel, max_channel;
	minMaxLoc(_image, _min, _max, _min_loc, _max_loc, &min_channel, &max_channel, _mask);
}

void ocv::minMaxLoc(const cv::Mat& _image, double* _min, double* _max, cv::Point* _min_loc, cv::Point* _max_loc, unsigned int* _channel_min, unsigned int* _channel_max, const ocv::Tmask _mask) {

	bool l_ignore_opacity_channel = true;
	minMaxLoc(_image, _min, _max, _min_loc, _max_loc, _channel_min, _channel_max, l_ignore_opacity_channel, _mask);

}

void ocv::minMaxLoc(const cv::Mat& _image, double* _min, double* _max, cv::Point* _min_loc, cv::Point* _max_loc, unsigned int* _channel_min, unsigned int* _channel_max, bool _l_ignore_opacity_channel, const ocv::Tmask _mask) {

	*_min = std::numeric_limits<double>::max();
	*_max = std::numeric_limits<double>::lowest();

	std::vector<cv::Mat> split;
	cv::split(_image, split);
	if (_l_ignore_opacity_channel && (split.size() == 2 || split.size() == 4)) {
		/*! Remove opacity channel.*/
		split.pop_back();
	}
	double min, max;
	cv::Point min_loc, max_loc;
	for (unsigned int c = 0; c < split.size(); c++) {
		if (_mask.size() == _image.size()) {
			cv::minMaxLoc(split[c], &min, &max, &min_loc, &max_loc, _mask);
		} else {
			cv::minMaxLoc(split[c], &min, &max, &min_loc, &max_loc);
		}

		if (min <= *_min) {
			*_min = min;
			*_min_loc = min_loc;
			*_channel_min = c;
		}
		if (max >= *_max) {
			*_max = max;
			*_max_loc = max_loc;
			*_channel_max = c;
		}
	}

}
