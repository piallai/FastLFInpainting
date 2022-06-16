/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "Images4D_base.h"

Images4D_base::Images4D_base() {

}

Images4D_base::~Images4D_base() {

}

const std::string& Images4D_base::get_name() const {

	return name;
}

void Images4D_base::set_name(const std::string& _name) {

	name = _name;
}

Images4D_base::operator bool() const {

	return !empty();
}

bool Images4D_base::empty() const {

	return get_outer_size().width == 0 || get_outer_size().height == 0;
}

cv::Size Images4D_base::get_outer_size_original(bool _l_original_coordinates) const {

	cv::Size outer_size_original = get_outer_size();
	if (_l_original_coordinates) {
		if (is_inverted()) {
			std::swap(outer_size_original.width, outer_size_original.height);
		}
	}

	return outer_size_original;
}

Images4D_base::Tarray<UIpair> Images4D_base::get_coordinates_from_original(bool _l_original_coordinates) const {

	Tarray<UIpair> coordinates;

	cv::Size outer_size = get_outer_size();
	cv::Size outer_size_original = outer_size;
	if (_l_original_coordinates) {
		if (is_inverted()) {
			std::swap(outer_size_original.width, outer_size_original.height);
		}
	}
	resize(coordinates, outer_size_original);

	UIpair original_coordinates;
	for (unsigned int i = 0; i < (unsigned int)outer_size.width; i++) {
		for (unsigned int j = 0; j < (unsigned int)outer_size.height; j++) {

			if (_l_original_coordinates) {
				original_coordinates = get_original_coordinates(UIpair(i, j));
				coordinates[original_coordinates.first][original_coordinates.second] = UIpair(i, j);
			} else {
				coordinates[i][j] = UIpair(i, j);
			}


		}
	}

	return coordinates;
}