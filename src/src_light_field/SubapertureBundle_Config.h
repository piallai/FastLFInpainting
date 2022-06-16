/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once
#include "ConfigParameters.h"
#include "SubapertureBundle.h"

template <>
struct ConfigParametersSpecializations<SubapertureBundle> {

private:
	enum ParametersId {
		l_single_index_for_subapertures,
		uvt_position_in_name,
		UVdims,
		l_begin0
	};
	static const std::map<ParametersId, std::string> all_parameters;

public:

	static bool set_value(SubapertureBundle::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory);
	static void deduce_values(SubapertureBundle::Parameters& _parameters);
	static void error_message();
	static std::vector<std::string> check_read_parameters(const std::vector<std::string>& _read_parameters);
	static bool check(const ConfigParametersSpecializations<SubapertureBundle>& _config);
};