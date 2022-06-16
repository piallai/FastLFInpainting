/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "ConfigParameters.h"
#include "SpsInterpolation.h"

template <>
struct ConfigParametersSpecializations<SpsInterpolation> {

private:
	enum ParametersId {
		Nweight_pixels,
		distance_coef,
		sigma_coef
	};
	static const std::map<ParametersId, std::string> all_parameters;

public:

	static bool set_value(SpsInterpolation::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory);
	static void deduce_values(SpsInterpolation::Parameters& _parameters);
	static void error_message();
	static std::vector<std::string> check_read_parameters(const std::vector<std::string>& _read_parameters);
};
