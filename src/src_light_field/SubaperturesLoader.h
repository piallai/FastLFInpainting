/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include <string>
#include <vector>
#include "Typedefs.h"
#include "SubapertureBundle.h"

class SubaperturesLoader {

public :

	struct Parameters {
		/*! Path to light field.*/
		std::string LF_path = "";
		/*! Wether read files recursively in path or not.*/
		bool l_recursive = false;
		/*! Time index to consider if light field is a video.*/
		unsigned int time_index = -1;
		/*! If u and v are inverted (true most of the time).*/
		bool l_invert_uv = false;
		/*! Direction of each u v axis. Meaning if one direction has a negative value, angular flipping in this direction is applied. Also stands for axis coefficients (baseline).*/
		Fpair uv_axis_coef = { 1, 1 };
		/*!  Coordinates of central subaperture.*/
		Fpair center_coordinates{ -1., -1. };
		/*! Image scaling applied to read subapertures.*/
		Fpair image_scale{ 1.,1. };
		/*! Subaperture offsets (left, up, right, bottom). Number of ignored subapertures on the edge of the light field. Can be usefull for faster computing and testing.*/
		Leupribo<unsigned int> subaperture_offsets = { { 0,0,0,0 } };
		/*! Angular modulo of read subapertures. For instance if light field is dimension 7x7, angular modulo of 3x3 leads to a 3x3 light field (border images + center).*/
		Upair angular_modulo = { 1,1 };
		/*! Whether histogram matching is applied on every subapertures. Convenient for light fields which luminosity and colorimetry differs when changing subaperture. Reference image is center subaperture.*/
		bool l_histogram_matching = false;
		/*! Number of pixels to crop on the edges of the subapertures (left, up, right, bottom). Can be usefull if edges have some corrupted pixels (lytro capture for instance).*/
		Leupribo<unsigned int> Ncrop_pixels = { { 0,0,0,0 } };
		/*! Wether auto reading is activated or not. If activated, program will deduce u,v,t from files numbering.*/
		bool l_auto_read = true;
		/*! Path pointing to config file for light field parsing. Used only if l_auto_read is set to false.*/
		//std::string reading_format_path;
		SubapertureBundle::Parameters reading_parameters;
		/*! If l_auto_read is set to true, expected number of images in light fields.*/
		unsigned int Nimages_auto = 256;
		/*! If l_auto_read is set to true, standard deviation of number of images in light fields.*/
		//unsigned int Nimages_std_auto = 64;
		/*! If l_auto_read is set to true, ratio of Nimages_auto used as standard deviation for likeliness.*/
		float coef_std_Nimages_auto = 0.25;
		/*! If l_auto_read is set to true, dimensions ratio. Measures likeliness of light fields dimensions when Nu != Nv.
		Accounts for inverse of standard deviation, of distance of light fiel ratio to value 1. When high, light field has to be of very squares angular dimensions to be likely. When low, dimensions of light field doesn't matter much.*/
		float coef_std_ratio_auto = 0.5;
		/*! Filter strings for filenames. Example : mask can be enother type of image with same angular coordinates.*/
		std::vector<std::string> filter_strings = {"mask"};
		/*! Wether to display sub-apertures when loaded or not.*/
		bool l_display_subapertures = false;
		
	};

private :

	Parameters parameters;

	//deduced parameters
	/*! Wether if LF_path is a directory.*/
	bool l_directory;
	/*! Wether if LF_path is a video file.*/
	bool l_video;
	/*! Name of LF extracted from LF_path.*/
	std::string LF_name;
	/*! Extension of LF file (if file).*/
	std::string file_extension;
	/*! Wether time_index is used.*/
	bool l_time_index;

public:

	SubaperturesLoader();
	SubaperturesLoader(const Parameters& _parameters);
	~SubaperturesLoader();

	void set_parameters(const Parameters& _parameters);
	const Parameters& get_parameters() const;

	bool is_directory() const;
	bool is_video() const;
	const std::string& get_LF_name() const;

	static const Fpair center_coordinates_default();

private :

	void deduce();

};
