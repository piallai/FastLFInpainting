/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "ocv.h"
#include <limits>

namespace ocv {

	/*! Returns minimum and maximum value of \p _image if Tval is float/double. Otherwise return range of values.*/
	template <class Tval, int Dim>
	ocv::Range<Tval> get_minmax_range(const cv::Mat_< cv::Vec<Tval, Dim> > _image = cv::Mat_< cv::Vec<Tval, Dim> >(), bool _l_image_range_saturate = false);
	/*! Shortest possible range is [0,1].
	If _l_image_range_saturate is true : forces range from 0 to 1. Otherwise, uses min max as range provided they are off limits ([0,1]) and _image is valid.*/
	template <int Dim>
	ocv::Range<float> get_minmax_range(const cv::Mat_< cv::Vec<float, Dim> > _image = cv::Mat_< cv::Vec<float, Dim> >(), bool _l_image_range_saturate = false);
	/*! Shortest possible range is [0,1].
	If _l_image_range_saturate is true : forces range from 0 to 1. Otherwise, uses min max as range provided they are off limits ([0,1]) and _image is valid.*/
	template <int Dim>
	ocv::Range<double> get_minmax_range(const cv::Mat_< cv::Vec<double, Dim> > _image = cv::Mat_< cv::Vec<double, Dim> >(), bool _l_image_range_saturate = false);

	/*! Returns natural max value depending on opencv data type (uchar, char, float, etc..)*/
	template <class Tval>
	struct MaxRange;
	/*! Returns natural min value depending on opencv data type (uchar, char, float, etc..)*/
	template <class Tval>
	struct MinRange;

	template <class Tval>
	ocv::Range<Tval> get_minmax_range();

	/*! Get min and max value*/
	template <class Tval, int Dim>
	ocv::Range<Tval> get_minmax(const cv::Mat_< cv::Vec<Tval, Dim> > _image);
	/*! Get min and max value*/
	void minMaxLoc(const cv::Mat& _image, double* _min, double* _max, const ocv::Tmask _mask = ocv::Tmask());
	/*! Get min and max value*/
	void minMaxLoc(const cv::Mat& _image, double* _min, double* _max, cv::Point* _min_loc, cv::Point* _max_loc, const ocv::Tmask _mask = ocv::Tmask());
	/*! Get min and max value*/
	void minMaxLoc(const cv::Mat& _image, double* _min, double* _max, cv::Point* _min_loc, cv::Point* _max_loc, unsigned int* _channel_min, unsigned int* _channel_max, const ocv::Tmask _mask = ocv::Tmask());
	/*! Get min and max value*/
	void minMaxLoc(const cv::Mat& _image, double* _min, double* _max, cv::Point* _min_loc, cv::Point* _max_loc, unsigned int* _channel_min, unsigned int* _channel_max, bool _l_ignore_opacity_channel, const ocv::Tmask _mask = ocv::Tmask());


	/*! Template overload so min/max type is consistent with image type.*/
	template <class Tvec>
	void minMaxLoc(const cv::Mat_<Tvec>& _image, typename Tvec::value_type* _min, typename Tvec::value_type* _max, const ocv::Tmask _mask = ocv::Tmask());
	template <class Tvec>
	void minMaxLoc(const cv::Mat_<Tvec>& _image, typename Tvec::value_type* _min, typename Tvec::value_type* _max, cv::Point* _min_loc, cv::Point* _max_loc, const ocv::Tmask _mask = ocv::Tmask());
	template <class Tvec>
	void minMaxLoc(const cv::Mat_<Tvec>& _image, typename Tvec::value_type* _min, typename Tvec::value_type* _max, cv::Point* _min_loc, cv::Point* _max_loc, unsigned int* _channel_min, unsigned int* _channel_max, const ocv::Tmask _mask = ocv::Tmask());

	/*! Get min and max value. Ignores opacity channel by default.*/
	template <class Tvec>
	void minMaxLoc(const cv::Mat_<Tvec>& _image, Tvec* _min, Tvec* _max, const ocv::Tmask _mask = ocv::Tmask());
	/*! Get min and max value*/
	template <class Tvec>
	void minMaxLoc(const cv::Mat_<Tvec>& _image, Tvec* _min, Tvec* _max, bool _l_ignore_opacity_channel, const ocv::Tmask _mask = ocv::Tmask());

}

template <class Tval>
struct ocv::MinRange {
	static Tval get() {
		return std::numeric_limits<Tval>::lowest();
	}
};

namespace ocv {
	template <>
	struct MinRange<float> {
		static float get() {
			return 0.f;
		}
	};
	template <>
	struct MinRange<double> {
		static double get() {
			return 0.;
		}
	};
}

template <class Tval>
struct ocv::MaxRange {
	static Tval get() {
		return std::numeric_limits<Tval>::max();
	}
};

namespace ocv {
	template <>
	struct MaxRange<float> {
		static float get() {
			return 1.f;
		}
	};
	template <>
	struct MaxRange<double> {
		static double get() {
			return 1.;
		}
	};
}

template <class Tval>
ocv::Range<Tval> ocv::get_minmax_range() {

	return ocv::Range<Tval>(ocv::MinRange<Tval>::get(), ocv::MaxRange<Tval>::get());
}

template <class Tval, int Dim>
ocv::Range<Tval> ocv::get_minmax_range(const cv::Mat_< cv::Vec<Tval, Dim> > _image, bool _l_image_range_saturate) {

	ocv::Range<Tval> range;
	range.first = std::numeric_limits<Tval>::lowest();
	range.second = std::numeric_limits<Tval>::max();
	return range;
}

template <int Dim>
ocv::Range<float> ocv::get_minmax_range(const cv::Mat_< cv::Vec<float, Dim> > _image, bool _l_image_range_saturate) {

	ocv::Range<float> range;
	if (is_valid(_image)) {
		double min, max;
		cv::minMaxLoc(_image, &min, &max);
		//std::cout << "min = " << min << ", max = " << max << std::endl;
		if (!_l_image_range_saturate && min < 0.) {
			range.first = (float)min;
		} else {
			range.first = (float)0.;
		}
		if (!_l_image_range_saturate && max > 1.) {
			range.second = (float)max;
		} else {
			range.second = (float)1.;
		}
	} else {
		range.first = (float)0.;
		range.second = (float)1.;
	}
	return range;
}

template <int Dim>
ocv::Range<double> ocv::get_minmax_range(const cv::Mat_< cv::Vec<double, Dim> > _image, bool _l_image_range_saturate) {

	ocv::Range<double> range;
	if (is_valid(_image)) {
		double min, max;
		cv::minMaxLoc(_image, &min, &max);
		if (!_l_image_range_saturate && min < 0.) {
			range.first = min;
		} else {
			range.first = 0.;
		}
		if (!_l_image_range_saturate && max > 1.) {
			range.second = max;
		} else {
			range.second = 1.;
		}
	} else {
		range.first = 0.;
		range.second = 1.;
	}
	return range;
}

template <class Tval, int Dim>
ocv::Range<Tval> ocv::get_minmax(const cv::Mat_< cv::Vec<Tval, Dim> > _image) {

	ocv::Range<Tval> range;
	ocv::minMaxLoc(_image, &range.first, &range.second);
	return range;
}

template <class Tvec>
void ocv::minMaxLoc(const cv::Mat_<Tvec>& _image, typename Tvec::value_type* _min, typename Tvec::value_type* _max, const ocv::Tmask _mask) {

	double min, max;
	ocv::minMaxLoc(*static_cast<const cv::Mat*>(&_image), &min, &max, _mask);
	*_min = (typename Tvec::value_type)min;
	*_max = (typename Tvec::value_type)max;
}

template <class Tvec>
void ocv::minMaxLoc(const cv::Mat_<Tvec>& _image, typename Tvec::value_type* _min, typename Tvec::value_type* _max, cv::Point* _min_loc, cv::Point* _max_loc, const ocv::Tmask _mask) {

	double min, max;
	ocv::minMaxLoc(*static_cast<const cv::Mat*>(&_image), &min, &max, _min_loc, _max_loc, _mask);
	*_min = (typename Tvec::value_type)min;
	*_max = (typename Tvec::value_type)max;
}

template <class Tvec>
void ocv::minMaxLoc(const cv::Mat_<Tvec>& _image, typename Tvec::value_type* _min, typename Tvec::value_type* _max, cv::Point* _min_loc, cv::Point* _max_loc, unsigned int* _channel_min, unsigned int* _channel_max, const ocv::Tmask _mask) {

	double min, max;
	ocv::minMaxLoc(*static_cast<const cv::Mat*>(&_image), &min, &max, _min_loc, _max_loc, _channel_min, _channel_max, _mask);
	*_min = (typename Tvec::value_type)min;
	*_max = (typename Tvec::value_type)max;

}

template <class Tvec>
void ocv::minMaxLoc(const cv::Mat_<Tvec>& _image, Tvec* _min, Tvec* _max, const ocv::Tmask _mask) {

	bool l_ignore_opacity_channel = true;
	minMaxLoc(_image, _min, _max, l_ignore_opacity_channel, _mask);

}

template <class Tvec>
void ocv::minMaxLoc(const cv::Mat_<Tvec>& _image, Tvec* _min, Tvec* _max, bool _l_ignore_opacity_channel, const ocv::Tmask _mask) {

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
		(*_min)[c] = (typename Tvec::value_type)min;
		(*_max)[c] = (typename Tvec::value_type)max;
	}

	/*! Set default values for min max for opacity channel.*/
	if (_l_ignore_opacity_channel && (split.size() == 2 || split.size() == 4)) {
		(*_min)[(int)split.size()] = (typename Tvec::value_type)0.;
		(*_max)[(int)split.size()] = (typename Tvec::value_type)0.;
	}
}
