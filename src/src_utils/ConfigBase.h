/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/


#pragma once

#include <string>
#include <vector>

/*! Base class for configuration class dedicated to read parameters in a config text file. Contains members and methods common to config classes.*/
class ConfigBase {

protected :

	/*! Path of the cfg file.*/
	std::string config_path;
	/*! Default path of the cfg file.*/
	static const std::string default_config_path;
	/*! Directory in which config file is found.*/
	//static std::string default_config_directory;


	/*! Defines comment character.*/
	static const char comment_char;
	/*! Defines parameter declarator string.*/
	static const std::string parameter_separator;

public:

	ConfigBase();
	~ConfigBase();

	/*! Read config file pointed by \p #_cfg_path.*/
	bool read_cfg_file(const std::string _cfg_path);// , bool _l_default_location = false);
	/*! Read config file provided by CLI arguments.*/
	bool read_cfg(int argc, char** argv);

	const std::string& get_config_path() const;
	/*! Extracts directory from config_path.*/
	std::string get_config_directory() const;

	static void display_unknown_parameter(const std::string& _parameter_name);

	/*! List of files which have been read.*/
	static std::vector<std::string> read_files;

	static void write_config_files(const std::string& _cfg_path);

private :

	/*! Deduce \p _parameter_name from read \p _sub_strings. Returns read status.*/
	static bool analyse_sub_strings(const std::vector<std::string>& _sub_strings, std::string& _parameter_name);
	/*! Check if \p _sub_strings is ok for reading parameter value.*/
	static bool check_sub_strings_value(const std::vector<std::string>& _sub_strings);
	/*! Assign value contained in \p _sub_strings to parameter \p _parameter_name.*/
	virtual bool set_value(const std::string& _parameter_name, const std::vector<std::string>& _sub_strings)=0;
	/*! Deduce other values depending on already read ones.*/
	virtual void deduce_values() = 0;
	/*! Display error message if reading issue.*/
	virtual void error_message() const = 0;
	/*! Check if all parameters have been read (possible omission in config file).*/
	virtual std::vector<std::string> check_read_all_parameters() const = 0;

protected :
	/*! Check if \p _sub_strings correspond to an empty line (void value for parameter).*/
	static bool is_empty_line(const std::vector<std::string>& _sub_strings);
};

