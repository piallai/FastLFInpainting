/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "SubaperturesInpainting_Config.h"
#include "misc_funcs.h"
#include "ConfigParameter.h"
#include "ConfigReader.h"

#include "InpaintingAngular_Config.h"

const std::map<ConfigParametersSpecializations<SubaperturesInpainting>::ParametersId, std::string> ConfigParametersSpecializations<SubaperturesInpainting>::all_parameters = {
	{ inpainted_subaperture_path, "inpainted_subaperture_path" },
{ subaperture_position, "subaperture_position" },
{ mask_path, "mask_path" },
{ inpainting_config_path, "inpainting_config_path" },
};

bool ConfigParametersSpecializations<SubaperturesInpainting>::set_value(SubaperturesInpainting::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory) {

	bool l_keep_reading = true;

	if (_parameter_name == all_parameters.at(ParametersId::inpainted_subaperture_path)) {

		l_keep_reading = ConfigParameter::read(_parameters.inpainted_subaperture_path, _sub_strings, _parameter_name);

	} else
	if (_parameter_name == all_parameters.at(ParametersId::subaperture_position)) {

		std::vector<unsigned int> subaperture_position;
		l_keep_reading = ConfigParameter::read(subaperture_position, 2, _sub_strings, _parameter_name);
		_parameters.subaperture_position.first = subaperture_position[0];
		_parameters.subaperture_position.second = subaperture_position[1];

	}
	else if (_parameter_name == all_parameters.at(ParametersId::mask_path)) {

		l_keep_reading = ConfigParameter::read(_parameters.mask_path, _sub_strings, _parameter_name);

	}
	else if (_parameter_name == all_parameters.at(ParametersId::inpainting_config_path)) {
		std::string inpainting_config_path;
		l_keep_reading = ConfigParameter::read(inpainting_config_path, _sub_strings, _parameter_name);

		l_keep_reading &= ConfigReader::read<InpaintingAngular>(_parameters.inpainting_angular_parameters, Misc::concat_path_and_filename(_config_directory, inpainting_config_path));
		

	}
	else {
		ConfigBase::display_unknown_parameter(_parameter_name);
		l_keep_reading = false;
	}

	return l_keep_reading;
}

void ConfigParametersSpecializations<SubaperturesInpainting>::deduce_values(SubaperturesInpainting::Parameters& _parameters) {

}

void ConfigParametersSpecializations<SubaperturesInpainting>::error_message() {

}

std::vector<std::string> ConfigParametersSpecializations<SubaperturesInpainting>::check_read_parameters(const std::vector<std::string>& _read_parameters) {

	return ConfigParametersSpecializationsBase::check_read_all_parameters(all_parameters, _read_parameters);
}


