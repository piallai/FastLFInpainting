/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "DisparityFastGradient.h"

#include "ocv_derivative.h"

DisparityFastGradient::DisparityFastGradient() {

}

DisparityFastGradient::~DisparityFastGradient() {

}

void DisparityFastGradient::set_parameters(const Parameters& _parameters) {

	parameters = _parameters;
}

const DisparityFastGradient::Parameters& DisparityFastGradient::get_parameters() const {

	return parameters;
}

void DisparityFastGradient::compute(const SubaperturesData<>& _subapertures, const UVindices& _subaperture_position, ocv::Timg1& _disparity) const {

	Buffer buffer;
	compute(_subapertures, _subaperture_position, _disparity, buffer);
}

void DisparityFastGradient::compute(const SubaperturesData<>& _subapertures, const UVindices& _subaperture_position, ocv::Timg1& _disparity, Buffer& _buffer) const {

	ocv::VecImg disparities;
	compute(_subapertures, _subaperture_position, disparities, _buffer);

	_subapertures.average_baseline_weighted(disparities, _disparity);

}

void DisparityFastGradient::compute(const SubaperturesData<>& _subapertures, const UVindices& _subaperture_position, ocv::VecImg& _disparities) const {

	Buffer buffer;
	compute(_subapertures, _subaperture_position, _disparities, buffer);
}

void DisparityFastGradient::compute(const SubaperturesData<>& _subapertures, const UVindices& _subaperture_position, ocv::VecImg& _disparities, Buffer& _buffer) const {

	if (Misc::l_verbose_low) {
		std::cout << "DisparityFastGradient::compute" << std::endl;
	}

	if (_subapertures.get_Nu() > 0 && _subapertures.get_Nv() > 0) {

		/*! Compute u gradient*/
		unsigned int u_backward, u_forward;
		if (_subaperture_position.first == 0) {
			u_backward = 0;
			u_forward = 1;
		} else if (_subaperture_position.first == _subapertures.get_Nu() - 1) {
			u_backward = _subapertures.get_Nu() - 2;
			u_forward = _subapertures.get_Nu() - 1;
		} else if (_subaperture_position.first < _subapertures.get_Nu() - 1) {
			u_backward = _subaperture_position.first - 1;
			u_forward = _subaperture_position.first + 1;
		} else {
			std::cout << "DisparityFastGradient::compute : Out of range" << std::endl;
			u_backward = 0;
			u_forward = 0;
		}
		/*! Compute v gradient*/
		unsigned int v_backward, v_forward;
		if (_subaperture_position.second == 0) {
			v_backward = 0;
			v_forward = 1;
		} else if (_subaperture_position.second == _subapertures.get_Nv() - 1) {
			v_backward = _subapertures.get_Nv() - 2;
			v_forward = _subapertures.get_Nv() - 1;
		} else if (_subaperture_position.second < _subapertures.get_Nv() - 1) {
			v_backward = _subaperture_position.second - 1;
			v_forward = _subaperture_position.second + 1;
		} else {
			std::cout << "DisparityFastGradient::compute : Out of range" << std::endl;
			v_backward = 0;
			v_forward = 0;
		}

		ocv::Timg image_backward;
		ocv::Timg image_forward;
		ocv::Timg image_xy;

		/*! Compute u gradient*/
		if (u_forward < _subapertures.get_Nu()) {

			image_backward = _subapertures(u_backward, _subaperture_position.second);
			if (!ocv::is_valid(image_backward)) {
				u_backward = _subaperture_position.first;
				image_backward = _subapertures(u_backward, _subaperture_position.second);
			}
			image_forward = _subapertures(u_forward, _subaperture_position.second);
			if (!ocv::is_valid(image_forward)) {
				u_forward = _subaperture_position.first;
				image_forward = _subapertures(u_forward, _subaperture_position.second);
			}

			if (u_backward != u_forward) {

				if (ocv::is_valid(image_backward) && ocv::is_valid(image_forward)) {
					cvtColor(image_backward, _buffer.image_gray_backward, cv::COLOR_BGR2GRAY);
					cvtColor(image_forward, _buffer.image_gray_forward, cv::COLOR_BGR2GRAY);
					cv::subtract(_buffer.image_gray_forward, _buffer.image_gray_backward, _buffer.gradient_uv);
					_buffer.gradient_uv /= u_forward - u_backward;
					_buffer.gradient_uv *= -1.;

					image_xy = _subapertures[_subaperture_position];
					if (!ocv::is_valid(image_xy)) {
						image_xy = image_backward;//could be forward too
					}
					cvtColor(image_xy, _buffer.image_gray_forward, cv::COLOR_BGR2GRAY);
					ocv::derivative(_buffer.image_gray_forward, _buffer.gradient_xy, 1, 0, 1);

					_buffer.tensor.tensor_product(_buffer.gradient_xy, _buffer.gradient_uv);
					_buffer.eigen.set(_buffer.tensor, true);
					_buffer.eigen.get_eigen_vector2().first.copyTo(_disparities.first);
					cv::divide(_disparities.first, _buffer.eigen.get_eigen_vector2().second, _disparities.first);
				} else {
					_disparities.first.create(_subapertures.get_image_size());
					_disparities.first = ocv::Tvec1::all(0.);
				}

			}

		} else {/*! Means in practice that _subapertures.get_Nu() equals 1.*/
			_disparities.first.create(_subapertures.get_image_size());
			_disparities.first = ocv::Tvec1::all(0.);
		}

		/*! Compute v gradient*/
		if (v_forward < _subapertures.get_Nv()) {

			image_backward = _subapertures(_subaperture_position.first, v_backward);
			if (!ocv::is_valid(image_backward)) {
				v_backward = _subaperture_position.second;
				image_backward = _subapertures(_subaperture_position.first, v_backward);
			}
			image_forward = _subapertures(_subaperture_position.first, v_forward);
			if (!ocv::is_valid(image_forward)) {
				v_forward = _subaperture_position.second;
				image_forward = _subapertures(_subaperture_position.first, v_forward);
			}

			if (v_backward != v_forward) {

				if (ocv::is_valid(image_backward) && ocv::is_valid(image_forward)) {
					cvtColor(image_backward, _buffer.image_gray_backward, cv::COLOR_BGR2GRAY);
					cvtColor(image_forward, _buffer.image_gray_forward, cv::COLOR_BGR2GRAY);
					cv::subtract(_buffer.image_gray_forward, _buffer.image_gray_backward, _buffer.gradient_uv);
					_buffer.gradient_uv /= u_forward - u_backward;

					image_xy = _subapertures[_subaperture_position];
					if (!ocv::is_valid(image_xy)) {
						image_xy = image_backward;//could be forward too
					}
					cvtColor(image_xy, _buffer.image_gray_forward, cv::COLOR_BGR2GRAY);
					ocv::derivative(_buffer.image_gray_forward, _buffer.gradient_xy, 0, 1, 1);

					_buffer.tensor.tensor_product(_buffer.gradient_xy, _buffer.gradient_uv);
					_buffer.eigen.set(_buffer.tensor, true);
					_buffer.eigen.get_eigen_vector2().first.copyTo(_disparities.second);
					cv::divide(_disparities.second, _buffer.eigen.get_eigen_vector2().second, _disparities.second);
				} else {
					_disparities.second.create(_subapertures.get_image_size());
					_disparities.second = ocv::Tvec1::all(0.);
				}

			}

		} else {/*! Means in practice that _subapertures.get_Nv() equals 1.*/
			_disparities.second.create(_subapertures.get_image_size());
			_disparities.second = ocv::Tvec1::all(0.);
		}


		ocv::bound(_disparities.first, _disparities.first, -parameters.disparity_bound, parameters.disparity_bound);
		ocv::bound(_disparities.second, _disparities.second, -parameters.disparity_bound, parameters.disparity_bound);

		if (parameters.l_denoise_disparity) {
			Denoising::denoiseTVL1(_disparities.first, _disparities.first, _buffer.denoising_buffer, parameters.lambda_denoise, parameters.Niterations_denoise);
			Denoising::denoiseTVL1(_disparities.second, _disparities.second, _buffer.denoising_buffer, parameters.lambda_denoise, parameters.Niterations_denoise);
		}



	} else {
		std::cout << "Not enough subapertures to compute local properties" << std::endl;
	}

}

void DisparityFastGradient::compute(const SubaperturesData<>& _subapertures, SubaperturesData<ocv::Timg1>& _disparity) const {

	Buffer buffer;
	compute(_subapertures, _disparity, buffer);

}

void DisparityFastGradient::compute(const SubaperturesData<>& _subapertures, SubaperturesData<ocv::Timg1>& _disparity, Buffer& _buffer) const {

	ocv::Vec< SubaperturesData<ocv::Timg1> > disparities;
	compute(_subapertures, disparities, _buffer);

	_subapertures.average_baseline_weighted(disparities, _disparity);

}

void DisparityFastGradient::compute(const SubaperturesData<>& _subapertures, ocv::Vec< SubaperturesData<ocv::Timg1> >& _disparities, Buffer& _buffer) const {

	UVindices subaperture_position;

	ocv::VecImg disparities;
	_subapertures.copyPropertiesTo(_disparities.first);
	_subapertures.copyPropertiesTo(_disparities.second);

	for (unsigned int u = 0; u < _subapertures.get_Nu(); u++) {
		subaperture_position.first = u;
		for (unsigned int v = 0; v < _subapertures.get_Nv(); v++) {
			subaperture_position.second = v;

			compute(_subapertures, subaperture_position, disparities, _buffer);

			 disparities.first.copyTo(_disparities.first[subaperture_position]);
			 disparities.second.copyTo(_disparities.second[subaperture_position]);
		}
	}

}