/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "SuperPixelSegmentation.h"

#include <opencv2/ximgproc/slic.hpp>
#if defined(_MSC_VER)
#pragma warning(push, 0)//Disable warnings for boost accumulators (full of unresolved warnings)
#endif
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#include "Contour.h"

SuperPixelSegmentation::SuperPixelSegmentation() {

	Nsuperpixels_slic = 0;
	set_parameters();
}

SuperPixelSegmentation::SuperPixelSegmentation(const Parameters& _parameters):SuperPixelSegmentation() {

	set_parameters(_parameters);
}

SuperPixelSegmentation::~SuperPixelSegmentation() {

}

void SuperPixelSegmentation::clear() {

	image_original.release();
	image_recolored.release();

	contour.release();
	labels.release();

	Nsuperpixels_slic = 0;

	labels_id.clear();
	labels_id_inv.clear();
	Npixels_in_sp.clear();
	centroid_of_sp.clear();
	median_in_sp.clear();
	mean_in_sp.clear();
	std_in_sp.clear();

}

void SuperPixelSegmentation::set_parameters(const Parameters& _parameters) {

	parameters = _parameters;
}

void SuperPixelSegmentation::set_parameters(const unsigned int _Nsuperpixels, const float _ruler_coef, const unsigned int _Niterations, const int _conversion_type, const cv::Size _sigma_blur) {

	Parameters parameters;
	parameters.Nsuperpixels = _Nsuperpixels;
	parameters.Nsuperpixels_per_pixel = 0.;
	parameters.ruler_coef = _ruler_coef;
	parameters.Niterations = _Niterations;
	parameters.conversion_type = _conversion_type;
	parameters.sigma_blur = _sigma_blur;
	set_parameters(parameters);
}

const int SuperPixelSegmentation::label_index_begin = 1;

const ocv::Timg& SuperPixelSegmentation::get_image_original() const {

	return image_original;
}

const ocv::Timg& SuperPixelSegmentation::get_image_recolored() const {

	return image_recolored;
}

void SuperPixelSegmentation::get_image_segmented(ocv::Timg& _image_segmented, const cv::Mat _labels) const {

	cv::Mat labels;
	if (_labels.size() == cv::Size()) {
		labels = this->labels;
	} else {
		labels = _labels;
	}
	Contour::apply_segmentation_from_labels(image_original, labels, _image_segmented);
}

const unsigned int& SuperPixelSegmentation::get_Nsuperpixels() const {

	return parameters.Nsuperpixels;

}

const unsigned int& SuperPixelSegmentation::get_Niterations() const {

	return parameters.Niterations;
}

const int& SuperPixelSegmentation::get_conversion_type() const {

	return parameters.conversion_type;
}

bool SuperPixelSegmentation::is_recolored() const {

	return parameters.conversion_type != -1;
}

const unsigned int& SuperPixelSegmentation::get_Nsuperpixels_slic() const {

	return Nsuperpixels_slic;
}

cv::Size SuperPixelSegmentation::size() const {

	return image_original.size();
}

const cv::Mat& SuperPixelSegmentation::get_labels() const {

	return labels;
}

const std::vector<SuperPixelSegmentation::Tlabel>& SuperPixelSegmentation::get_labels_id() const {

	return labels_id;
}

const SuperPixelSegmentation::Tlabel_map<unsigned int>& SuperPixelSegmentation::get_labels_id_inv() const {

	return labels_id_inv;
}

const SuperPixelSegmentation::Tlabel_map<unsigned int>& SuperPixelSegmentation::get_Npixels_in_sp() const {

	return Npixels_in_sp;
}

const SuperPixelSegmentation::Tlabel_map<cv::Point2f>& SuperPixelSegmentation::get_centroid_of_sp() const {

	return centroid_of_sp;
}

const SuperPixelSegmentation::Tlabel_map<ocv::Tvec>& SuperPixelSegmentation::get_mean_in_sp() const {

	return mean_in_sp;
}

void SuperPixelSegmentation::compute(const ocv::Timg& _image) {

	Result result;
	compute(_image, result);

}

void SuperPixelSegmentation::compute(const ocv::Timg& _image, Result& _result) {

	std::cout << "Computing superpixel segmentation" << std::endl;

	image_original = _image;

	if (parameters.Nsuperpixels == 0) {
		parameters.Nsuperpixels = (unsigned int)(image_original.size().area()*parameters.Nsuperpixels_per_pixel);
	}

	/*! Compute an average size of superpixels in pixels.*/
	unsigned int Npixels = (unsigned int)std::sqrt(image_original.size().area()/ parameters.Nsuperpixels);
	/*! ruler is a sharpness factor. Lower it is, sharper it is.
	Set to be proportional to Npixels. Looks like a good heuristic.
	*/
	float ruler = Npixels*parameters.ruler_coef;

	if (parameters.conversion_type >= 0) {
		cvtColor(image_original, image_recolored, parameters.conversion_type);
	} else {
		image_recolored = image_original;
	}

	ocv::Timg image_blured;

	
	cv::GaussianBlur(image_recolored, image_blured, parameters.sigma_blur, 0, 0);
	if (!is_recolored()) {
		/*! To roughly match scale after CieLAB conversion.*/
		image_blured *= 100.;
	}
	
	cv::Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(image_blured, cv::ximgproc::SLIC, Npixels, ruler);
	slic->iterate(parameters.Niterations);
	cv::Mat result_slic;
	slic->getLabelContourMask(contour);
	contour *= ocv::mask_value;
	slic->getLabels(labels);
	labels += label_index_begin;

	/*! Maximum number of superpixels found by slic. This number can raise upper than parameters.Nsuperpixels.
	The actual number of superpixels left can be lower than Nsuperpixels_slic though. In short, slic creates labels, but can also delete some.*/
	Nsuperpixels_slic = slic->getNumberOfSuperpixels();

	labels_id = calc_labels_id(labels);
	labels_id_inv = calc_labels_id_inv(labels);
	calc_Npixels_in_sp();
	centroid_of_sp = calc_centroid_of_sp(labels);
	median_in_sp = calc_median_in_sp(labels);
	mean_in_sp = calc_mean_in_sp(labels);
	std_in_sp = calc_std_in_sp(labels);

	Nsuperpixels_real = (unsigned int)get_labels_id().size();

	////// Fill result //////////////////////

	_result.centroids_position.resize(Nsuperpixels_real);
	_result.centroids_color.resize(Nsuperpixels_real);

	SuperPixelSegmentation::Tlabel label;

	for (int n = 0; n < (int)Nsuperpixels_real; n++) {

		label = get_labels_id()[n];

		_result.centroids_position[n].x = (int)get_centroid_of_sp().at(label).x;
		_result.centroids_position[n].y = (int)get_centroid_of_sp().at(label).y;

		_result.centroids_color[n] = get_mean_in_sp().at(label);

	}

	get_labels().copyTo(_result.labels_distribution);

	/*! So the labels are ordinated from 0 to Nlabels-1, since slic process can make labels disappear over iterations.*/
	for (cv::Mat_<cv::Vec1i>::iterator it = _result.labels_distribution.begin(); it != _result.labels_distribution.end(); ++it) {

		(*it)[0] = get_labels_id_inv().at((*it)[0]);

	}

}

std::vector<SuperPixelSegmentation::Tlabel> SuperPixelSegmentation::calc_labels_id(const cv::Mat& _labels) {

	std::vector<Tlabel> labels_id;

	Tlabel_map<unsigned int> Npixels_in_sp = calc_Npixels_in_sp(_labels);

	for (Tlabel_map<unsigned int>::const_iterator it = Npixels_in_sp.begin(); it != Npixels_in_sp.end(); ++it) {
		labels_id.push_back(it->first);
	}


	return labels_id;

}

SuperPixelSegmentation::Tlabel_map<unsigned int> SuperPixelSegmentation::calc_labels_id_inv(const cv::Mat& _labels) {

	Tlabel_map<unsigned int> labels_id_inv;

	std::vector<Tlabel> labels_id = calc_labels_id(_labels);

	std::vector<Tlabel>::const_iterator begin = labels_id.begin();
	for (std::vector<Tlabel>::const_iterator it = begin; it != labels_id.end(); ++it) {

		labels_id_inv[(*it)] = (unsigned int)std::distance(begin, it);

	}

	return labels_id_inv;
}

void SuperPixelSegmentation::calc_Npixels_in_sp() {

	Npixels_in_sp = calc_Npixels_in_sp(labels);
}

SuperPixelSegmentation::Tlabel_map<unsigned int> SuperPixelSegmentation::calc_Npixels_in_sp(const cv::Mat& _labels) {

	Tlabel_map<unsigned int> Npixels_in_sp;

	for (cv::MatConstIterator_<cv::Vec1i> it_labels = _labels.begin<cv::Vec1i>(); it_labels != _labels.end<cv::Vec1i>(); ++it_labels) {

		if (Npixels_in_sp.count((*it_labels)[0]) == 0) {
			Npixels_in_sp[(*it_labels)[0]] = 0;
		}

		Npixels_in_sp[(*it_labels)[0]]++;

	}

	return Npixels_in_sp;
}

SuperPixelSegmentation::Tlabel_map<cv::Point2f> SuperPixelSegmentation::calc_centroid_of_sp(const cv::Mat& _labels) {

	Tlabel_map<cv::Point2f> centroid_of_sp;

	cv::Point coordinates(0, 0);
	for (cv::MatConstIterator_<cv::Vec1i> it_labels = _labels.begin<cv::Vec1i>(); it_labels != _labels.end<cv::Vec1i>(); ++it_labels) {

		if (centroid_of_sp.count((*it_labels)[0]) == 0) {
			centroid_of_sp[(*it_labels)[0]] = cv::Point2f(0., 0.);
		}

		centroid_of_sp[(*it_labels)[0]] += cv::Point2f(coordinates);

		coordinates.x++;
		if (coordinates.x == _labels.size().width) {
			coordinates.y++;
			coordinates.x = 0;
		}
	}

	Tlabel_map<unsigned int> Npixels_in_sp = calc_Npixels_in_sp(_labels);

	Tlabel_map<unsigned int>::const_iterator it_number = Npixels_in_sp.begin();
	for (Tlabel_map<cv::Point2f>::iterator it_centroid = centroid_of_sp.begin(); it_centroid != centroid_of_sp.end(); ++it_centroid, ++it_number) {
		it_centroid->second.x /= it_number->second;
		it_centroid->second.y /= it_number->second;
	}

	return centroid_of_sp;
}

SuperPixelSegmentation::Tlabel_map<ocv::Tvec> SuperPixelSegmentation::calc_median_in_sp(const cv::Mat& _labels, bool _l_recolored, const Tlabel _label_filter) const {

	ocv::Timg image_use;
	if (_l_recolored) {
		image_use = image_recolored;
	} else {
		image_use = image_original;
	}

	return calc_median_in_sp(_labels, image_use, _label_filter);
}

SuperPixelSegmentation::Tlabel_map<ocv::Tvec> SuperPixelSegmentation::calc_median_in_sp(const cv::Mat& _labels, const ocv::Timg& _image, const Tlabel _label_filter) {

	Tlabel_map<ocv::Tvec> median_in_sp;

	if (_labels.size() == _image.size()) {

		namespace bacc = boost::accumulators;

		typedef Tlabel_map< bacc::accumulator_set<ocv::Tvalue, bacc::stats<bacc::tag::median > > > Tmap_accumulators;

		Tmap_accumulators accumulators_b;
		Tmap_accumulators accumulators_g;
		Tmap_accumulators accumulators_r;


		Tlabel label_value;
		cv::MatConstIterator_<ocv::Tvec> it_image = _image.begin();
		for (cv::MatConstIterator_<cv::Vec1i> it_labels = _labels.begin<cv::Vec1i>(); it_labels != _labels.end<cv::Vec1i>(); ++it_labels, ++it_image) {

			label_value = (*it_labels)[0];
			if (_label_filter == no_label || label_value == _label_filter) {
				accumulators_b[label_value]((*it_image)[0]);
				accumulators_g[label_value]((*it_image)[1]);
				accumulators_r[label_value]((*it_image)[2]);
			}

		}

		for (Tmap_accumulators::const_iterator it = accumulators_b.begin(); it != accumulators_b.end(); ++it) {
			median_in_sp[it->first][0] = bacc::median(accumulators_b[it->first]);
			median_in_sp[it->first][1] = bacc::median(accumulators_g[it->first]);
			median_in_sp[it->first][2] = bacc::median(accumulators_r[it->first]);
		}

	} else {
		std::cout << "calc_median_in_sp : wrong size" << std::endl;
	}

	return median_in_sp;

}

SuperPixelSegmentation::Tlabel_map<ocv::Tvec> SuperPixelSegmentation::calc_mean_in_sp(const cv::Mat& _labels, bool _l_recolored, const Tlabel _label_filter) const {

	ocv::Timg image_use;
	if (_l_recolored) {
		image_use = image_recolored;
	} else {
		image_use = image_original;
	}

	return calc_mean_in_sp(_labels, image_use, _label_filter);
}

SuperPixelSegmentation::Tlabel_map<ocv::Tvec> SuperPixelSegmentation::calc_mean_in_sp(const cv::Mat& _labels, const ocv::Timg& _image, const Tlabel _label_filter) {

	Tlabel_map<ocv::Tvec> mean_in_sp;

	if (_labels.size() == _image.size()) {

		namespace bacc = boost::accumulators;

		typedef Tlabel_map< bacc::accumulator_set<ocv::Tvalue, bacc::stats<bacc::tag::mean > > > Tmap_accumulators;

		Tmap_accumulators accumulators_b;
		Tmap_accumulators accumulators_g;
		Tmap_accumulators accumulators_r;

		Tlabel label_value;
		cv::MatConstIterator_<ocv::Tvec> it_image = _image.begin();
		for (cv::MatConstIterator_<cv::Vec1i> it_labels = _labels.begin<cv::Vec1i>(); it_labels != _labels.end<cv::Vec1i>(); ++it_labels, ++it_image) {

			label_value = (*it_labels)[0];
			if (_label_filter == no_label || label_value == _label_filter) {
				accumulators_b[label_value]((*it_image)[0]);
				accumulators_g[label_value]((*it_image)[1]);
				accumulators_r[label_value]((*it_image)[2]);
			}

		}

		for (Tmap_accumulators::const_iterator it = accumulators_b.begin(); it != accumulators_b.end(); ++it) {
			mean_in_sp[it->first][0] = bacc::mean(accumulators_b[it->first]);
			mean_in_sp[it->first][1] = bacc::mean(accumulators_g[it->first]);
			mean_in_sp[it->first][2] = bacc::mean(accumulators_r[it->first]);
		}

	} else {
		std::cout << "calc_mean_in_sp : wrong size" << std::endl;
	}

	return mean_in_sp;

}

SuperPixelSegmentation::Tlabel_map<ocv::Tvec> SuperPixelSegmentation::calc_std_in_sp(const cv::Mat& _labels, bool _l_recolored, const Tlabel _label_filter) const {

	ocv::Timg image_use;
	if (_l_recolored) {
		image_use = image_recolored;
	} else {
		image_use = image_original;
	}

	return calc_std_in_sp(_labels, image_use, _label_filter);
}

SuperPixelSegmentation::Tlabel_map<ocv::Tvec> SuperPixelSegmentation::calc_std_in_sp(const cv::Mat& _labels, const ocv::Timg& _image, const Tlabel _label_filter) {

	Tlabel_map<ocv::Tvec> std_in_sp;

	if (_labels.size() == _image.size()) {

		namespace bacc = boost::accumulators;

		typedef Tlabel_map< bacc::accumulator_set<ocv::Tvalue, bacc::stats<bacc::tag::variance > > > Tmap_accumulators;

		Tmap_accumulators accumulators_b;
		Tmap_accumulators accumulators_g;
		Tmap_accumulators accumulators_r;

		Tlabel label_value;
		cv::MatConstIterator_<ocv::Tvec> it_image = _image.begin();
		for (cv::MatConstIterator_<cv::Vec1i> it_labels = _labels.begin<cv::Vec1i>(); it_labels != _labels.end<cv::Vec1i>(); ++it_labels, ++it_image) {

			label_value = (*it_labels)[0];
			if (_label_filter == no_label || label_value == _label_filter) {
				accumulators_b[label_value]((*it_image)[0]);
				accumulators_g[label_value]((*it_image)[1]);
				accumulators_r[label_value]((*it_image)[2]);
			}

		}

		for (Tmap_accumulators::const_iterator it = accumulators_b.begin(); it != accumulators_b.end(); ++it) {
			std_in_sp[it->first][0] = std::sqrt(bacc::variance(accumulators_b[it->first]));
			std_in_sp[it->first][1] = std::sqrt(bacc::variance(accumulators_g[it->first]));
			std_in_sp[it->first][2] = std::sqrt(bacc::variance(accumulators_r[it->first]));
		}

	} else {
		std::cout << "calc_std_in_sp : wrong size" << std::endl;
	}

	return std_in_sp;

}


void SuperPixelSegmentation::show_labels(const cv::Mat _labels) const {

	/*! If Nsuperpixels_slic is 0, means no computation has been performed yet. Thus scaling is not possible.*/
	if (Nsuperpixels_slic) {

		cv::Mat labels_display;
		if (_labels.size() == cv::Size()) {
			labels.copyTo(labels_display);
		} else {
			_labels.copyTo(labels_display);
		}
		labels_display *= std::pow(255, 2) / Nsuperpixels_slic;
		ocv::imshow(labels_display, "Labels");
	}
	
}

void SuperPixelSegmentation::show_segmentation(const ocv::Tmask _contour) const {

	ocv::Tmask contour_use;
	if (_contour.size() == cv::Size()) {
		contour_use = contour;
	} else {
		contour_use = _contour;
	}
	/*! Call static member function.*/
	Contour::show_segmentation(image_original, contour_use);

}

void SuperPixelSegmentation::show_median_pixels() const {

	ocv::Timg image_median(image_original.size());

	Tlabel label_value;
	cv::MatIterator_<ocv::Tvec> it_image_median = image_median.begin();
	for (cv::MatConstIterator_<cv::Vec1i> it_labels = labels.begin<cv::Vec1i>(); it_labels != labels.end<cv::Vec1i>(); ++it_labels, ++it_image_median) {

		label_value = (*it_labels)[0];

		*it_image_median = median_in_sp.at(label_value);

	}

	ocv::imshow(image_median);

}

void SuperPixelSegmentation::show_mean_pixels() const {

	ocv::Timg image_mean(image_original.size());

	Tlabel label_value;
	cv::MatIterator_<ocv::Tvec> it_image_mean = image_mean.begin();
	for (cv::MatConstIterator_<cv::Vec1i> it_labels = labels.begin<cv::Vec1i>(); it_labels != labels.end<cv::Vec1i>(); ++it_labels, ++it_image_mean) {

		label_value = (*it_labels)[0];

		*it_image_mean = mean_in_sp.at(label_value);

	}

	ocv::imshow(image_mean);

}

void SuperPixelSegmentation::show_std_pixels() const {

	ocv::Timg image_std(image_original.size());

	Tlabel label_value;
	cv::MatIterator_<ocv::Tvec> it_image_std = image_std.begin();
	for (cv::MatConstIterator_<cv::Vec1i> it_labels = labels.begin<cv::Vec1i>(); it_labels != labels.end<cv::Vec1i>(); ++it_labels, ++it_image_std) {

		label_value = (*it_labels)[0];

		*it_image_std = std_in_sp.at(label_value);

	}

	ocv::rescale_max(image_std);//Warning : better for display, not for measurement
	ocv::imshow(image_std);


}

void SuperPixelSegmentation::get_label(const Tlabel _label, cv::Mat& _image_output) const {

	/*! Calls static member function.*/
	get_label(labels, _label, _image_output);

}

void SuperPixelSegmentation::get_label(const cv::Mat& _labels, const Tlabel _label, cv::Mat& _image_output) {

	_image_output.create(_labels.size(), CV_8UC1);
	_image_output = 0;

	Tlabel label_value;
	cv::MatIterator_<uchar> it_label = _image_output.begin<uchar>();
	for (cv::MatConstIterator_<cv::Vec1i> it_labels = _labels.begin<cv::Vec1i>(); it_labels != _labels.end<cv::Vec1i>(); ++it_labels, ++it_label) {

		label_value = (*it_labels)[0];

		if (label_value == _label) {
			(*it_label) = 1;
		}

	}

}

SuperPixelSegmentation::Tlabels_datas SuperPixelSegmentation::get_labels_datas(const cv::Mat _labels, const SuperPixelSegmentation::Tlabel _no_label, bool _l_recolored) const {

	cv::Mat labels_use;
	if (_labels.size() == cv::Size()) {
		labels_use = labels;
	} else {
		labels_use = _labels;
	}

	ocv::Timg image_use;
	if (parameters.conversion_type >= 0 && !_l_recolored) {
		image_use = image_original;
	} else {
		/*! If no conversion, image_recolored equals image_original.*/
		image_use = image_recolored;
	}

	return get_labels_datas(labels_use, image_use, _no_label);
}

SuperPixelSegmentation::Tlabels_datas SuperPixelSegmentation::get_labels_datas(const cv::Mat& _labels, const ocv::Timg& _image, const Tlabel _no_label) {

	Tlabels_datas labels_datas;

	if (_labels.size() == _image.size()) {

		std::pair<const ocv::Tvec*, cv::Point> element;
		cv::MatConstIterator_<ocv::Tvec> it_image = _image.begin();
		cv::Point coordinates(0, 0);
		cv::Size size = _image.size();
		SuperPixelSegmentation::Tlabel label_value;
		for (cv::MatConstIterator_<cv::Vec1i> it_labels = _labels.begin<cv::Vec1i>(); it_labels != _labels.end<cv::Vec1i>(); ++it_labels, ++it_image) {

			label_value = (*it_labels)[0];

			if (label_value != _no_label) {
				element.first = &(*it_image);
				element.second = coordinates;
				labels_datas[label_value].push_back(element);
			}

			coordinates.x++;
			if (coordinates.x == size.width) {
				coordinates.y++;
				coordinates.x = 0;
			}
		}

	} else {
		std::cout << "get_labels_datas : wrong size" << std::endl;
	}

	return labels_datas;

}
