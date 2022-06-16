/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/


#pragma once
#include "misc_funcs.h"

/*! Class deducing the type when reading lines of a config text file.*/
class ConfigParameter {
public :
	template <class Tparameter>
	static bool read(Tparameter& _parameter, const std::vector<std::string>& _strings, const std::string _parameter_name);
	template <class Tparameter>
	static bool read(std::vector<Tparameter>& _parameters, const unsigned int _Nparameters, const std::vector<std::string>& _strings, const std::string _parameter_name);
	template <class Tparameter, std::size_t N>
	static bool read(std::array<Tparameter, N>& _parameters, const std::vector<std::string>& _strings, const std::string _parameter_name);
	template <class Tparameter>
	static bool read(std::pair<Tparameter, Tparameter>& _parameter, const std::vector<std::string>& _strings, const std::string _parameter_name);
};

template <class Tparameter>
bool ConfigParameter::read(Tparameter& _parameter, const std::vector<std::string>& _strings, const std::string _parameter_name) {

	std::vector<Tparameter> parameters(1);

	if (read(parameters, 1, _strings, _parameter_name)) {
		_parameter = parameters[0];
		return true;
	} else {
		return false;
	}

}

template <class Tparameter>
bool ConfigParameter::read(std::vector<Tparameter>& _parameters, const unsigned int _Nparameters, const std::vector<std::string>& _strings, const std::string _parameter_name) {

	if (_Nparameters == _strings.size()) {

		_parameters.resize(_Nparameters);

		for (unsigned int i = 0; i < _Nparameters; i++) {

			_parameters[i] = Misc::str_to<Tparameter>(_strings[i]);

		}

		return true;

	} else {

		std::cout << "Wrong number of parameters when reading " << _parameter_name << ". Expect " << _Nparameters << ", but " << _strings.size() << " are provided" << std::endl;
		return false;
	}

}

template <class Tparameter, std::size_t N>
bool ConfigParameter::read(std::array<Tparameter, N>& _parameters, const std::vector<std::string>& _strings, const std::string _parameter_name) {

	std::vector<Tparameter> parameters;
	if (read(parameters, N, _strings, _parameter_name)) {
		for (unsigned int n = 0; n < N; n++) {
			_parameters[n] = parameters[n];
		}
		return true;
	} else {
		return false;
	}

}

template <class Tparameter>
bool ConfigParameter::read(std::pair<Tparameter, Tparameter>& _parameter, const std::vector<std::string>& _strings, const std::string _parameter_name) {

	std::vector<Tparameter> parameters;
	if (read(parameters, 2, _strings, _parameter_name)) {
		_parameter.first = parameters[0];
		_parameter.second = parameters[1];
		return true;
	} else {
		return false;
	}

}
