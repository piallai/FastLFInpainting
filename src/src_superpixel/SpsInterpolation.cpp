/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "SpsInterpolation.h"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/median.hpp>//for median distances
#include "SpsMaskMerge.h"

SpsInterpolation::SpsInterpolation() {

	set_parameters();
}

SpsInterpolation::SpsInterpolation(const SpsMaskMerge* _merger) {

	set_parameters();
	compute(_merger);

}

SpsInterpolation::~SpsInterpolation() {

	if (ocv::is_valid(weights_image)) {
		/*! Delete weights pointers.*/
		for (ocv::Timg_ptr::const_iterator it_weights = weights_image.begin(); it_weights != weights_image.end(); ++it_weights) {

			delete (Tweights*)(*it_weights)[0];
		}
	}
	
}

void SpsInterpolation::set_parameters(const Parameters& _parameters) {

	parameters = _parameters;

}

void SpsInterpolation::set_parameters(unsigned int _Nweight_pixels, double _distance_coef, double _sigma_coef) {

	parameters.Nweight_pixels = _Nweight_pixels;
	parameters.distance_coef = _distance_coef;
	parameters.sigma_coef = _sigma_coef;

}

void SpsInterpolation::compute(const SpsMaskMerge* _merger) {

	std::cout << "Computing interpolation weights" << std::endl;

	bool l_recolored = _merger->get_sps()->is_recolored();

	
	/*! std::map of all pixels coordatas by label.*/
	SuperPixelSegmentation::Tlabels_datas labels_datas = _merger->get_sps()->get_labels_datas(_merger->get_labels(), SuperPixelSegmentation::no_label, l_recolored);
	/*! Compute for each label a coefficient for distance measurement based on superpixels properties.*/
	SuperPixelSegmentation::Tlabel_map<double> distance_coefs;
	calc_distance_coefs(labels_datas, distance_coefs);
	/*! std::map of unmasked pixels coordatas by label.*/
	SuperPixelSegmentation::Tlabels_datas labels_datas_unmasked = _merger->get_labels_datas_masked(true, l_recolored);
	/*! Init image containing pointers to weights.*/
	weights_image.create(_merger->get_sps()->size());


	cv::MatConstIterator_<cv::Vec1b> it_mask = _merger->get_mask().begin();
	cv::MatConstIterator_<ocv::Tvec> it_image;
	/*! depending on l_recolored, iterate over original image or recolored one.*/
	if (!l_recolored) {
		it_image = _merger->get_sps()->get_image_original().begin();
	} else {
		it_image = _merger->get_sps()->get_image_recolored().begin();
	}
	ocv::Timg_ptr::iterator it_weights = weights_image.begin();

	Tweights* weights;
	cv::Point coordinates(0, 0);
	cv::Size size = _merger->get_sps()->size();
	SuperPixelSegmentation::Tlabel label_value;
	SuperPixelSegmentation::Tcoordata coordata;
	/*! coordatas for a given label.*/
	std::vector<SuperPixelSegmentation::Tcoordata>* coordatas;
	/*! mu and sigma for gaussian.*/
	std::pair<double, double> weight_parameters;
	typedef std::vector< std::pair<double, const cv::Point*> > Tdistances;
	/*! for Nweight_pixels minimum distance. Each distance also point to corresponding pixel coordinates.*/
	Tdistances distances_min(parameters.Nweight_pixels);
	/*! buffer vector to compute meta data on distances.*/
	std::vector<double> distances_min_stats(parameters.Nweight_pixels);

	unsigned int Nmask = cv::countNonZero(_merger->get_mask());
	unsigned int i_mask = 0;
	for (cv::MatConstIterator_<cv::Vec1i> it_labels = _merger->get_labels().begin<cv::Vec1i>(); it_labels != _merger->get_labels().end<cv::Vec1i>(); ++it_labels, ++it_mask, ++it_image, ++it_weights) {

		/*! If inside the mask.*/
		if ((*it_mask)[0] == ocv::mask_value) {

			label_value = (*it_labels)[0];

			weights = new Tweights;

			/*! Assign coordata color value and coordinates.*/
			coordata.first = &(*it_image);
			coordata.second = coordinates;
			/*! Get all coordatas belonging to same superpixel and being outside the mask.*/
			coordatas = &labels_datas_unmasked.at(label_value);
			/*! Compute minimum distances.*/
			calc_distances_min(coordata, coordatas, distance_coefs[label_value] * parameters.distance_coef, distances_min);
			/*! Compute gaussian parameters based on these distances.*/
			weight_parameters = calc_weight_parameters(distances_min, distances_min_stats);
			/*! Multiply sigma by user-defined coefficient.*/
			weight_parameters.second *= parameters.sigma_coef;
			/*! Compute weights.*/
			calc_weights(distances_min, weight_parameters, size, weights);
			/*! Assign computed weights to pixel position.*/
			*it_weights = (ocv::Timg_ptr_type)weights;//assign weights pointer

			Misc::display_progression(i_mask, Nmask);
			i_mask++;

		} else {
			/*! If outside the mask, there are no weights.*/
			*it_weights = 0;

		}


		/*! Increase coordinates while iterating to know x,y position.*/
		coordinates.x++;
		if (coordinates.x == size.width) {
			coordinates.y++;
			coordinates.x = 0;
		}
	}

}

void SpsInterpolation::calc_distance_coefs(const SuperPixelSegmentation::Tlabels_datas& _labels_datas, SuperPixelSegmentation::Tlabel_map<double>& _distance_coefs) const {

	_distance_coefs.clear();

	cv::Point coordinates_mean;
	cv::Point coordinates_var;
	cv::Point coordinates;

	ocv::Tvec color_mean;
	ocv::Tvec color_var;
	ocv::Tvec color;


	/*! For each label.*/
	for (SuperPixelSegmentation::Tlabels_datas::const_iterator it_map = _labels_datas.begin(); it_map != _labels_datas.end(); ++it_map) {

		/*! If number of coordatas for this label is greater than 0.*/
		if (it_map->second.size() > 0) {

			/*! Init mean variables.*/
			coordinates_mean = cv::Point(0, 0);
			color_mean = ocv::Tvec::all(0);
			/*! For each coordata.*/
			//std::cout << "it_map->second.size() = " << it_map->second.size() << std::endl;
			for (std::vector<SuperPixelSegmentation::Tcoordata>::const_iterator it_coordata = it_map->second.begin(); it_coordata != it_map->second.end(); ++it_coordata) {

				coordinates_mean += it_coordata->second;
				color_mean += *it_coordata->first;
				//std::cout << "*it_coordata->first = " << *it_coordata->first << std::endl;
			}
			coordinates_mean.x /= (unsigned int)it_map->second.size();
			coordinates_mean.y /= (unsigned int)it_map->second.size();
			color_mean[0] /= it_map->second.size();
			color_mean[1] /= it_map->second.size();
			color_mean[2] /= it_map->second.size();

			/*! Manage variance.*/
			coordinates_var = cv::Point(0, 0);
			color_var = ocv::Tvec::all(0);
			/*! For each coordata.*/
			for (std::vector<SuperPixelSegmentation::Tcoordata>::const_iterator it_coordata = it_map->second.begin(); it_coordata != it_map->second.end(); ++it_coordata) {

				coordinates = it_coordata->second;
				coordinates -= coordinates_mean;
				coordinates_var.x += coordinates.x * coordinates.x;
				coordinates_var.y += coordinates.y * coordinates.y;

				color = *it_coordata->first;
				color -= color_mean;
				color_var[0] += color[0] * color[0];
				color_var[1] += color[1] * color[1];
				color_var[2] += color[2] * color[2];
			}
			color_var[0] /= it_map->second.size();
			color_var[1] /= it_map->second.size();
			color_var[2] /= it_map->second.size();

			if (color_var == ocv::Tvec::all(0)) {
				std::cout << "can't divide" << std::endl;
				std::cout << "color_var = " << color_var << std::endl;
				_distance_coefs[it_map->first] = std::numeric_limits<double>::max();
			} else {
				_distance_coefs[it_map->first] = (coordinates_var.x + coordinates_var.y) / (color_var[0] + color_var[1] + color_var[2]);
			}

		} else {
			/*! if no coordatas, sets value to 1.*/
			_distance_coefs[it_map->first] = 1.;
		}

		
	}
}


void SpsInterpolation::calc_distances_min(const SuperPixelSegmentation::Tcoordata& _coordata, const std::vector<SuperPixelSegmentation::Tcoordata>* _coordatas, const double _distance_coef, std::vector< std::pair<double, const cv::Point*> >& _distances_min) const {

	/*! If number of known coordatas is lower to the number of required distances (Nweight_pixels), then resize.*/
	if (_coordatas->size() < _distances_min.size()) {
		_distances_min.resize(_coordatas->size());
	}

	double distance;
	std::pair<ocv::Tvec, cv::Point> calc_distance_buffer;
	std::vector< std::pair<double, const cv::Point*> >::iterator it_dist_search;
	std::vector< std::pair<double, const cv::Point*> >::iterator it_dist_shift_next, it_dist_shift_prev;
	std::vector< std::pair<double, const cv::Point*> >::iterator it_dist_begin = _distances_min.begin();
	std::vector< std::pair<double, const cv::Point*> >::iterator it_dist_end = _distances_min.end();

	double init_value = std::numeric_limits<double>::max();
	/*! Init minimum distances.*/
	for (it_dist_search = _distances_min.begin(); it_dist_search != _distances_min.end(); ++it_dist_search) {
		it_dist_search->first = init_value;
	}

	/*! For every coordata.*/
	for (std::vector<SuperPixelSegmentation::Tcoordata>::const_iterator it_vec = _coordatas->begin(); it_vec != _coordatas->end(); ++it_vec) {
		/*! Compute distance to other coordata.*/
		distance = calc_distance(_coordata, *it_vec, _distance_coef, calc_distance_buffer);
		/*! Insert distance in distances_min in ascending order.*/
		it_dist_search = it_dist_begin;
		/*! While distance is greater or equal to element of distances_min.*/
		while (it_dist_search != it_dist_end && distance >= it_dist_search->first) {
			/*! Go forward.*/
			++it_dist_search;
		}
		/*! If iterator is in distances_min range.*/
		if (it_dist_search != it_dist_end) {

		///*! Insert distance in distances_min in descending order. Theoretically faster than ascending since most data are likely to be out of range.*/
		//it_dist_search = it_dist_end-1;
		///*! While distance is greater or equal to element of distances_min.*/
		//while (it_dist_search != it_dist_end && distance < it_dist_search->first) {
		//	/*! Go backward.*/
		//	--it_dist_search;
		//}
		///*! If iterator is in distances_min range.*/
		//if (it_dist_search != it_dist_end) {
			//std::cout << "it_dist_search - _distances_min.begin() = " << it_dist_search - _distances_min.begin() << std::endl;
			/*! Then shift values after iterator.*/
			/*! Init from back.*/
			it_dist_shift_next = it_dist_end - 1;
			/*! Iterator safety*/
			if (it_dist_shift_next != it_dist_begin) {
				it_dist_shift_prev = it_dist_shift_next - 1;
			}
			/*! Search iterator is not reached.*/
			while (it_dist_shift_next != it_dist_search) {
				//std::cout << "it_dist_shift_next - _distances_min.begin() = " << it_dist_shift_next - _distances_min.begin() << std::endl;
				//std::cout << "it_dist_shift_prev - _distances_min.begin() = " << it_dist_shift_prev - _distances_min.begin() << std::endl;
				/*! Move value upward.*/
				*it_dist_shift_next = *it_dist_shift_prev;
				it_dist_shift_next--;
				/*! Iterator safety*/
				if (it_dist_shift_prev != it_dist_begin) {
					it_dist_shift_prev--;
				}
			}
			///* Move final value upward.*/
			//*it_dist_shift_next = *it_dist_shift_prev;
			/*! And insert computed distance.*/
			it_dist_search->first = distance;
			it_dist_search->second = &it_vec->second;
		}
	}

	/*! Remove value_init if some are left.
	Meaning there are not enough pixel to fill Nweight_pixels weights.*/
	it_dist_search = _distances_min.begin();
	while (it_dist_search != _distances_min.end() && it_dist_search->first < init_value) {
		++it_dist_search;
	}
	/*! If an init_value was found.*/
	if (it_dist_search != _distances_min.end()) {
		/*! Erase all of them.*/
		while (it_dist_search->first == init_value) {
			it_dist_search = _distances_min.erase(it_dist_search);
		}
	}
	

	/*! Apply square root to kept distances in order to have a correct dimensionality.*/
	for (it_dist_search = _distances_min.begin(); it_dist_search != _distances_min.end(); ++it_dist_search) {
		it_dist_search->first = std::sqrt(it_dist_search->first);
	}

}

std::pair<double,double> SpsInterpolation::calc_weight_parameters(const std::vector< std::pair<double, const cv::Point*> >& _distances_min, std::vector<double>& _distances_min_stats) const {

	/*! mu and sigma.*/
	std::pair<double, double> weight_parameters;
	/*! Assign distance value to a vector without pair to use generic methods.*/
	std::vector<double>::iterator it_dist_stats = _distances_min_stats.begin();
	for (std::vector< std::pair<double, const cv::Point*> >::const_iterator it_dist = _distances_min.begin(); it_dist != _distances_min.end(); ++it_dist, ++it_dist_stats) {
		*it_dist_stats = it_dist->first;
	}

	/*! Mu is set as the minimum distance. Ie: maximum weight intensity for minimal distance.*/
	weight_parameters.first = _distances_min_stats[0];
	/*! Sigma is set as the standard deviation around mu.*/
	weight_parameters.second = Misc::get_std(_distances_min_stats, weight_parameters.first);
	/*! If sigma is 0, set to 1.*/
	if (weight_parameters.second == 0) {
		weight_parameters.second = 1.;
	}

	return weight_parameters;
}


double SpsInterpolation::calc_distance(const SuperPixelSegmentation::Tcoordata& _value1, const SuperPixelSegmentation::Tcoordata& _value2, const double _distance_coef, std::pair<ocv::Tvec, cv::Point>& _buffer) const {

	/*! Color difference*/
	_buffer.first = *_value1.first;
	_buffer.first -= *_value2.first;
	/*! Compute square L2.*/
	double distance = cv::norm(_buffer.first, cv::NORM_L2SQR);
	/*! Apply coefficient.*/
	distance *= _distance_coef;

	/*! Coordinates difference.*/
	_buffer.second = _value1.second;
	_buffer.second -= _value2.second;
	/*! Compute square L2.*/
	int int_buff;
	int_buff = _buffer.second.x;
	int_buff *= _buffer.second.x;
	distance += int_buff;
	int_buff = _buffer.second.y;
	int_buff *= _buffer.second.y;
	distance += int_buff;

	return distance;

}


void SpsInterpolation::calc_weights(const std::vector< std::pair<double, const cv::Point*> >& _distances_min, const std::pair<double, double>& _weight_parameters, const cv::Size& _size, Tweights* _weights) const {

	/*! Resize number of weights according to number of minimum distances.*/
	_weights->resize(_distances_min.size());

	double weights_sum = 0.;
	std::vector< std::pair<double, const cv::Point*> >::const_iterator it_dist = _distances_min.begin();
	int previous_position = -1;
	int position;
	double sigma2 = _weight_parameters.second;
	sigma2 *= -_weight_parameters.second;;
	/*! For each weight in weights vector.*/
	for (Tweights::iterator it_weight = _weights->begin(); it_weight != _weights->end(); ++it_weight, ++it_dist) {
		/*! Compute gaussian weight.*/
		/*! Distance - mu*/
		it_weight->first = it_dist->first;
		it_weight->first -= _weight_parameters.first;
		/*! Square*/
		it_weight->first *= it_weight->first;
		/*! Divide by squared sigma.*/
		it_weight->first /= sigma2;
		it_weight->first = std::exp(it_weight->first);
		weights_sum += it_weight->first;
		/*! Get index position from 2D coordinates.*/
		position = ocv::get_position(*it_dist->second, _size);
		it_weight->second = position;
		/*! If previous position is not the first one in image.*/
		if (previous_position > 0) {
			/*! Then position is set as the difference between current and previous to make easie iterator lookup when using weights.*/
			it_weight->second -= previous_position;
		}
		previous_position = position;
	}

	/*! Debug safety, checks if a problem appears in weights computation due to some parametrization.*/
	if (weights_sum == 0 || weights_sum != weights_sum) {

		std::cout << "######### Issue :" << std::endl;
		for (unsigned int i = 0; i < _distances_min.size(); i++) {
			std::cout << "i = " << i << std::endl;
			std::cout << "distances_min[i].first = " << _distances_min[i].first << std::endl;
			std::cout << "distances_min[i].second = " << _distances_min[i].second << std::endl;
			std::cout << "(*weights)[i].first = " << (*_weights)[i].first << std::endl;
			std::cout << "(*weights)[i].second = " << (*_weights)[i].second << std::endl;
		}
		std::cout << "_weight_parameters = " << _weight_parameters << std::endl;
		std::cout << "weights_sum is null" << std::endl;
		DEBUG_BREAK;
	}

	/*! Normalize weights.*/
	for (Tweights::iterator it_weight = _weights->begin(); it_weight != _weights->end(); ++it_weight) {
		it_weight->first /= weights_sum;
	}

}


void SpsInterpolation::get_weights_image(ocv::Timg1& _image) const {

	_image.create(weights_image.size());
	_image = 0.;

	Tweights* weights;
	ocv::Tvec1 buffer;
	/*! For each weights pointer.*/
	for (ocv::Timg_ptr::const_iterator it_weights = weights_image.begin(); it_weights != weights_image.end(); ++it_weights) {
		
		/*! If not null.*/
		if ((*it_weights)[0] != 0) {
			weights = (Tweights*)(*it_weights)[0];
			/*! Apply.*/
			apply_get_weights(weights, _image, buffer);
			
		}

	}

}

void SpsInterpolation::get_weights_image_display(ocv::Timg1& _image) const {

	/*! Get image of computed weights by SpsInterpolation instance.*/
	get_weights_image(_image);
	/*! Apply pow to display a wider spectrum of values.*/
	cv::pow(_image, 0.4, _image);
	/*! Rescale so display is more readable.*/
	ocv::rescale_max(_image);

}

void SpsInterpolation::apply_get_weights(const Tweights* _weights, ocv::Timg1& _image, ocv::Tvec1& _buffer) const {

	ocv::Timg1::iterator it_image = _image.begin();
	/*! For each weights in weights.*/
	for (Tweights::const_iterator it_weights = _weights->begin(); it_weights != _weights->end(); ++it_weights) {
		/*! Move image iterator to position of weight.*/
		it_image += it_weights->second;
		/*! Assign weight value on 3 channels.*/
		_buffer[0] = (ocv::Tvalue)it_weights->first;
		/*! Add weight value.*/
		*it_image += _buffer;
	}

}

void SpsInterpolation::show_weights() const {

	ocv::Timg1 image;
	/*! Get image of computed weights by SpsInterpolation instance.*/
	get_weights_image_display(image);
	ocv::imshow(image, "Interpolation weights");
}

