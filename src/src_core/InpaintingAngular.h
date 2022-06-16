/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "SubaperturesData.h"
#include "DisparityFastGradient.h"
#include "SpsMaskMerge.h"
#include "SpsInterpolation.h"

class InpaintingAngular {

public :

	static const std::string directory_name();

	struct Parameters {
		/*! Smooth window size applied to disparity in image space before being used for in epis for shift and/or diffusion.*/
		unsigned int disparity_smoothness = (unsigned int)1;

		/*! Parameters for tensor properties of subapertures.*/
		DisparityFastGradient::Parameters disparity_fast_gradient_parameters;
		/*! Superpixel segmentation parameters.*/
		SuperPixelSegmentation::Parameters sps_parameters;
		/*! Superpixel merge parameters.*/
		SpsMaskMerge::Parameters sps_merger_parameters;
		/*! Superpixel interpolation parameters.*/
		SpsInterpolation::Parameters sps_interpolation_parameters;

	};


private:

	struct SPS {
		/*! Instance of superpixel segmentation.*/
		SuperPixelSegmentation sps;
		/*! Instance of superpixel merge with respect to image mask.*/
		SpsMaskMerge sps_merger;
		/*! Instance of superpixel interpolation (weights computing).*/
		SpsInterpolation sps_interpolation;
	};
	
	Parameters parameters;

public:
	InpaintingAngular();
	InpaintingAngular(const Parameters& _parameters);
	~InpaintingAngular();

	void set_parameters(const Parameters& _parameters);
	void inpaint(const SubaperturesData<>& _subapertures, const ocv::Timg& _inpainted_subaperture, const ocv::Tmask& _mask, const UVindices& _inpainted_indices, SubaperturesData<>& _subapertures_output, const std::string _directory_name = "") const;

private :

	/*! Initialize superpixel interpolation. Ie : compute interpolation weights in #sps_interpolation.*/
	void superpixel_interpolation_init(SPS& _sps, const ocv::Timg& _segmentation_image, const ocv::Tmask& _mask, const std::string& _write_path) const;
};
