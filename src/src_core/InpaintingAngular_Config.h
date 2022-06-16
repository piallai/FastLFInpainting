/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once
#include "ConfigParameters.h"
#include "InpaintingAngular.h"

template <>
struct ConfigParametersSpecializations<InpaintingAngular> {

private:
	enum ParametersId {
		disparity_smoothness,
		disparity_computing_config_path,
		sps_config_path,
		sps_merger_config_path,
		sps_interpolation_config_path,
	};
	static const std::map<ParametersId, std::string> all_parameters;

public:

	static bool set_value(InpaintingAngular::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory);
	static void deduce_values(InpaintingAngular::Parameters& _parameters);
	static void error_message();
	static std::vector<std::string> check_read_parameters(const std::vector<std::string>& _read_parameters);
};

