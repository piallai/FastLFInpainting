/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "SubaperturesData.h"
#include "ocv_minmax.h"

/*! Overload of some ocv namespace functions dedicated to opencv images. So the same function applies to subapertures.*/
/*! Meant to be included in SubaperturesData.h */

namespace ocv {
	template <class Tvec>
	void minMaxLoc(const SubaperturesData< cv::Mat_<Tvec> >& _subapertures, Tvec* _min, Tvec* _max, const SubaperturesData<ocv::Tmask> _mask = SubaperturesData<ocv::Tmask>(), bool _l_inv_mask = false) {

		*_min = Tvec::all(std::numeric_limits<typename Tvec::value_type>::max());
		*_max = Tvec::all(std::numeric_limits<typename Tvec::value_type>::lowest());

		Tvec min_image, max_image;

		ocv::Tmask mask;
		for (unsigned int u = 0; u<_subapertures.get_Nu(); u++) {
			for (unsigned int v = 0; v<_subapertures.get_Nv(); v++) {
				if (_mask.has_same_dimensions(_subapertures)) {
					if (_l_inv_mask) {
						ocv::mask_inverse(_mask(u, v), mask);
					} else {
						mask = _mask(u, v);
					}
					ocv::minMaxLoc(_subapertures(u, v), &min_image, &max_image, mask);
				} else {
					ocv::minMaxLoc(_subapertures(u, v), &min_image, &max_image);
				}
				for (unsigned int c = 0; c < Tvec::channels; c++) {
					if (min_image[c] < (*_min)[c]) {
						(*_min)[c] = min_image[c];
					}
					if (max_image[c] > (*_max)[c]) {
						(*_max)[c] = max_image[c];
					}
				}
			}
		}
	}

	template <class Tvec>
	void minMaxLoc(const SubaperturesData< cv::Mat_<Tvec> >& _subapertures, typename Tvec::value_type* _min, typename Tvec::value_type* _max, const SubaperturesData<ocv::Tmask> _mask = SubaperturesData<ocv::Tmask>(), bool _l_inv_mask = false) {

		*_min = std::numeric_limits<typename Tvec::value_type>::max();
		*_max = std::numeric_limits<typename Tvec::value_type>::lowest();

		typename Tvec::value_type min_image, max_image;

		ocv::Tmask mask;
		for (unsigned int u = 0; u<_subapertures.get_Nu(); u++) {
			for (unsigned int v = 0; v<_subapertures.get_Nv(); v++) {
				if (_mask.has_same_dimensions(_subapertures)) {
					if (_l_inv_mask) {
						ocv::mask_inverse(_mask(u, v), mask);
					} else {
						mask = _mask(u, v);
					}
					ocv::minMaxLoc(_subapertures(u, v), &min_image, &max_image, mask);
				} else {
					ocv::minMaxLoc(_subapertures(u, v), &min_image, &max_image);
				}
				if (min_image < *_min) {
					*_min = (typename Tvec::value_type)min_image;
				}
				if (max_image > *_max) {
					*_max = (typename Tvec::value_type)max_image;
				}
			}
		}

	}

	template <class Tvec>
	void bound(const SubaperturesData< cv::Mat_<Tvec> >& _input, SubaperturesData< cv::Mat_<Tvec> >& _output, const Tvec _value_min, const Tvec _value_max) {

		_output.resize(_input.size(), _input.get_image_size());

		for (unsigned int u = 0; u<_input.get_Nu(); u++) {
			for (unsigned int v = 0; v<_input.get_Nv(); v++) {

				ocv::bound(_input(u, v), _output(u, v), _value_min, _value_max);
			}
		}

	}

	template <class Tval, int Dim>
	std::pair<Tval, Tval> get_minmax_range(const SubaperturesData< cv::Mat_< cv::Vec<Tval, Dim> > > _subapertures = SubaperturesData< cv::Mat_< cv::Vec<float, Dim> > >(), bool _l_image_range_saturate = false) {

		std::pair<Tval, Tval> range;
		range.first = std::numeric_limits<Tval>::lowest();
		range.second = std::numeric_limits<Tval>::max();
		return range;
	}

	template <int Dim>
	std::pair<float, float> get_minmax_range(const SubaperturesData< cv::Mat_< cv::Vec<float, Dim> > > _subapertures = SubaperturesData< cv::Mat_< cv::Vec<float, Dim> > >(), bool _l_image_range_saturate = false) {

		std::pair<float, float> range;
		if (_subapertures) {
			float min, max;
			ocv::minMaxLoc(_subapertures, &min, &max);
			//std::cout << "min = " << min << ", max = " << max << std::endl;
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

	template <int Dim>
	std::pair<double, double> get_minmax_range(const SubaperturesData< cv::Mat_< cv::Vec<double, Dim> > > _subapertures = SubaperturesData< cv::Mat_< cv::Vec<float, Dim> > >(), bool _l_image_range_saturate = false) {

		std::pair<double, double> range;
		if (_subapertures) {
			double min, max;
			ocv::minMaxLoc(_subapertures, &min, &max);
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


	template <class Timg>
	void build_mask_angular_boundaries(const SubaperturesData<Timg>& _subapertures, SubaperturesData<ocv::Tmask>& _masks) {

		_subapertures.copyPropertiesTo(_masks);
		_masks.resize_images(_subapertures.get_image_size());
		_masks = 0;
		for (unsigned int u = 0; u < _subapertures.get_Nu(); u++) {
			for (unsigned int v = 0; v < _subapertures.get_Nv(); v++) {
				_masks(u, v).create(_subapertures(u, v).size());
				if (u == 0 || u == _subapertures.get_Nu() - 1 || v == 0 || v == _subapertures.get_Nv() - 1) {
					_masks(u, v) = ocv::mask_value;
				}
			}
		}

	}

	template <class Tvalue, int Dim1, int Dim2>
	void cvtColor(const SubaperturesData< cv::Mat_< cv::Vec<Tvalue, Dim1> > >& _subapertures, SubaperturesData< cv::Mat_< cv::Vec<Tvalue, Dim2> > >& _subapertures_output, const unsigned int _conversion_type) {

		_subapertures.copyPropertiesTo(_subapertures_output);
		for (unsigned int u = 0; u < _subapertures.get_Nu(); u++) {
			for (unsigned int v = 0; v < _subapertures.get_Nv(); v++) {

				cvtColor(_subapertures(u, v), _subapertures_output(u, v), _conversion_type);

			}
		}

	}

	template <class Tvalue>
	void convert(const SubaperturesData< cv::Mat_< cv::Vec<Tvalue, 3> > >& _subapertures, SubaperturesData< cv::Mat_< cv::Vec<Tvalue, 1> > >& _subapertures_output) {

		ocv::cvtColor(_subapertures, _subapertures_output, cv::COLOR_BGR2GRAY);

	}

	template <class Tvalue>
	void convert(const SubaperturesData< cv::Mat_< cv::Vec<Tvalue, 1> > >& _subapertures, SubaperturesData< cv::Mat_< cv::Vec<Tvalue, 1> > >& _subapertures_output) {

		_subapertures.copyTo(_subapertures_output);

	}
}

/*! Merges two masks.*/
void operator&=(SubaperturesData<ocv::Tmask>& _masks1, const SubaperturesData<ocv::Tmask>& _masks2);
void operator|=(SubaperturesData<ocv::Tmask>& _masks1, const SubaperturesData<ocv::Tmask>& _masks2);
