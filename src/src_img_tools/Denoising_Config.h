/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once
#include "ConfigParameters.h"
#include "Denoising.h"

template <>
struct ConfigParametersSpecializations<Denoising> {

private:
	enum ParametersId {
		denoising_type,
		lambda,
		Niterations,
		h,
		templateWindowSize,
		searchWindowSize,
		hColor,
		temporalWindowSize
	};
	static const std::map<ParametersId, std::string> all_parameters;

public:

	static bool set_value(Denoising::Parameters& _parameters, const std::string& _parameter_name, const std::vector<std::string>& _sub_strings, const std::string& _config_directory);
	static void deduce_values(Denoising::Parameters& _parameters);
	static void error_message();
	static std::vector<std::string> check_read_parameters(const std::vector<std::string>& _read_parameters);
};

/*! Specialization.*/
#include "misc_funcs.h"
namespace Misc {
	template <>
	Denoising::DenoisingType str_to_value<Denoising::DenoisingType>(const std::string& _string);
}
