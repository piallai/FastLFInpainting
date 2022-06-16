/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "InpaintingAngular_Config.h"
#include "misc_funcs.h"
#include "ConfigParameter.h"
#include "ConfigReader.h"

#include "DisparityFastGradient_Config.h"
#include "SuperPixelSegmentation_Config.h"
#include "SpsMaskMerge_Config.h"
#include "SpsInterpolation_Config.h"

const std::map<ConfigParametersSpecializations<InpaintingAngular>::ParametersId, std::string> ConfigParametersSpecializations<InpaintingAngular>::all_parameters = {
{ disparity_smoothness, "disparity_smoothness" },
{ disparity_computing_config_path, "disparity_computing_config_path" },
{ sps_config_path, "sps_config_path" },
{ sps_merger_config_path, "sps_merger_config_path" },
{ sps_interpolation_config_path, "sps_interpolation_config_path" },
};

bool ConfigParametersSpecializations<InpaintingAngular>::set_value(InpaintingAngular::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory) {

	bool l_keep_reading = true;

	if (_parameter_name == "disparity_smoothness") {

		l_keep_reading = ConfigParameter::read(_parameters.disparity_smoothness, _sub_strings, _parameter_name);
		if (l_keep_reading) {
			if (!Misc::is_positive_odd(_parameters.disparity_smoothness)) {
				std::cout << _parameter_name << " must have one odd unsigned integer value" << std::endl;
				l_keep_reading = false;
			}
		}
	}
	else if (_parameter_name == all_parameters.at(ParametersId::disparity_computing_config_path)) {
		std::string disparity_computing_config_path;
		l_keep_reading = ConfigParameter::read(disparity_computing_config_path, _sub_strings, _parameter_name);

			l_keep_reading &= ConfigReader::read<DisparityFastGradient>(_parameters.disparity_fast_gradient_parameters, Misc::concat_path_and_filename(_config_directory, disparity_computing_config_path));


	} else if (_parameter_name == all_parameters.at(ParametersId::sps_config_path)) {

		std::string sps_config_path;
		l_keep_reading = ConfigParameter::read(sps_config_path, _sub_strings, _parameter_name);
		l_keep_reading &= ConfigReader::read<SuperPixelSegmentation>(_parameters.sps_parameters, Misc::concat_path_and_filename(_config_directory, sps_config_path));

	} else if (_parameter_name == all_parameters.at(ParametersId::sps_merger_config_path)) {

		std::string sps_merger_config_path;
		l_keep_reading = ConfigParameter::read(sps_merger_config_path, _sub_strings, _parameter_name);
		l_keep_reading &= ConfigReader::read<SpsMaskMerge>(_parameters.sps_merger_parameters, Misc::concat_path_and_filename(_config_directory, sps_merger_config_path));

	} else if (_parameter_name == all_parameters.at(ParametersId::sps_interpolation_config_path)) {

		std::string sps_interpolation_config_path;
		l_keep_reading = ConfigParameter::read(sps_interpolation_config_path, _sub_strings, _parameter_name);
		l_keep_reading &= ConfigReader::read<SpsInterpolation>(_parameters.sps_interpolation_parameters, Misc::concat_path_and_filename(_config_directory, sps_interpolation_config_path));

	}
	else {
		ConfigBase::display_unknown_parameter(_parameter_name);
		l_keep_reading = false;
	}

	return l_keep_reading;
}

void ConfigParametersSpecializations<InpaintingAngular>::deduce_values(InpaintingAngular::Parameters& _parameters) {

}

void ConfigParametersSpecializations<InpaintingAngular>::error_message() {

}

std::vector<std::string> ConfigParametersSpecializations<InpaintingAngular>::check_read_parameters(const std::vector<std::string>& _read_parameters) {

	return ConfigParametersSpecializationsBase::check_read_all_parameters(all_parameters, _read_parameters);
}

