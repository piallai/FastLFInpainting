/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "ConfigParameters.h"
#include "SpsMaskMerge.h"

template <>
struct ConfigParametersSpecializations<SpsMaskMerge> {

private:
	enum ParametersId {
		Nknown_pixels_min,
		merge_coef
	};
	static const std::map<ParametersId, std::string> all_parameters;

public:

	static bool set_value(SpsMaskMerge::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory);
	static void deduce_values(SpsMaskMerge::Parameters& _parameters);
	static void error_message();
	static std::vector<std::string> check_read_parameters(const std::vector<std::string>& _read_parameters);
};