/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "SpsMaskMerge.h"

#include "misc_funcs.h"//ostream vector
#include "Contour.h"
#include "ocv_rw.h"
/*! For writing video of superpixel merging.*/

SpsMaskMerge::SpsMaskMerge() {

	sps = 0;

	set_parameters();
}

SpsMaskMerge::SpsMaskMerge(const SuperPixelSegmentation* _sps, const ocv::Tmask& _mask, unsigned int _Nknown_pixels_min, float _merge_coef) {

	set_parameters(_Nknown_pixels_min, _merge_coef);
	compute(_sps, _mask);
}

SpsMaskMerge::~SpsMaskMerge() {

}

void SpsMaskMerge::set_parameters(const Parameters& _parameters) {

	parameters = _parameters;

}

void SpsMaskMerge::set_parameters(unsigned int _Nknown_pixels_min, float _merge_coef) {

	parameters.Nknown_pixels_min = _Nknown_pixels_min;
	parameters.merge_coef = _merge_coef;
}

void SpsMaskMerge::compute(const SuperPixelSegmentation* _sps, const ocv::Tmask& _mask, const ocv::Timg _image_stats) {

	if (_sps->size() == _mask.size()) {

		std::cout << "Computing superpixels merging" << std::endl;

		sps = _sps;
		/*! Init labels of merging.*/
		sps->get_labels().copyTo(labels);

		mask = _mask;

		if (_image_stats.size() == sps->get_image_original().size()) {
			image_stats = _image_stats;
		} else {
			if (sps->is_recolored()) {
				image_stats = sps->get_image_recolored();
			} else {
				image_stats = sps->get_image_original();
			}
		}

		/*! Compute number of pixels per label in ascending order.*/
		std::vector< std::pair< std::pair<unsigned int, unsigned int>, SuperPixelSegmentation::Tlabel> > sorted_Nknown_pixels = calc_Nknown_pixels();
		/*! Merge superpixels.*/
		merging(sorted_Nknown_pixels);

		//sps->show_labels(labels);
		ocv::Tmask contour;
		Contour::get_segmentation(labels, contour);
		//sps->show_segmentation(contour);

		/*! Optional post processing for result display. Merge superpixels without any pixel inside the mask as one "outside" label.
		Convenient to only display superpixels being partly in the mask.*/
		get_filtered_outside_labels(labels_filtered_outside);
		ocv::Timg image_filtered;
		sps->get_image_segmented(image_filtered, labels_filtered_outside);
		Contour::apply_segmentation_from_labels(image_filtered, mask, image_filtered, ocv::Tvec(0, 0, 1.));
		ocv::imwrite("image_filtered", image_filtered);

	} else {
		std::cout << "_sps->size() = " << _sps->size() << ", _mask.size() = " << _mask.size() << std::endl;
		DEBUG_BREAK;
	}

}

const SuperPixelSegmentation* SpsMaskMerge::get_sps() const {

	return sps;
}

const ocv::Tmask& SpsMaskMerge::get_mask() const {

	return mask;
}

const cv::Mat& SpsMaskMerge::get_labels() const {

	return labels;
}

const cv::Mat& SpsMaskMerge::get_labels_filtered_outside() const {

	return labels_filtered_outside;
}

void SpsMaskMerge::get_image_segmented(ocv::Timg& _image_segmented) const {

	if (sps) {
		sps->get_image_segmented(_image_segmented, labels);
	}

}

void SpsMaskMerge::get_image_segmented_with_mask(ocv::Timg& _image_segmented) const {

	get_image_segmented(_image_segmented);
	Contour::apply_segmentation_from_labels(_image_segmented, mask, _image_segmented, ocv::Tvec(0, 0, 1.));
}

std::vector< std::pair< std::pair<unsigned int, unsigned int>, SuperPixelSegmentation::Tlabel> > SpsMaskMerge::calc_Nknown_pixels() const {

	/*! Sort by : 1) Number of known pixel, 2) Number of pixels. If two superpixels have, let's say 0 known pixels, the smallest one will be merged first.*/

	SuperPixelSegmentation::Tlabel_map<unsigned int> map_Npixels = SuperPixelSegmentation::calc_Npixels_in_sp(labels);

	SuperPixelSegmentation::Tlabel_map<unsigned int> map_Nknown_pixels;
	SuperPixelSegmentation::Tlabel label_value;
	cv::MatConstIterator_<cv::Vec1b> it_mask = mask.begin();
	/*! Compute number of known (ie unmasked) pixels for every superpixel.*/
	for (cv::MatConstIterator_<cv::Vec1i> it_labels = labels.begin<cv::Vec1i>(); it_labels != labels.end<cv::Vec1i>(); ++it_labels, ++it_mask) {

		label_value = (*it_labels)[0];

		if (label_value != 0) {
			/*! If map key is not created yet, initialize it.*/
			if (!map_Nknown_pixels.count(label_value)) {
				map_Nknown_pixels[label_value] = 0;
			}

			if ((*it_mask)[0] != ocv::mask_value) {
				map_Nknown_pixels[label_value]++;
			}
		}

	}

	/*! Prepare sort algorithm. Sort based on number of known pixels in SP. Increasing.*/
	std::vector< std::pair< std::pair<unsigned int, unsigned int>, SuperPixelSegmentation::Tlabel> > sorted_Nknown_pixels;
	std::pair< std::pair<unsigned int, unsigned int>, SuperPixelSegmentation::Tlabel> element;//Npixels, SP label
	std::map<SuperPixelSegmentation::Tlabel, unsigned int>::iterator it_number = map_Npixels.begin();
	for (std::map<SuperPixelSegmentation::Tlabel, unsigned int>::iterator it_known = map_Nknown_pixels.begin(); it_known != map_Nknown_pixels.end(); ++it_known, ++it_number) {

		element.first.first = it_known->second;
		element.first.second = it_number->second;
		element.second = it_known->first;
		sorted_Nknown_pixels.push_back(element);
	}

	std::sort(sorted_Nknown_pixels.begin(), sorted_Nknown_pixels.end());

	return sorted_Nknown_pixels;

}

void SpsMaskMerge::merging(std::vector< std::pair< std::pair<unsigned int, unsigned int>, SuperPixelSegmentation::Tlabel> >& _sorted_Nknown_pixels) {

	bool l_write_evolution = false;
	/*! For debug.*/
	ocv::Timg image_evolution;

	/*! Sort them again, just in case.*/
	std::sort(_sorted_Nknown_pixels.begin(), _sorted_Nknown_pixels.end());

	/*! Get median and std for each superpixel. Use recolored version or not depending on sps parametrization.*/
	SuperPixelSegmentation::Tlabel_map<ocv::Tvec> median_in_sp = SuperPixelSegmentation::calc_median_in_sp(labels, image_stats);
	SuperPixelSegmentation::Tlabel_map<ocv::Tvec> std_in_sp = SuperPixelSegmentation::calc_std_in_sp(labels, image_stats);

	SuperPixelSegmentation::Tlabel_map<unsigned int> sort_map;

	SuperPixelSegmentation::Tlabel label_merge;
	SuperPixelSegmentation::Tlabel label_best_neighbour=1;

	/*! Calc number of superpixels to merge.*/
	unsigned int Nsuperpixels_to_merge=0;
	std::vector< std::pair< std::pair<unsigned int, unsigned int>, SuperPixelSegmentation::Tlabel> >::const_iterator it = _sorted_Nknown_pixels.begin();
	while (it != _sorted_Nknown_pixels.end() && it->first.first < parameters.Nknown_pixels_min) {
		Nsuperpixels_to_merge++;
		++it;
	}
	

	unsigned int count_superpixels_merged = 0;
	/*! While there's still superpixels containing less than Nknown_pixels_min known pixels.*/
	while (_sorted_Nknown_pixels.size() > 0 && _sorted_Nknown_pixels[0].first.first < parameters.Nknown_pixels_min && label_best_neighbour != 0) {


		/*! Prepare ordering of labels.*/
		sort_map.clear();
		for (unsigned int i = 0; i < _sorted_Nknown_pixels.size(); i++) {
			sort_map[_sorted_Nknown_pixels[i].second] = i;
		}

		/*! Label to merge is the first.*/
		label_merge = _sorted_Nknown_pixels[0].second;
		label_best_neighbour = find_best_neighbour(label_merge, median_in_sp, std_in_sp);
		if (label_best_neighbour != 0) {

			/*! Safety debug.*/
			if (sort_map.find(label_best_neighbour) == sort_map.end()) {
				std::cout << "label_best_neighbour = " << label_best_neighbour << " is not in sort_map = " << sort_map << std::endl;
				DEBUG_BREAK;
			}

			merge_super_pixels(label_best_neighbour, label_merge, _sorted_Nknown_pixels, sort_map);
			std::sort(_sorted_Nknown_pixels.begin(), _sorted_Nknown_pixels.end());
			/*! Update median and std*/
			median_in_sp.erase(label_merge);
			median_in_sp[label_best_neighbour] = SuperPixelSegmentation::calc_median_in_sp(labels, image_stats, label_best_neighbour)[label_best_neighbour];
			std_in_sp.erase(label_merge);
			std_in_sp[label_best_neighbour] = SuperPixelSegmentation::calc_std_in_sp(labels, image_stats, label_best_neighbour)[label_best_neighbour];
		} else {
			std::cout << "There's no neighbour for super pixel : " << label_merge << std::endl;
			std::cout << "Therefore, merging leads to a unique superpixel. Try lowering Nknown_pixels_min = " << parameters.Nknown_pixels_min << std::endl;
			//DEBUG_BREAK;
		}

		Misc::display_progression(count_superpixels_merged, Nsuperpixels_to_merge);
		count_superpixels_merged++;

		if (l_write_evolution) {
			get_image_segmented_with_mask(image_evolution);
			ocv::imwrite("merge_evolution_" + Misc::int_to_string(count_superpixels_merged), image_evolution);
		}
		
	}

}

std::vector<SuperPixelSegmentation::Tlabel> SpsMaskMerge::find_neighbours(const SuperPixelSegmentation::Tlabel _label) const {

	std::vector<SuperPixelSegmentation::Tlabel> neighbours_list;

	if (sps) {

		/*! Get image of only _label.*/
		cv::Mat label_specified;
		SuperPixelSegmentation::get_label(labels, _label, label_specified);

		/*! Dilate it.*/
		cv::Mat dilated;
		cv::dilate(label_specified, dilated, cv::Mat());

		/*! Subtract and leave only outer edges of _label image.*/
		dilated -= label_specified;

		/*! Look for all labels existing in computed edge.*/
		cv::MatConstIterator_<cv::Vec1b> it_dilated = dilated.begin<cv::Vec1b>();
		cv::MatConstIterator_<cv::Vec1b> it_mask = mask.begin();
		for (cv::MatConstIterator_<cv::Vec1i> it_labels = labels.begin<cv::Vec1i>(); it_labels != labels.end<cv::Vec1i>(); ++it_labels, ++it_dilated, ++it_mask) {

			if ((*it_dilated)[0] == 1) {

				neighbours_list.push_back((*it_labels)[0]);
			}

		}

		/*! Sort the list.*/
		std::sort(neighbours_list.begin(), neighbours_list.end());
		/*! Remove contiguous duplicates.*/
		neighbours_list.erase(std::unique(neighbours_list.begin(), neighbours_list.end()), neighbours_list.end());

	} else {
		std::cout << "No sps set" << std::endl;
	}

	return neighbours_list;
}


SuperPixelSegmentation::Tlabel SpsMaskMerge::find_best_neighbour(const SuperPixelSegmentation::Tlabel _label, const SuperPixelSegmentation::Tlabel_map<ocv::Tvec>& _median_in_sp, const SuperPixelSegmentation::Tlabel_map<ocv::Tvec>& _std_in_sp) const {

	SuperPixelSegmentation::Tlabel best_label_index=0;

	/*! Get all neighbours around _label.*/
	std::vector<SuperPixelSegmentation::Tlabel> neighbours_list = find_neighbours(_label);

	if (neighbours_list.size() > 0) {
		double value;
		double value_min = std::numeric_limits<double>::max();
		best_label_index = 0;

		for (unsigned int n = 0; n < neighbours_list.size(); n++) {

			/*! Compute distance.*/
			value = cv::norm(_std_in_sp.at(_label) - _std_in_sp.at(neighbours_list[n]), cv::NORM_L2SQR);
			value *= parameters.merge_coef;
			value += cv::norm(_median_in_sp.at(_label) - _median_in_sp.at(neighbours_list[n]), cv::NORM_L2SQR);

			if (value < value_min) {
				value_min = value;
				best_label_index = neighbours_list[n];
			}
		}

		/*! Safety debug.*/
		if (best_label_index == _label) {
			std::cout << "best_label_index == _index = " << _label << std::endl;
			DEBUG_BREAK;
		}
	}

	return best_label_index;
}


void SpsMaskMerge::merge_super_pixels(SuperPixelSegmentation::Tlabel _label, SuperPixelSegmentation::Tlabel _label_merged, std::vector< std::pair< std::pair<unsigned int, unsigned int>, SuperPixelSegmentation::Tlabel> >& _sorted_Nknown_pixels, const SuperPixelSegmentation::Tlabel_map<unsigned int>& _sort_map) {

	/*! Replace labels on image.*/
	for (cv::MatIterator_<int> it_labels = labels.begin<int>(); it_labels != labels.end<cv::Vec1i>(); ++it_labels) {

		if ((*it_labels) == _label_merged) {
			(*it_labels) = _label;
		}
	}

	/*! Add number of known pixels from merged superpixel, to the destination one.*/
	_sorted_Nknown_pixels[_sort_map.at(_label)].first.first += _sorted_Nknown_pixels[0].first.first;
	/*! Add number of pixels from merged superpixel, to the destination one.*/
	_sorted_Nknown_pixels[_sort_map.at(_label)].first.second += _sorted_Nknown_pixels[0].first.second;
	/*! Remove merged superpixel.*/
	_sorted_Nknown_pixels.erase(_sorted_Nknown_pixels.begin());
}

SuperPixelSegmentation::Tlabels_datas SpsMaskMerge::get_labels_datas_masked(bool _l_opposite, bool _l_recolored) const {

	cv::Mat labels_tmp;
	labels.copyTo(labels_tmp);

	bool l_filter;

	/*! Prepare an image of labels in which unmasked data are set to a nan value.*/
	/*! Lookup on labels image.*/
	cv::MatIterator_<cv::Vec1i> it_labels = labels_tmp.begin<cv::Vec1i>();
	for (cv::MatConstIterator_<cv::Vec1b> it_mask = mask.begin(); it_mask != mask.end(); ++it_mask, ++it_labels) {

		l_filter = (*it_mask)[0] == ocv::mask_value;
		if (!_l_opposite) {
			l_filter = !l_filter;
		}

		if (l_filter) {
			(*it_labels)[0] = SuperPixelSegmentation::no_label;
		}

	}

	/*! Get labels datas using this image and corresponding nan value filter.*/

	return SuperPixelSegmentation::get_labels_datas(labels_tmp, image_stats, SuperPixelSegmentation::no_label);
}

void SpsMaskMerge::get_filtered_outside_labels(cv::Mat& _labels_outside) const {

	SuperPixelSegmentation::Tlabel_map<unsigned int> map_Nunknown_pixels;
	SuperPixelSegmentation::Tlabel label_value;
	cv::MatConstIterator_<cv::Vec1b> it_mask = mask.begin();
	/*! Compute number of known (ie unmasked) pixels for every superpixel.*/
	for (cv::MatConstIterator_<cv::Vec1i> it_labels = labels.begin<cv::Vec1i>(); it_labels != labels.end<cv::Vec1i>(); ++it_labels, ++it_mask) {

		label_value = (*it_labels)[0];

		if (label_value != 0) {
			/*! If map key is not created yet, initialize it.*/
			if (!map_Nunknown_pixels.count(label_value)) {
				map_Nunknown_pixels[label_value] = 0;
			}

			if ((*it_mask)[0] == ocv::mask_value) {
				map_Nunknown_pixels[label_value]++;
			}
		}

	}

	labels.copyTo(_labels_outside);
	SuperPixelSegmentation::Tlabel filtered_label_value = std::numeric_limits<int>::max();

	for (cv::MatIterator_<cv::Vec1i> it_labels = _labels_outside.begin<cv::Vec1i>(); it_labels != _labels_outside.end<cv::Vec1i>(); ++it_labels) {

		label_value = (*it_labels)[0];

		/*! If there is no unknown pixel.*/
		if (map_Nunknown_pixels[label_value] == 0) {

			(*it_labels)[0] = filtered_label_value;

		}

	}

}