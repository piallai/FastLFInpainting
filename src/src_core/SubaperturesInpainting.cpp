/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "SubaperturesInpainting.h"

const std::string SubaperturesInpainting::directory_name() {
	static const std::string _directory_name_ = "Inpainting";
	return _directory_name_;
}

SubaperturesInpainting::SubaperturesInpainting() {

}

SubaperturesInpainting::~SubaperturesInpainting() {

}

void SubaperturesInpainting::set_parameters(const Parameters& _parameters) {

	parameters = _parameters;
}

const SubaperturesInpainting::Parameters& SubaperturesInpainting::get_parameters() const {

	return parameters;
}

void SubaperturesInpainting::inpaint(const SubaperturesData<>& _subapertures, SubaperturesData<>& _subapertures_output, const std::string _directory_name) const {

	std::cout << "Light Field inpainting" << std::endl;

	std::string directory_name_up = _directory_name;
	if (directory_name_up.empty()) {
		directory_name_up = _subapertures.get_name();
	}
	const std::string directory_path = Misc::concat_paths(directory_name_up, directory_name());


	if (_subapertures.check_uv_indices(parameters.subaperture_position)) {



		/*! Read inpainted subaperture and mask.*/
		ocv::Timg inpainted_image;
		if (!parameters.inpainted_subaperture_path.empty()) {
			ocv::imread(parameters.inpainted_subaperture_path, inpainted_image);
		}

		ocv::Tmask mask;
		if (!parameters.mask_path.empty()) {
			ocv::imread(parameters.mask_path, mask);
			/*! Applies threshold to mask so values are either 0 or ocv::mask_value*/
			ocv::mask_filter(mask);
		}

		if (mask.size() == _subapertures.get_image_size()) {


			int count_masked = cv::countNonZero(mask == ocv::mask_value);
			std::cout << "Mask ratio = " << float(count_masked) / float(mask.total()) * 100. << " %" << std::endl;

			InpaintingAngular inpainting;
			inpainting.set_parameters(parameters.inpainting_angular_parameters);
			inpainting.inpaint(_subapertures, inpainted_image, mask, parameters.subaperture_position, _subapertures_output, directory_path);

		} else {
			std::cout << "SubaperturesInpainting : wrong image dimension." << std::endl;
			std::cout << "Subapertures image size : " << _subapertures.get_image_size() << std::endl;
			std::cout << "Inpainted image size : " << inpainted_image.size() << std::endl;
			std::cout << "Mask image size : " << mask.size() << std::endl;
		}

	} else {

		std::cout << "SubaperturesInpainting::inpaint : parameters.subaperture_position is out of range for the light field." << std::endl;
	}



}

