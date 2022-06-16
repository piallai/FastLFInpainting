/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "SpsInterpolation_Config.h"
#include "misc_funcs.h"
#include "ConfigParameter.h"

const std::map<ConfigParametersSpecializations<SpsInterpolation>::ParametersId, std::string> ConfigParametersSpecializations<SpsInterpolation>::all_parameters = {
	{ Nweight_pixels, "Nweight_pixels" },
{ distance_coef, "distance_coef" },
{ sigma_coef , "sigma_coef" }
};

bool ConfigParametersSpecializations<SpsInterpolation>::set_value(SpsInterpolation::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory) {

	bool l_keep_reading = true;

	if (_parameter_name == all_parameters.at(ParametersId::Nweight_pixels)) {

		l_keep_reading = ConfigParameter::read(_parameters.Nweight_pixels, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::distance_coef)) {

		l_keep_reading = ConfigParameter::read(_parameters.distance_coef, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::sigma_coef)) {

		l_keep_reading = ConfigParameter::read(_parameters.sigma_coef, _sub_strings, _parameter_name);

	} else {
		ConfigBase::display_unknown_parameter(_parameter_name);
		l_keep_reading = false;
	}

	return l_keep_reading;
}

void ConfigParametersSpecializations<SpsInterpolation>::deduce_values(SpsInterpolation::Parameters& _parameters) {

}

void ConfigParametersSpecializations<SpsInterpolation>::error_message() {

}

std::vector<std::string> ConfigParametersSpecializations<SpsInterpolation>::check_read_parameters(const std::vector<std::string>& _read_parameters) {

	return ConfigParametersSpecializationsBase::check_read_all_parameters(all_parameters, _read_parameters);
}