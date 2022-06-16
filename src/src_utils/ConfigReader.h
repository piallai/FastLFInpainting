/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/


#pragma once

#include "ConfigParameters.h"

/*! Convenience structure to easily read parameters of a class.*/
struct ConfigReader {

	/*! Assign parameters to corresponding class.*/
	template <class Tclass>
	static bool read(Tclass& _class, int argc, char** argv);
	template <class Tclass>
	static bool read(Tclass& _class, const std::string& _cfg_path);

	/*! Assign parameters directly.*/
	template <class Tclass>
	static bool read(typename Tclass::Parameters& _parameters, int argc, char** argv);
	template <class Tclass>
	static bool read(typename Tclass::Parameters& _parameters, const std::string& _cfg_path);
};

template <class Tclass>
bool ConfigReader::read(Tclass& _class, int argc, char** argv) {

	ConfigParameters<Tclass> config;
	return config.set(_class, argc, argv);

}

template <class Tclass>
bool ConfigReader::read(Tclass& _class, const std::string& _cfg_path) {

	ConfigParameters<Tclass> config;
	return config.set(_class, _cfg_path);

}

template <class Tclass>
bool ConfigReader::read(typename Tclass::Parameters& _parameters, int argc, char** argv) {

	ConfigParameters<Tclass> config;
	return config.set(_parameters, argc, argv);
}

template <class Tclass>
bool ConfigReader::read(typename Tclass::Parameters& _parameters, const std::string& _cfg_path) {

	ConfigParameters<Tclass> config;
	return config.set(_parameters, _cfg_path);
}