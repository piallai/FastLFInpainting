/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "ocv_rw.h"

template <class Timg>
Images4D<Timg>::Images4D() {

}

template <class Timg>
Images4D<Timg>::~Images4D() {

}

template <class Timg>
bool Images4D<Timg>::is_coherent() const {

	bool l_coherent = true;
	for (unsigned int i = 0; i < (unsigned int)get_outer_size().width && l_coherent; i++) {
		for (unsigned int j = 0; j < (unsigned int)get_outer_size().height && l_coherent; j++) {
			l_coherent = (*this)(i, j).size() == get_inner_size();
		}
	}
	return l_coherent;
}

template <class Timg>
bool Images4D<Timg>::is_sparse() const {

	bool l_sparse = false;
	for (unsigned int i = 0; i < (unsigned int)get_outer_size().width && !l_sparse; i++) {
		for (unsigned int j = 0; j < (unsigned int)get_outer_size().height && !l_sparse; j++) {
			l_sparse = (*this)(i, j).size() == cv::Size(0, 0);
		}
	}
	return l_sparse;
}

template <class Timg>
bool Images4D<Timg>::is_coherent_sparsity() const {

	bool l_coherent_sparsity = true;
	for (unsigned int i = 0; i < (unsigned int)get_outer_size().width && l_coherent_sparsity; i++) {
		for (unsigned int j = 0; j < (unsigned int)get_outer_size().height && l_coherent_sparsity; j++) {
			l_coherent_sparsity = ((*this)(i, j).size() == get_inner_size() || (*this)(i, j).size() == cv::Size(0, 0));
		}
	}
	return l_coherent_sparsity;
}

template <class Timg>
bool Images4D<Timg>::is_identical() const {

	bool l_identical = is_coherent();
	for (unsigned int i = 0; i < (unsigned int)get_outer_size().width && l_identical; i++) {
		for (unsigned int j = 0; j < (unsigned int)get_outer_size().height && l_identical; j++) {
			l_identical = ocv::is_equal((*this)(i, j), (*this)(0, 0));
		}
	}
	return l_identical;
}

template <class Timg>
template <class Timg_arg>
bool Images4D<Timg>::has_same_dimensions(const Images4D<Timg_arg>& _images) const {

	return get_outer_size() == _images.get_outer_size() && get_inner_size() == _images.get_inner_size();
}

template <class Timg>
void Images4D<Timg>::build_full_image(Timg& _image, bool _l_original_coordinates, const unsigned int _spacing, const typename Timg::value_type _space_value) const {

	/*! Not sure it actually makes sense to have an original coordinates option. Work in progress.*/
	//_l_original_coordinates = false;

	if (!this->empty()) {

		std::pair<bool, bool> l_transposes = get_full_image_transposes();

		cv::Size inner_size = get_inner_size();
		if (l_transposes.first != l_transposes.second) {
			std::swap(inner_size.width, inner_size.height);
		}
		cv::Size outer_size = get_outer_size();
		if (l_transposes.second) {
			std::swap(outer_size.width, outer_size.height);
		}

		//////////////

		Images4D_base::Tarray<UIpair> coordinates;
		coordinates = get_coordinates_from_original(_l_original_coordinates);
		cv::Size outer_size_original = get_outer_size_original(_l_original_coordinates);

		cv::Size size = inner_size;
		size.width *= outer_size_original.height;
		size.width += _spacing * (outer_size_original.height - 1);
		size.height *= outer_size_original.width;
		size.height += _spacing * (outer_size_original.width - 1);
		_image.create(size);
		_image = _space_value;

		Timg image;
		Timg image_transposed;

		unsigned int i1, j1;
		for (unsigned int i_ori = 0; i_ori < (unsigned int)outer_size_original.width; i_ori++) {
			for (unsigned int j_ori = 0; j_ori < (unsigned int)outer_size_original.height; j_ori++) {
				i1 = i_ori;
				j1 = j_ori;

				unsigned int rect_x = j1 * (inner_size.width + _spacing);
				unsigned int rect_y = i1 * (inner_size.height + _spacing);

				if (l_transposes.second) {
					std::swap(i1, j1);
				}

				image = (*this)(coordinates[i_ori][j_ori].first, coordinates[i_ori][j_ori].second);

				if (l_transposes.first != l_transposes.second) {
					cv::transpose(image, image_transposed);
					image_transposed.copyTo(_image(cv::Rect(rect_x, rect_y, inner_size.width, inner_size.height)));
				} else {
					image.copyTo(_image(cv::Rect(rect_x, rect_y, inner_size.width, inner_size.height)));
				}
			}
		}

	} else {

		_image.create(cv::Size(0, 0));

	}

}



template <class Timg>
void Images4D<Timg>::display_full_image(const unsigned int _spacing, const typename Timg::value_type _space_value, bool _l_original_coordinates) const {

	Timg full_image;
	build_full_image(full_image, _l_original_coordinates, _spacing, _space_value);
	ocv::imshow(full_image);
}


template <class Timg>
void Images4D<Timg>::display(const std::string _text, bool _l_position_label) const {

	std::string text;
	if (_text.empty()) {
		text = get_class_name();
	} else {
		text = _text;
	}

	Timg image_display;
	std::string label;
	for (unsigned int i = 0; i < (unsigned int)get_outer_size().width; i++) {
		for (unsigned int j = 0; j < (unsigned int)get_outer_size().height; j++) {
			if (_l_position_label) {
				label = "[" + Misc::to_string(i) + "," + Misc::to_string(j) + "]";
				(*this)(i, j).copyTo(image_display);
				ocv::putText(image_display, label);
			} else {
				image_display = (*this)(i, j);
			}
			ocv::imshow(image_display, text, 0, true);
		}
	}
	cv::destroyWindow(text);
}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix) const {

	cv::Mat segmentation_image;
	imwrite(_prefix, segmentation_image);
}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const cv::Mat& _segmentation_image) const {

	imwrite(_prefix, true, _segmentation_image);
}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const char* _file_extension) const {

	imwrite(_prefix, std::string(_file_extension));
}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const std::string _file_extension) const {

	cv::Mat segmentation_image;
	ocv::Range<Tvalue> range_input((Tvalue)0, (Tvalue)0);
	imwrite(_prefix, true, segmentation_image, range_input, _file_extension);
}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const ocv::Range<Tvalue>& _range_input) const {

	cv::Mat segmentation_image;
	imwrite(_prefix, true, segmentation_image, _range_input);
}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const ocv::Range<Tvalue>& _range_input, bool _l_original_coordinates) const {

	cv::Mat segmentation_image;
	imwrite(_prefix, _l_original_coordinates, segmentation_image, _range_input);
}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const std::string& _file_extension, const ocv::Range<Tvalue>& _range_input, bool _l_original_coordinates) const {

	cv::Mat segmentation_image;
	imwrite(_prefix, _l_original_coordinates, segmentation_image, _range_input, _file_extension);
}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const Tvalue _range_min, const Tvalue _range_max) const {

	ocv::Range<Tvalue> range_input(_range_min, _range_max);
	imwrite(_prefix, range_input);

}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const OuterModulo& _modulo) const {

	imwrite(_prefix, true, cv::Mat(), ocv::Range<Tvalue>(Tvalue(0), Tvalue(0)), ocv::write_extension, false, cv::COLORMAP_JET, _modulo);

}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const Tvalue _range_min, const Tvalue _range_max, const OuterModulo& _modulo) const {

	imwrite(_prefix, true, cv::Mat(), ocv::Range<Tvalue>(_range_min, _range_max), ocv::write_extension, false, cv::COLORMAP_JET, _modulo);
}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const ocv::Range<Tvalue>& _range_input, const OuterModulo& _modulo) const {

	imwrite(_prefix, true, cv::Mat(), _range_input, ocv::write_extension, false, cv::COLORMAP_JET, _modulo);
}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, const std::string& _file_extension, const ocv::Range<Tvalue>& _range_input, bool _l_original_coordinates, bool _l_colormap, cv::ColormapTypes _colormap_type) const {

	imwrite(_prefix, true, cv::Mat(), _range_input, _file_extension, true, _colormap_type, OuterModulo(1, 1));

}

template <class Timg>
void Images4D<Timg>::imwrite(const std::string _prefix, bool _l_original_coordinates, const cv::Mat _segmentation_image, const ocv::Range<Tvalue> _range_input, const std::string _file_extension, bool _l_colormap, cv::ColormapTypes _colormap_type, const OuterModulo _modulo) const {

	Timg image;
	Timg image_segmented;
	/*! Usefull only if colormap is used.*/
	typename ocv::ColorMapImageTypeValue<Timg, uchar>::Timg image_colormap;
	typename ocv::ColorMapImageTypeValue<Timg, uchar>::Timg image_segmented_colormap;
	

	std::string prefix = _prefix;
	if (!prefix.empty() && prefix.back() != '/' && prefix.back() != '\\') {
		prefix += "-";
	}

	std::string label;
	std::string full_name;
	cv::Size outer_size = get_outer_size();
	if (_l_original_coordinates && is_inverted()) {
		//std::swap(outer_size.width, outer_size.height);
	}

	UIpair writing_coordinates;

	for (unsigned int i = 0; i<(unsigned int)outer_size.width; i++) {
		for (unsigned int j = 0; j<(unsigned int)outer_size.height; j++) {

			if (i%_modulo.first == 0 && j%_modulo.second == 0) {

				if (_l_original_coordinates) {
					writing_coordinates = get_original_coordinates(UIpair(i, j));
				} else {
					writing_coordinates.first = i;
					writing_coordinates.second = j;
				}

				label = Misc::to_string(writing_coordinates.first) + "_" + Misc::to_string(writing_coordinates.second);
				//BM5D//label = Misc::int_to_string(i) + "_" + Misc::int_to_string(j);
				full_name = prefix + get_name() + "_" + label;

				if (_l_original_coordinates) {
					//image = get_image_original_coordinates(i, j);
					image = (*this)(i, j);
				} else {
					image = (*this)(i, j);
				}

				if (ocv::is_valid(image)) {

							image_segmented = image;

						ocv::imwrite(full_name, image_segmented, _range_input, _file_extension);


				}

			}

		}
	}
}


