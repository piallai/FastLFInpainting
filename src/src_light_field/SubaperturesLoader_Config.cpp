/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "SubaperturesLoader_Config.h"
#include "misc_funcs.h"
#include "ConfigParameter.h"
#include "ConfigReader.h"

/*! Dedicated config classes. */
#include "SubapertureBundle_Config.h"

const std::map<ConfigParametersSpecializations<SubaperturesLoader>::ParametersId, std::string> ConfigParametersSpecializations<SubaperturesLoader>::all_parameters = {
	{ LF_path, "LF_path" },
{ time_index, "time_index" },
{ l_invert_uv, "l_invert_uv" },
{ uv_axis_coef, "uv_axis_coef" },
{ center_coordinates, "center_coordinates" },
{ image_scale, "image_scale" },
{ subaperture_offsets, "subaperture_offsets" },
{ angular_modulo, "angular_modulo" },
{ l_histogram_matching, "l_histogram_matching" },
{ Ncrop_pixels, "Ncrop_pixels" },
{ l_auto_read, "l_auto_read" },
{ reading_format_config_path, "reading_format_config_path" },
{ Nimages_auto, "Nimages_auto"},
{ coef_std_Nimages_auto, "coef_std_Nimages_auto" },
{ coef_std_ratio_auto, "coef_std_ratio_auto" },
{ filter_strings, "filter_strings" },
{ l_display_subapertures, "l_display_subapertures" }
};


bool ConfigParametersSpecializations<SubaperturesLoader>::set_value(SubaperturesLoader::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory) {

	bool l_keep_reading = true;

	if (_parameter_name == all_parameters.at(ParametersId::LF_path)) {

		if (_sub_strings.size() > 0) {
			_parameters.LF_path = _sub_strings[0];
			if (_sub_strings.size() > 1) {
				if (_sub_strings[1] == "R") {
					_parameters.l_recursive = true;
				} else {
					std::cout << "second argument for LF_path must be 'R' or nothing" << std::endl;
					l_keep_reading = false;
				}

				if (_sub_strings.size() > 2) {
					std::cout << "LF_path can only take maximum 2 arguments" << std::endl;
					l_keep_reading = false;
				}
			} else {
				_parameters.l_recursive = false;
			}
		}

	} else
	if (_parameter_name == all_parameters.at(ParametersId::time_index)) {

		if (_sub_strings.size() == 1 && !_sub_strings[0].empty()) {
			l_keep_reading = ConfigParameter::read(_parameters.time_index, _sub_strings, _parameter_name);
		} else if (_sub_strings.size() == 0 || _sub_strings[0].empty()) {
			_parameters.time_index = -1;
		} else {
			std::cout << "time_index must have only one value" << std::endl;
			l_keep_reading = false;
		}

	} else if (_parameter_name == all_parameters.at(ParametersId::l_invert_uv)) {

		l_keep_reading = ConfigParameter::read(_parameters.l_invert_uv, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::uv_axis_coef)) {

		std::vector<float> uv_axis_coef;
		l_keep_reading = ConfigParameter::read(uv_axis_coef, 2, _sub_strings, _parameter_name);
		if (l_keep_reading) {
			_parameters.uv_axis_coef.first = uv_axis_coef[0];
			_parameters.uv_axis_coef.second = uv_axis_coef[1];
		}

	} else if (_parameter_name == all_parameters.at(ParametersId::center_coordinates)) {

		std::vector<float> center_coordinates;
		l_keep_reading = ConfigParameter::read(center_coordinates, 2, _sub_strings, _parameter_name);
		if (l_keep_reading) {
			_parameters.center_coordinates.first = center_coordinates[0];
			_parameters.center_coordinates.second = center_coordinates[1];
		}

	} else if (_parameter_name == all_parameters.at(ParametersId::image_scale)) {

		std::vector<float> image_scale;
		l_keep_reading = ConfigParameter::read(image_scale, 2, _sub_strings, _parameter_name);
		if (l_keep_reading) {
			_parameters.image_scale.first = image_scale[0];
			_parameters.image_scale.second = image_scale[1];
			if (_parameters.image_scale.first <= 0 || _parameters.image_scale.second <= 0) {
				std::cout << all_parameters.at(ParametersId::image_scale) <<  " must be superior to 0." << std::endl;
				l_keep_reading = false;
			}
		}

	} else if (_parameter_name == all_parameters.at(ParametersId::subaperture_offsets)) {

		l_keep_reading = ConfigParameter::read(_parameters.subaperture_offsets, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::angular_modulo)) {

		l_keep_reading = ConfigParameter::read(_parameters.angular_modulo, _sub_strings, _parameter_name);
		if (_parameters.angular_modulo.first == 0 || _parameters.angular_modulo.second == 0) {
			std::cout << all_parameters.at(ParametersId::angular_modulo) << " must be superior to 0." << std::endl;
			l_keep_reading = false;
		}

	} else if (_parameter_name == all_parameters.at(ParametersId::l_histogram_matching)) {

		l_keep_reading = ConfigParameter::read(_parameters.l_histogram_matching, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::Ncrop_pixels)) {

		l_keep_reading = ConfigParameter::read(_parameters.Ncrop_pixels, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::l_auto_read)) {

		l_keep_reading = ConfigParameter::read(_parameters.l_auto_read, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::reading_format_config_path)) {

		std::string reading_format_config_path;
		l_keep_reading = ConfigParameter::read(reading_format_config_path, _sub_strings, _parameter_name);
		l_keep_reading &= ConfigReader::read<SubapertureBundle>(_parameters.reading_parameters, Misc::concat_path_and_filename(_config_directory, reading_format_config_path));

	} else if (_parameter_name == all_parameters.at(ParametersId::Nimages_auto)) {

		l_keep_reading = ConfigParameter::read(_parameters.Nimages_auto, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::coef_std_Nimages_auto)) {

		l_keep_reading = ConfigParameter::read(_parameters.coef_std_Nimages_auto, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::coef_std_ratio_auto)) {

		l_keep_reading = ConfigParameter::read(_parameters.coef_std_ratio_auto, _sub_strings, _parameter_name);

	} else if (_parameter_name == all_parameters.at(ParametersId::filter_strings)) {

		_parameters.filter_strings = _sub_strings;

	} else if(_parameter_name == all_parameters.at(ParametersId::l_display_subapertures)) {

		l_keep_reading = ConfigParameter::read(_parameters.l_display_subapertures, _sub_strings, _parameter_name);

	} else {

		ConfigBase::display_unknown_parameter(_parameter_name);
		l_keep_reading = false;
	}


	return l_keep_reading;
}

void ConfigParametersSpecializations<SubaperturesLoader>::deduce_values(SubaperturesLoader::Parameters& _parameters) {

}

void ConfigParametersSpecializations<SubaperturesLoader>::error_message() {

}

std::vector<std::string> ConfigParametersSpecializations<SubaperturesLoader>::check_read_parameters(const std::vector<std::string>& _read_parameters) {

	return ConfigParametersSpecializationsBase::check_read_all_parameters(all_parameters, _read_parameters);
}