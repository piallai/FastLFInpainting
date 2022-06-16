/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "SubapertureBundle_Config.h"
#include "misc_funcs.h"
#include "ConfigParameter.h"

const std::map<ConfigParametersSpecializations<SubapertureBundle>::ParametersId, std::string> ConfigParametersSpecializations<SubapertureBundle>::all_parameters = {
	{ l_single_index_for_subapertures, "l_single_index_for_subapertures" },
{ uvt_position_in_name, "uvt_position_in_name" },
{ UVdims, "UVdims" },
{ l_begin0, "l_begin0" }
};

bool ConfigParametersSpecializations<SubapertureBundle>::set_value(SubapertureBundle::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory) {

	bool l_keep_reading = true;

	if (_parameter_name == all_parameters.at(ParametersId::l_single_index_for_subapertures)) {

		l_keep_reading = ConfigParameter::read(_parameters.l_single_index_for_subapertures, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::uvt_position_in_name)) {

		if (_sub_strings.size() > 0 && _sub_strings.size() <= 3) {
			l_keep_reading = ConfigParameter::read(_parameters.uvt_position_in_name, (unsigned int)_sub_strings.size(), _sub_strings, _parameter_name);
		} else {
			std::cout << "uvt_position_in_name must have between 1 and 3 values" << std::endl;
			l_keep_reading = false;
		}

	} else if (_parameter_name == all_parameters.at(ParametersId::UVdims)) {

		std::vector<unsigned int> UVdims;
		l_keep_reading = ConfigParameter::read(UVdims, 2, _sub_strings, _parameter_name);
		if (l_keep_reading) {
			_parameters.UVdims.first = UVdims[0];
			_parameters.UVdims.second = UVdims[1];
		}

	} else if (_parameter_name == all_parameters.at(ParametersId::l_begin0)) {

		l_keep_reading = ConfigParameter::read(_parameters.l_begin0, _sub_strings, _parameter_name);

	} else {
		ConfigBase::display_unknown_parameter(_parameter_name);
		l_keep_reading = false;
	}

	return l_keep_reading;
}

void ConfigParametersSpecializations<SubapertureBundle>::deduce_values(SubapertureBundle::Parameters& _parameters) {

}

void ConfigParametersSpecializations<SubapertureBundle>::error_message() {

}

std::vector<std::string> ConfigParametersSpecializations<SubapertureBundle>::check_read_parameters(const std::vector<std::string>& _read_parameters) {

	return ConfigParametersSpecializationsBase::check_read_all_parameters(all_parameters, _read_parameters);
}

bool ConfigParametersSpecializations<SubapertureBundle>::check(const ConfigParametersSpecializations<SubapertureBundle>& _config) {

	return true;
}