/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "ocv_minmax.h"

namespace ocv {

	template <class Timg>
	void calcHist(const Timg& _image, cv::Mat& _histogram, const unsigned int _dim, const unsigned int _Nvalues, const ocv::Range<typename Timg::value_type::value_type> _range = { (typename Timg::value_type::value_type)0, (typename Timg::value_type::value_type)0 });
	template <class Timg>
	void calcHist(const Timg& _image, cv::Mat& _histogram, const unsigned int _Nvalues = 100, const ocv::Range<typename Timg::value_type::value_type> _range = { (typename Timg::value_type::value_type)0, (typename Timg::value_type::value_type)0 });

	/*! To get min max value in a median fashion parameters by ratios.*/
	template <class Timg>
	ocv::Range<typename Timg::value_type::value_type> get_minmax_median(const Timg& _image, const double _min_ratio = 0., const double _max_ratio = 1., const unsigned int _Nvalues = 100);
	template <class Timg>
	void get_minmax_median(const Timg& _image, ocv::Range<typename Timg::value_type::value_type>& _range, const double _min_ratio = 0., const double _max_ratio = 1., const unsigned int _Nvalues = 100);
	template <class Timg>
	void get_minmax_median(const Timg& _image, typename Timg::value_type::value_type& _min_value, typename Timg::value_type::value_type& _max_value, const double _min_ratio = 0., const double _max_ratio = 1., const unsigned int _Nvalues = 100);
}


template <class Timg>
void ocv::calcHist(const Timg& _image, cv::Mat& _histogram, const unsigned int _dim, const unsigned int _Nvalues, const ocv::Range<typename Timg::value_type::value_type> _range) {

	typedef typename Timg::value_type::value_type Tvalue;

	int Nvalues = _Nvalues;
	std::vector<int> histSize = { Nvalues };
	ocv::Range<Tvalue> range;
	if (_range == ocv::Range<Tvalue>(Tvalue(0), Tvalue(0))) {
		/*! If no range is provided.*/
		/*! Default range is a mix of natural range of type of data (ex [0, 1] for float) and min max value.*/
		range = get_minmax_range(_image);
	} else {
		range = _range;
	}

	/*! Lazy trick to make cv::calcHist not to crash.*/
	if (range.first == range.second) {
		range.second = range.first * (Tvalue)1.01;
	}

	std::vector<float> ranges = { (float)range.first, (float)range.second };
	int dim = _dim;
	std::vector<int> channels = { dim };
	std::vector<cv::Mat> mats = { _image };
	cv::calcHist(mats, channels, cv::Mat(), _histogram, histSize, ranges, false);
	_histogram /= int(_image.total());

}

template <class Timg>
void ocv::calcHist(const Timg& _image, cv::Mat& _histogram, const unsigned int _Nvalues, const ocv::Range<typename Timg::value_type::value_type> _range) {

	std::vector<cv::Mat> histograms(Timg::value_type::channels);
	for (unsigned int d = 0; d<Timg::value_type::channels; d++) {

		calcHist(_image, histograms[d], d, _Nvalues, _range);
	}
	cv::merge(histograms, _histogram);

}

template <class Timg>
ocv::Range<typename Timg::value_type::value_type> ocv::get_minmax_median(const Timg& _image, const double _min_ratio, const double _max_ratio, const unsigned int _Nvalues) {

	ocv::Range<typename Timg::value_type::value_type> range;
	get_minmax_median(_image, range, _min_ratio, _max_ratio, _Nvalues);
	return range;
}

template <class Timg>
void ocv::get_minmax_median(const Timg& _image, ocv::Range<typename Timg::value_type::value_type>& _range, const double _min_ratio, const double _max_ratio, const unsigned int _Nvalues) {

	get_minmax_median(_image, _range.first, _range.second, _min_ratio, _max_ratio, _Nvalues);
}

template <class Timg>
void ocv::get_minmax_median(const Timg& _image, typename Timg::value_type::value_type& _min_value, typename Timg::value_type::value_type& _max_value, const double _min_ratio, const double _max_ratio, const unsigned int _Nvalues) {
	
	if (ocv::is_valid(_image)) {

		typedef typename Timg::value_type Tvec;
		typedef typename Tvec::value_type Tvalue;

		ocv::Range<Tvalue> range = get_minmax(_image);

		Timg histogram;
		ocv::calcHist(_image, histogram, _Nvalues, range);

		Tvalue coef = (range.second - range.first) / (_Nvalues - 1);

		_min_value = std::numeric_limits<ocv::Tvalue>::max();
		_max_value = std::numeric_limits<ocv::Tvalue>::lowest();

		bool l_reached_min = false;
		bool l_reached_max = false;
		bool l_reached_max_last = false;

		Tvec sum = Tvec::all(0.);
		int dim;
		int i = 0;

		for (typename Timg::const_iterator it = histogram.begin(); it != histogram.end() && !l_reached_max_last; ++it) {

			if (!l_reached_min) {
				l_reached_min = true;
				for (dim = 0; dim < Tvec::channels; dim++) {
					l_reached_min &= (sum[dim] >= _min_ratio);
				}
				if (l_reached_min) {
					_min_value = range.first + coef * i;
				}
			}

			if (!l_reached_max) {
				for (dim = 0; dim < Tvec::channels; dim++) {
					l_reached_max |= (sum[dim] >= _max_ratio);
				}
			}

			if (l_reached_max && !l_reached_max_last) {
				for (dim = 0; dim < Tvec::channels; dim++) {
					l_reached_max_last |= ((*it)[dim] > ocv::Tvalue(0.));
				}
				_max_value = range.first + coef * i;
			}

			sum += *it;
			i++;
		}

		/*! Because sum of histogram is not always exactly 1.*/
		if (!l_reached_min) {
			_min_value = range.second;
		}
		if (!l_reached_max_last) {
			_max_value = range.second;
		}

		/*! For multi channel images, max can be reached before min. In this case :*/
		if (l_reached_max_last && !l_reached_min) {
			_min_value = _max_value - coef;
		}

	} else {

		_min_value = Tvalue(0);
		_max_value = Tvalue(0);
	}
	
}