/******************************************************************/
/***                Fast Inpainting, INRIA license				  */
/******************************************************************/

#pragma once
#include "ConfigParameters.h"
#include "Master.h"


template <>
struct ConfigParametersSpecializations<Master> {

private :
	enum ParametersId { LF_loader_config_path,
		method_config_path,
		data_path,
};
	static const std::map<ParametersId, std::string> all_parameters;

public:

	static bool set_value(Master::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory);
	static void deduce_values(Master::Parameters& _parameters);
	static void error_message();
	static std::vector<std::string> check_read_parameters(const std::vector<std::string>& _read_parameters);

};