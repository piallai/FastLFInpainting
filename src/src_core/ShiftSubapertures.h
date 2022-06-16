/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "SubaperturesData.h"
#include "Inpainting.h"

template <class Timg>
class ShiftSubapertures {

	static Timg white_image;
	static ocv::Timg1 matissa_image;
	static ocv::Timg1 disparity_mean_image_buff;
	static ocv::Timg1 disparity_mean_image;
	/*! For detection of zero weight values in image, so that they are not filled with inpainting.*/
	static ocv::Tmask zero_values;
	static cv::Mat_< cv::Vec<uchar, Timg::value_type::channels> > convert_image;
	/*! Assumption of likely pixels to be shifted inside real mask.*/
	static ocv::Tmask image_mask_dilated;

public :
	static void warp_backward(const SubaperturesData<Timg>& _subapertures_input, const ocv::VecImg& _disparities, const ocv::Timg& _central_image, const ocv::Tmask& _image_mask, const UVindices _central_image_indices, SubaperturesData<Timg>& _subapertures_output);
	static void warp_backward(const Timg& _image_input, const ocv::VecImg& _disparities, const std::pair<float, float>& _coef, Timg& _image_output);
	static void warp_backward(const Timg& _image_input, const ocv::VecImg& _disparities, const std::pair<float, float>& _coef, Timg& _image_output, const ocv::Tmask& _image_mask);

	static void warp_forward(const SubaperturesData<Timg>& _subapertures_input, const ocv::VecImg& _disparities, const ocv::Timg& _central_image, const ocv::Tmask& _image_mask, const UVindices _central_image_indices, SubaperturesData<Timg>& _subapertures_output);
	static void warp_forward(const ocv::VecImg& _disparities, const ocv::Timg& _central_image, const UVindices _central_image_indices, const Fpair& _baseline_coef, SubaperturesData<Timg>& _subapertures_output);
	static void warp_forward(const Timg& _image_input, const ocv::VecImg& _disparities, const Fpair& _baseline, const Fpair& _coef, Timg& _image_output, bool _l_disparity_mean=true, bool _l_inpaint_crack=true, const ocv::Tmask _image_mask=ocv::Tmask());

};

template <class Timg>
Timg ShiftSubapertures<Timg>::white_image;
template <class Timg>
ocv::Timg1 ShiftSubapertures<Timg>::matissa_image;
template <class Timg>
ocv::Timg1 ShiftSubapertures<Timg>::disparity_mean_image_buff;
template <class Timg>
ocv::Timg1 ShiftSubapertures<Timg>::disparity_mean_image;
template <class Timg>
ocv::Tmask ShiftSubapertures<Timg>::zero_values;
template <class Timg>
cv::Mat_< cv::Vec<uchar, Timg::value_type::channels> > ShiftSubapertures<Timg>::convert_image;
template <class Timg>
ocv::Tmask ShiftSubapertures<Timg>::image_mask_dilated;

template <class Timg>
void ShiftSubapertures<Timg>::warp_backward(const SubaperturesData<Timg>& _subapertures_input, const ocv::VecImg& _disparities, const ocv::Timg& _central_image, const ocv::Tmask& _image_mask, const UVindices _central_image_indices, SubaperturesData<Timg>& _subapertures_output) {

	_subapertures_input.copyTo(_subapertures_output);

	_central_image.copyTo(_subapertures_output[_central_image_indices]);

	std::pair<float, float> offset;

	for (unsigned int u = 0; u < _subapertures_input.get_Nu(); u++) {
		for (unsigned int v = 0; v < _subapertures_input.get_Nv(); v++) {

			offset.first = u;
			offset.first -= _central_image_indices.first;
			offset.second = v;
			offset.second -= _central_image_indices.second;

			warp_backward(_central_image, _disparities, offset, _subapertures_output(u, v), _image_mask);

		}
	}

}

template <class Timg>
void ShiftSubapertures<Timg>::warp_forward(const SubaperturesData<Timg>& _subapertures_input, const ocv::VecImg& _disparities, const ocv::Timg& _central_image, const ocv::Tmask& _image_mask, const UVindices _central_image_indices, SubaperturesData<Timg>& _subapertures_output) {

	std::cout << "Warping supapertures" << std::endl;

	_subapertures_input.copyTo(_subapertures_output);

	_central_image.copyTo(_subapertures_output[_central_image_indices]);

	Fpair offset;

	for (unsigned int u = 0; u < _subapertures_input.get_Nu(); u++) {
		for (unsigned int v = 0; v < _subapertures_input.get_Nv(); v++) {

			offset.first = u;
			offset.first -= _central_image_indices.first;
			offset.second = v;
			offset.second -= _central_image_indices.second;

			offset.first *= -1;
			offset.second *= -1;

			warp_forward(_central_image, _disparities, _subapertures_input.get_baseline(), offset, _subapertures_output(u, v), true, true, _image_mask);
			
		}
		Misc::display_progression(u, _subapertures_input.get_Nu());
	}

}

template <class Timg>
void ShiftSubapertures<Timg>::warp_forward(const ocv::VecImg& _disparities, const ocv::Timg& _central_image, const UVindices _central_image_indices, const Fpair& _baseline_coef, SubaperturesData<Timg>& _subapertures_output) {

	std::cout << "Warping supapertures" << std::endl;

	_central_image.copyTo(_subapertures_output[_central_image_indices]);

	Fpair offset;

	for (unsigned int u = 0; u < _subapertures_output.get_Nu(); u++) {
		for (unsigned int v = 0; v < _subapertures_output.get_Nv(); v++) {

			offset.first = u;
			offset.first -= _central_image_indices.first;
			offset.second = v;
			offset.second -= _central_image_indices.second;

			offset.first *= std::abs(_subapertures_output.get_baseline().first);
			offset.second *= std::abs(_subapertures_output.get_baseline().second);

			offset.first *= _baseline_coef.first;
			offset.second *= _baseline_coef.second;

			offset.first *= -1;
			offset.second *= -1;

			warp_forward(_central_image, _disparities, _subapertures_output.get_baseline(), offset, _subapertures_output(u, v));

		}
		Misc::display_progression(u, _subapertures_output.get_Nu());
	}

}


template <class Tlist, class Titerator>
cv::Point iterator_to_coordinates(const Titerator& _iterator,const unsigned int _width, const Tlist& _list) {

	cv::Point coordinates;
	coordinates.x = std::distance(_list.begin(), _iterator) % _width;
	coordinates.y = (std::distance(_list.begin(), _iterator) - coordinates.x) / _width;
	return coordinates;
}



template <class Timg>
void ShiftSubapertures<Timg>::warp_forward(const Timg& _image_input, const ocv::VecImg& _disparities, const Fpair& _baseline_abs, const Fpair& _coef, Timg& _image_output, bool _l_disparity_mean, bool _l_inpaint_crack, const ocv::Tmask _image_mask) {
	
	bool l_mask = ocv::is_valid(_image_mask);

	if (!l_mask || _image_mask.size() == _image_input.size()) {

		if (_image_input.size() == _disparities.first.size() && _image_input.size() == _disparities.second.size()) {

			if (l_mask) {
				_image_input.copyTo(_image_output, _image_mask);
				/*! Dilate mask by 4, thus assuming disparity doesn't exceed this value.*/
				cv::dilate(_image_mask, image_mask_dilated, cv::Mat(), cv::Point(-1, -1), 4);
			} else {
				_image_input.copyTo(_image_output);
			}

			cv::Point2f disparity;
			cv::Point2f displacement;

			cv::Point2f shift;
			cv::Point2f matissa, matissa_m1;
			ocv::Tvalue matissa_coef;
			cv::Point2i floor_shift, ceil_shift;
			cv::Point2i floor_displacement, ceil_displacement;
			ocv::Tvalue disparity_norm;
			ocv::Tvalue disparity_diff;

			typename Timg::value_type disparity_weight_vec;

			typename Timg::value_type value, value_null, value_one;
			value_null = Timg::value_type::all(0.);
			value_one = Timg::value_type::all(1.);

			white_image.create(_image_input.size());
			white_image = value_one;
			matissa_image.create(_image_input.size());
			matissa_image = 1.;

			ocv::Tvalue disparity_offset = (ocv::Tvalue) - 0.1;// 2;//negative : let pass more data (ie: ghost effect), positive : more picky and can yield cracks
			if (_l_disparity_mean) {
				SubaperturesDataBase::average_baseline_weighted(_disparities, _baseline_abs.second / _baseline_abs.first, disparity_mean_image_buff);
				ShiftSubapertures<ocv::Timg1>::warp_forward(disparity_mean_image_buff, _disparities, _baseline_abs, _coef, disparity_mean_image, false, false, _image_mask);
			}

			ocv::Tmask::const_iterator it_mask;
			if (l_mask) {
				it_mask = _image_mask.begin();
			}

			typename Timg::iterator it_white_image = white_image.begin();
			typename ocv::Timg1::iterator it_matissa_image = matissa_image.begin();
			for (typename Timg::iterator it_image = _image_output.begin(); it_image != _image_output.end(); ++it_image, ++it_white_image, ++it_matissa_image) {

				if (!l_mask || (*it_mask)[0] == ocv::mask_value) {
					(*it_image) = value_null;
					(*it_white_image) = value_null;
					(*it_matissa_image) = 0.;
				}

				if (l_mask) {
					++it_mask;
				}
			}

			int iterator_offset;
			cv::Point coordinates(0, 0);
			ocv::Timg1::const_iterator it_disp_x = _disparities.first.begin();
			ocv::Timg1::const_iterator it_disp_y = _disparities.second.begin();
			ocv::Tmask::const_iterator it_mask_dilated;
			if (l_mask) {
				it_mask = _image_mask.begin();
				it_mask_dilated = image_mask_dilated.begin();
			}
			it_white_image = white_image.begin();
			it_matissa_image = matissa_image.begin();
			ocv::Timg1::const_iterator it_disparity_mean;
			if (_l_disparity_mean) {
				it_disparity_mean = disparity_mean_image.begin();
			}

			typename Timg::const_iterator it_input_image = _image_input.begin();
			for (typename Timg::iterator it_image = _image_output.begin(); it_image != _image_output.end(); ++it_image, ++it_disp_x, ++it_disp_y, ++it_input_image, ++it_white_image, ++it_matissa_image, ++it_disparity_mean) {

				if (!l_mask || (*it_mask_dilated)[0] == ocv::mask_value) {


					disparity.x = (*it_disp_x)[0];
					disparity.y = (*it_disp_y)[0];

					displacement.x = disparity.x * (float)_coef.first;
					displacement.y = disparity.y * (float)_coef.second;

					if (_l_disparity_mean) {
						SubaperturesDataBase::average_values_baseline_weighted(disparity, _baseline_abs.second / _baseline_abs.first, disparity_norm);
					}

					shift.x = (float)coordinates.x;
					shift.x += displacement.x;
					shift.y = (float)coordinates.y;
					shift.y += displacement.y;

					floor_shift.x = (int)std::floor(shift.x);
					floor_shift.y = (int)std::floor(shift.y);
					ceil_shift.x = (int)std::ceil(shift.x);
					ceil_shift.y = (int)std::ceil(shift.y);

					floor_displacement.x = (int)std::floor(displacement.x);
					floor_displacement.y = (int)std::floor(displacement.y);
					ceil_displacement.x = (int)std::ceil(displacement.x);
					ceil_displacement.y = (int)std::ceil(displacement.y);

					matissa.x = (float)ceil_shift.x;
					matissa.x -= shift.x;
					matissa_m1.x = 1.;
					matissa_m1.x -= matissa.x;

					matissa.y = (float)ceil_shift.y;
					matissa.y -= shift.y;
					matissa_m1.y = 1.;
					matissa_m1.y -= matissa.y;

					if (floor_shift.x >= 0 && floor_shift.x < _image_input.size().width) {

						if (floor_shift.y >= 0 && floor_shift.y < _image_input.size().height) {

							iterator_offset = floor_displacement.x;
							iterator_offset += floor_displacement.y*_image_input.size().width;

							if (l_mask) {
								it_mask += iterator_offset;
							}

							if (!l_mask || (*it_mask)[0] == ocv::mask_value) {

								if (_l_disparity_mean) {
									it_disparity_mean += iterator_offset;
									disparity_diff = disparity_norm;
									disparity_diff -= (*it_disparity_mean)[0];
									it_disparity_mean -= iterator_offset;

								}

								if (!_l_disparity_mean || disparity_diff >= disparity_offset) {

									matissa_coef = matissa.x;
									matissa_coef *= matissa.y;

									it_image += iterator_offset;
									value = *it_input_image;
									ocv::mul(value, matissa_coef);
									(*it_image) += value;
									it_image -= iterator_offset;

									it_white_image += iterator_offset;
									it_matissa_image += iterator_offset;
									value = value_one;
									ocv::mul(value, matissa_coef);

									(*it_white_image) += value;
									(*it_matissa_image)[0] += matissa_coef;
									it_white_image -= iterator_offset;
									it_matissa_image -= iterator_offset;

								} else {

								}

							}

							if (l_mask) {
								it_mask -= iterator_offset;
							}

						} else {

						}

						if (ceil_shift.y >= 0 && ceil_shift.y < _image_input.size().height) {

							iterator_offset = (int)floor_displacement.x;
							iterator_offset += ceil_displacement.y*_image_input.size().width;

							if (l_mask) {
								it_mask += iterator_offset;
							}

							if (!l_mask || (*it_mask)[0] == ocv::mask_value) {

								matissa_coef = matissa.x;
								matissa_coef *= matissa_m1.y;

								if (_l_disparity_mean) {
									it_disparity_mean += iterator_offset;
									disparity_diff = disparity_norm;
									disparity_diff -= (*it_disparity_mean)[0];
									it_disparity_mean -= iterator_offset;

								}

								if (!_l_disparity_mean || disparity_diff >= disparity_offset) {

									it_image += iterator_offset;
									value = *it_input_image;
									ocv::mul(value, matissa_coef);
									(*it_image) += value;
									it_image -= iterator_offset;

									it_white_image += iterator_offset;
									it_matissa_image += iterator_offset;
									value = value_one;
									ocv::mul(value, matissa_coef);
									(*it_white_image) += value;
									(*it_matissa_image)[0] += matissa_coef;
									it_white_image -= iterator_offset;
									it_matissa_image -= iterator_offset;

								} else {

								}

							}

							if (l_mask) {
								it_mask -= iterator_offset;
							}

						}

					} else {

					}

					if (ceil_shift.x >= 0 && ceil_shift.x < _image_input.size().width) {

						if (ceil_shift.y >= 0 && ceil_shift.y < _image_input.size().height) {

							iterator_offset = (int)ceil_displacement.x;
							iterator_offset += ceil_displacement.y*_image_input.size().width;

							if (l_mask) {
								it_mask += iterator_offset;
							}

							if (!l_mask || (*it_mask)[0] == ocv::mask_value) {

								if (_l_disparity_mean) {
									it_disparity_mean += iterator_offset;
									disparity_diff = disparity_norm;
									disparity_diff -= (*it_disparity_mean)[0];
									it_disparity_mean -= iterator_offset;

								}

								if (!_l_disparity_mean || disparity_diff >= disparity_offset) {

									matissa_coef = matissa_m1.x;
									matissa_coef *= matissa_m1.y;

									it_image += iterator_offset;
									value = *it_input_image;
									ocv::mul(value, matissa_coef);
									(*it_image) += value;
									it_image -= iterator_offset;

									it_white_image += iterator_offset;
									it_matissa_image += iterator_offset;
									value = value_one;
									ocv::mul(value, matissa_coef);
									(*it_white_image) += value;
									(*it_matissa_image)[0] += matissa_coef;
									it_white_image -= iterator_offset;
									it_matissa_image -= iterator_offset;

								} else {

								}

							}

							if (l_mask) {
								it_mask -= iterator_offset;
							}

						}

						if (floor_shift.y >= 0 && floor_shift.y < _image_input.size().height) {

							iterator_offset = (int)ceil_displacement.x;
							iterator_offset += floor_displacement.y*_image_input.size().width;

							if (l_mask) {
								it_mask += iterator_offset;
							}

							if (!l_mask || (*it_mask)[0] == ocv::mask_value) {

								if (_l_disparity_mean) {
									it_disparity_mean += iterator_offset;
									disparity_diff = disparity_norm;
									disparity_diff -= (*it_disparity_mean)[0];
									it_disparity_mean -= iterator_offset;
								}

								if (!_l_disparity_mean || disparity_diff >= disparity_offset) {

									matissa_coef = matissa_m1.x;
									matissa_coef *= matissa.y;

									it_image += iterator_offset;
									value = *it_input_image;
									ocv::mul(value, matissa_coef);
									(*it_image) += value;
									it_image -= iterator_offset;

									it_white_image += iterator_offset;
									it_matissa_image += iterator_offset;
									value = value_one;
									ocv::mul(value, matissa_coef);
									(*it_white_image) += value;
									(*it_matissa_image)[0] += matissa_coef;
									it_white_image -= iterator_offset;
									it_matissa_image -= iterator_offset;

								} else {

								}

							}

							if (l_mask) {
								it_mask -= iterator_offset;
							}

						}

					}



				}//end if mask dilated

				if (l_mask) {
					++it_mask;
					++it_mask_dilated;
				}

				coordinates.x++;
				if (coordinates.x == _image_input.size().width) {
					coordinates.y++;
					coordinates.x = 0;
				}

			}

			cv::divide(_image_output, white_image, _image_output);

			if (_l_inpaint_crack) {

				/*! Get zero values in input image, to not be mistaken in mask inpainting.*/
				zero_values.create(_image_input.size());
				zero_values = 0;
				typename Timg::const_iterator it_white_image_inpaint = white_image.begin();
				for (ocv::Tmask::iterator it_zero = zero_values.begin(); it_zero != zero_values.end(); ++it_zero, ++it_white_image_inpaint) {
					if ((*it_white_image_inpaint) == value_null) {
						(*it_zero)[0] = ocv::mask_value;
					}
				}

				//cv::dilate(zero_values, zero_values, cv::Mat(), cv::Point(-1, -1), 1);

				std::pair<double, double> scaling = ocv::convertTo(_image_output, convert_image, true);
				Inpainting::inpaintTelea(convert_image, zero_values);
				ocv::convertTo(convert_image, _image_output, false);
				ocv::rescale_inv(_image_output, scaling);

				/*! Cracks are sometimes not entirely inpainted when using Inpainting::inpaintTelea_modified (not verified yet with Inpainting::inpaintTelea).
				Some pixels of regions supposed to be inpainted remain black.
				Following code is meant to show it for further resolution.*/
				bool l_display_telea_bug = false;
				if (l_display_telea_bug) {
					ocv::Tmask crack_image(_image_output.size());
					crack_image = (uchar)0;
					it_white_image_inpaint = white_image.begin();
					ocv::Tmask::iterator it_crack = crack_image.begin();
					for (typename Timg::const_iterator it = _image_output.begin(); it != _image_output.end(); ++it, ++it_white_image_inpaint, ++it_crack) {
						if (*it == Timg::value_type::all(0.)) {
							(*it_crack)[0] = ocv::mask_value;
						}
					}
					ocv::imshow(zero_values, "zero values");
					ocv::imshow(crack_image, "crack image");
				}
				

				/*cv::dilate(crack_image, crack_image, cv::Mat(), cv::Point(-1, -1), 2);
				scaling = ocv::convertTo(_image_output, convert_image, true);
				Inpainting::inpaintTelea_modified(convert_image, crack_image);
				ocv::convertTo(convert_image, _image_output, false);
				ocv::rescale_inv(_image_output, scaling);*/

			}

		} else {
			
			std::cout << "Incoherent image and disparity dimensions in ShiftSubapertures<Timg>::warp_forward" << std::endl;
		}

	} else {

		std::cout << "Incoherent mask in ShiftSubapertures<Timg>::warp_forward" << std::endl;

	}

	
}


///////////////////

template <class Timg>
void ShiftSubapertures<Timg>::warp_backward(const Timg& _image_input, const ocv::VecImg& _disparities, const std::pair<float, float>& _coef, Timg& _image_output) {

	ocv::Tmask image_mask(_image_input.size());
	image_mask = ocv::mask_value;
	shift(_image_input, _disparities, _coef, _image_output, image_mask);
}

template <class Timg>
void ShiftSubapertures<Timg>::warp_backward(const Timg& _image_input, const ocv::VecImg& _disparities, const std::pair<float, float>& _coef, Timg& _image_output, const ocv::Tmask& _image_mask) {


	_image_input.copyTo(_image_output, _image_mask);

	cv::Point2f disparity;

	cv::Point2f shift;
	cv::Point2f matissa, matissa_m1;

	typename Timg::value_type value, value_null;
	value_null = Timg::value_type::all(0.);


	cv::Point coordinates(0, 0);
	ocv::Timg1::const_iterator it_disp_x = _disparities.first.begin();
	ocv::Timg1::const_iterator it_disp_y = _disparities.second.begin();
	ocv::Tmask::const_iterator it_mask = _image_mask.begin();
	typename Timg::const_iterator it_input_image = _image_input.begin();
	for (typename Timg::iterator it_image = _image_output.begin(); it_image != _image_output.end(); ++it_image, ++it_disp_x, ++it_disp_y, ++it_mask, ++it_input_image) {

		if ((*it_mask)[0] == ocv::mask_value) {

			disparity.x = (*it_disp_x)[0] * _coef.first;
			disparity.y = (*it_disp_y)[0] * _coef.second;

			shift.x = coordinates.x;
			shift.x += disparity.x;
			shift.y = coordinates.y;
			shift.y += disparity.y;

			matissa.x = std::ceil(shift.x);
			matissa.x -= shift.x;
			matissa_m1.x = 1.;
			matissa_m1.x -= matissa.x;

			matissa.y = std::ceil(shift.y);
			matissa.y -= shift.y;
			matissa_m1.y = 1.;
			matissa_m1.y -= matissa.y;

			if (std::floor(shift.x) >= 0 && std::floor(shift.x) <= _image_input.size().width) {

				if (std::floor(shift.y) >= 0 && std::floor(shift.y) <= _image_input.size().height) {

					it_input_image += std::floor(disparity.x);
					it_input_image += std::floor(disparity.y)*_image_input.size().width;
					value = *it_input_image;
					ocv::mul(value, matissa.x*matissa.y);
					(*it_image) = value;
					it_input_image -= std::floor(disparity.x);
					it_input_image -= std::floor(disparity.y)*_image_input.size().width;

				} else {
					(*it_image) = value_null;
				}

				if (std::ceil(shift.y) >= 0 && std::ceil(shift.y) <= _image_input.size().height) {

					it_input_image += std::floor(disparity.x);
					it_input_image += std::ceil(disparity.y)*_image_input.size().width;
					value = *it_input_image;
					ocv::mul(value, matissa.x*matissa_m1.y);
					(*it_image) += value;
					it_input_image -= std::floor(disparity.x);
					it_input_image -= std::ceil(disparity.y)*_image_input.size().width;

				}

			} else {
				(*it_image) = value_null;
			}

			if (std::ceil(shift.x) >= 0 && std::ceil(shift.x) <= _image_input.size().width) {

				if (std::ceil(shift.y) >= 0 && std::ceil(shift.y) <= _image_input.size().height) {

					it_input_image += std::ceil(disparity.x);
					it_input_image += std::ceil(disparity.y)*_image_input.size().width;
					value = *it_input_image;
					ocv::mul(value, matissa_m1.x*matissa_m1.y);
					(*it_image) += value;
					it_input_image -= std::ceil(disparity.x);
					it_input_image -= std::ceil(disparity.y)*_image_input.size().width;

				}

				if (std::floor(shift.y) >= 0 && std::floor(shift.y) <= _image_input.size().height) {

					it_input_image += std::ceil(disparity.x);
					it_input_image += std::floor(disparity.y)*_image_input.size().width;
					value = *it_input_image;
					ocv::mul(value, matissa_m1.x*matissa.y);
					(*it_image) += value;
					it_input_image -= std::ceil(disparity.x);
					it_input_image -= std::floor(disparity.y)*_image_input.size().width;

				}

			}

		}

		coordinates.x++;
		if (coordinates.x == _image_input.size().width) {
			coordinates.y++;
			coordinates.x = 0;
		}

	}

}
