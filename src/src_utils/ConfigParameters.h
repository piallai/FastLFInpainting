/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/


#pragma once

#include "ConfigParametersBase.h"
#include "ConfigParametersSpecializations.h"

/*! Class to specialize for each Tclass containing Parameters class.*/
template <class Tclass>
class ConfigParameters : public ConfigParametersBase<typename Tclass::Parameters> {

	/*! name of parameters that I have been read when reading a config file.*/
	std::vector<std::string> read_parameters;

public:
	ConfigParameters();
	ConfigParameters(const std::string& _cfg_path);
	ConfigParameters(Tclass& _class, const std::string& _cfg_path);
	~ConfigParameters();

	/*! To set class parameters. Supposed to be run only once for master config file.*/
	bool set(Tclass& _class, int argc, char** argv);
	/*! To set class parameters.*/
	bool set(Tclass& _class, const std::string& _cfg_path);
	/*! To set class parameters. Supposed to be run only once for master config file.*/
	bool set(typename Tclass::Parameters& _parameters, int argc, char** argv);
	/*! To set class parameters.*/
	bool set(typename Tclass::Parameters& _parameters, const std::string& _cfg_path);

private:

	/*! Pure virtual methods from ConfigBase*/
	/*! Assign values in \p _sub_strings to corresponding \p _parameter_name.*/
	bool set_value(const std::string& _parameter_name, const std::vector<std::string>& _sub_strings);
	void deduce_values();
	void error_message() const;
	std::vector<std::string> check_read_all_parameters() const;
	/*! Check if a config is compatible with another.*/
	template <class Tclass_arg>
	bool check(const ConfigParameters<Tclass_arg>& _config);
};

template <class Tclass>
ConfigParameters<Tclass>::ConfigParameters() {

}

template <class Tclass>
ConfigParameters<Tclass>::ConfigParameters(const std::string& _cfg_path) {

	this->read_cfg_file(_cfg_path);
}

template <class Tclass>
ConfigParameters<Tclass>::ConfigParameters(Tclass& _class, const std::string& _cfg_path) {

	if (this->read_cfg_file(_cfg_path)) {
		_class.set_parameters(this->get_parameters());
	}
	
}

template <class Tclass>
ConfigParameters<Tclass>::~ConfigParameters() {

}

template <class Tclass>
bool ConfigParameters<Tclass>::set(Tclass& _class, const std::string& _cfg_path) {

	read_parameters.clear();

	if (this->read_cfg_file(_cfg_path)) {//, true)) {
		_class.set_parameters(this->get_parameters());
		return true;
	} else {
		return false;
	}

}

template <class Tclass>
bool ConfigParameters<Tclass>::set(Tclass& _class, int argc, char** argv) {

	read_parameters.clear();

	if (this->read_cfg(argc, argv)) {
		_class.set_parameters(this->get_parameters());
		return true;
	} else {
		return false;
	}

}

template <class Tclass>
bool ConfigParameters<Tclass>::set(typename Tclass::Parameters& _parameters, const std::string& _cfg_path) {

	read_parameters.clear();

	if (this->read_cfg_file(_cfg_path)) {//, true)) {
		_parameters = this->get_parameters();
		return true;
	} else {
		return false;
	}

}

template <class Tclass>
bool ConfigParameters<Tclass>::set(typename Tclass::Parameters& _parameters, int argc, char** argv) {

	read_parameters.clear();

	if (this->read_cfg(argc, argv)) {
		_parameters = this->get_parameters();
		return true;
	} else {
		return false;
	}

}

template <class Tclass>
void ConfigParameters<Tclass>::deduce_values() {

	ConfigParametersSpecializations<Tclass>::deduce_values(this->parameters);
}

template <class Tclass>
void ConfigParameters<Tclass>::error_message() const {

	ConfigParametersSpecializations<Tclass>::error_message();
}

template <class Tclass>
std::vector<std::string> ConfigParameters<Tclass>::check_read_all_parameters() const {

	return ConfigParametersSpecializations<Tclass>::check_read_parameters(read_parameters);
}

template <class Tclass>
bool ConfigParameters<Tclass>::set_value(const std::string& _parameter_name, const std::vector<std::string>& _sub_strings) {

	bool l_keep_reading = ConfigParametersSpecializations<Tclass>::set_value(this->parameters, _parameter_name, _sub_strings, this->get_config_directory());
	if (l_keep_reading) {
		read_parameters.push_back(_parameter_name);
	}
	return l_keep_reading;
}

template <class Tclass>
template <class Tclass_arg>
bool ConfigParameters<Tclass>::check(const ConfigParameters<Tclass_arg>& _config) {

	return ConfigParametersSpecializations<Tclass>::check(_config);
}