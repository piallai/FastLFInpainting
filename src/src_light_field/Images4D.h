/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "Images4D_base.h"
#include "ocv_convert.h"
#include "ocv_rw.h"

/*! Class to be inherited by 4D light field strucutures. SubaperturesData, EpiLF or LightField*/
template <class Timg>
class Images4D : virtual public Images4D_base {

public :

	/*! Defined for clarity of declarations in the methods.*/
	typedef typename Timg::value_type Tvec;
	/*! Defined for clarity of declarations in the methods.*/
	typedef typename Tvec::value_type Tvalue;
	/*! Usefull when need to access value_type of Timg in functions applying to a Timg. So Images4D inherihted class can also be applied.*/
	typedef Tvec value_type;

	Images4D();
	~Images4D();

	virtual Timg& operator() (const unsigned int _i, const unsigned int j) = 0;
	virtual const Timg& operator() (const unsigned int _i, const unsigned int j) const = 0;
	virtual Timg& get_image_original_coordinates(const unsigned int u, const unsigned int v) = 0;
	virtual const Timg& get_image_original_coordinates(const unsigned int u, const unsigned int v) const = 0;

	/*! Checks if every image has same size.*/
	bool is_coherent() const;
	/*! Checks if there are empty images.*/
	bool is_sparse() const;
	/*! Checks if every non empty image has same size.*/
	bool is_coherent_sparsity() const;
	/*! Checks if every image are the same.*/
	bool is_identical() const;
	/*! Checks if 4D structure has same dimensions.*/
	template <class Timg_arg>
	bool has_same_dimensions(const Images4D<Timg_arg>& _images) const;

	/*! Asks child class if transpositions are needed for full image design.*/
	virtual std::pair<bool, bool> get_full_image_transposes() const = 0;
	/*! Builds a single image containing of views.*/
	void build_full_image(Timg& _image, bool _l_original_coordinates = true, const unsigned int _spacing = 2, const typename Timg::value_type _space_value = Timg::value_type::all(ocv::MaxRange<Tvalue>::get())) const;
	void display_full_image(const unsigned int _spacing = 2, const typename Timg::value_type _space_value = Timg::value_type::all(ocv::MaxRange<Tvalue>::get()), bool _l_original_coordinates = true) const;



	/*! Display images.*/
	void display(const std::string _text = "", bool _l_position_label = false) const;

	/*! Methods for writing images. Different overloadings.*/
	void imwrite(const std::string _prefix) const;
	void imwrite(const std::string _prefix, const cv::Mat& _segmentation_image) const;
	void imwrite(const std::string _prefix, const char* _file_extension) const;
	void imwrite(const std::string _prefix, const std::string _file_extension) const;
	void imwrite(const std::string _prefix, const ocv::Range<Tvalue>& _range_input) const;
	void imwrite(const std::string _prefix, const ocv::Range<Tvalue>& _range_input, bool _l_original_coordinates) const;
	void imwrite(const std::string _prefix, const std::string& _file_extension, const ocv::Range<Tvalue>& _range_input, bool _l_original_coordinates) const;
	void imwrite(const std::string _prefix, const Tvalue _range_min, const Tvalue _range_max) const;
	void imwrite(const std::string _prefix, const OuterModulo& _modulo) const;
	void imwrite(const std::string _prefix, const Tvalue _range_min, const Tvalue _range_max, const OuterModulo& _modulo) const;
	void imwrite(const std::string _prefix, const ocv::Range<Tvalue>& _range_input, const OuterModulo& _modulo) const;
	void imwrite(const std::string _prefix, const std::string& _file_extension, const ocv::Range<Tvalue>& _range_input, bool _l_original_coordinates, bool _l_colormap, cv::ColormapTypes _colormap_type = cv::COLORMAP_JET) const;
	/*! Final method. Write subapertures with segmentation image to display contour (such as a mask for instance)*/
	void imwrite(const std::string _prefix, bool _l_original_coordinates, const cv::Mat _segmentation_image=cv::Mat(), const ocv::Range<Tvalue> _range_input = ocv::Range<Tvalue>((Tvalue)0, (Tvalue)0), const std::string _file_extension = ocv::write_extension, bool _l_colormap = false, cv::ColormapTypes _colormap_type = cv::COLORMAP_JET, const OuterModulo _modulo = OuterModulo(1, 1)) const;


};

#include "Images4D_impl.h"
