/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "SpsMaskMerge_Config.h"
#include "misc_funcs.h"
#include "ConfigParameter.h"

const std::map<ConfigParametersSpecializations<SpsMaskMerge>::ParametersId, std::string> ConfigParametersSpecializations<SpsMaskMerge>::all_parameters = {
	{ Nknown_pixels_min, "Nknown_pixels_min" },
{ merge_coef, "merge_coef" }
};

bool ConfigParametersSpecializations<SpsMaskMerge>::set_value(SpsMaskMerge::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory) {

	bool l_keep_reading = true;

	if (_parameter_name == all_parameters.at(ParametersId::Nknown_pixels_min)) {

		l_keep_reading = ConfigParameter::read(_parameters.Nknown_pixels_min, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::merge_coef)) {

		l_keep_reading = ConfigParameter::read(_parameters.merge_coef, _sub_strings, _parameter_name);

	} else {
		ConfigBase::display_unknown_parameter(_parameter_name);
		l_keep_reading = false;
	}

	return l_keep_reading;
}

void ConfigParametersSpecializations<SpsMaskMerge>::deduce_values(SpsMaskMerge::Parameters& _parameters) {

}

void ConfigParametersSpecializations<SpsMaskMerge>::error_message() {

}

std::vector<std::string> ConfigParametersSpecializations<SpsMaskMerge>::check_read_parameters(const std::vector<std::string>& _read_parameters) {

	return ConfigParametersSpecializationsBase::check_read_all_parameters(all_parameters, _read_parameters);
}