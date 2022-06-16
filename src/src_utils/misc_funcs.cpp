/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/


#include "misc_funcs.h"

/*! Bug in old versions of boost*/
#include <boost/version.hpp>
#if BOOST_VERSION < 106100
#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif
#include <boost/filesystem.hpp>
#if BOOST_VERSION < 106100
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#endif

std::string Misc::data_path = "../../../../DATA";
bool Misc::l_verbose_high = false;
bool Misc::l_verbose_low = false;

std::string Misc::get_file_extension(const std::string& _name) {

	size_t position = _name.find_last_of('.');

	if (position != std::string::npos) {
		return _name.substr(position, _name.size() - position);
	} else {
		return "";
	}

}

std::string Misc::remove_file_extension(const std::string& _name) {

	std::string name_no_ext = _name;
	std::string ext = get_file_extension(_name);

	if (!ext.empty()) {
		name_no_ext.erase(_name.find(ext));
	}

	return name_no_ext;
}

bool Misc::is_directory(const std::string& _name) {


	namespace fs = boost::filesystem;
	fs::path directory(_name);

	return fs::exists(directory) && fs::is_directory(directory);

}

std::string Misc::extract_name_from_path(const std::string& _path) {

	std::size_t found = _path.find_last_of("/\\");
	return _path.substr(found + 1);
}

std::string Misc::extract_directory_from_path(const std::string& _path) {

	std::size_t found = _path.find_last_of("/\\");
	return _path.substr(0, found);
}

std::string Misc::concat_path_and_filename(const std::string& _path, const std::string& _file_name) {

	std::string path;

	boost::filesystem::path path1(_path);
	boost::filesystem::path path2(_file_name);
	if (path2.is_relative()) {
		boost::filesystem::path full_path = path1 / path2;
		path = full_path.string();
	} else {
		path = _file_name;
	}

	return path;
}

std::string Misc::concat_paths(const std::string& _path1, const std::string& _path2) {

	std::string path = concat_path_and_filename(_path1, _path2);
	if (path.back() != '/' && path.back() != '\\') {

	}
	path += "/";

	return path;
}

std::string Misc::concat_paths(const std::string& _path1, const std::string& _path2, const std::string& _path3) {

	return concat_paths(concat_paths(_path1, _path2), _path3);
}

std::string Misc::concat_paths(const std::string& _path1, const std::string& _path2, const std::string& _path3, const std::string& _path4) {

	return concat_paths(concat_paths(_path1, _path2, _path3), _path4);
}

std::string Misc::to_data_path(const std::string& _file_name) {

	return concat_path_and_filename(data_path, _file_name);
}

bool Misc::create_directory(const std::string& _path) {

	boost::filesystem::path dir(Misc::to_data_path(_path));
	if (!boost::filesystem::is_directory(_path)) {
		boost::system::error_code error;
		bool l_created = boost::filesystem::create_directories(dir, error);

		if (error) {
			error.message();
			displug(error);
		}

		return l_created;
	} else {
		return true;
	}
	
}

void Misc::copy_file(const std::string& _file_path, const std::string& _destination_path) {

	boost::filesystem::path file_path(_file_path);
	boost::filesystem::path destination_path(_destination_path);
	boost::filesystem::copy_file(file_path, destination_path, boost::filesystem::copy_option::overwrite_if_exists);
}

void Misc::copy_file_in_directory(const std::string& _file_path, const std::string& _destination_directory_path) {

	if (Misc::is_directory(_destination_directory_path)) {
		std::string file_name = Misc::extract_name_from_path(_file_path);
		Misc::copy_file(_file_path, Misc::concat_path_and_filename(_destination_directory_path, file_name));
	} else {
		std::cout << "Misc::copy_file_in_directory : _destination_directory_path = " << _destination_directory_path << ", is not a valid directory" << std::endl;
	}

}

void Misc::delete_files_in_directory(const std::string& _directory_path, const std::string _file_extension) {

	namespace fs = boost::filesystem;

	if (Misc::is_directory(_directory_path)) {

		bool l_extension_filter = !_file_extension.empty();

		fs::path directory(_directory_path);
		fs::directory_iterator end;
		for (fs::directory_iterator it(directory); it != end; ++it) {
			if (fs::is_regular_file(it->status()) && (!l_extension_filter || it->path().extension() == _file_extension)) {
				fs::remove(it->path());
			}
		}
	} else {
		std::cout << "Misc::delete_files_in_directory : _directory_path = " << _directory_path << ", is not a valid directory" << std::endl;
	}
}

/*! Return file names relatively to _directory_path (doesn't return full path).*/
std::vector<std::string> Misc::get_file_names(const std::string& _directory_path, FileType _file_type) {

	std::vector<std::string> file_names;

	namespace fs = boost::filesystem;
	fs::path directory(_directory_path);
	fs::directory_iterator end_iter;

	if (fs::exists(directory) && fs::is_directory(directory)) {
		for (fs::directory_iterator dir_iter(directory); dir_iter != end_iter; dir_iter++) {
			if ((fs::is_regular_file(dir_iter->status()) && _file_type == FileType::file) || (fs::is_directory(dir_iter->status()) && _file_type == FileType::directory)) {
				file_names.push_back(extract_name_from_path(dir_iter->path().string()));
			}
		}
	}

	return file_names;

}

std::vector<std::string> Misc::image_extensions = { ".png", ".jpg", ".jpeg", ".bmp", ".tiff" };
std::vector<std::string> Misc::video_extensions = { ".avi", ".mpg", ".mpeg", ".bmp", ".mp4" };

bool Misc::is_image_file(const std::string& _file_name) {

	std::string file_extension = get_file_extension(_file_name);
	unsigned int i = 0;
	while (i < image_extensions.size() && file_extension != image_extensions[i]) {
		i++;
	}
	return i != image_extensions.size();

}

bool Misc::is_video_file(const std::string& _file_name) {

	std::string file_extension = get_file_extension(_file_name);
	unsigned int i = 0;
	while (i < video_extensions.size() && file_extension != video_extensions[i]) {
		i++;
	}
	return i != video_extensions.size();

}

std::vector<std::string> Misc::get_images_in_path(const std::string& _path) {

	std::vector<std::string> file_names = Misc::get_file_names(_path, Misc::FileType::file);

	std::vector<std::string>::iterator iterator = file_names.begin();
	while (iterator != file_names.end()) {
		if (!Misc::is_image_file(*iterator)) {
			iterator = file_names.erase(iterator);
		} else {
			iterator++;
		}
	}

	return file_names;
}

std::vector<std::string> Misc::get_images_in_path_recursive(const std::string& _path, bool _l_recursive) {

	std::vector<std::string> images_path;

	images_path = get_images_in_path(_path);

	if (_l_recursive) {

		std::vector<std::string> directory_names = get_file_names(_path, Misc::FileType::directory);

		std::vector<std::string> images_in_directory;

		for (unsigned int i = 0; i < directory_names.size(); i++) {

			images_in_directory = get_images_in_path_recursive(Misc::concat_path_and_filename(_path, directory_names[i]));

			for (unsigned int j = 0; j < images_in_directory.size(); j++) {
				images_in_directory[j] = Misc::concat_path_and_filename(directory_names[i], images_in_directory[j]);
			}

			Misc::add_vector(images_path, images_in_directory);

		}

	}

	return images_path;

}


std::vector<std::string> Misc::get_sub_strings(const std::string& _string, const char _separator) {

	std::vector<std::string> sub_strings;
	std::string string(_string);

	size_t pos;
	/*! Shift string if starts by _separator.*/
	if (string[0] == _separator) {
		pos = string.find_first_not_of(_separator);
		if (pos != std::string::npos) {
			string = string.substr(pos);
		} else {
			string.clear();
		}
	} else {
		pos = 0;
	}
	
	while (pos != std::string::npos) {

		pos = string.find_first_of(_separator);
		/*! Assign sub string.*/
		sub_strings.push_back(string.substr(0, pos));
		/*! Remove sub string from string.*/
		if (pos != std::string::npos) {
			string = string.substr(pos);
		} else {
			string.clear();
		}

		if (!string.empty()) {
			/*! Need to check if string is empty, otherwise pos is set to 0. Looks like a bug.*/
			pos = string.find_first_not_of(_separator);
		} else {
			pos = std::string::npos;
		}

		if (pos != std::string::npos) {
			string = string.substr(pos);
		}

	}

	return sub_strings;
}

std::string Misc::int_to_string(const unsigned int i, const unsigned int _Ndigits) {

	std::string i_string = to_string(i);
	if (_Ndigits == 1) {
	} else if (_Ndigits == 2) {
		if (i < 10) {
			i_string = std::string("0") + i_string;
		}
	} else if (_Ndigits == 3) {
		if (i < 100) {
			i_string = std::string("0") + i_string;
			if (i < 10) {
				i_string = std::string("0") + i_string;
			}
		}
	} else if (_Ndigits == 4) {
		if (i < 1000) {
			i_string = std::string("0") + i_string;
			if (i < 100) {
				i_string = std::string("0") + i_string;
				if (i < 10) {
					i_string = std::string("0") + i_string;
				}
			}
		}
	} else if (_Ndigits == 5) {
		if (i < 10000) {
			i_string = std::string("0") + i_string;
			if (i < 1000) {
				i_string = std::string("0") + i_string;
				if (i < 100) {
					i_string = std::string("0") + i_string;
					if (i < 10) {
						i_string = std::string("0") + i_string;
					}
				}
			}
		}
	} else if (_Ndigits == 6) {
		if (i < 100000) {
			i_string = std::string("0") + i_string;
			if (i < 10000) {
				i_string = std::string("0") + i_string;
				if (i < 1000) {
					i_string = std::string("0") + i_string;
					if (i < 100) {
						i_string = std::string("0") + i_string;
						if (i < 10) {
							i_string = std::string("0") + i_string;
						}
					}
				}
			}
		}
	}

	return i_string;
}

std::vector<unsigned int> Misc::get_numbers(std::string _string) {

	std::vector<unsigned int> numbers;

	static std::string numeric_values = "0123456789";

	std::size_t pos = 0;

	while (pos != std::string::npos) {

		pos = _string.find_first_of(numeric_values);
		if (pos != std::string::npos) {
			_string = _string.substr(pos, _string.size());
			pos = _string.find_first_not_of(numeric_values);
			if (pos != std::string::npos) {
				numbers.push_back(Misc::str_to<unsigned int>(_string.substr(0, pos)));
				_string = _string.substr(pos, _string.size());
			} else {
				numbers.push_back(Misc::str_to<unsigned int>(_string));
			}
		}
	}

	return numbers;

}

std::vector<unsigned int> Misc::get_count_vector(unsigned int _N) {

	std::vector<unsigned int> count_vector;
	for (unsigned int i = 0; i < _N; i++) {
		count_vector.push_back(i);
	}
	return count_vector;
}

void Misc::filter_strings(std::vector<std::string>& _strings, const std::vector<std::string>& _filter_strings) {


	for (unsigned int i = 0; i < _strings.size();) {
		unsigned int s = 0;
		while (s < _filter_strings.size() && _strings[i].find(_filter_strings[s]) == std::string::npos) {
			s++;
		}
		if (s != _filter_strings.size()) {
			_strings.erase(_strings.begin() + i);
		} else {
			i++;
		}

	}

}

template <>
bool Misc::str_to_value<bool>(const std::string& _string) {

	return std::stoul(_string);
}

template <>
unsigned int Misc::str_to_value<unsigned int>(const std::string& _string) {

	return std::stoul(_string);
}

template <>
int Misc::str_to_value<int>(const std::string& _string) {

	return std::stoi(_string);
}

template <>
float Misc::str_to_value<float>(const std::string& _string) {

	return std::stof(_string);
}

template <>
double Misc::str_to_value<double>(const std::string& _string) {

	return std::stod(_string);
}

template <>
std::string Misc::str_to_value<std::string>(const std::string& _string) {

	return _string;
}


bool Misc::is_verbose() {
	return true;
}

void Misc::display_progression(const unsigned int _i_loop, const unsigned int _Nloop, const float _ratio_modulo) {

	/*! If delta is null, means ratio_modulo is too small for _Nloop. All iterations will be displayed then.*/
	unsigned int delta = (unsigned int)(_Nloop / _ratio_modulo);
	if (delta == 0 || _i_loop % delta == 0 || _i_loop == _Nloop - 1) {
		if (_Nloop != 1) {
			std::cout << float(_i_loop) / float(_Nloop - 1)*100. << "%";
			if (_i_loop < _Nloop - 1) {
				std::cout << " - ";
			}
		}
	}
	
	if (_i_loop == _Nloop - 1) {
		std::cout << std::endl;
	}
}
