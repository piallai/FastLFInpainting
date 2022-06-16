/******************************************************************/
/***                Fast Inpainting, INRIA license				  */
/******************************************************************/

#include "Master_Config.h"
#include "misc_funcs.h"
#include "ConfigParameter.h"
#include "ConfigReader.h"

/*! Dedicated config classes. */
#include "SubaperturesLoader_Config.h"
#include "SubaperturesInpainting_Config.h"

const std::map<ConfigParametersSpecializations<Master>::ParametersId, std::string> ConfigParametersSpecializations<Master>::all_parameters = {
	{ LF_loader_config_path, "LF_loader_config_path" },
{ method_config_path, "method_config_path" },
{ data_path, "data_path" }
};

bool ConfigParametersSpecializations<Master>::set_value(Master::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory) {

	bool l_keep_reading = true;

	if (_parameter_name == all_parameters.at(ParametersId::LF_loader_config_path)) {

		std::string LF_loader_config_path;
		l_keep_reading = ConfigParameter::read(LF_loader_config_path, _sub_strings, _parameter_name);
		l_keep_reading &= ConfigReader::read<SubaperturesLoader>(_parameters.LF_loader_parameters, Misc::concat_path_and_filename(_config_directory, LF_loader_config_path));

	}
	else if (_parameter_name == all_parameters.at(ParametersId::method_config_path)) {

		std::string method_config_path;
		l_keep_reading = ConfigParameter::read(method_config_path, _sub_strings, _parameter_name);

		l_keep_reading &= ConfigReader::read<SubaperturesInpainting>(_parameters.inpainting_parameters, Misc::concat_path_and_filename(_config_directory, method_config_path));

	}
	else if (_parameter_name == all_parameters.at(ParametersId::data_path)) {

		l_keep_reading = ConfigParameter::read(_parameters.data_path, _sub_strings, _parameter_name);

	} else {
		ConfigBase::display_unknown_parameter(_parameter_name);
		l_keep_reading = false;
	}

	return l_keep_reading;
}

void ConfigParametersSpecializations<Master>::deduce_values(Master::Parameters& _parameters) {

}

void ConfigParametersSpecializations<Master>::error_message() {

}

std::vector<std::string> ConfigParametersSpecializations<Master>::check_read_parameters(const std::vector<std::string>& _read_parameters) {

	return ConfigParametersSpecializationsBase::check_read_all_parameters(all_parameters, _read_parameters);
}
