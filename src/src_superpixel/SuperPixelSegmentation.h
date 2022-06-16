/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "ocv.h"
#include <map>

/*! Class managing segmentation of image using superpixels.*/
class SuperPixelSegmentation {

public:

	/*! Type of labels.*/
	typedef int Tlabel;

	/*! Defines a std::map whose keys are labels.*/
	template<class T>
	using Tlabel_map = std::map<Tlabel, T>;

	class Config;

	struct Parameters {
		/*! Number of superpixels provided as parameter. Stands for initialization. If 0, Nsuperpixels_per_pixel is used instead.*/
		unsigned int Nsuperpixels = (unsigned int)0;
		/*! Number of superpixels per pixel in image. Supposed to be << 1. If Nsuperpixels is set to 0, this value is used instead.*/
		double Nsuperpixels_per_pixel = 0.001;
		/*! Coefficient applied to ruler parameter of SLIC method.
		10.0 => almost grid
		1.0 => smooth segmentation
		0.1 => quite detailed.*/
		float ruler_coef = float(0.5);
		/*! Number of iterations provided as parameter.*/
		unsigned int Niterations = (unsigned int)30;
		/*! Conversion type for image before applying segmentation. (ex: cv::COLOR_BGR2Lab). For original image, set as negative.*/
		int conversion_type = cv::COLOR_BGR2Lab;
		/*! Smoothing window size applied before applying SLIC algorithm.*/
		cv::Size sigma_blur = cv::Size(5, 5);
	};

	struct Result {
		std::vector<cv::Point> centroids_position;
		std::vector<ocv::Tvec> centroids_color;
		cv::Mat_<cv::Vec1i> labels_distribution;
	};

private :

	/*! Original image provided.*/
	ocv::Timg image_original;
	/*! Recolored image if #conversion_type > -1. Otherwise, same a image_original.*/
	ocv::Timg image_recolored;

	/*! Contours of segmentation as provided by SLIC algorithm.*/
	cv::Mat contour;
	/*! Labels of segmentation as provided by SLIC algorithm.*/
	cv::Mat labels;

	Parameters parameters;
	

	/*! Maximum number of superpixels found by slic. This number can raise upper than parameters.Nsuperpixels.
	The actual number of superpixels left can be lower than Nsuperpixels_slic though. In short, slic creates labels, but can also delete some.*/
	unsigned int Nsuperpixels_slic;
	/*! Number of actual superpixels after computation. This number is computed by looking the label's map. This is the real number of superpixels*/
	unsigned int Nsuperpixels_real;
	
	/*! Offset applied to labels computed by SLIC. First label is 0 as output, which in some cases is not convenient.
	Has to different of 0, otherwise causes problems with multiplication operations.*/
	static const int label_index_begin;

	/*! Contains all labels obtained by slic.*/
	std::vector<Tlabel> labels_id;
	/*! Inverse of labels_id. Stores indices at which labels are stored in labels_id.*/
	Tlabel_map<unsigned int> labels_id_inv;
	/*! Number of pixels in each superpixel.*/
	Tlabel_map<unsigned int> Npixels_in_sp;
	/*! Centroid of each superpixel.*/
	Tlabel_map<cv::Point2f> centroid_of_sp;
	/*! Median value in each superpixel.*/
	Tlabel_map<ocv::Tvec> median_in_sp;
	/*! Mean value in each superpixel.*/
	Tlabel_map<ocv::Tvec> mean_in_sp;
	/*! Standard deviation in each superpixel.*/
	Tlabel_map<ocv::Tvec> std_in_sp;

public:

	SuperPixelSegmentation();
	SuperPixelSegmentation(const Parameters& _parameters);
	~SuperPixelSegmentation();

	void clear();

	void set_parameters(const Parameters& _parameters);
	void set_parameters(const unsigned int _Nsuperpixels = 100, const float _ruler_coef=1., const unsigned int _Niterations = 10, const int _conversion_type = -1, const cv::Size _sigma_blur=cv::Size(5,5));
	/*! Start computation.*/
	void compute(const ocv::Timg& _image);
	void compute(const ocv::Timg& _image, Result& _result);

	const ocv::Timg& get_image_original() const;
	const ocv::Timg& get_image_recolored() const;
	void get_image_segmented(ocv::Timg& _image_segmented, const cv::Mat _labels = cv::Mat()) const;
	cv::Size size() const;

	const unsigned int& get_Nsuperpixels() const;
	const unsigned int& get_Niterations() const;
	const int& get_conversion_type() const;
	bool is_recolored() const;

	const unsigned int& get_Nsuperpixels_slic() const;

	const cv::Mat& get_labels() const;
	const std::vector<Tlabel>& get_labels_id() const;
	const Tlabel_map<unsigned int>& get_labels_id_inv() const;
	const Tlabel_map<unsigned int>& get_Npixels_in_sp() const;
	const Tlabel_map<cv::Point2f>& get_centroid_of_sp() const;
	const Tlabel_map<ocv::Tvec>& get_mean_in_sp() const;

	/*! Either apply #contour provided by SLIC or \p _contour and display the result.*/
	void show_segmentation(const ocv::Tmask _contour = ocv::Tmask()) const;
	/*! Either display the #labels provided by SLIC, or \p _labels.*/
	void show_labels(const cv::Mat _labels = cv::Mat()) const;
	/*! Display superpixels by median color.*/
	void show_median_pixels() const;
	/*! Display superpixels by mean color.*/
	void show_mean_pixels() const;
	/*! Display superpixels by standard deviation.*/
	void show_std_pixels() const;
	/*! Sets \p _image_output as 0, and 1 for pixels contained in superpixel labeled \p _label.*/
	void get_label(const Tlabel _label, cv::Mat& _image_output) const;
	/*! Sets \p _image_output as 0, and 1 for pixels contained in superpixel in \p _labels labeled \p _label.*/
	static void get_label(const cv::Mat& _labels, const Tlabel _label, cv::Mat& _image_output);

	///////////////////
	/*! Value standing for no label.*/
	static const Tlabel no_label = std::numeric_limits<Tlabel>::min();
	/*! Type for a pointer to a value in a ocv::Timg, with its correponding coordinates.*/
	typedef std::pair<const ocv::Tvec*, cv::Point> Tcoordata;
	/*! Type for a map which keys are labels, and values are vectors of #Tcoordata.*/
	typedef Tlabel_map< std::vector<Tcoordata> > Tlabels_datas;
	/*! Returns a set of all #Tcoordatas pointing to either #original image if \p l_recolored is false, or #recolored_image if false.
	By default labels used are #labels.*/
	Tlabels_datas get_labels_datas(const cv::Mat _labels = cv::Mat(), const Tlabel _no_label=no_label, bool _l_recolored=false) const;
	static Tlabels_datas get_labels_datas(const cv::Mat& _labels, const ocv::Timg& _image, const Tlabel _no_label = no_label);
	///////////////////

private :
	/*! Computes number of pixels in each superpixel.*/
	void calc_Npixels_in_sp();
public :
	/*! Finds all labels and store them in a vector.*/
	static std::vector<Tlabel> calc_labels_id(const cv::Mat& _labels);
	/*! Computes a map associating to each label the corresponding ordered index.*/
	static Tlabel_map<unsigned int> calc_labels_id_inv(const cv::Mat& _labels);
	/*! Computes number of pixels in each superpixel.*/
	static Tlabel_map<unsigned int> calc_Npixels_in_sp(const cv::Mat& _labels);
	/*! Computes centroid of each superpixel.*/
	static Tlabel_map<cv::Point2f> calc_centroid_of_sp(const cv::Mat& _labels);
	/*! Returns a std::map of median value for each superpixel label in \p _labels. Computed either on original or recolored image depending on \p _l_recolored.
	If \p _label_filter > 0, metric is computed only for this label.*/
	Tlabel_map<ocv::Tvec> calc_median_in_sp(const cv::Mat& _labels, bool _l_recolored=false, const Tlabel _label_filter=no_label) const;
	static Tlabel_map<ocv::Tvec> calc_median_in_sp(const cv::Mat& _labels, const ocv::Timg& _image, const Tlabel _label_filter = no_label);
	/*! Returns a std::map of mean value for each superpixel label in \p _labels. Computed either on original or recolored image depending on \p _l_recolored.
	If \p _label_filter > 0, metric is computed only for this label.*/
	Tlabel_map<ocv::Tvec> calc_mean_in_sp(const cv::Mat& _labels, bool _l_recolored = false, const Tlabel _label_filter = no_label) const;
	static Tlabel_map<ocv::Tvec> calc_mean_in_sp(const cv::Mat& _labels, const ocv::Timg& _image, const Tlabel _label_filter = no_label);
	/*! Returns a std::map of standard deviation value for each superpixel label in \p _labels. Computed either on original or recolored image depending on \p _l_recolored.
	If \p _label_filter > 0, metric is computed only for this label.*/
	Tlabel_map<ocv::Tvec> calc_std_in_sp(const cv::Mat& _labels, bool _l_recolored = false, const Tlabel _label_filter = no_label) const;
	static Tlabel_map<ocv::Tvec> calc_std_in_sp(const cv::Mat& _labels, const ocv::Timg& _image, const Tlabel _label_filter = no_label);
};
