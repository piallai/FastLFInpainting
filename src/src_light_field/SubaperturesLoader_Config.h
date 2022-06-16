/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once
#include "ConfigParameters.h"
#include "SubaperturesLoader.h"

template <>
struct ConfigParametersSpecializations<SubaperturesLoader> {

private:
	enum ParametersId {
		LF_path,
		time_index,
		l_invert_uv,
		uv_axis_coef,
		center_coordinates,
		image_scale,
		subaperture_offsets,
		angular_modulo,
		l_histogram_matching,
		Ncrop_pixels,
		l_auto_read,
		reading_format_config_path,
		Nimages_auto,
		coef_std_Nimages_auto,
		coef_std_ratio_auto,
		filter_strings,
		l_display_subapertures
	};
	static const std::map<ParametersId, std::string> all_parameters;

public:

	static bool set_value(SubaperturesLoader::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory);
	static void deduce_values(SubaperturesLoader::Parameters& _parameters);
	static void error_message();
	static std::vector<std::string> check_read_parameters(const std::vector<std::string>& _read_parameters);
};
