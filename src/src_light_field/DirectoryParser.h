/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "SubapertureBundle.h"

/*! In charge of converting a light field path and reading options into a SubapertureBundle instance.*/
struct DirectoryParser {

public :

	/*! Entry method. \p _file_reading can be such as l_auto_read=true.*/
	/*! Explore every possible ConfigFileReading with respect to images in \p _path and return the corresponding most likely SubapertureBundle.*/
	static SubapertureBundle analyse(const std::string& _path, bool _l_recursive, unsigned int _time_index, const std::vector<std::string> _filter_strings = std::vector<std::string>());

};