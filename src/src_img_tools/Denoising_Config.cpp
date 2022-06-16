/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "Denoising_Config.h"
#include "misc_funcs.h"
#include "ConfigParameter.h"
#include "ConfigReader.h"

const std::map<ConfigParametersSpecializations<Denoising>::ParametersId, std::string> ConfigParametersSpecializations<Denoising>::all_parameters = {
	{ denoising_type, "denoising_type" },
{ lambda, "lambda" },
{ Niterations, "Niterations" },
{ h, "h" },
{ templateWindowSize, "templateWindowSize" },
{ searchWindowSize, "searchWindowSize" },
{ hColor, "hColor" },
{ temporalWindowSize, "temporalWindowSize" }
};

bool ConfigParametersSpecializations<Denoising>::set_value(Denoising::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory) {

	bool l_keep_reading = true;

	if (_parameter_name == all_parameters.at(ParametersId::denoising_type)) {

		l_keep_reading = ConfigParameter::read(_parameters.denoising_type, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::lambda)) {

		l_keep_reading = ConfigParameter::read(_parameters.lambda, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::Niterations)) {

		l_keep_reading = ConfigParameter::read(_parameters.Niterations, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::h)) {

		l_keep_reading = ConfigParameter::read(_parameters.h, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::templateWindowSize)) {

		l_keep_reading = ConfigParameter::read(_parameters.templateWindowSize, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::searchWindowSize)) {

		l_keep_reading = ConfigParameter::read(_parameters.searchWindowSize, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::hColor)) {

		l_keep_reading = ConfigParameter::read(_parameters.hColor, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::temporalWindowSize)) {

		l_keep_reading = ConfigParameter::read(_parameters.temporalWindowSize, _sub_strings, _parameter_name);

	} else {
		ConfigBase::display_unknown_parameter(_parameter_name);
		l_keep_reading = false;
	}

	return l_keep_reading;
}

void ConfigParametersSpecializations<Denoising>::deduce_values(Denoising::Parameters& _parameters) {

}

void ConfigParametersSpecializations<Denoising>::error_message() {

}

std::vector<std::string> ConfigParametersSpecializations<Denoising>::check_read_parameters(const std::vector<std::string>& _read_parameters) {

	return ConfigParametersSpecializationsBase::check_read_all_parameters(all_parameters, _read_parameters);
}

/*! Specialization.*/
template <>
Denoising::DenoisingType Misc::str_to_value<Denoising::DenoisingType>(const std::string& _string) {
	return (Denoising::DenoisingType)str_to_value<int>(_string);
}
