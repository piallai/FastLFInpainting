/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/


#pragma once
#include <map>

struct ConfigParametersSpecializationsBase {

	/*! Check if every parameter name in \p _all_parameters is contained in \p _read_parameters.*/
	template <class Tparameters>
	static std::vector<std::string> check_read_all_parameters(const std::map<Tparameters, std::string>& _all_parameters, const std::vector<std::string>& _read_parameters);
};

template <class Tparameters>
std::vector<std::string> ConfigParametersSpecializationsBase::check_read_all_parameters(const std::map<Tparameters, std::string>& _all_parameters, const std::vector<std::string>& _read_parameters) {

	std::vector<std::string> unread_parameters;
	for (typename std::map<Tparameters, std::string>::const_iterator it = _all_parameters.begin(); it != _all_parameters.end(); ++it) {

		unsigned int i = 0;
		while (i < _read_parameters.size() && it->second != _read_parameters[i]) {
			i++;
		}

		if (i == _read_parameters.size()) {
			unread_parameters.push_back(it->second);
		}

	}

	return unread_parameters;
}

/*! Specialization of methods of ConfigParameters. Cannot be nested in ConfigParameters for Tclass being template class.
This class is in charge of defining the specifities of reading a dedicated cfg file to Tclass::Paramerers.*/
template <class Tclass>
struct ConfigParametersSpecializations;