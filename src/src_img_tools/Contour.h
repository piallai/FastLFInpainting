/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "ocv_rw.h"

/*! Class managing segmentation.*/
namespace Contour {

	typedef ocv::Tmask Tcontour;

	/*! Using an image of \p _labels, determines a \p _contour using gradients.*/
	void get_segmentation(const cv::Mat& _labels, Tcontour& _contour);
	/*! Apply a \p _contour to an \p _image and write it in \p _image_segmented.*/
	template <class Tvalue, int Dim>
	void apply_segmentation(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const Tcontour& _contour, cv::Mat_< cv::Vec<Tvalue, Dim> >& _image_segmented, const cv::Vec<Tvalue, Dim> _segmentation_color = cv::Vec<Tvalue, Dim>::all(std::numeric_limits<Tvalue>::max()));
	/*! Apply a \p _contour to an \p _image and display the result.*/
	template <class Tvalue, int Dim>
	void show_segmentation(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const Tcontour& _contour);
	/*! Apply a contour to an \p _image, based on image of \p _labels and write it in \p _image_segmented.*/
	template <class Tvalue, int Dim>
	void apply_segmentation_from_labels(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const cv::Mat& _labels, cv::Mat_< cv::Vec<Tvalue, Dim> >& _image_segmented, const cv::Vec<Tvalue, Dim> _segmentation_color = cv::Vec<Tvalue, Dim>::all(std::numeric_limits<Tvalue>::max()));
	/*! Apply a contour to an \p _image, based on image of \p _labels and display the result.*/
	template <class Tvalue, int Dim>
	void show_segmentation_from_labels(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const cv::Mat& _labels);

	template <class Tvalue, int Dim>
	void imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const std::pair<Tvalue, Tvalue>& _range_input, const cv::Mat& _labels, const cv::Vec<Tvalue, Dim> _segmentation_color = cv::Vec<Tvalue, Dim>::all(std::numeric_limits<Tvalue>::max()));
	template <class Tvalue, int Dim>
	void imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const std::pair<Tvalue, Tvalue>& _range_input, const cv::Mat& _labels1, const cv::Vec<Tvalue, Dim> _segmentation_color1, const cv::Mat& _labels2, const cv::Vec<Tvalue, Dim> _segmentation_color2);

}

template <class Tvalue, int Dim>
void Contour::apply_segmentation(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const Tcontour& _contour, cv::Mat_< cv::Vec<Tvalue, Dim> >& _image_segmented, const cv::Vec<Tvalue, Dim> _segmentation_color) {

	_image.copyTo(_image_segmented);

	cv::Vec<Tvalue, Dim> value_contour;
	if (_segmentation_color == cv::Vec<Tvalue, Dim>::all(std::numeric_limits<Tvalue>::max())) {
		if (_image.depth() < CV_32F) {
			value_contour = cv::Vec<Tvalue, Dim>::all(std::numeric_limits<Tvalue>::max());
		} else {
			double min, max;
			cv::minMaxLoc(_image, &min, &max);
			value_contour = cv::Vec<Tvalue, Dim>::all((Tvalue)max);
		}
	} else {
		value_contour = _segmentation_color;
	}

	cv::MatConstIterator_<cv::Vec1b> it_contour = _contour.begin();
	for (cv::MatIterator_< cv::Vec<Tvalue, Dim> > it_image = _image_segmented.begin(); it_image != _image_segmented.end(); ++it_image, ++it_contour) {

		if ((*it_contour)[0] == ocv::mask_value) {
			(*it_image) = value_contour;
		}

	}

}

template <class Tvalue, int Dim>
void Contour::show_segmentation(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const Tcontour& _contour) {

	cv::Mat_< cv::Vec<Tvalue, Dim> > image_segmented;
	apply_segmentation(_image, _contour, image_segmented);
	ocv::imshow(image_segmented);

}

template <class Tvalue, int Dim>
void Contour::apply_segmentation_from_labels(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const cv::Mat& _labels, cv::Mat_< cv::Vec<Tvalue, Dim> >& _image_segmented, const cv::Vec<Tvalue, Dim> _segmentation_color) {

	Tcontour contour;
	/*! Call static member function.*/
	get_segmentation(_labels, contour);
	/*! Call static member function.*/
	apply_segmentation(_image, contour, _image_segmented, _segmentation_color);

}

template <class Tvalue, int Dim>
void Contour::show_segmentation_from_labels(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const cv::Mat& _labels) {

	Tcontour contour;
	/*! Call static member function.*/
	get_segmentation(_labels, contour);
	/*! Call static member function.*/
	show_segmentation(_image, contour);

}


template <class Tvalue, int Dim>
void Contour::imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const std::pair<Tvalue, Tvalue>& _range_input, const cv::Mat& _labels, const cv::Vec<Tvalue, Dim> _segmentation_color) {

	cv::Mat_< cv::Vec<Tvalue, Dim> > image_segmented;
	Contour::apply_segmentation_from_labels(_image, _labels, image_segmented, _segmentation_color);
	ocv::imwrite(_write_name, image_segmented, _range_input);
}


template <class Tvalue, int Dim>
void Contour::imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tvalue, Dim> >& _image, const std::pair<Tvalue, Tvalue>& _range_input, const cv::Mat& _labels1, const cv::Vec<Tvalue, Dim> _segmentation_color1, const cv::Mat& _labels2, const cv::Vec<Tvalue, Dim> _segmentation_color2) {

	cv::Mat_< cv::Vec<Tvalue, Dim> > image_segmented;
	Contour::apply_segmentation_from_labels(_image, _labels1, image_segmented, _segmentation_color1);
	Contour::apply_segmentation_from_labels(image_segmented, _labels2, image_segmented, _segmentation_color2);
	ocv::imwrite(_write_name, image_segmented, _range_input);
}

