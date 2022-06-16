/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include <opencv2/core.hpp>
#include "Typedefs.h"

class Images4D_base {

protected:

	std::string name;

	/*! Usefull for original data structure. More specifically, SubaperturesData angular inversion.*/
	virtual bool is_inverted() const = 0;

public:

	Images4D_base();
	virtual ~Images4D_base();

	const std::string& get_name() const;
	void set_name(const std::string& _name);
	virtual std::string get_class_name() const = 0;

	operator bool() const;
	bool empty() const;

	/*! Type of sub-apertures.*/
	template <class T>
	using Tarray = std::vector< std::vector<T> >;

	template <class T>
	static void resize(Tarray<T>& _array, const unsigned int& _N1, const unsigned int& _N2);
	template <class T>
	static void resize(Tarray<T>& _array, const cv::Size& _size);
	template <class T>
	static unsigned int get_N1(const Tarray<T>& _array);
	template <class T>
	static unsigned int get_N2(const Tarray<T>& _array);

	Tarray<UIpair> get_coordinates_from_original(bool _l_original_coordinates = true) const;

	virtual UIpair get_original_coordinates(const UIpair& _indices) const = 0;

	virtual cv::Size get_inner_size() const = 0;
	virtual cv::Size get_outer_size() const = 0;
	cv::Size get_outer_size_original(bool _l_original_coordinates = true) const;

	/*! Modulo of images.
	If Images4D is inherited by SubaperturesData, this modulo is related to angular coordinates.
	If Images4D is inherited by EpiLF, this modulo is related to angular and spatial coordinates.*/
	struct OuterModulo {
		unsigned int first;
		unsigned int second;
		OuterModulo(unsigned int _first, unsigned int _second) :first(_first), second(_second) {}
	};

};

#include "Images4D_base_impl.h"