/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "ConfigParameters.h"
#include "SuperPixelSegmentation.h"

template <>
struct ConfigParametersSpecializations<SuperPixelSegmentation> {
	
private:
	enum ParametersId {
		Nsuperpixels,
		Nsuperpixels_per_pixel,
		ruler_coef,
		Niterations_sps,
		conversion_type,
		sigma_blur
	};
	static const std::map<ParametersId, std::string> all_parameters;

public :

	static bool set_value(SuperPixelSegmentation::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory);
	static void deduce_values(SuperPixelSegmentation::Parameters& _parameters);
	static void error_message();
	static std::vector<std::string> check_read_parameters(const std::vector<std::string>& _read_parameters);
};

