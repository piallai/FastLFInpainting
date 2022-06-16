/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "SubaperturesData.h"
#include "DirectoryParser.h"
#include "misc_funcs.h"
#ifdef ENABLE_LIB_OPENEXR
#include "OpenEXR_API.h"
#endif
#include "HistMatch.h"
#include "ocv_utils.h"/* For calcHist*/
#include "SubaperturesData_ocv.h"

template <class Timg>
SubaperturesData<Timg>::SubaperturesData() {

	this->name = "subapertures";
	center_coordinates = Fpair(0., 0.);
}

template <class Timg>
SubaperturesData<Timg>::SubaperturesData(const SubaperturesData<Timg>& _subapertures) {

	_subapertures.copyPropertiesTo(*this);
	resize_images(_subapertures.get_image_size());
	*this = _subapertures;
}

template <class Timg>
template <class Timg_arg>
SubaperturesData<Timg>::SubaperturesData(const SubaperturesData<Timg_arg>& _subapertures) {

	_subapertures.copyPropertiesTo(*this);
	resize_images(_subapertures.get_image_size());
	*this = _subapertures;
}

template <class Timg>
SubaperturesData<Timg>::~SubaperturesData() {

}

template <class Timg>
void SubaperturesData<Timg>::clear() {

	SubaperturesDataBase::clear();
	subaperture_images.clear();
}

template <class Timg>
std::string SubaperturesData<Timg>::get_class_name() const {

	return "Subapertures";
}

template <class Timg>
SubaperturesData<Timg> SubaperturesData<Timg>::copy() const {

	SubaperturesData<Timg> subapertures;

	copyTo(subapertures);

	return subapertures;
}

template <class Timg>
void SubaperturesData<Timg>::copyTo(SubaperturesData<Timg>& _subapertures) const {

	copyPropertiesTo(_subapertures);

	for (unsigned int u = 0; u < get_Nu(); u++) {
		for (unsigned int v = 0; v < get_Nv(); v++) {
			if (ocv::is_valid((*this)(u, v))) {
				(*this)(u, v).copyTo(_subapertures(u, v));
			} else {
				_subapertures(u, v).release();
			}
		}
	}

}

template <class Timg>
template <class Timg_arg>
void SubaperturesData<Timg>::copyTo(SubaperturesData<Timg_arg>& _subapertures) const {


	copyPropertiesTo(_subapertures);

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			if (ocv::is_valid((*this)(u, v))) {
				(*this)(u, v).copyTo(_subapertures(u, v));
			} else {
				_subapertures(u, v).release();
			}
		}
	}

}

template <class Timg>
template <class Timg_arg>
void SubaperturesData<Timg>::copyPropertiesTo(SubaperturesData<Timg_arg>& _subapertures) const {

	_subapertures.set_name(this->name);
	_subapertures.set_baseline(baseline);
	_subapertures.set_uv_inverted(l_invert_uv);
	_subapertures.set_center_coordinates(center_coordinates);

	_subapertures.resize(get_Nu(), get_Nv());
}

template <class Timg>
unsigned int SubaperturesData<Timg>::get_Nu() const {

	return SubaperturesDataBase::get_Nu(subaperture_images);
}

template <class Timg>
unsigned int SubaperturesData<Timg>::get_Nv() const {

	return SubaperturesDataBase::get_Nv(subaperture_images);

}

template <class Timg>
cv::Size SubaperturesData<Timg>::size() const {

	cv::Size size(get_Nu(), get_Nv());
	return size;
}


template <class Timg>
const Fpair SubaperturesData<Timg>::get_center_coordinates() const {

	Fpair output;

	if (this->center_coordinates == SubaperturesLoader::center_coordinates_default()) {
		if (!this->empty()) {
			output.first = (float)(float(get_Nu() - 1) / 2.);
			output.second = (float)(float(get_Nv() - 1) / 2.);
		} else {
			output.first = 0.;
			output.second = 0.;
		}
	} else {
		output = this->center_coordinates;
	}

	return output;
}

template <class Timg>
const UVindices SubaperturesData<Timg>::get_center_indices() const {

	Fpair center_float = get_center_coordinates();
	Ipair center((int)std::round(center_float.first), (int)std::round(center_float.second));

	if (center.first < 0) {
		center.first = 0;
	}
	if (center.first >= (int)get_Nu()) {
		center.first = (int)get_Nu() - 1;
	}

	if (center.second < 0) {
		center.second = 0;
	}
	if (center.second >= (int)get_Nv()) {
		center.second = (int)get_Nv() - 1;
	}

	UVindices indices;
	indices.first = (unsigned int)center.first;
	indices.second = (unsigned int)center.second;

	return indices;
}

template <class Timg>
void SubaperturesData<Timg>::resize(const unsigned int _Nu, const unsigned int _Nv, const cv::Size& _image_size) {

	resize(_Nu, _Nv);
	resize_images(_image_size);

}

template <class Timg>
void SubaperturesData<Timg>::resize(const unsigned int _Nu, const unsigned int _Nv) {

	SubaperturesDataBase::resize(subaperture_images, _Nu, _Nv);
}

template <class Timg>
void SubaperturesData<Timg>::resize(const UVindices& _Nuv, const cv::Size& _image_size) {

	resize(_Nuv.first, _Nuv.second, _image_size);
}

template <class Timg>
void SubaperturesData<Timg>::resize(const cv::Size& _subapertures_size, const cv::Size& _image_size) {

	resize(_subapertures_size.width, _subapertures_size.height, _image_size);
}

template <class Timg>
void SubaperturesData<Timg>::resize_images(const cv::Size& _image_size) {

	for (unsigned int u = 0; u < get_Nu(); u++) {
		for (unsigned int v = 0; v < get_Nv(); v++) {
			(*this)(u, v).create(_image_size);
		}
	}

}

template <class Timg>
void SubaperturesData<Timg>::resize(const SubaperturesData<Timg>& _subapertures) {

	resize(_subapertures.get_Nu(), _subapertures.get_Nv(), _subapertures.get_image_size());
}

template <class Timg>
template <class Timg2>
void SubaperturesData<Timg>::resize(const SubaperturesData<Timg2>& _subapertures) {

	resize(_subapertures.get_Nu(), _subapertures.get_Nv(), _subapertures.get_image_size());
}

template <class Timg>
const Timg& SubaperturesData<Timg>::operator() (const unsigned int u, const unsigned int v) const {

	return subaperture_images[u][v];
}

template <class Timg>
Timg& SubaperturesData<Timg>::operator() (const unsigned int u, const unsigned int v) {

	return subaperture_images[u][v];
}

template <class Timg>
const Timg& SubaperturesData<Timg>::operator[] (const UVindices& _indices) const {

	return (*this)(_indices.first, _indices.second);
}

template <class Timg>
Timg& SubaperturesData<Timg>::operator[] (const UVindices& _indices) {

	return (*this)(_indices.first, _indices.second);
}

template <class Timg>
bool SubaperturesData<Timg>::check_uv_indices(const UVindices& _uv_indices) const {

	return _uv_indices.first < get_Nu() && _uv_indices.second < get_Nv();

}

template <class Timg>
Timg SubaperturesData<Timg>::get_center_image() const {

	unsigned int u_center;
	u_center = (get_Nu() - 1) / 2;

	unsigned int v_center;
	v_center = (get_Nv() - 1) / 2;

	if (get_Nu() > 0 && get_Nv() > 0) {
		return (*this)(u_center, v_center);
	} else {
		return Timg();
	}

}


template <class Timg>
const Timg& SubaperturesData<Timg>::get_image_original_coordinates(const unsigned int u, const unsigned int v) const {

	return const_cast<SubaperturesData<Timg>*>(this)->get_image_original_coordinates(u, v);

}

template <class Timg>
Timg& SubaperturesData<Timg>::get_image_original_coordinates(const unsigned int u, const unsigned int v) {

	UVindices uv = get_original_coordinates(UVindices(u, v));
	return (*this)[uv];
}

template <class Timg>
UVindices SubaperturesData<Timg>::get_original_coordinates(const UVindices& _uv) const {

	UVindices uv = _uv;
	UVindices uv_original;

	unsigned int Nu = get_Nu();
	unsigned int Nv = get_Nv();

	Fpair baseline = this->baseline;

	if (is_inverted()) {
		std::swap(uv.first, uv.second);
		std::swap(Nu, Nv);
		std::swap(baseline.first, baseline.second);
	}

	if (this->baseline.first < 0) {
		uv_original.first = Nu - 1 - uv.first;
	} else {
		uv_original.first = uv.first;
	}

	if (this->baseline.second < 0) {
		uv_original.second = Nv - 1 - uv.second;
	} else {
		uv_original.second = uv.second;
	}

	return uv_original;

}

template <class Timg>
cv::Size SubaperturesData<Timg>::get_image_size() const {

	cv::Size size(0, 0);

	bool l_valid_image = false;
	for (unsigned int u = 0; u < get_Nu() && !l_valid_image; u++) {
		for (unsigned int v = 0; v < get_Nv() && !l_valid_image; v++) {
			size = (*this)(u, v).size();
			l_valid_image = (size != cv::Size(0, 0));
		}
	}

	return size;
}

template <class Timg>
cv::Size SubaperturesData<Timg>::get_size_spatial() const {

	return get_image_size();

}

template <class Timg>
UVindices SubaperturesData<Timg>::get_size_angular() const {

	UVindices Nuv;
	cv::Size outer_size = get_outer_size();
	Nuv.first = outer_size.width;
	Nuv.second = outer_size.height;
	return Nuv;

}

template <class Timg>
cv::Size SubaperturesData<Timg>::get_inner_size() const {

	return get_image_size();
}

template <class Timg>
cv::Size SubaperturesData<Timg>::get_outer_size() const {

	return size();
}

template <class Timg>
bool SubaperturesData<Timg>::is_inverted() const {

	return l_invert_uv;
}

template <class Timg>
std::pair<bool, bool> SubaperturesData<Timg>::get_full_image_transposes() const {

	return std::pair<bool, bool>(false, false);
}

template <class Timg>
void SubaperturesData<Timg>::crop_spatially(const Leupribo<unsigned int>& _Ncrop_pixels) {

	Timg image;
	for (unsigned int u = 0; u < this->get_Nu(); u++) {
		for (unsigned int v = 0; v < this->get_Nv(); v++) {

			if (ocv::crop((*this)(u, v), image, _Ncrop_pixels)) {
				if (ocv::is_valid(image)) {
					image.copyTo((*this)(u, v));
				}
			}
			
		}
	}

}

template <class Timg>
void SubaperturesData<Timg>::crop_angular(const Leupribo<unsigned int>& _Ncrop_angular) {

	Leupribo<int> cropping_angular;
	for (unsigned int i = 0; i < 4; i++) {
		cropping_angular[i] = -(int)_Ncrop_angular[i];
	}
	angular_resize(cropping_angular);

}

template <class Timg>
void SubaperturesData<Timg>::crop_angular_spatially(const Leupribo<unsigned int>& _Ncrop_angular, const Leupribo<unsigned int>& _Ncrop_spatial) {

	if (_Ncrop_angular != Leupribo<unsigned int>({ {0, 0, 0, 0} })) {
		crop_angular(_Ncrop_angular);
	}
	if (_Ncrop_spatial != Leupribo<unsigned int>({ {0, 0, 0, 0} })) {
		crop_spatially(_Ncrop_spatial);
	}

}

template <class Timg>
void SubaperturesData<Timg>::rescale_spatially(const Fpair& _image_scale) {

	/*! If scaling is valid.*/
	if (_image_scale.first > 0. && _image_scale.second > 0) {

		if (_image_scale.first != 1. || _image_scale.second != 1.) {

			for (unsigned int u = 0; u < this->get_Nu(); u++) {
				for (unsigned int v = 0; v < this->get_Nv(); v++) {

					if (ocv::is_valid((*this)(u, v))) {
						cv::resize((*this)(u, v), (*this)(u, v), cv::Size(0, 0), _image_scale.first, _image_scale.second);

					}

				}
			}

		}

	}

	/*! Rescaling images makes baseline change.*/
	set_baseline_ratio(get_baseline_ratio()*_image_scale.second / _image_scale.first);

}

template <class Timg>
void SubaperturesData<Timg>::apply_transforms(const SubaperturesDataBase::DimensionsTransforms& _transforms) {

	for (unsigned int t = 0; t < _transforms.get_transforms().size(); t++) {

		if (_transforms.get_transforms()[t].get_type() == SubaperturesDataBase::DimensionsTransforms::Transform::TransposeAngular) {
			transpose_angular();
		} else if (_transforms.get_transforms()[t].get_type() == SubaperturesDataBase::DimensionsTransforms::Transform::FlipAngularHorizontal) {
			flip_angular(ocv::horizontal);
		} else if (_transforms.get_transforms()[t].get_type() == SubaperturesDataBase::DimensionsTransforms::Transform::FlipAngularVertical) {
			flip_angular(ocv::vertical);
		} else if (_transforms.get_transforms()[t].get_type() == SubaperturesDataBase::DimensionsTransforms::Transform::RemoveAngular) {
			SubaperturesData<Timg> subapertures_coarsened;
			if (get_coarsened(subapertures_coarsened, _transforms.get_transforms()[t].get_Nviews_removed())) {
				subapertures_coarsened.copyTo(*this);
			}
		} else if (_transforms.get_transforms()[t].get_type() == SubaperturesDataBase::DimensionsTransforms::Transform::CropAngular) {
			crop_angular(_transforms.get_transforms()[t].get_Ncrop_angular());
		} else if (_transforms.get_transforms()[t].get_type() == SubaperturesDataBase::DimensionsTransforms::Transform::CropSpatially) {
			crop_spatially(_transforms.get_transforms()[t].get_Ncrop_spatial());
		} else if (_transforms.get_transforms()[t].get_type() == SubaperturesDataBase::DimensionsTransforms::Transform::RescaleSpatially) {
			rescale_spatially(_transforms.get_transforms()[t].get_image_scale());
		} else if (_transforms.get_transforms()[t].get_type() == SubaperturesDataBase::DimensionsTransforms::Transform::Baseline) {
			set_baseline(_transforms.get_transforms()[t].get_baseline());
		} else if (_transforms.get_transforms()[t].get_type() == SubaperturesDataBase::DimensionsTransforms::Transform::Null) {
			std::cout << "SubaperturesData<Timg>::apply_transforms : Wrong transform ID" << std::endl;
		}

	}

}

template <class Timg>
const typename SubaperturesData<Timg>::TsubApStrip SubaperturesData<Timg>::get_strip(unsigned int _index, const ocv::Orientation _orientation) const {

	return const_cast<SubaperturesData*>(this)->get_strip(_index, _orientation);
}

template <class Timg>
typename SubaperturesData<Timg>::TsubApStrip SubaperturesData<Timg>::get_strip(unsigned int _index, const ocv::Orientation _orientation) {

	TsubApStrip subapertures_strip;

	unsigned int u=0, v=0;
	unsigned int j_max;
	if (_orientation == ocv::horizontal) {

		subapertures_strip.baseline = baseline.first;

		j_max = get_Nu();
		if (_index < get_Nv()) {
			v = _index;
		} else {
			std::cout << "SubaperturesData<Timg>::get_strip : _index = " << _index << " is out of range of get_Nv() = " << get_Nv() << std::endl;
			DEBUG_BREAK;
		}
	} else if (_orientation == ocv::vertical) {

		subapertures_strip.baseline = baseline.second;

		j_max = get_Nv();
		if (_index < get_Nu()) {
			u = _index;
		} else {
			std::cout << "SubaperturesData<Timg>::get_strip : _index = " << _index << " is out of range of get_Nu() = " << get_Nu() << std::endl;
			DEBUG_BREAK;
		}
	} else {
		DEBUG_BREAK;
		j_max = 0;
	}


	for (unsigned int j = 0; j<j_max; j++) {

		if (_orientation == ocv::horizontal) {
			u = j;
		} else if (_orientation == ocv::vertical) {
			v = j;
		} else {
			DEBUG_BREAK;
		}

		/*! Manage sparsity of light field*/
		if (ocv::is_valid((*this)(u, v))) {
			subapertures_strip.images.push_back((*this)(u, v));
		} else {
			Tvec void_value = Tvec::all(ocv::MaxRange<Tvalue>::get());
			Timg void_image(get_size_spatial());
			void_image = void_value;
			subapertures_strip.images.push_back(void_image);
		}
		
	}

	return subapertures_strip;

}

template <class Timg>
void SubaperturesData<Timg>::set_strip(const TsubApStrip& _subapertures_strip, unsigned int _index, const ocv::Orientation _orientation) {

	unsigned int u=0, v=0;
	unsigned int j_max;
	if (_orientation == ocv::horizontal) {
		j_max = get_Nu();
		v = _index;
	} else if (_orientation == ocv::vertical) {
		j_max = get_Nv();
		u = _index;
	} else {
		DEBUG_BREAK;
		j_max = 0;
	}

	if (_subapertures_strip.images.size() == j_max) {
		for (unsigned int j = 0; j<j_max; j++) {

			if (_orientation == ocv::horizontal) {
				u = j;
			} else if (_orientation == ocv::vertical) {
				v = j;
			} else {
				DEBUG_BREAK;
			}

			_subapertures_strip.images[j].copyTo((*this)(u, v));
		}
	} else {
		std::cout << "_subaperture_strip has size : " << _subapertures_strip.images.size() << ", but should be : " << j_max << std::endl;
	}

}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::copyFrom(const Timg& _image) {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			_image.copyTo((*this)(u, v));
		}
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator+=(const SubaperturesData<Timg>& _subapertures) {

	if (size() == _subapertures.size()) {
		for (unsigned int u = 0; u<get_Nu(); u++) {
			for (unsigned int v = 0; v<get_Nv(); v++) {
				cv::add((*this)(u, v), _subapertures(u, v), (*this)(u, v));
			}
		}
	} else {
		std::cout << "Wrong subapertures sizes" << std::endl;
		DEBUG_BREAK;
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator-=(const SubaperturesData<Timg>& _subapertures) {

	if (size() == _subapertures.size()) {
		for (unsigned int u = 0; u<get_Nu(); u++) {
			for (unsigned int v = 0; v<get_Nv(); v++) {
				cv::subtract((*this)(u, v), _subapertures(u, v), (*this)(u, v));
			}
		}
	} else {
		std::cout << "Wrong subapertures sizes" << std::endl;
		DEBUG_BREAK;
	}

	return *this;

}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator*=(const SubaperturesData<Timg>& _subapertures) {

	if (size() == _subapertures.size()) {
		for (unsigned int u = 0; u<get_Nu(); u++) {
			for (unsigned int v = 0; v<get_Nv(); v++) {
				cv::multiply((*this)(u, v), _subapertures(u, v), (*this)(u, v));
			}
		}
	} else {

		std::cout << "SubaperturesData<Timg>::operator*= : this->size() = " << size() << ", _subapertures.size() = " << _subapertures.size() << std::endl;
	}

	return *this;
}

template <class Timg>
template <class Timg_input>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator*=(const SubaperturesData<Timg_input>& _subapertures) {

	if (size() == _subapertures.size()) {
		for (unsigned int u = 0; u<get_Nu(); u++) {
			for (unsigned int v = 0; v<get_Nv(); v++) {
				typename Timg_input::const_iterator it_input = _subapertures(u,v).begin();
				for (typename Timg::iterator it = (*this)(u, v).begin(); it != (*this)(u, v).end(); ++it, ++it_input) {
					ocv::mul(*it, *it_input);
				}
			}
		}
	} else {

		std::cout << "SubaperturesData<Timg>::operator*= : this->size() = " << size() << ", _subapertures.size() = " << _subapertures.size() << std::endl;
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator/=(const SubaperturesData<Timg>& _subapertures) {

	if (size() == _subapertures.size()) {
		for (unsigned int u = 0; u<get_Nu(); u++) {
			for (unsigned int v = 0; v<get_Nv(); v++) {
				cv::divide((*this)(u, v), _subapertures(u, v), (*this)(u, v));
			}
		}
	} else {

		std::cout << "SubaperturesData<Timg>::operator/= : this->size() = " << size() << ", _subapertures.size() = " << _subapertures.size() << std::endl;
	}

	return *this;
}

template <class Timg>
template <class Timg_input>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator=(const SubaperturesData<Timg_input>& _subapertures) {

	_subapertures.copyPropertiesTo(*this);

	for (unsigned int u = 0; u < get_Nu(); u++) {
		for (unsigned int v = 0; v < get_Nv(); v++) {
			ocv::convertTo(_subapertures(u, v), (*this)(u, v), true);
		}
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator=(const Timg& _image) {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			_image.copyTo((*this)(u, v));
		}
	}

	return *this;

}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator=(const Tvec& _value) {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			(*this)(u, v) = _value;
		}
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator+=(const Tvec& _value) {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			(*this)(u, v) += _value;
		}
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator*=(const Tvalue& _value) {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			(*this)(u, v) *= _value;
		}
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::operator/=(const Tvalue& _value) {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			(*this)(u, v) /= _value;
		}
	}

	return *this;
}



template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::inverse() {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			cv::divide(1., (*this)(u, v), (*this)(u, v));
		}
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::sqrt() {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			cv::sqrt((*this)(u, v), (*this)(u, v));
		}
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::pow(const Tvalue& _power) {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			cv::pow((*this)(u, v), _power,  (*this)(u, v));
		}
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::log() {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			cv::log((*this)(u, v), (*this)(u, v));
		}
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::log(const Tvalue& _base) {

	log();
	*this /= Tvalue(std::log(_base));

	return *this;
}


template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::abs() {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			(*this)(u, v) = cv::abs((*this)(u, v));
		}
	}

	return *this;
}

template <class Timg>
SubaperturesData<Timg>& SubaperturesData<Timg>::exp() {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			cv::exp((*this)(u, v), (*this)(u, v));
		}
	}

	return *this;
}


template <class Timg>
void SubaperturesData<Timg>::get_sum_image(Timg& _result) const {

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {

			if (u == 0 && v == 0) {
				(*this)(u, v).copyTo(_result);
			} else {
				cv::add(_result, (*this)(u, v), _result);
			}

		}
	}

}

template <class Timg>
void SubaperturesData<Timg>::get_mean_image(Timg& _result) const {

	get_sum_image(_result);

	_result /= get_Nu() * get_Nv();
}

template <class Timg>
void SubaperturesData<Timg>::get_mean_image_weighted(Timg& _result, const SubaperturesData<ocv::Timg1>& _weights) const {

	Timg buffer;
	ocv::Timg1 buffer2;
	get_mean_image_weighted(_result, _weights, buffer, buffer2);
}

template <class Timg>
void SubaperturesData<Timg>::get_mean_image_weighted(Timg& _result, const SubaperturesData<ocv::Timg1>& _weights, Timg& _buffer, ocv::Timg1& _buffer2) const {

	if (size() == _weights.size() && get_image_size() == _weights.get_image_size()) {

		for (unsigned int u = 0; u<get_Nu(); u++) {
			for (unsigned int v = 0; v<get_Nv(); v++) {

				(*this)(u, v).copyTo(_buffer);
				cv::multiply(_buffer, _weights(u, v), _buffer);

				if (u == 0 && v == 0) {
					_buffer.copyTo(_result);
				} else {
					cv::add(_result, _buffer, _result);
				}

			}
		}

		_weights.get_sum_image(_buffer2);
		cv::divide(_result, _buffer2, _result);
	} else {

		std::cout << "SubaperturesData<Timg>::get_mean_image_weighted : Wrong weights dimensions" << std::endl;
	}
	
}

template <class Timg>
void SubaperturesData<Timg>::get_highest_image_weighted(Timg& _result, ocv::Timg1& _weights_highest, const SubaperturesData<ocv::Timg1>& _weights) const {

	if (this->get_outer_size() == _weights.get_outer_size() && this->get_inner_size() == _weights.get_inner_size()) {

		_result.create(this->get_image_size());
		_weights_highest.create(this->get_image_size());
		_weights_highest = std::numeric_limits<ocv::Tvalue>::lowest();

		for (unsigned int u = 0; u < this->get_Nu(); u++) {
			for (unsigned int v = 0; v < this->get_Nv(); v++) {

				typename Timg::iterator it_val_result = _result.begin();
				ocv::Timg1::iterator it_w_result = _weights_highest.begin();
				typename Timg::const_iterator it_val = (*this)(u, v).begin();
				ocv::Timg1::const_iterator it_w = _weights(u, v).begin();

				for (; it_val != (*this)(u, v).end(); ++it_val, ++it_w, ++it_val_result, ++it_w_result) {

					if ((*it_w)[0] > (*it_w_result)[0]) {

						*it_val_result = *it_val;
						*it_w_result = *it_w;

					}

				}

			}
		}

	} else {
		std::cout << " SubaperturesData<Timg>::get_highest_image_weighted : _values and _weights have different dimensions" << std::endl;
	}

}

template <class Timg>
void SubaperturesData<Timg>::threshold(const Tvalue& _threshold_value, const Tvec& _replacement_value, bool _l_inferior) {

	for (unsigned int u = 0; u < this->get_Nu(); u++) {
		for (unsigned int v = 0; v < this->get_Nv(); v++) {

			int dim;

			for (typename Timg::iterator it = (*this)(u, v).begin(); it != (*this)(u, v).end(); ++it) {

				for (dim = 0; dim < Tvec::channels; dim++) {
					if (_l_inferior) {
						if ((*it)[dim] < _threshold_value) {
							(*it)[dim] = _replacement_value[dim];
						}
					} else {
						if ((*it)[dim] > _threshold_value) {
							(*it)[dim] = _replacement_value[dim];
						}
					}
				}

			}

		}
	}

}

template <class Timg>
void SubaperturesData<Timg>::threshold_mask(const Tvalue& _threshold_value, SubaperturesData<ocv::Tmask>& _result) const {

	copyPropertiesTo(_result);

	ocv::Tmask::iterator it_result;
	typename Timg::const_iterator it;

	for (unsigned int u = 0; u < this->get_Nu(); u++) {
		for (unsigned int v = 0; v < this->get_Nv(); v++) {

			_result(u, v).create(this->get_image_size());

			it_result = _result(u, v).begin();

			for (it = (*this)(u, v).begin(); it != (*this)(u, v).end(); ++it, ++it_result) {

				if ((*it)[0] >= _threshold_value) {
					(*it_result)[0] = ocv::mask_value;
				} else {
					(*it_result)[0] = 0;
				}

			}

		}
	}

}

template <class Timg>
void SubaperturesData<Timg>::refine(const unsigned int& _n_images) {

	SubaperturesData<Timg> subapertures;
	get_refined(subapertures, _n_images);
	subapertures.copyTo(*this);
}

template <class Timg>
void SubaperturesData<Timg>::refine(const Upair& _n_images) {

	SubaperturesData<Timg> subapertures;
	get_refined(subapertures, _n_images);
	subapertures.copyTo(*this);
}

template <class Timg>
void SubaperturesData<Timg>::get_refined(SubaperturesData<Timg>& _subapertures, const unsigned int& _n_images) const {

	Upair n_images(_n_images, _n_images);
	get_refined(_subapertures, n_images);
}

template <class Timg>
void SubaperturesData<Timg>::get_refined(SubaperturesData<Timg>& _subapertures, const Upair& _n_images) const {

	copyPropertiesTo(_subapertures);

	unsigned int Nu, Nv;

	Nu = get_Nu() + (get_Nu() - 1)*_n_images.first;
	Nv = get_Nv() + (get_Nv() - 1)*_n_images.second;

	_subapertures.resize(Nu, Nv, get_image_size());
	_subapertures = Tvalue(0.);

	for (unsigned u = 0; u < get_Nu(); u++) {
		for (unsigned v = 0; v < get_Nv(); v++) {

			(*this)(u, v).copyTo(_subapertures(u*(_n_images.first+1), v*(_n_images.second+1)));

		}
	}

}

template <class Timg>
bool SubaperturesData<Timg>::get_coarsened(SubaperturesData<Timg>& _subapertures, const Upair& _n_images) const {

	bool l_ok = true;

	unsigned int Nu, Nv;

	if ((get_Nu()+_n_images.first)%(_n_images.first+1) == 0) {
		Nu = (get_Nu() + _n_images.first) / (_n_images.first + 1);
	} else {
		l_ok = false;
		Nu = 0;
		std::cout << "Angular coarsening doesn't apply for angular size get_Nu() = " << get_Nu() << ", with number of images to remove equal to " << _n_images.first << std::endl;
	}

	if ((get_Nv() + _n_images.second) % (_n_images.second + 1) == 0) {
		Nv = (get_Nv() + _n_images.second) / (_n_images.second + 1);
	} else {
		l_ok = false;
		Nv = 0;
		std::cout << "Angular coarsening doesn't apply for angular size get_Nv() = " << get_Nv() << ", with number of images to remove equal to " << _n_images.second << std::endl;
	}

	if (l_ok) {

		copyPropertiesTo(_subapertures);
		/*! Update baseline when views are removed.*/
		Fpair baseline = this->get_baseline();
		baseline.first *= (_n_images.first + 1);
		baseline.second *= (_n_images.second + 1);
		baseline.second /= baseline.first;
		baseline.first = 1.;
		_subapertures.set_baseline(baseline);
		/////////////////////////////////////////////

		_subapertures.resize(Nu, Nv, get_image_size());

		for (unsigned u = 0; u < Nu; u++) {
			for (unsigned v = 0; v < Nv; v++) {

				(*this)(u*(_n_images.first + 1), v*(_n_images.second + 1)).copyTo(_subapertures(u, v));

			}
		}

	}

	return l_ok;
}

template <class Timg>
void SubaperturesData<Timg>::angular_resize(const Leupribo<unsigned int>& _offsets) {

	SubaperturesData<Timg> subapertures;
	get_angular_resized(subapertures, _offsets);
	subapertures.copyTo(*this);
}

template <class Timg>
void SubaperturesData<Timg>::angular_resize(const Leupribo<int>& _offsets) {

	SubaperturesData<Timg> subapertures;
	get_angular_resized(subapertures, _offsets);
	subapertures.copyTo(*this);
}

template <class Timg>
void SubaperturesData<Timg>::get_angular_resized(SubaperturesData<Timg>& _subapertures, const Leupribo<unsigned int>& _offsets) const {

	Leupribo<int> offsets;
	for (unsigned int i = 0; i < _offsets.size(); i++) {
		offsets[i] = _offsets[i];
	}

	get_angular_resized(_subapertures, offsets);
}

template <class Timg>
void SubaperturesData<Timg>::get_angular_resized(SubaperturesData<Timg>& _subapertures, const Leupribo<int>& _offsets) const {

	copyPropertiesTo(_subapertures);

	int Nu = get_Nu();
	int du = _offsets[0] + _offsets[2];

	if (Nu > -du) {
		Nu += du;
	} else {
		Nu = 0;
	}

	int Nv = get_Nv();
	int dv = _offsets[1] + _offsets[3];
	if (Nv > -dv) {
		Nv += dv;
	} else {
		Nv = 0;
	}

	if (Nu != 0 && Nv != 0) {

		_subapertures.resize(Nu, Nv, get_image_size());
		_subapertures = (Tvalue)0.;

		int u2, v2;
		for (unsigned int u = 0; u < get_Nu(); u++) {
			for (unsigned int v = 0; v < get_Nv(); v++) {

				u2 = (int)u + _offsets[0];
				v2 = (int)v + _offsets[1];

				if (u2 >= 0 && v2 >= 0 && u2 < Nu && v2 < Nv) {
					(*this)(u, v).copyTo(_subapertures(u2, v2));
				}

			}
		}

	}

}

template <class Timg>
void SubaperturesData<Timg>::flip_angular(const ocv::Orientation& _orientation) {

	Timg image_swap;
	unsigned int N;
	if (_orientation == ocv::horizontal) {

		if (get_Nu() % 2 == 1) {
			N = (get_Nu() - 1) / 2;
		} else {
			N = get_Nu()/ 2;
		}
		
		for (unsigned int u = 0; u < N; u++) {
			for (unsigned int v = 0; v < get_Nv(); v++) {

				image_swap = (*this)(u, v);
				(*this)(u, v) = (*this)(get_Nu() - u - 1, v);
				(*this)(get_Nu() - u - 1, v) = image_swap;

			}
		}

	} else if (_orientation == ocv::vertical) {

		if (get_Nv() % 2 == 1) {
			N = (get_Nv() - 1) / 2;
		} else {
			N = get_Nv() / 2;
		}

		for (unsigned int v = 0; v < N; v++) {
			for (unsigned int u = 0; u < get_Nu(); u++) {

				image_swap = (*this)(u, v);
				(*this)(u, v) = (*this)(u, get_Nv() - v - 1);
				(*this)(u, get_Nv() - v - 1) = image_swap;

			}
		}

	}

}

template <class Timg>
void SubaperturesData<Timg>::transpose_angular() {

	unsigned int Nu = get_Nu();
	unsigned int Nv = get_Nv();

	unsigned int N = std::max(get_Nu(), get_Nv());
	resize(N, N);

	Timg image_swap;
	for (unsigned int u = 0; u < N; u++) {
		for (unsigned int v = 0; v < u; v++) {

			image_swap = (*this)(u, v);
			(*this)(u, v) = (*this)(v, u);
			(*this)(v, u) = image_swap;

		}
	}

	resize(Nv, Nu);

	l_invert_uv = !l_invert_uv;
	std::swap(center_coordinates.first, center_coordinates.second);
	std::swap(baseline.first, baseline.second);
}

template <class Timg>
void SubaperturesData<Timg>::reduce_channels(SubaperturesData< cv::Mat_< cv::Vec<Tvalue, 1> > >& _subapertures, const int _rtype) const {

	copyPropertiesTo(_subapertures);

	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			_subapertures(u, v) = ocv::reduce_channels((*this)(u, v), _rtype);
		}
	}

}

template <class Timg>
void SubaperturesData<Timg>::set_channel(int _nchannel, Tvalue _value) {

	if (_nchannel < Timg::value_type::channels) {

		for (unsigned int u = 0; u < get_Nu(); u++) {
			for (unsigned int v = 0; v < get_Nv(); v++) {

				for (typename Timg::iterator it = (*this)(u, v).begin(); it != (*this)(u, v).end(); ++it) {
					(*it)[_nchannel] = _value;
				}

			}
		}

	} else {

		std::cout << "SubaperturesData<Timg>::set_channel : _nchannel = " << _nchannel << ", Timg::value_type::channels = " << Timg::value_type::channels << std::endl;

	}

}

template <class Timg>
void SubaperturesData<Timg>::set_channel(int _nchannel, const SubaperturesData< cv::Mat_< cv::Vec<Tvalue, 1> > >& _subapertures1C) {

	if (_nchannel < Timg::value_type::channels) {

		if (this->has_same_dimensions(_subapertures1C)) {

			for (unsigned int u = 0; u < get_Nu(); u++) {
				for (unsigned int v = 0; v < get_Nv(); v++) {

					typename cv::Mat_< cv::Vec<Tvalue, 1> >::const_iterator it_1C = _subapertures1C(u, v).begin();
					for (typename Timg::iterator it = (*this)(u, v).begin(); it != (*this)(u, v).end(); ++it, ++it_1C) {
						(*it)[_nchannel] = (*it_1C)[0];
					}

				}
			}

		} else {
			std::cout << "SubaperturesData<Timg>::set_channel : Wrong subapertures dimension" << std::endl;
		}

	} else {
		std::cout << "SubaperturesData<Timg>::set_channel : _nchannel = " << _nchannel << ", Timg::value_type::channels = " << Timg::value_type::channels << std::endl;
	}

}


template <class Timg>
bool SubaperturesData<Timg>::has_nan() const {

	bool l_has_nan = false;
	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			if (!l_has_nan) {
				l_has_nan = ocv::has_nan((*this)(u, v), "test");
			}
		}
	}

	return l_has_nan;
}

template <class Timg>
void SubaperturesData<Timg>::rescale_min_max() {

	Tvalue min;
	Tvalue max;
	ocv::minMaxLoc(*this, &min, &max);
	ocv::Range<Tvalue> range_output = ocv::get_minmax_range(Timg());
	ocv::Tscaling scaling = ocv::get_scaling(ocv::Range<Tvalue>(min, max), range_output);

	for (unsigned int u = 0; u < get_Nu(); u++) {
		for (unsigned int v = 0; v < get_Nv(); v++) {

			ocv::rescale((*this)(u, v), scaling);

		}
	}

}

template <class Timg>
std::vector<typename SubaperturesData<Timg>::Tvalue> SubaperturesData<Timg>::calcHist(cv::Mat& _histogram, const unsigned int _Nvalues, const ocv::Range<Tvalue> _range) const {

	//https://stackoverflow.com/questions/42902527/how-to-set-the-parameters-of-opencv3-calchist-using-vectors

	ocv::Range<Tvalue> range;
	if (_range == ocv::Range<Tvalue>(Tvalue(0),Tvalue(0))) {
		range = ocv::get_minmax_range(*this);
	} else {
		range = _range;
	}

	cv::Mat histogram_image;
	for (unsigned int u = 0; u<get_Nu(); u++) {
		for (unsigned int v = 0; v<get_Nv(); v++) {
			ocv::calcHist((*this)(u, v), histogram_image, 100, range);
			if (u == 0 && v ==0) {
				histogram_image.copyTo(_histogram);
			} else {
				_histogram += histogram_image;
			}
		}
	}

	_histogram /= get_Nu() * get_Nv();

	std::vector<Tvalue> values(_Nvalues);
	for (unsigned int i = 0; i < _Nvalues; i++) {
		values[i] = range.first + Tvalue((range.second - range.first)*i / float(_Nvalues - 1));
	}

	return values;

}

template <class Timg>
std::vector<typename SubaperturesData<Timg>::Tvalue> SubaperturesData<Timg>::calcHist_minmax(cv::Mat& _histogram, const unsigned int _Nvalues) const {

	Tvalue min, max;
	ocv::minMaxLoc(*this, &min, &max);
	ocv::Range<Tvalue> range(min, max);
	return calcHist(_histogram, _Nvalues, range);
}

template <class Timg>
std::vector<typename SubaperturesData<Timg>::Tvec> SubaperturesData<Timg>::get_medians(const unsigned int _Nmedians, const unsigned int _Nvalues) const {

	std::vector<Tvec> medians(_Nmedians);
	cv::Mat histogram;
	std::vector<Tvalue> values = calcHist_minmax(histogram, _Nvalues);

	Tvec sum = Tvec::all(0);
	cv::Vec<int, Tvec::channels> n_median = cv::Vec<int, Tvec::channels>::all(0);
	unsigned int v = 0;
	for (cv::MatConstIterator_<Tvec> it = histogram.begin<Tvec>(); it != histogram.end<Tvec>(); ++it) {
		for (unsigned int d=0;d<Tvec::channels;d++) {
			sum[d] += (*it)[d];
			while (sum[d] >= 1./(_Nmedians+1)) {
				sum[d] -= Tvalue(1. / (_Nmedians + 1));
				medians[n_median[d]][d] = values[v];
				n_median[d]++;
			}
		}
		
		v++;
	}

	return medians;
}

template <class Timg>
bool SubaperturesData<Timg>::is_in_uv_range(const unsigned int u, const unsigned int v) const {

	std::array<unsigned int, 4> subapertures_offsets= {{ 0, 0, 0, 0 }};
	return is_in_uv_range(u, v, subapertures_offsets);

}

template <class Timg>
bool SubaperturesData<Timg>::is_in_uv_range(const unsigned int u, const unsigned int v, const std::array<unsigned int, 4>& _subapertures_offsets) const {

	return SubaperturesDataBase::is_in_uv_range(u, v, get_Nu(), get_Nv(), _subapertures_offsets);

}

template <class Timg>
bool SubaperturesData<Timg>::load(const SubaperturesLoader& _loader) {

	SubaperturesDataBase::clear();

	bool l_loaded;

		if (_loader.is_directory()) {

			SubapertureBundle subapertures_bundle = get_subapertures_bundle(_loader);

			/*! If subapertures is valid.*/
			if (subapertures_bundle) {
				/*! Load*/
				/*! Use SubapertureBundle to read _images.*/
				load_SubapertureBundle(subapertures_bundle, _loader.get_parameters().subaperture_offsets, _loader.get_parameters().angular_modulo, _loader.get_parameters().image_scale);
				l_loaded = true;

			} else {

				l_loaded = false;
			}


		} else if (_loader.is_video()) {

			std::cout << "No video management for loading light field" << std::endl;
			DEBUG_BREAK;

		} else {

			std::cout << "Can't find a light field in this path : " << _loader.get_parameters().LF_path << std::endl;
			l_loaded = false;
		}


		if (l_loaded) {

			loader_assign(_loader);

			preprocessing(_loader.get_parameters());

			l_loaded = !this->empty();
		}

		return l_loaded;
}

template <class Timg>
bool SubaperturesData<Timg>::load(const SubaperturesLoader& _loader, const SubapertureBundle& _subapertures_bundle) {

	/*! If subapertures is valid.*/
	if (_subapertures_bundle) {

		SubaperturesDataBase::clear();

		/*! Load*/
		/*! Use SubapertureBundle to read _images.*/
		load_SubapertureBundle(_subapertures_bundle, _loader.get_parameters().subaperture_offsets, _loader.get_parameters().angular_modulo, _loader.get_parameters().image_scale);
		loader_assign(_loader);
		preprocessing(_loader.get_parameters());

		return true;

	} else {

		return false;

	}

}

template <class Timg>
void SubaperturesData<Timg>::loader_assign(const SubaperturesLoader& _loader) {

	this->set_name(_loader.get_LF_name());

	if (_loader.get_parameters().l_invert_uv) {
		transpose_angular();
	}

	baseline.first = std::fabs(_loader.get_parameters().uv_axis_coef.first);
	if (_loader.get_parameters().uv_axis_coef.first < 0.) {
		flip_angular(ocv::horizontal);
	}

	baseline.second = std::fabs(_loader.get_parameters().uv_axis_coef.second);
	if (_loader.get_parameters().uv_axis_coef.second < 0.) {
		flip_angular(ocv::vertical);
	}

	if (_loader.get_parameters().center_coordinates == SubaperturesLoader::center_coordinates_default()) {
		/*! If _cfg doesn't provide center, then a default value is set. Getting this value will compute the center as half of supapertures dimension.*/
		set_center_coordinates();
	} else {
		Fpair center_coordinates = _loader.get_parameters().center_coordinates;
		if (is_uv_inverted()) {
			std::swap(center_coordinates.first, center_coordinates.second);
		}
		set_center_coordinates(center_coordinates);
	}

}

template <class Timg>
SubapertureBundle SubaperturesData<Timg>::get_subapertures_bundle(const SubaperturesLoader& _loader) {

	SubapertureBundle subapertures_bundle;

	if (_loader.is_directory()) {

		if (!_loader.get_parameters().l_auto_read) {
			/*! If not autoread, configure SubapertureBundle with provided cfg file.*/
			subapertures_bundle.set_parameters(_loader.get_parameters().reading_parameters);
			subapertures_bundle.read_subapertures(_loader.get_parameters().LF_path, _loader.get_parameters().l_recursive, _loader.get_parameters().time_index, _loader.get_parameters().filter_strings);
		} else {
			/*! If autoread, explore possible bundle possibilities and select the most likely.*/
			/*! Calls directory parser to return a sub-aperture bundle.*/
			/*! First, set likeliness parameters to compare SubapertureBundle.*/
			SubapertureBundle::Nimages_auto() = _loader.get_parameters().Nimages_auto;
			SubapertureBundle::coef_std_Nimages_auto() = _loader.get_parameters().coef_std_Nimages_auto;
			SubapertureBundle::coef_std_ratio_auto() = _loader.get_parameters().coef_std_ratio_auto;
			subapertures_bundle = DirectoryParser::analyse(_loader.get_parameters().LF_path, _loader.get_parameters().l_recursive, _loader.get_parameters().time_index, _loader.get_parameters().filter_strings);
		}

		if (Misc::l_verbose_low) {
			std::cout << "subapertures bundle = " << subapertures_bundle << std::endl;
		}

	}

	return subapertures_bundle;

}

template <class Timg>
void SubaperturesData<Timg>::load_SubapertureBundle(const SubapertureBundle& _subapertures_bundle, const std::array<unsigned int, 4>& _subaperture_offsets, const Upair& _angular_modulo, const Fpair& _image_scale) {

	UVindices Nuv_bundle(_subapertures_bundle.Nu, _subapertures_bundle.Nv);
	std::pair<UVindices, bool> result = get_Nuv_transforms(Nuv_bundle, _subaperture_offsets, _angular_modulo);
	UVindices Nuv = result.first;
	bool l_valid_angular_modulo = result.second;//If result.first is valid, second member of result is angular modulo validity.
	unsigned int Nu = Nuv.first;
	unsigned int Nv = Nuv.second;

	resize(Nu, Nv);

	UVindices uv;
	std::vector<UVindices> all_uv_read;
	cv::Mat tmp_img;
	unsigned int i_subaperture = 0;
	if (Misc::l_verbose_high) std::cout << "Reading images" << std::endl;
	for (SubapertureBundle::Tmapping::const_iterator iterator = _subapertures_bundle.uv_images_mapping.begin(); iterator != _subapertures_bundle.uv_images_mapping.end(); ++iterator) {

		UVindices uv_read = get_uv_transforms(iterator->first, Nuv_bundle, _subaperture_offsets, _angular_modulo, l_valid_angular_modulo);

		if (uv_read != UVindices(-1, -1)) {

#ifdef ENABLE_LIB_OPENEXR
			if (Misc::get_file_extension(iterator->second) == ".exr") {
				OpenEXR::imread(iterator->second, subaperture_images[uv_read.first][uv_read.second]);
			} else
#endif
			{
				ocv::imread(iterator->second, subaperture_images[uv_read.first][uv_read.second]);
			}

			all_uv_read.push_back(uv_read);


			if (_image_scale.first > 0 && _image_scale.second > 0) {
				if (ocv::is_valid(subaperture_images[uv_read.first][uv_read.second])) {
					cv::resize(subaperture_images[uv_read.first][uv_read.second], subaperture_images[uv_read.first][uv_read.second], cv::Size(0, 0), _image_scale.first, _image_scale.second);
				}
			} else {
				std::cout << "Can't rescale subapertures using parameter : image_scale = " << _image_scale << std::endl;
			}

		}

		if (Misc::l_verbose_high) Misc::display_progression(i_subaperture, _subapertures_bundle.Nuv_images);
		i_subaperture++;

	}

	/*! Release memory of unread UVindices.*/
	for (unsigned int u = 0; u < Nu; u++) {
		for (unsigned int v = 0; v < Nv; v++) {

			/*! If uv indices are not among the read indices.*/
			if (std::find(all_uv_read.begin(), all_uv_read.end(), UVindices(u, v)) == all_uv_read.end()) {
				subaperture_images[u][v].release();
			}

		}
	}

}

template <class Timg>
void SubaperturesData<Timg>::preprocessing(const SubaperturesLoader::Parameters& _loader_parameters) {

	if (Misc::l_verbose_high) std::cout << "Preprocessing" << std::endl;

	crop_spatially(_loader_parameters.Ncrop_pixels);

	Timg image_display;
	for (unsigned int u = 0; u < this->get_Nu(); u++) {
		for (unsigned int v = 0; v < this->get_Nv(); v++) {

			if (_loader_parameters.l_display_subapertures) {
				std::string label = std::string("subaperture [") + Misc::to_string(u) + "," + Misc::to_string(v) + "]";
				/*! If timer is null.*/
				if (ocv::imshow_wait_time_animation == 0) {
					/*! Just show image.*/
					ocv::imshow((*this)(u, v), label);
				} else {
					/*! Copy image to add text label on it.*/
					(*this)(u, v).copyTo(image_display);
					ocv::putText(image_display, label);// , _misc_loader_parameters.font_scale, _misc_loader_parameters.font_color);
					ocv::imshow(image_display, "SubaperturesData");// , _misc_loader_parameters.animation_timer);
				}

			}

		}
	}

	bool l_normalize = false;
	if (l_normalize) {
		rescale_min_max();
	}

	if (_loader_parameters.l_histogram_matching) {

		cv::Mat_<cv::Vec3b> mat_center;
		if (ocv::is_valid(get_center_image())) {
			ocv::convertTo(get_center_image(), mat_center, true);
			for (unsigned int u = 0; u < get_Nu(); u++) {
				for (unsigned int v = 0; v < get_Nv(); v++) {

					if ((*this)(u, v).size() == get_center_image().size()) {
						const cv::Mat* mask = 0;
						cv::Mat_<cv::Vec3b> mat;
						std::pair<double, double> scaling;
						scaling = ocv::convertTo((*this)(u, v), mat, true);
						HistMatch::histMatchRGB(mat, mask, mat_center, mask);
						ocv::convertTo(mat, (*this)(u, v));
						ocv::rescale_inv((*this)(u, v), scaling);
					}
					
				}
			}
		}

	}

}



template <class Timg>
void SubaperturesData<Timg>::TsubApStrip::copyTo(TsubApStrip& _strip) const {

	_strip.baseline = baseline;

	_strip.images.resize(images.size());
	for (unsigned int i = 0; i < images.size(); i++) {
		images[i].copyTo(_strip.images[i]);
	}

}