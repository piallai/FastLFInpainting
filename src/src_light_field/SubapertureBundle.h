/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include <map>
#include <vector>
#include "Typedefs.h"

class SubapertureBundle {

public :
	struct Parameters {
		/*! Wether indexing of subapertures is described by only one index (i=0,..,N) or two (u=0,..,N and v=0,..,M).*/
		bool l_single_index_for_subapertures = false;
		/*! Position of each dimension numbering in file names.
		If first number in file name stands for u dimension, then uvt_position_in_name[0] = 0.
		Size depends on l_single_index_for_subapertures and l_time.
		If only one value, position of [index] coordinate. l_single_index_for_subapertures must be set to true.
		If only two values,  position of [u, v] coordinates if l_single_index_for_subapertures==false.
		If only two values, position of [index, time] coordinates if l_single_index_for_subapertures==true.
		If three values,  position of [u,v,time] coordinates in file name.*/
		std::vector<unsigned int> uvt_position_in_name = {0, 1, 2};
		/*! number of subapertures in dimension u and v (when subapertures are only labelled by a single index, ie: l_single_index_for_subapertures=true).
		If [0,0], values are automatically guessed based 1) on the range of indices, 2) U dimension being higher than V's, 3) Maximizing dimensions product.*/
		UVindices UVdims = UVindices(0, 0);
		/*! If l_single_index_for_subapertures is true, wether first index is 0 or first found index.*/
		bool l_begin0 = false;

		/*! Wether FileReading instances are the same or not.*/
		bool operator==(const Parameters& _parameters) const;
		/*! Wether FileReading instances are different or not.*/
		bool operator!=(const Parameters& _parameters) const;
		/*! Terminal display function.*/
		friend std::ostream& operator<< (std::ostream& _os, const Parameters& _parameters);
	};

	/*! If l_auto_read is set to true, expected number of images in light fields.*/
	static unsigned int& Nimages_auto();
	/*! If l_auto_read is set to true, ratio of standard deviation of number of images in light fields.*/
	static float& coef_std_Nimages_auto();
	/*! If l_auto_read is set to true, dimensions ratio. Measures likeliness of light fields dimensions when Nu != Nv.
	Accounts for inverse of standard deviation, of distance of light fiel ratio to value 1. When high, light field has to be of very squares angular dimensions to be likely. When low, dimensions of light field doesn't matter much.*/
	static float& coef_std_ratio_auto();

private :

	Parameters parameters;

public :

	/*! Wether light field is a time video.*/
	bool l_time;
	/*! Time index to consider if light field is a video.*/
	unsigned int time_position;

	/*! Type for mapping sub-aperture images.*/
	typedef std::map<UVindices, std::string> Tmapping;
	/*! Mapping of sub-aperture images. For each \p UVindices corresponds a path to an image.*/
	Tmapping uv_images_mapping;

	/*! Number of sub-aperture images mapped. Equal to uv_images_mapping.size(). Can be inferior to Nu*Nv is mapping is sparse.*/
	unsigned int Nuv_images;

	/*! Minimum index of sub-apertures. \sa analyse*/
	UVindices uv_min;
	/*! Maximum index of sub-apertures. \sa analyse*/
	UVindices uv_max;

	/*! Number of theoretical images in u axis. Such as \p Nuv_images <= \p Nu * \p Nv. \sa analyse.
	Difference bewteen max and min index.*/
	unsigned int Nu;
	/*! Number of theoretical images in v axis. Such as \p Nuv_images <= \p Nu * \p Nv. \sa analyse.
	Difference bewteen max and min index.*/
	unsigned int Nv;

	/*! If bundle is part of a time dependent light field, \p time_index stands for the temporal index at which light-field is considered.*/
	unsigned int time_index;
	/*! If bundle is part of a time dependent light field, \p time_index_min stands for the minimal temporal index at which light-field is considered.*/
	unsigned int time_index_min;
	/*! If bundle is part of a time dependent light field, \p time_index_max stands for the maximal temporal index at which light-field is considered.*/
	unsigned int time_index_max;
	/*! If bundle is part of a time dependent light field, \p time_indices_var stands for the quadratic variation of time indices offset. Usefull to compute a likeliness function of bundle. \sa get_likeliness.*/
	float time_indices_var;

public :

	SubapertureBundle();
	SubapertureBundle(const Parameters& _parameters);
	~SubapertureBundle();

	const Parameters& get_parameters() const;
	void set_parameters(const Parameters& _parameters);

	void clear();

	bool is_time() const;
	unsigned int get_time_position() const;

	static const unsigned int no_time_index;

private:
	/*! Deduces #l_time and #time_position from #uvt_position_in_name analysis.*/
	void analyse_parameters();
	/*! Analyses \p uv_images_mapping . Computes dimensions of light-field.*/
	void analyse_mapping();

	/*! Adds (if not redundant) #_uv \image_path pair to #_uv_images_mapping.*/
	static bool map(Tmapping& _uv_images_mapping, const UVindices& _uv, const std::string& _image_path);

	/*! Gets uv indices from a file path using #_uv_position to provide information about numbers to consider in file name.*/
	static UVindices get_uv_from_file_name(const std::string& _file_name, const std::vector<unsigned int> _uv_position = { 0,1 });

	/*! Gets single index from a file path using #_index_position to provide information about number to consider in file name.*/
	static unsigned int get_index_from_file_name(const std::string& _file_name, const unsigned int _index_position = 0);

	/*! Checks wether #_uv has correct values.*/
	static bool check_uv_indices(const UVindices& _uv);
	/*! Checks wether #_index has a correct value.*/
	static bool check_index(const unsigned int _index);

	/*! Computes U and V dimensions (number of sub-apertures in each dimension). #_indices contains the list of all 1D indices describing a 2D angular light field.*/
	/*! If \p _l_begin0 i set to true, also rescale indices so that first one is 0.*/
	static UVindices calc_UVdims(const std::vector<unsigned int>& _indices, bool _l_begin0 = true);
	static UVindices calc_UVdims(const std::pair<unsigned int, unsigned int>& _minmax_indices, bool _l_begin0 = true);
	/*! From 1D #_index and 2D dimensions of sub-apertures, return corresponding 2D index.*/
	static UVindices get_uv_index(const unsigned int _index, const UVindices& _UVdims);
	/*! Converts a list of 1D indices into a list of 2D indices.*/
	static std::vector<UVindices> get_uv_indices(const std::vector<unsigned int>& _indices, bool _l_begin0);

public :
	bool read_subapertures(const std::string& _path, bool _l_recursive, unsigned int _time_index, const std::vector<std::string> _filter_strings = std::vector<std::string>());
	bool read_subapertures(const std::vector<std::string>& _images_paths, const std::string& _path, unsigned int _time_index);
private :
	/*! Builds and return a mapping of images. Called by \p read_subapertures.*/
	bool set_mapping(const std::vector<std::string>& _images_paths, unsigned int _time_index);
public :

	/*! Gets a likeliness coefficient of bundle.
	Uses uv dimensions in various ways to compute an overall coefficient describing how much this bundle is coherent from a subjective point of view.
	Contains parameters that can be changed to tweak this sort of IA.*/
	float get_likeliness() const;

	/*! Compares bundles using get_likeliness.*/
	bool operator>(const SubapertureBundle& _subapertures) const;
	/*! Compares bundles using get_likeliness.*/
	bool operator<(const SubapertureBundle& _subapertures) const;

	/*! Checks wether bundle contains a reasonable number of images. Usefull to prevent further allocation of huge number of images in case of DirectoryParser returns a wrong value.*/
	operator bool() const;

	/*! Terminal display function.*/
	friend std::ostream& operator<< (std::ostream& _os, const SubapertureBundle& _subapertures);

};

