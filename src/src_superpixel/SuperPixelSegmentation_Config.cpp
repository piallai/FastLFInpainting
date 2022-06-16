/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "SuperPixelSegmentation_Config.h"
#include "misc_funcs.h"
#include "ConfigParameter.h"

const std::map<ConfigParametersSpecializations<SuperPixelSegmentation>::ParametersId, std::string> ConfigParametersSpecializations<SuperPixelSegmentation>::all_parameters = {
	{ Nsuperpixels, "Nsuperpixels" },
{ Nsuperpixels_per_pixel, "Nsuperpixels_per_pixel" },
{ ruler_coef, "ruler_coef" },
{ Niterations_sps, "Niterations_sps" },
{ conversion_type, "conversion_type" },
{ sigma_blur, "sigma_blur" }
};

bool ConfigParametersSpecializations<SuperPixelSegmentation>::set_value(SuperPixelSegmentation::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory) {

	bool l_keep_reading = true;

	if (_parameter_name == all_parameters.at(ParametersId::Nsuperpixels)) {

		l_keep_reading = ConfigParameter::read(_parameters.Nsuperpixels, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::Nsuperpixels_per_pixel)) {

		l_keep_reading = ConfigParameter::read(_parameters.Nsuperpixels_per_pixel, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::ruler_coef)) {

		l_keep_reading = ConfigParameter::read(_parameters.ruler_coef, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::Niterations_sps)) {

		l_keep_reading = ConfigParameter::read(_parameters.Niterations, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::conversion_type)) {

		l_keep_reading = ConfigParameter::read(_parameters.conversion_type, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::sigma_blur)) {

		std::vector<unsigned int> sigma_blur;
		l_keep_reading = ConfigParameter::read(sigma_blur, 2, _sub_strings, _parameter_name);
		if (l_keep_reading) {
			_parameters.sigma_blur.width = sigma_blur[0];
			_parameters.sigma_blur.height = sigma_blur[1];
			l_keep_reading &= Misc::is_positive_odd(_parameters.sigma_blur.width);
			l_keep_reading &= Misc::is_positive_odd(_parameters.sigma_blur.height);
		}

	} else {
		ConfigBase::display_unknown_parameter(_parameter_name);
		l_keep_reading = false;
	}

	return l_keep_reading;
}

void ConfigParametersSpecializations<SuperPixelSegmentation>::deduce_values(SuperPixelSegmentation::Parameters& _parameters) {

}

void ConfigParametersSpecializations<SuperPixelSegmentation>::error_message() {

}

std::vector<std::string> ConfigParametersSpecializations<SuperPixelSegmentation>::check_read_parameters(const std::vector<std::string>& _read_parameters) {

	return ConfigParametersSpecializationsBase::check_read_all_parameters(all_parameters, _read_parameters);
}