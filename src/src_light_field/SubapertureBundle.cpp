/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "SubapertureBundle.h"
#include "misc_funcs.h"

unsigned int& SubapertureBundle::Nimages_auto() {
	static unsigned int Nimages_auto = 256;
	return Nimages_auto;
}

float& SubapertureBundle::coef_std_Nimages_auto() {
	static float coef_std_Nimages_auto = 0.25;
	return coef_std_Nimages_auto;
}

float& SubapertureBundle::coef_std_ratio_auto() {
	static float coef_std_ratio_auto = 0.5;
	return coef_std_ratio_auto;
}

const unsigned int SubapertureBundle::no_time_index = -1;

SubapertureBundle::SubapertureBundle() {

	clear();
}

SubapertureBundle::SubapertureBundle(const Parameters& _parameters):SubapertureBundle() {

	set_parameters(_parameters);
}

SubapertureBundle::~SubapertureBundle() {

}

const SubapertureBundle::Parameters& SubapertureBundle::get_parameters() const {

	return parameters;
}

void SubapertureBundle::set_parameters(const Parameters& _parameters) {

	parameters = _parameters;
	analyse_parameters();
}

void SubapertureBundle::clear() {

	l_time = false;
	time_position = -1;
	Nuv_images = 0;
	uv_min = UVindices(0, 0);
	uv_max = UVindices(0, 0);
	Nu = 0;
	Nv = 0;
	time_index = -1;
	time_index_min = -1;
	time_index_max = -1;
	time_indices_var = 0;

	uv_images_mapping.clear();

	analyse_parameters();
}

void SubapertureBundle::analyse_parameters() {

	if (parameters.l_single_index_for_subapertures) {
		if (parameters.uvt_position_in_name.size() == 2) {
			l_time = true;
			time_position = parameters.uvt_position_in_name[1];
		} else {
			l_time = false;
			time_position = -1;
		}
	} else {
		if (parameters.uvt_position_in_name.size() == 3) {
			l_time = true;
			time_position = parameters.uvt_position_in_name[2];
		} else {
			l_time = false;
			time_position = -1;
		}
	}
}

bool SubapertureBundle::is_time() const {

	return l_time;
}

unsigned int SubapertureBundle::get_time_position() const {

	return time_position;
}


void SubapertureBundle::analyse_mapping() {

	/*! Looks up mapping to compute u,v dimensions.*/

	Nuv_images = 0;

	uv_min.first = -1;
	uv_min.second = -1;
	uv_max.first = 0;
	uv_max.second = 0;

	UVindices uv;
	for (Tmapping::const_iterator iterator = uv_images_mapping.begin(); iterator != uv_images_mapping.end(); ++iterator) {

		Nuv_images++;

		uv = iterator->first;

		if (uv.first > uv_max.first) {
			uv_max.first = uv.first;
		}
		if (uv.first < uv_min.first) {
			uv_min.first = uv.first;
		}
		if (uv.second > uv_max.second) {
			uv_max.second = uv.second;
		}
		if (uv.second < uv_min.second) {
			uv_min.second = uv.second;
		}
	}

	if (uv_min.first != (unsigned int)-1) {
		Nu = uv_max.first - uv_min.first + 1;
	} else {
		Nu = 0;
	}
	if (uv_min.second != (unsigned int)-1) {
		Nv = uv_max.second - uv_min.second + 1;
	} else {
		Nv = 0;
	}

	if (uv_min.first != 0 || uv_min.second != 0) {
		/*! Apply min/max offset.*/
		Tmapping mapping_buff;
		for (Tmapping::const_iterator iterator = uv_images_mapping.begin(); iterator != uv_images_mapping.end(); ++iterator) {

			uv.first = iterator->first.first - uv_min.first;
			uv.second = iterator->first.second - uv_min.second;

			mapping_buff[uv] = iterator->second;
		}

		uv_images_mapping = mapping_buff;
	}

}

bool SubapertureBundle::map(Tmapping& _uv_images_mapping, const UVindices& _uv, const std::string& _image_path) {

	/*! If u,v indices are valid.*/
	if (check_uv_indices(_uv)) {

		/*! Find iterator to _uv.*/
		std::map<UVindices, std::string>::iterator iterator = _uv_images_mapping.find(_uv);
		/*! If iterator is valid.*/
		if (iterator == _uv_images_mapping.end()) {
			/*! Adds _image_path to mapping with key _uv.*/
			_uv_images_mapping[_uv] = _image_path;
			return true;

		} else {

			if (Misc::l_verbose_low) {

				std::cout << "Conflicting images for uv = " << _uv << std::endl;
				std::cout << "image : " << iterator->second << std::endl;
				std::cout << "image : " << _image_path << std::endl;

			}

			return false;
		}

	} else {
		return false;
	}
}

UVindices SubapertureBundle::calc_UVdims(const std::vector<unsigned int>& _indices, bool _l_begin0) {

	return calc_UVdims(Misc::calc_minmax(_indices), _l_begin0);
}

UVindices SubapertureBundle::calc_UVdims(const std::pair<unsigned int, unsigned int>& _minmax_indices, bool _l_begin0) {

	UVindices UVdims;

	/*! Total number of sub-apertures corresponds to index_max+1. But _indices can be sparse.*/
	unsigned int Nsubapertures;
	if (_l_begin0) {//**
		Nsubapertures = _minmax_indices.second + 1;
	} else {
		Nsubapertures = _minmax_indices.second - _minmax_indices.first + 1;
	}


	/*!For now, U V dimensions are just deduced, can be improved later using a config parameter.
	U is supposed to be larger than V.
	Look for maximum U*V product.*/

	UVdims.second = (unsigned int)std::floor(std::sqrt(Nsubapertures));
	while (Nsubapertures%UVdims.second != 0 && UVdims.second > 1) {
		UVdims.second--;
	}
	UVdims.first = Nsubapertures / UVdims.second;

	if (Misc::l_verbose_low) {
		std::cout << "Auto find, UVdims = " << UVdims << std::endl;
	}


	return UVdims;

}

UVindices SubapertureBundle::get_uv_index(const unsigned int _index, const UVindices& _UVdims) {

	/*! if set to true, read u direction first.*/
	bool l_mode = true;
	UVindices UVdims_ = _UVdims;
	if (l_mode) {
		std::swap(UVdims_.first, UVdims_.second);
	}
	
	UVindices uv;
	if (UVdims_.first > 1) {
		uv.second = _index % UVdims_.second;
		uv.first = (_index - uv.second) / UVdims_.second;
	} else {
		uv.first = 0;
		uv.second = _index;
	}

	if (uv.first >= UVdims_.first || uv.second >= UVdims_.second) {
		if (Misc::l_verbose_low) {
			std::cout << "Problem with uv index : uv = " << uv << ", UVdims_ = " << UVdims_ << std::endl;
		}
		uv.first = -1;
		uv.second = -1;
	}

	return uv;

}

std::vector<UVindices> SubapertureBundle::get_uv_indices(const std::vector<unsigned int>& _indices, bool _l_begin0) {

	std::vector<UVindices> uv_indices;

	/*! Deduce dimensions in u,v axis.*/
	UVindices UVdims = calc_UVdims(_indices, _l_begin0);

	for (unsigned int i = 0; i < _indices.size(); i++) {

		/*! Convert index into u,v coordinates using dimensions in u,v axis.*/
		uv_indices.push_back(get_uv_index(_indices[i], UVdims));
	}

	return uv_indices;
}

bool SubapertureBundle::read_subapertures(const std::string& _path, bool _l_recursive, unsigned int _time_index, const std::vector<std::string> _filter_strings) {

	/*! _path has to be a directory.*/
	if (Misc::is_directory(_path)) {

		/*! Gets all images' path in _path.*/
		std::vector<std::string> images_paths = Misc::get_images_in_path_recursive(_path, _l_recursive);

		Misc::filter_strings(images_paths, _filter_strings);

		return read_subapertures(images_paths, _path, _time_index);

	} else {
		std::cout << _path << " is not a directory. Can't read subapertures." << std::endl;
		return false;
	}

}

bool SubapertureBundle::read_subapertures(const std::vector<std::string>& _images_paths, const std::string& _path, unsigned int _time_index) {

	clear();

	bool l_valid_time_index = true;
	/*! If _file_reading is not configured for handling a time index, then discard _time_index.*/
	if (!is_time()) {

		_time_index = -1;
		time_index = no_time_index;
		time_index_min = no_time_index;
		time_index_max = no_time_index;
		time_indices_var = 0;

	} else {

		unsigned int time_index_read;
		std::vector<unsigned int> time_indices;
		time_index_min = std::numeric_limits<unsigned int>::max();
		time_index_max = std::numeric_limits<unsigned int>::lowest();
		/*! For each image path.*/
		for (unsigned int i = 0; i < _images_paths.size(); i++) {
			/*! Get time index in image path using _file_reading.time_position.*/
			time_index_read = SubapertureBundle::get_index_from_file_name(_images_paths[i], get_time_position());
			/*! Adds read time inde to others.*/
			time_indices.push_back(time_index_read);
			/*! Check for minimum.*/
			if (time_index_read < time_index_min) {
				time_index_min = time_index_read;
			}
			if (time_index_read > time_index_max) {
				time_index_max = time_index_read;
			}

		}
		/*! While performing this, compute variation of time indices. Usefull for SubapertureBundle and likeliness coefficient.*/
		/*! Sort time indices in ascending order.*/
		std::sort(time_indices.begin(), time_indices.end());
		/*! Compute time indices offsets.*/
		std::vector<unsigned int> dt;
		for (unsigned int i = 0; i < time_indices.size() - 1; i++) {
			dt.push_back(time_indices[i + 1] - time_indices[i]);
		}
		/*! Compute quadratic variation of offsets.*/
		time_indices_var = (float)Misc::get_var(dt);

		if (_time_index == no_time_index) {
			time_index = time_index_min;
		} else {
			time_index = _time_index;
			if (time_index < time_index_min || time_index > time_index_max) {
				l_valid_time_index = false;
			}
		}

	}

	/*! Now that time_index is properly set (wether it exists or not (-1)), performs mapping of images paths with u,v coordinates.*/
	if (l_valid_time_index && set_mapping(_images_paths, time_index)) {
		/*! Adds relative path to images path so that mapping provides directly pointer to images.*/
		for (SubapertureBundle::Tmapping::iterator iterator = uv_images_mapping.begin(); iterator != uv_images_mapping.end(); ++iterator) {
			iterator->second = Misc::concat_path_and_filename(_path, iterator->second);
		}

		analyse_mapping();
		return true;
	} else {
		clear();
		return false;
	}

}

bool SubapertureBundle::set_mapping(const std::vector<std::string>& _images_paths, unsigned int _time_index) {

	/*! Check wether time information are consistant.*/

	UVindices uv;
	unsigned int camera_index;
	std::vector<unsigned int> camera_indices;
	std::pair<unsigned int, unsigned int> minmax_indices;
	unsigned int time_index_read;

	/*! Check UVdims.*/
	UVindices UVdims;
	/*! If _file_reading read single index sub-apertures.*/
	if (parameters.l_single_index_for_subapertures) {
		/*! If UVdims is null and then supposed not defined.*/
		if (parameters.UVdims.first == 0 || parameters.UVdims.second == 0) {
			/*! Read all sub-aperture indices.*/
			for (unsigned int i = 0; i < _images_paths.size(); i++) {
				camera_index = SubapertureBundle::get_index_from_file_name(_images_paths[i], parameters.uvt_position_in_name[0]);
				if (SubapertureBundle::check_index(camera_index)) {
					if (!Misc::vector_find_bool(camera_indices, camera_index)) {
						camera_indices.push_back(camera_index);
					}
				}
			}
			/*! Deduce UVdims from camera_indices.*/
			minmax_indices = Misc::calc_minmax(camera_indices);
			UVdims = calc_UVdims(minmax_indices, parameters.l_begin0);
		} else {
			/*! Use UVdims from config _file_reading.*/
			UVdims = parameters.UVdims;
		}
	}
	///////////////////////////////////////////

	/*! Map sub-apertures.*/
	bool l_match_time;
	bool l_mapping_ok = true;
	for (unsigned int i = 0; i < _images_paths.size() && l_mapping_ok; i++) {

		/*! First check if time index read in image path matches _time_index.*/
		/*! If a time position has been specified in config file, check wether read time index matches the one specified with LFpath.*/
		if (is_time()) {

			time_index_read = SubapertureBundle::get_index_from_file_name(_images_paths[i], get_time_position());
			l_match_time = (time_index_read == _time_index);

		} else {
			/*! Otherwise, l_match_time is considered as true.*/
			l_match_time = true;
		}

		/*! Time index is ok.*/
		if (l_match_time) {

			/*! If single index for sub-apertures, then read it and convert it to u,v indices.*/
			if (parameters.l_single_index_for_subapertures) {
				camera_index = SubapertureBundle::get_index_from_file_name(_images_paths[i], parameters.uvt_position_in_name[0]);
				if (!parameters.l_begin0) {//**
					camera_index -= minmax_indices.first;
				}
				if (SubapertureBundle::check_index(camera_index)) {
					uv = get_uv_index(camera_index, UVdims);
				} else {
					uv.first = -1;
					uv.second = -1;
				}

			} else {
				/*! Otherwise directly read u,v indices.*/
				uv = SubapertureBundle::get_uv_from_file_name(_images_paths[i], parameters.uvt_position_in_name);
			}

			/*! Checks u,v indices.*/
			if (SubapertureBundle::check_uv_indices(uv)) {
				/*! If mapping with u,v indices isn't possible (already exists).*/
				if (!SubapertureBundle::map(uv_images_mapping, uv, _images_paths[i])) {
					l_mapping_ok = false;
					if (Misc::l_verbose_low) {
						std::cout << "Mapping has stopped" << std::endl;
					}
				}
			}

		}

	}

	return l_mapping_ok;

}


UVindices SubapertureBundle::get_uv_from_file_name(const std::string& _file_name, const std::vector<unsigned int> _uv_position) {

	UVindices uv;
	
	/*! Gets all numbers in _file_name.*/
	std::vector<unsigned int> numbers = Misc::get_numbers(_file_name);

	/*! _uv_position has to be of minimum size 2 to contain index position of u and v.*/
	/*! If number of numbers in _file_name is superior to u index position.*/
	if (_uv_position.size() > 0 && numbers.size() > _uv_position[0]) {
		/*! Sets u coordinate.*/
		uv.first = numbers[_uv_position[0]];
		/*! If number of numbers in _file_name is superior to v index position.*/
		if (_uv_position.size() > 1 && numbers.size() > _uv_position[1]) {
			/*! Sets v coordinate.*/
			uv.second = numbers[_uv_position[1]];
		} else {
			/*! Otherwise sets v as invalid.*/
			uv.second = -1;
		}
	} else {
		/*! Otherwise sets u,v as invalid.*/
		uv.first = -1;
		uv.second = -1;
	}
	
	return uv;
}

unsigned int SubapertureBundle::get_index_from_file_name(const std::string& _file_name, unsigned int _index_position) {

	/*! Gets all numbers in _file_name.*/
	std::vector<unsigned int> numbers = Misc::get_numbers(_file_name);

	/*! If number of numbers in _file_name is superior to sub-aperture single index position.*/
	if (numbers.size() > _index_position) {
		return numbers[_index_position];
	} else {
		/*! Otherwise returns invalid index.*/
		return -1;
	}

}

bool SubapertureBundle::check_uv_indices(const UVindices& _uv) {
	/*! Checks wether neither u or v has invalid value.*/
	return _uv.first != (unsigned int)-1 && _uv.second != (unsigned int)-1;
}
bool SubapertureBundle::check_index(const unsigned int _index) {
	/*! Checks wether sub-aperture single index has invalid value.*/
	return _index != (unsigned int)-1;
}

float SubapertureBundle::get_likeliness() const {

	/*! Can be improved, or even parameterized.*/

	float likeliness = (float)Nuv_images;

	/*! Parameter accounting for how many sub-apertures contains an average light field. Center parameter for gaussian.*/
	unsigned int Nuv_images_estimation = Nimages_auto();// 256;//64 is fine too
	/*! Parameter accounting for the standard deviation of the previous parameter Nuv_images_estimation. Smoothing parameter for gaussian.*/
	float Nuv_images_offset = (float)Nimages_auto() * coef_std_Nimages_auto(); //64;

	float ratio_coef = coef_std_ratio_auto();//30.;
	/*! First likeliness stands for the number of sub-apertures in bundle as compared to estimate of Nuv_images_estimation relatively to Nuv_images_offset.*/
	likeliness = Misc::gaussian((float)Nuv_images, (float)Nuv_images_estimation, (float)Nuv_images_offset);

	if (Nu != 0 && Nv != 0) {
		/*! Coefficient accounting for light field dimension tending to be equal horizontally and vertically. For instance 9*9 is likely. 1*81 is unlikely.*/
		likeliness *= Misc::gaussian((float)std::min(Nu, Nv) / (float)std::max(Nu, Nv), (float)1, (float)1./ratio_coef);
	} else {
		likeliness = 0;
	}
	likeliness *= Misc::gaussian((float)Nu*Nv, (float)Nuv_images_estimation, (float)Nuv_images_offset);

	if (is_time()) {
		/*! Likeliness coefficient for variation of time indices. If time indices are not regular (ex: 1, 3, 11, 15), likeliness is low.*/
		likeliness *= Misc::gaussian((float)time_indices_var, (float)0, (float)10.);
	}

	float sparsity_ratio;
	if (uv_max.first != 0 && uv_max.second != 0) {
		sparsity_ratio = (float)Nuv_images / ((float)(uv_max.first+1)*(uv_max.second+1));
	} else {
		sparsity_ratio = 0.;
	}

	//sparsity
	float likeliness_sparsity = Misc::gaussian(sparsity_ratio, (float)1., (float)0.5);
	likeliness *= likeliness_sparsity;

	return likeliness;
}

SubapertureBundle::operator bool() const {

	bool l_ok = true;
	unsigned int Nimages_max = 10000;
	if (Nu*Nv < Nimages_max) {
		l_ok = true;
	} else {
		std::cout << "SubapertureBundle contains Nu = " << Nu << ", Nv = " << Nv << " images which is more than " << Nimages_max << " limit." << std::endl;
		l_ok = false;
	}

	if (Nu >= 1 && Nv >= 1) {
		l_ok &= true;
	} else {
		std::cout << "SubapertureBundle contains Nu = " << Nu << ", Nv = " << Nv << " images." << std::endl;
		l_ok &= false;
	}

	if (!l_ok) {
		std::cout << "SubapertureBundle is probably configured by a wrong DirectoryParser." << std::endl;
		std::cout << "Please verify there are no files with same coordinates. If so, use parameters filter_strings in loader." << std::endl;
	}

	return l_ok;
}


bool SubapertureBundle::operator>(const SubapertureBundle& _subapertures) const {

	return get_likeliness() > _subapertures.get_likeliness();
}

bool SubapertureBundle::operator<(const SubapertureBundle& _subapertures) const {

	return get_likeliness() < _subapertures.get_likeliness();
}

std::ostream& operator<< (std::ostream& _os, const SubapertureBundle& _subapertures) {

	_os << "Nuv_images = " << _subapertures.Nuv_images << std::endl;
	_os << "Allocation dimensions : " << std::endl;
	_os << "Nu = " << _subapertures.Nu << std::endl;
	_os << "Nv = " << _subapertures.Nv << std::endl;
	_os << "uv_min = " << _subapertures.uv_min << std::endl;
	_os << "uv_max = " << _subapertures.uv_max << std::endl;
	if (_subapertures.time_index != (unsigned int)-1) {
		_os << "time_index = " << _subapertures.time_index << std::endl;
		_os << "time_indices_var = " << _subapertures.time_indices_var << std::endl;
	}
	_os << "likeliness = " << _subapertures.get_likeliness() << std::endl;

	bool l_display_files = false;

	if (l_display_files) {
		/*! Displays list of files.*/
		_os << "List of files : " << std::endl;
		for (SubapertureBundle::Tmapping::const_iterator iterator = _subapertures.uv_images_mapping.begin(); iterator != _subapertures.uv_images_mapping.end(); ++iterator) {
			_os << "uv coordinates : " << iterator->first << "; file : " << iterator->second << std::endl;
		}
	}

	return _os;
}

///////////// PARAMETERS /////////////////

bool SubapertureBundle::Parameters::operator==(const SubapertureBundle::Parameters& _parameters) const {

	return (l_single_index_for_subapertures == _parameters.l_single_index_for_subapertures) \
		&& (uvt_position_in_name == _parameters.uvt_position_in_name) \
		&& UVdims == _parameters.UVdims \
		&& l_begin0 == _parameters.l_begin0;

}

bool SubapertureBundle::Parameters::operator!=(const SubapertureBundle::Parameters& _parameters) const {

	return !(*this == _parameters);

}

std::ostream& operator<< (std::ostream& _os, const SubapertureBundle::Parameters& _parameters) {
	_os << "l_single_index_for_subapertures = " << _parameters.l_single_index_for_subapertures << std::endl;
	_os << "uvt_position_in_name = " << _parameters.uvt_position_in_name << std::endl;
	_os << "UVdims = " << _parameters.UVdims << std::endl;
	_os << "l_begin0 = " << _parameters.l_begin0 << std::endl;
	return _os;
}