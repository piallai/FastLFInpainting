/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "SubaperturesDataBase.h"
#include "SubaperturesLoader.h"

SubaperturesDataBase::SubaperturesDataBase() {

	clear();
	
}

SubaperturesDataBase::~SubaperturesDataBase() {

}

void SubaperturesDataBase::clear() {

	l_invert_uv = false;
	baseline = Fpair(1., 1.);
	set_center_coordinates();

}

bool SubaperturesDataBase::is_uv_inverted() const {

	return l_invert_uv;
}

void SubaperturesDataBase::set_uv_inverted(bool _l_invert_uv) {

	l_invert_uv = _l_invert_uv;
}

const Fpair& SubaperturesDataBase::get_baseline() const {

	return baseline;
}

double SubaperturesDataBase::get_baseline_ratio() const {

	return baseline.second / baseline.first;

}

void SubaperturesDataBase::set_baseline(const Fpair& _baseline) {

	if (_baseline.first > 0. && _baseline.second > 0.) {
		baseline = _baseline;
	} else {
		std::cout << "SubaperturesDataBase::set_baseline : negative/null baseline" << std::endl;
	}

}

void SubaperturesDataBase::set_baseline_ratio(const double& _baseline_ratio) {

	if (_baseline_ratio > 0.) {
		baseline.first = 1.;
		baseline.second = _baseline_ratio;
	} else {
		std::cout << "SubaperturesDataBase::set_baseline_ratio : negative/null baseline" << std::endl;
	}

}

void SubaperturesDataBase::set_center_coordinates() {

	center_coordinates = SubaperturesLoader::center_coordinates_default();
}

void SubaperturesDataBase::set_center_coordinates(const Fpair _center_coordinates) {

	center_coordinates = _center_coordinates;
}

void SubaperturesDataBase::average_values_baseline_weighted(const cv::Point2f& _values, const double& _baseline_ratio, float& _average) {

	ocv::Vec<float> values(_values.x, _values.y);
	average_values_baseline_weighted(values, _baseline_ratio, _average);

}

std::pair<UVindices, bool> SubaperturesDataBase::is_valid_angular_cropping(const UVindices& _Nuv, const std::array<unsigned int, 4>& _subaperture_offsets) {

	std::pair<UVindices, bool> result;
	result.second = true;

	if (_subaperture_offsets[0] + _subaperture_offsets[2] <= _Nuv.first) {
		result.first.first = _Nuv.first - (_subaperture_offsets[0] + _subaperture_offsets[2]);
	} else {
		result.first.first = 0;
		result.second = false;
		std::cout << "Subapertures crop may be too large for this dataset. Check subaperture_offsets parameter." << std::endl;
	}

	if (_subaperture_offsets[1] + _subaperture_offsets[3] <= _Nuv.second) {
		result.first.second = _Nuv.second - (_subaperture_offsets[1] + _subaperture_offsets[3]);
	} else {
		result.first.second = 0;
		result.second = false;
		std::cout << "Subapertures crop may be too large for this dataset. Check subaperture_offsets parameter." << std::endl;
	}


	return result;

}

std::pair<UVindices, bool> SubaperturesDataBase::is_valid_angular_modulo(const UVindices& _Nuv, const Upair& _angular_modulo) {

	std::pair<UVindices, bool> result;

	result.second = ((_Nuv.first - 1) % _angular_modulo.first == 0 && (_Nuv.second - 1) % _angular_modulo.second == 0);

	if (result.second) {
		result.first.first = (_Nuv.first - 1) / _angular_modulo.first + 1;
		result.first.second = (_Nuv.second - 1) / _angular_modulo.second + 1;
	} else {
		result.first = _Nuv;
	}

	return result;
}

std::pair<UVindices, bool> SubaperturesDataBase::get_Nuv_transforms(const UVindices& _Nuv, const std::array<unsigned int, 4>& _subaperture_offsets, const Upair& _angular_modulo) {

	std::pair<UVindices, bool> result;

	result = is_valid_angular_cropping(_Nuv, _subaperture_offsets);
	if (result.second) {

		result = is_valid_angular_modulo(result.first, _angular_modulo);

		if (!result.second) {
			std::cout << "Ignoring angular modulo " << _angular_modulo << ". Doesn't match angular dimensions of light field." << std::endl;
		}

	} else {
		std::cout << "Subapertures crop may be too large for this dataset. Check subaperture_offsets parameter." << std::endl;
	}

	return result;
}

UVindices SubaperturesDataBase::get_uv_transforms(const UVindices& _uv, const UVindices& _Nuv, const std::array<unsigned int, 4>& _subaperture_offsets, const Upair& _angular_modulo, bool _l_apply_angular_modulo) {

	UVindices uv_read;

	unsigned int Nu = _Nuv.first;
	unsigned int Nv = _Nuv.second;
	unsigned int u, v;
	unsigned int u_off, v_off;
	unsigned int u_mod, v_mod;
	unsigned int u_read, v_read;

	u = _uv.first;
	v = _uv.second;

	if (SubaperturesDataBase::is_in_uv_range(u, v, Nu, Nv, _subaperture_offsets)) {

		u_off = u - _subaperture_offsets[0];
		v_off = v - _subaperture_offsets[1];

		/*! If angular modulo is being applied (valid) and the u, v indices match the modulo.*/
		if (_l_apply_angular_modulo && u_off%_angular_modulo.first == 0 && v_off%_angular_modulo.second == 0) {

			u_mod = u_off / _angular_modulo.first;
			v_mod = v_off / _angular_modulo.second;

			u_read = u_mod;
			v_read = v_mod;

		} else {

			/*! If the u, v indices don't match the modulo.*/
			if (_l_apply_angular_modulo) {
				u_read = -1;
				v_read = -1;
			} else {
				/*! Ignore modulo.*/
				u_read = u_off;
				v_read = v_off;
			}
		}


	} else {

		u_read = -1;
		v_read = -1;
	}

	uv_read.first = u_read;
	uv_read.second = v_read;

	return uv_read;
}

bool SubaperturesDataBase::is_in_uv_range(const unsigned int u, const unsigned int v, const unsigned int Nu, const unsigned int Nv, const std::array<unsigned int, 4>& _subapertures_offsets) {

	return u >= _subapertures_offsets[0] && u < Nu - _subapertures_offsets[2] && \
		v >= _subapertures_offsets[1] && v < Nv - _subapertures_offsets[3];

}

///////// DimensionsTransforms

const std::vector<SubaperturesDataBase::DimensionsTransforms::Transform>& SubaperturesDataBase::DimensionsTransforms::get_transforms() const {

	return transforms;

}

void SubaperturesDataBase::DimensionsTransforms::transpose_angular() {

	Transform transform;
	transform.transpose_angular();
	transforms.push_back(transform);

}

void SubaperturesDataBase::DimensionsTransforms::flip_angular(ocv::Orientation _orientation) {

	Transform transform;
	transform.flip_angular(_orientation);
	transforms.push_back(transform);
	
}

void SubaperturesDataBase::DimensionsTransforms::remove_angular(const Upair& _Nviews) {

	Transform transform;
	transform.remove_angular(_Nviews);
	transforms.push_back(transform);

}

void SubaperturesDataBase::DimensionsTransforms::crop_angular(const Leupribo<unsigned int>& _Ncrop_angular) {

	Transform transform;
	transform.crop_angular(_Ncrop_angular);
	transforms.push_back(transform);

}

void SubaperturesDataBase::DimensionsTransforms::crop_spatially(const Leupribo<unsigned int>& _Ncrop_spatial) {

	Transform transform;
	transform.crop_spatially(_Ncrop_spatial);
	transforms.push_back(transform);

}

void SubaperturesDataBase::DimensionsTransforms::rescale_spatially(const Fpair& _image_scale) {

	Transform transform;
	transform.rescale_spatially(_image_scale);
	transforms.push_back(transform);

}

void SubaperturesDataBase::DimensionsTransforms::set_baseline(const Fpair& _baseline) {

	Transform transform;
	transform.set_baseline(_baseline);
	transforms.push_back(transform);

}

///////// DimensionsTransforms::Transform

SubaperturesDataBase::DimensionsTransforms::Transform::Type SubaperturesDataBase::DimensionsTransforms::Transform::get_type() const {

	return transform_type;

}

void SubaperturesDataBase::DimensionsTransforms::Transform::transpose_angular() {

	if (!l_set) {
		l_set = true;
		transform_type = Type::TransposeAngular;
		l_transpose_angular = true;
	}

}

void SubaperturesDataBase::DimensionsTransforms::Transform::flip_angular(ocv::Orientation _orientation) {

	if (!l_set) {
		l_set = true;
		if (_orientation == ocv::horizontal) {
			transform_type = Type::FlipAngularHorizontal;
			l_flip_angular.first = true;
		} else if (_orientation == ocv::vertical) {
			transform_type = Type::FlipAngularVertical;
			l_flip_angular.second = true;
		}
	}

}

void SubaperturesDataBase::DimensionsTransforms::Transform::remove_angular(const Upair& _Nviews) {

	if (!l_set) {
		l_set = true;
		transform_type = Type::RemoveAngular;
		upair = _Nviews;
	}

}

void SubaperturesDataBase::DimensionsTransforms::Transform::crop_angular(const Leupribo<unsigned int>& _Ncrop_angular) {

	if (!l_set) {
		l_set = true;
		transform_type = Type::CropAngular;
		for (unsigned int i = 0; i < 4; i++) {
			Ncrop[i] = _Ncrop_angular[i];
		}
	}

}

void SubaperturesDataBase::DimensionsTransforms::Transform::crop_spatially(const Leupribo<unsigned int>& _Ncrop_spatial) {

	if (!l_set) {
		l_set = true;
		transform_type = Type::CropSpatially;
		for (unsigned int i = 0; i < 4; i++) {
			Ncrop[i] = _Ncrop_spatial[i];
		}
	}

}

void SubaperturesDataBase::DimensionsTransforms::Transform::rescale_spatially(const Fpair& _image_scale) {

	if (!l_set) {
		l_set = true;
		transform_type = Type::RescaleSpatially;
		fpair = _image_scale;
	}

}

void SubaperturesDataBase::DimensionsTransforms::Transform::set_baseline(const Fpair& _baseline) {

	if (!l_set) {
		l_set = true;
		transform_type = Type::Baseline;
		fpair = _baseline;
	}

}

const Upair& SubaperturesDataBase::DimensionsTransforms::Transform::get_Nviews_removed() const {

	return upair;

}

const Leupribo<unsigned int>& SubaperturesDataBase::DimensionsTransforms::Transform::get_Ncrop_angular() const {

	return Ncrop;

}

const Leupribo<unsigned int>& SubaperturesDataBase::DimensionsTransforms::Transform::get_Ncrop_spatial() const {

	return Ncrop;

}

const Fpair& SubaperturesDataBase::DimensionsTransforms::Transform::get_image_scale() const {

	return fpair;

}

const Fpair& SubaperturesDataBase::DimensionsTransforms::Transform::get_baseline() const {

	return fpair;

}