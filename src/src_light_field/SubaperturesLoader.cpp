/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "SubaperturesLoader.h"
#include "misc_funcs.h"

const Fpair SubaperturesLoader::center_coordinates_default() {
	static const Fpair _center_coordinates_default(-1., -1.);
	return _center_coordinates_default;
}


SubaperturesLoader::SubaperturesLoader() {

}

SubaperturesLoader::SubaperturesLoader(const Parameters& _parameters) {

	set_parameters(_parameters);
}

SubaperturesLoader::~SubaperturesLoader() {

}

void SubaperturesLoader::set_parameters(const Parameters& _parameters) {

	parameters = _parameters;
	deduce();

}

const SubaperturesLoader::Parameters& SubaperturesLoader::get_parameters() const {

	return parameters;
}

bool SubaperturesLoader::is_directory() const {

	return l_directory;
}

bool SubaperturesLoader::is_video() const {

	return l_video;
}

const std::string& SubaperturesLoader::get_LF_name() const {

	return LF_name;
}

void SubaperturesLoader::deduce() {

	l_directory = Misc::is_directory(parameters.LF_path);
	l_video = Misc::is_video_file(parameters.LF_path);

	/*! Finds last folder separator and extract last name.*/
	LF_name = Misc::extract_name_from_path(parameters.LF_path);

	/*! If LF is contained in a single file, gets its file extension.*/
	if (!l_directory) {

		file_extension = Misc::get_file_extension(LF_name);
		LF_name = Misc::remove_file_extension(LF_name);
		
	} else {

		file_extension.clear();
	}


	if (parameters.time_index != (unsigned int)-1) {
		l_time_index = true;
	} else {
		l_time_index = false;
	}

}
