/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once
#include "ConfigParameters.h"
#include "DisparityFastGradient.h"

template <>
struct ConfigParametersSpecializations<DisparityFastGradient> {

private:
	enum ParametersId {
		disparity_bound,
		l_denoise_disparity,
		lambda_denoise,
		Niterations_denoise
	};
	static const std::map<ParametersId, std::string> all_parameters;

public:

	static bool set_value(DisparityFastGradient::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory);
	static void deduce_values(DisparityFastGradient::Parameters& _parameters);
	static void error_message();
	static std::vector<std::string> check_read_parameters(const std::vector<std::string>& _read_parameters);
};