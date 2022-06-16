/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "SuperPixelSegmentation.h"
class SpsMaskMerge;

/*! Class in charge of computing interpolation weights using a #SpsMaskMerge and applying them on a new image.*/
class SpsInterpolation {

public :

	class Config;

	struct Parameters {
		/*! Maximum number of pixels used for interpolation weights.*/
		unsigned int Nweight_pixels = (unsigned int)20;
		/*! Coefficient applied to color distance (vs pixel spatial distance).
		If set ton infinity, yields color-driven interpolation.
		If set to 0, yields distance-driven interpolation.
		0.00001 seems like a good trade-off.*/
		double distance_coef = 0.00001;
		/*! Coefficient applied to standard deviation used for gaussian weighting. If infinity, all weights have the same value.*/
		double sigma_coef = 1.;
	};

private :

	/*! Image containing pointers to #Tweights.*/
	ocv::Timg_ptr weights_image;

	/*! Type of weight. First element of pair stands for weight value, second one for iterator offset relative to the previous weight in ascending image iteration order.*/
	typedef std::pair<double, int> Tweight;
	/*! Type of weights. A pixel contains a pointer to this type.*/
	typedef std::vector<Tweight> Tweights;

	Parameters parameters;

public:

	SpsInterpolation();
	SpsInterpolation(const SpsMaskMerge* _merger);
	~SpsInterpolation();

	void set_parameters(const Parameters& _parameters);
	void set_parameters(unsigned int _Nweight_pixels = 10, double _distance_coef = 0.00001, double _sigma_coef = 1.);
	/*! Start computation.*/
	void compute(const SpsMaskMerge* _merger);
	
	/*! Apply interpolation weights (ie: apply convolution) to \p _input_image and save result in \p _output_image.*/
	template <class Tvec>
	void apply(const cv::Mat_<Tvec>& _input_image, cv::Mat_<Tvec>& _output_image) const;
	/*! Sum weights values at their respective image position on \p _image.*/
	void get_weights_image(ocv::Timg1& _image) const;
	/*! Sum weights values at their respective image position on \p _image.*/
	void get_weights_image_display(ocv::Timg1& _image) const;
	/*! Show the result of #get_weights_image function and applies log/norm operator for display purpose.*/
	void show_weights() const;

private :

	/*! Compute for each label a user-free coefficient applied to distance calculation between two Tcoordata in #calc_distance.*/
	void calc_distance_coefs(const SuperPixelSegmentation::Tlabels_datas& _labels_datas, SuperPixelSegmentation::Tlabel_map<double>& _distance_coefs) const;
	/*! Compute the Nweight_pixels mininmum distances corresponding to the Nweight_pixels closest pixels in \p _coordatas relatively to \p _coordata.*/
	void calc_distances_min(const SuperPixelSegmentation::Tcoordata& _coordata, const std::vector<SuperPixelSegmentation::Tcoordata>* _coordatas, const double _distance_coef, std::vector< std::pair<double, const cv::Point*> >& _distances_min) const;
	/*! Compute mu and sigma for gaussian function used in weight calculation. \p _distances_min_stats is a buffer vector*/
	std::pair<double, double> calc_weight_parameters(const std::vector< std::pair<double, const cv::Point*> >& _distances_min, std::vector<double>& _distances_min_stats) const;
	/*! Compute weights \p _weights using detemined pixel distances and parameters.*/
	void calc_weights(const std::vector< std::pair<double, const cv::Point*> >& _distances_min, const std::pair<double, double>& _weight_parameters, const cv::Size& _size, Tweights* _weights) const;

	/*! Compute distance between two pixels.*/
	double calc_distance(const SuperPixelSegmentation::Tcoordata& _value1, const SuperPixelSegmentation::Tcoordata& _value2, const double _distance_coef, std::pair<ocv::Tvec, cv::Point>& _buffer) const;
	/*! Apply \p _weights at a pixel on an \p _image and write \p _result.*/
	template <class Tvec>
	void apply_weights(const Tweights* _weights, const cv::Mat_<Tvec>& _image, Tvec& _result) const;
	/*! Apply \p _weights values on an \p _image.*/
	void apply_get_weights(const Tweights* _weights, ocv::Timg1& _image, ocv::Tvec1& _buffer) const;

};

template <class Tvec>
void SpsInterpolation::apply(const cv::Mat_<Tvec>& _input_image, cv::Mat_<Tvec>& _output_image) const {

	/*! Resize _input_image so it fits dimensions of this SpsInterpolation instance.*/
	cv::resize(_input_image, _output_image, weights_image.size());

	Tweights* weights;
	cv::MatIterator_<Tvec> it_image = _output_image.begin();
	/*! Lookup in weights_image to get corresponding weights pointer at pixel.*/
	for (ocv::Timg_ptr::const_iterator it_weights = weights_image.begin(); it_weights != weights_image.end(); ++it_weights, ++it_image) {

		/*! If pointer is not null.*/
		if ((*it_weights)[0] != 0) {
			/*! Cast it.*/
			weights = (Tweights*)(*it_weights)[0];
			/*! Apply these weights to image.*/
			apply_weights(weights, _input_image, *it_image);
		}

	}

}

template <class Tvec>
void SpsInterpolation::apply_weights(const Tweights* _weights, const cv::Mat_<Tvec>& _image, Tvec& _result) const {

	_result = 0;
	Tvec value;
	cv::MatConstIterator_<Tvec> it_image = _image.begin();
	/*! For each weight.*/
	for (Tweights::const_iterator it_weights = _weights->begin(); it_weights != _weights->end(); ++it_weights) {
		/*! Move image iterator to position of weight.*/
		it_image += it_weights->second;
		/*! Get image value.*/
		value = *it_image;
		/*! Multiply by weight value.*/
		value *= it_weights->first;
		/*! Add to result.*/
		_result += value;
	}

}
