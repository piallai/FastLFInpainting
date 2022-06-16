/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "SubaperturesData.h"
#include "ImgEigen.h"
#include "Denoising.h"

/*! Fast computation of disparity based on direct gradients using angular neighbours.
Convenient to find a proper angular orientation.
Not recommended for accuracy.*/
class DisparityFastGradient {

public:

	struct Parameters {
		/*! Min/max disparity value.*/
		ocv::Tvec1 disparity_bound = ocv::Tvalue(3.);
		/*! Whether disparities are denoised using TVL1.*/
		bool l_denoise_disparity = false;
		/*! Lambda for TVL1 denoising.*/
		double lambda_denoise = 0.5;
		/*! Number of iterations for denoising.*/
		unsigned int Niterations_denoise = 21;
	};

	struct Buffer {
		ocv::Timg1 image_gray_backward;
		ocv::Timg1 image_gray_forward;
		ocv::Timg1 gradient_uv;
		ocv::Timg1 gradient_xy;
		ImgMatrix<ocv::Timg1> tensor;
		ImgEigen<ocv::Timg1> eigen;
		/*! Denoising buffer.*/
		Denoising::Buffer<1> denoising_buffer;
	};

private:

	Parameters parameters;

public:
	DisparityFastGradient();
	~DisparityFastGradient();

	void set_parameters(const Parameters& _parameters);
	const Parameters& get_parameters() const;
	/*! Compute disparity at position \p _subaperture_position.*/
	void compute(const SubaperturesData<>& _subapertures, const UVindices& _subaperture_position, ocv::Timg1& _disparity) const;
	/*! Compute disparity at position \p _subaperture_position.*/
	void compute(const SubaperturesData<>& _subapertures, const UVindices& _subaperture_position, ocv::Timg1& _disparity, Buffer& _buffer) const;
	/*! Compute disparity at position \p _subaperture_position.*/
	void compute(const SubaperturesData<>& _subapertures, const UVindices& _subaperture_position, ocv::VecImg& disparities) const;
	/*! Compute disparity at position \p _subaperture_position.*/
	void compute(const SubaperturesData<>& _subapertures, const UVindices& _subaperture_position, ocv::VecImg& disparities, Buffer& _buffer) const;

	void compute(const SubaperturesData<>& _subapertures, SubaperturesData<ocv::Timg1>& _disparity) const;
	void compute(const SubaperturesData<>& _subapertures, SubaperturesData<ocv::Timg1>& _disparity, Buffer& _buffer) const;
	void compute(const SubaperturesData<>& _subapertures, ocv::Vec< SubaperturesData<ocv::Timg1> >& _disparities, Buffer& _buffer) const;

	const ocv::VecImg& get_disparities() const;
	const ocv::Timg1& get_disparity_depth() const;
};
