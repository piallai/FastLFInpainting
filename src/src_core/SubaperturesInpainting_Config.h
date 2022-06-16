/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once
#include "ConfigParameters.h"
#include "SubaperturesInpainting.h"

template <>
struct ConfigParametersSpecializations<SubaperturesInpainting> {

private:
	enum ParametersId {
		inpainted_subaperture_path,
		subaperture_position,
		mask_path,
		inpainting_config_path,
	};
	static const std::map<ParametersId, std::string> all_parameters;

public:

	static bool set_value(SubaperturesInpainting::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory);
	static void deduce_values(SubaperturesInpainting::Parameters& _parameters);
	static void error_message();
	static std::vector<std::string> check_read_parameters(const std::vector<std::string>& _read_parameters);
};

