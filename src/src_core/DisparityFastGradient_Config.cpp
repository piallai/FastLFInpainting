/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "DisparityFastGradient_Config.h"
#include "misc_funcs.h"
#include "ConfigParameter.h"
#include "ConfigReader.h"


const std::map<ConfigParametersSpecializations<DisparityFastGradient>::ParametersId, std::string> ConfigParametersSpecializations<DisparityFastGradient>::all_parameters = {
	{ disparity_bound, "disparity_bound" },
{ l_denoise_disparity, "l_denoise_disparity" },
{ lambda_denoise, "lambda_denoise" },
{ Niterations_denoise, "Niterations_denoise" }
};

bool ConfigParametersSpecializations<DisparityFastGradient>::set_value(DisparityFastGradient::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory) {

	bool l_keep_reading = true;

	if (_parameter_name == all_parameters.at(ParametersId::disparity_bound)) {

		l_keep_reading = ConfigParameter::read(_parameters.disparity_bound[0], _sub_strings, _parameter_name);

	}
	else if (_parameter_name == all_parameters.at(ParametersId::l_denoise_disparity)) {

		l_keep_reading = ConfigParameter::read(_parameters.l_denoise_disparity, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::lambda_denoise)) {

		l_keep_reading = ConfigParameter::read(_parameters.lambda_denoise, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::Niterations_denoise)) {

		l_keep_reading = ConfigParameter::read(_parameters.Niterations_denoise, _sub_strings, _parameter_name);

	} else {
		ConfigBase::display_unknown_parameter(_parameter_name);
		l_keep_reading = false;
	}

	return l_keep_reading;
}

void ConfigParametersSpecializations<DisparityFastGradient>::deduce_values(DisparityFastGradient::Parameters& _parameters) {

}

void ConfigParametersSpecializations<DisparityFastGradient>::error_message() {

}

std::vector<std::string> ConfigParametersSpecializations<DisparityFastGradient>::check_read_parameters(const std::vector<std::string>& _read_parameters) {

	return ConfigParametersSpecializationsBase::check_read_all_parameters(all_parameters, _read_parameters);
}
