/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "InpaintingAngular.h"
#include "ShiftSubapertures.h"
#include <ctime>

const std::string InpaintingAngular::directory_name() {
	static const std::string _directory_name_ = "Inpainting-Angular";
	return _directory_name_;
}

InpaintingAngular::InpaintingAngular() {}

InpaintingAngular::InpaintingAngular(const Parameters& _parameters) {

	set_parameters(_parameters);
}

InpaintingAngular::~InpaintingAngular() {}

void InpaintingAngular::set_parameters(const Parameters& _parameters) {

	parameters = _parameters;
}

void InpaintingAngular::inpaint(const SubaperturesData<>& _subapertures, const ocv::Timg& _inpainted_subaperture, const ocv::Tmask& _mask, const UVindices& _inpainted_indices, SubaperturesData<>& _subapertures_output, const std::string _directory_name) const {
	
	if (_inpainted_subaperture.size() == _subapertures.get_image_size()) {


		std::string directory_name_up = _directory_name;
		if (directory_name_up.empty()) {
			directory_name_up = _subapertures.get_name();
		}
		const std::string directory_path = Misc::concat_paths(directory_name_up, directory_name());



		clock_t start;
		double duration;

		SPS sps;

			start = clock();
			/*! Prepare superpixel interpolation weights.*/
			superpixel_interpolation_init(sps, _inpainted_subaperture, _mask, directory_path);
			if (Misc::l_verbose_high) {
				duration = (clock() - start) / (double)CLOCKS_PER_SEC;
				std::cout << "Superpixel weights computation time : " << (int)duration / 60 << " min, " << (long)duration % 60 << " s" << std::endl;
			}

		start = clock();
		ocv::VecImg disparities_used;
		ocv::Tvec1 disparity_bound;
		/*! Whether combination of x and y disparities is mixed into one, accounting for depth estimate.*/
		bool l_use_single_disparity = false;


			DisparityFastGradient properties_local;
			properties_local.set_parameters(parameters.disparity_fast_gradient_parameters);
			if (l_use_single_disparity) {
				properties_local.compute(_subapertures, _inpainted_indices, disparities_used.first);
				disparities_used.second = disparities_used.first;
			} else {
				properties_local.compute(_subapertures, _inpainted_indices, disparities_used);
			}

			disparity_bound = properties_local.get_parameters().disparity_bound;


		if (Misc::l_verbose_high) {
			duration = (clock() - start) / (double)CLOCKS_PER_SEC;
			std::cout << "Disparity computation time : " << (int)duration / 60 << " min, " << (long)duration % 60 << " s" << std::endl;
		}



			start = clock();
			sps.sps_interpolation.apply(disparities_used.first, disparities_used.first);
			if (!l_use_single_disparity) {
				sps.sps_interpolation.apply(disparities_used.second, disparities_used.second);
			}
			if (Misc::l_verbose_high) {
				duration = (clock() - start) / (double)CLOCKS_PER_SEC;
				std::cout << "Disparity interpolation time : " << (int)duration / 60 << " min, " << (long)duration % 60 << " s" << std::endl;
			}

		if (parameters.disparity_smoothness > 1) {
			cv::Size smooth_factor(parameters.disparity_smoothness, parameters.disparity_smoothness);
			cv::GaussianBlur(disparities_used.first, disparities_used.first, smooth_factor, 0, 0);

			if (!l_use_single_disparity) {
				/*! Mean second component is an independant image.*/
				cv::GaussianBlur(disparities_used.second, disparities_used.second, smooth_factor, 0, 0);
			}
		}


		start = clock();
		ShiftSubapertures<ocv::Timg>::warp_forward(_subapertures, disparities_used, _inpainted_subaperture, _mask, _inpainted_indices, _subapertures_output);
		if (Misc::l_verbose_high) {
			duration = (clock() - start) / (double)CLOCKS_PER_SEC;
			std::cout << "Warping time : " << (int)duration / 60 << " min, " << (long)duration % 60 << " s" << std::endl;
		}

	} else {

		std::cout << "InpaintingAngular : wrong image dimension." << std::endl;
		std::cout << "Subapertures image size : " << _subapertures.get_image_size() << std::endl;
		std::cout << "Inpainted image size : " << _inpainted_subaperture.size() << std::endl;

	}

}

void InpaintingAngular::superpixel_interpolation_init(SPS& _sps, const ocv::Timg& _segmentation_image, const ocv::Tmask& _mask, const std::string& _directory_path) const {

	/*! For writing results.*/
	ocv::Timg image_segmented;

	_sps.sps.set_parameters(parameters.sps_parameters);
	/*! Compute image segementation.*/
	_sps.sps.compute(_segmentation_image);

	_sps.sps_merger.set_parameters(parameters.sps_merger_parameters);
	/*! Compute merger using segmentation and image mask.*/
	_sps.sps_merger.compute(&_sps.sps, _mask);


	_sps.sps_interpolation.set_parameters(parameters.sps_interpolation_parameters);
	/*! Compute interpolation using merged segmentation.*/
	_sps.sps_interpolation.compute(&_sps.sps_merger);

}
