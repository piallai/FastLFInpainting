/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "ConfigBase.h"
#include <iostream>
#include <fstream>
#include "misc_funcs.h"

/*! Defines comment character.*/
const char ConfigBase::comment_char = '#';
/*! Defines parameter declarator string.*/
const std::string ConfigBase::parameter_separator = std::string(1,comment_char)+"=#";

const std::string ConfigBase::default_config_path = "./cfg/cfg_master.txt";
//std::string ConfigBase::default_config_directory = ".";

/*! List of files which have been read.*/
std::vector<std::string> ConfigBase::read_files;

ConfigBase::ConfigBase() {}


ConfigBase::~ConfigBase() {}

const std::string& ConfigBase::get_config_path() const {

	return config_path;
}

std::string ConfigBase::get_config_directory() const {

	return Misc::extract_directory_from_path(config_path);
}

bool ConfigBase::analyse_sub_strings(const std::vector<std::string>& _sub_strings, std::string& _parameter_name) {

	bool l_keep_reading = true;

	if (_sub_strings.size() > 1 && !_sub_strings[0].empty() && _sub_strings[0][0] == comment_char && _sub_strings[0] == parameter_separator) {
		if (_sub_strings.size() > 1) {
			_parameter_name = _sub_strings[1];
		} else {
			std::cout << parameter_separator << " should be followed by a parameter name" << std::endl;
			l_keep_reading = false;
		}
	} else {
		_parameter_name.clear();
	}

	return l_keep_reading;
}

bool ConfigBase::check_sub_strings_value(const std::vector<std::string>& _sub_strings) {

	return _sub_strings.size() == 0 || (!_sub_strings[0].empty() && _sub_strings[0][0] != comment_char) || _sub_strings[0].empty();
}

bool ConfigBase::read_cfg(int argc, char** argv) {

	bool l_read;

	std::string file_path;
	if (argc > 1) {

		if (argc > 2) {
			std::cout << "Only one argument is needed for loading config file. Ignoring others." << std::endl;
		}

		file_path = argv[1];
		l_read = read_cfg_file(file_path);

	} else {

		file_path = Misc::to_data_path(default_config_path);
		l_read = read_cfg_file(file_path);

	}

	if (!l_read) {
		std::cout << "Can't read config file " << file_path << std::endl;
		error_message();
	}

	return l_read;
}

bool ConfigBase::read_cfg_file(const std::string _cfg_path) {

	if (Misc::l_verbose_low) {
		std::cout << "Reading config file" << std::endl;
	}

	this->config_path = _cfg_path;
	std::string line;
	std::ifstream file(this->config_path);
	read_files.push_back(this->config_path);

	unsigned int count = 0;
	bool l_keep_reading = true;
	std::vector<std::string> sub_strings;
	std::string parameter_name;
	bool l_read_parameter_name = true;
	if (file.is_open()) {
		/*! While there's a line to get in the file.*/
		while (getline(file, line) && l_keep_reading) {

			/*! Removes carriage return. Usefull on linux.*/
			if (!line.empty() && (int)line.back() == 13) {
				line.pop_back();
			}

			/*! Gets vector of strings separated by a space.*/
			sub_strings = Misc::get_sub_strings(line);

			if (l_read_parameter_name) {
				/*! Read parameter name for next line to be read, or comment line if preceded by only comment_char.*/
				l_keep_reading = analyse_sub_strings(sub_strings, parameter_name);
				if (!parameter_name.empty()) {
					/*! If a parameter name has been read, then switch mode.*/
					l_read_parameter_name = false;
				}
			} else {
				/*! Reads parameter value.*/
				if (check_sub_strings_value(sub_strings)) {
					l_keep_reading = set_value(parameter_name, sub_strings);
					if (!l_keep_reading) {
						std::cout << "Problem reading parameter : " << parameter_name << std::endl;
					}
					l_read_parameter_name = true;
				}
			}


			count++;
		}/*! End of file.*/

		/*! If config file was properly read, then also checks if there are parameters which have not been read.*/
		if (l_keep_reading) {
			std::vector<std::string> unread_parameters = check_read_all_parameters();
			if (unread_parameters.size() > 0) {
				l_keep_reading = false;
				std::cout << "Failed to read missing parameters : ";
				for (unsigned int i = 0; i < unread_parameters.size(); i++) {
					std::cout << unread_parameters[i];
					if (i < unread_parameters.size() - 1) {
						std::cout << ", ";
					} else {
						std::cout << std::endl;
					}
				}
			}
		}

		/*! Deduce other values depending on already read ones.*/
		deduce_values();

		file.close();

		if (!l_keep_reading) {
			std::cout << "Failed to read " << this->config_path << std::endl;
		}

		return l_keep_reading;

	} else {
		
		std::cout << "Unable to open configuration file : " << this->config_path << std::endl;
		return false;
	}

}

bool ConfigBase::is_empty_line(const std::vector<std::string>& _sub_strings) {

	return _sub_strings.size() == 1 && _sub_strings[0].find_first_not_of(" ") == std::string::npos;
}

void ConfigBase::display_unknown_parameter(const std::string& _parameter_name) {

	std::cout << "parameter : " << _parameter_name << " is unknown" << std::endl;
}

void ConfigBase::write_config_files(const std::string& _cfg_path) {

	Misc::create_directory(_cfg_path);
	Misc::delete_files_in_directory(Misc::to_data_path(_cfg_path), ".txt");
	for (unsigned int f = 0; f < ConfigBase::read_files.size(); f++) {
		Misc::copy_file_in_directory(ConfigBase::read_files[f], Misc::to_data_path(_cfg_path));
	}

}