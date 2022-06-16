/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "SuperPixelSegmentation.h"

/*! Class managing merging of superpixels depending on a mask.*/
class SpsMaskMerge {

public :

	class Config;

	struct Parameters {
		/*! Minimum number of pixels unmasked per superpixel.*/
		unsigned int Nknown_pixels_min = (unsigned int)20;
		/*! Coefficient applied to standard deviation measure (versus median) for superpixels when computing distance between superpixels.*/
		float merge_coef = float(10.);
	};

private :

	/*! Pointer to #SuperPixelSegmentation instance.*/
	const SuperPixelSegmentation* sps;
	/*! Used mask.*/
	ocv::Tmask mask;
	/*! Image used for computing statistics such as median or std. To put it another way : In case image used for merging is different from the one used for segmentation.*/
	ocv::Timg image_stats;
	/*! Image of labels after merging.*/
	cv::Mat labels;
	/*! Same as labels, but labels completely outisde the mask are merged into as single one. Convenient to display only clusters partly in the mask.*/
	cv::Mat labels_filtered_outside;

	Parameters parameters;

public:
	SpsMaskMerge();
	SpsMaskMerge(const SuperPixelSegmentation* _sps, const ocv::Tmask& _mask, unsigned int _Nknown_pixels_min=100, float _merge_coef=10.);
	~SpsMaskMerge();

	void set_parameters(const Parameters& _parameters);
	void set_parameters(unsigned int _Nknown_pixels_min=100, float _merge_coef=1.);
	/*! Start computation.*/
	void compute(const SuperPixelSegmentation* _sps, const ocv::Tmask& _mask, const ocv::Timg _image_stats=ocv::Timg());

	const SuperPixelSegmentation* get_sps() const;
	const ocv::Tmask& get_mask() const;

	const cv::Mat& get_labels() const;
	const cv::Mat& get_labels_filtered_outside() const;

	void get_image_segmented(ocv::Timg& _image_segmented) const;
	void get_image_segmented_with_mask(ocv::Timg& _image_segmented) const;
	template <class Timg>
	void get_image_segmented_filtered(const Timg& _image, Timg& _image_segmented, const typename Timg::value_type _segmentation_color = Timg::value_type::all(std::numeric_limits<typename Timg::value_type::value_type>::max())) const;
	template <class Timg>
	void get_image_segmented_filtered_with_mask(const Timg& _image, Timg& _image_segmented, const typename Timg::value_type _segmentation_color = Timg::value_type::all(std::numeric_limits<typename Timg::value_type::value_type>::max())) const;

	/*! Returns datas inside mask. Or datas outside mask if l_opposite is true.*/
	SuperPixelSegmentation::Tlabels_datas get_labels_datas_masked(bool _l_opposite=false, bool _l_recolored=false) const;

private :
	/*! Returns a sorted vector of number of unmasked pixels per superpixel. Each number goes with the corresponding label.*/
	std::vector< std::pair< std::pair<unsigned int, unsigned int>, SuperPixelSegmentation::Tlabel> > calc_Nknown_pixels() const;
	/*! Enforce #Nknown_pixels_min criteria to \p _sorted_Nknown_pixels.*/
	void merging(std::vector< std::pair< std::pair<unsigned int, unsigned int>, SuperPixelSegmentation::Tlabel> >& _sorted_Nknown_pixels);
	/*! Returns neighbour labels of label \p _label*/
	std::vector<SuperPixelSegmentation::Tlabel> find_neighbours(const SuperPixelSegmentation::Tlabel _label) const;
	/*! Returns best neighbour.*/
	SuperPixelSegmentation::Tlabel find_best_neighbour(const SuperPixelSegmentation::Tlabel _label, const SuperPixelSegmentation::Tlabel_map<ocv::Tvec>& _median_in_sp, const SuperPixelSegmentation::Tlabel_map<ocv::Tvec>& _std_in_sp) const;
	/*! Merge two superpixels. \p _label_merged stands for the superpixel getting absorbed by the other, and so disappearing.*/
	void merge_super_pixels(SuperPixelSegmentation::Tlabel _label, SuperPixelSegmentation::Tlabel _label_merged, std::vector< std::pair< std::pair<unsigned int, unsigned int>, SuperPixelSegmentation::Tlabel> >& _sorted_Nknown_pixels, const SuperPixelSegmentation::Tlabel_map<unsigned int>& _sort_map);

	/*! Misc functions.*/
	/*! Merge superpixels without any pixel inside the mask as one "outside" label. Convenient to only display superpixels being partly in the mask.*/
	void get_filtered_outside_labels(cv::Mat& _labels_outside) const;
};

#include "Contour.h"

template <class Timg>
void SpsMaskMerge::get_image_segmented_filtered(const Timg& _image, Timg& _image_segmented, const typename Timg::value_type _segmentation_color) const {

	if (sps) {
		Contour::apply_segmentation_from_labels(_image, labels_filtered_outside, _image_segmented, _segmentation_color);
	}
	
}

template <class Timg>
void SpsMaskMerge::get_image_segmented_filtered_with_mask(const Timg& _image, Timg& _image_segmented, const typename Timg::value_type _segmentation_color) const {

	if (sps) {
		get_image_segmented_filtered(_image, _image_segmented);
		Contour::apply_segmentation_from_labels(_image_segmented, mask, _image_segmented, _segmentation_color);
	}
	
}
