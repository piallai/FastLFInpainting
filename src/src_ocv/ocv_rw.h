/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "ocv_minmax.h"/* For Range.*/

namespace ocv {

	EXTERN_CELF_API std::string write_extension;

	/*! Write images. _write_name must not contain image extension. Extension has to be separated in _ext.*/
	template <class Tval, int Dim>
	void imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tval, Dim> >& _image, const std::string _ext = write_extension);
	/*! Write images, with min max input*/
	template <class Tval, int Dim>
	void imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tval, Dim> >& _image, const ocv::Range<Tval>& _range_input, const std::string _ext = write_extension);
	/*! Write images. Uses buffer.*/
	template <class Tval, int Dim>
	void imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tval, Dim> >& _image, cv::Mat_< cv::Vec<uchar, Dim> >& _write_image_buffer, const std::string _ext = write_extension);
	/*! Write images. Uses buffer.*/
	template <class Tval, int Dim>
	void imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tval, Dim> >& _image, const ocv::Range<Tval>& _range_input, cv::Mat_< cv::Vec<uchar, Dim> >& _write_image_buffer, const std::string _ext = write_extension);

	/*! Read images.*/
	template <class Tval, int Dim>
	void imread(const std::string& _write_name, cv::Mat_< cv::Vec<Tval, Dim> >& _image);
	/*! Read images. Uses buffer.*/
	template <class Tval, int Dim>
	void imread(const std::string& _read_name, cv::Mat_< cv::Vec<Tval, Dim> >& _image, cv::Mat_< cv::Vec<uchar, 3> >& _read_image_buffer);

}

#include "ocv_convert.h"
#ifdef ENABLE_LIB_OPENEXR
#include "OpenEXR_API.h"
#endif

template <class Tval, int Dim>
void ocv::imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tval, Dim> >& _image, const std::string _ext) {

	cv::Mat_< cv::Vec<uchar, Dim> > write_image_buffer;
	ocv::Range<Tval> range_input((Tval)0., (Tval)0.);
	imwrite(_write_name, _image, range_input, write_image_buffer, _ext);
}

template <class Tval, int Dim>
void ocv::imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tval, Dim> >& _image, const ocv::Range<Tval>& _range_input, const std::string _ext) {

	cv::Mat_< cv::Vec<uchar, Dim> > write_image_buffer;
	imwrite(_write_name, _image, _range_input, write_image_buffer, _ext);
}

template <class Tval, int Dim>
void ocv::imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tval, Dim> >& _image, cv::Mat_< cv::Vec<uchar, Dim> >& _write_image_buffer, const std::string _ext) {

	ocv::Range<Tval> range_input((Tval)0, (Tval)0);
	imwrite(_write_name, _image, range_input, _write_image_buffer, _ext);
}

template <class Tval, int Dim>
void ocv::imwrite(const std::string& _write_name, const cv::Mat_< cv::Vec<Tval, Dim> >& _image, const ocv::Range<Tval>& _range_input, cv::Mat_< cv::Vec<uchar, Dim> >& _write_image_buffer, const std::string _ext) {

#ifdef ENABLE_LIB_OPENEXR
		if (_ext == ".exr") {
			OpenEXR::exrwrite(_image, _write_name + _ext);
		} else
#endif
		{
			//ocv::imwrite(full_name, image_segmented, _range_input, _file_extension);
			if (_range_input == ocv::Range<Tval>((Tval)0, (Tval)0)) {
				ocv::convertTo(_image, _write_image_buffer, true);
			} else {
				ocv::convertTo(_image, _write_image_buffer, _range_input);
			}


			std::string path = Misc::to_data_path(_write_name + _ext);
			if (!cv::imwrite(path, _write_image_buffer)) {
				std::cout << "WARNING : Failed to write image (string size : " << path.size() << ") " << path << std::endl;
			}
		}

}


template <class Tval, int Dim>
void ocv::imread(const std::string& _read_name, cv::Mat_< cv::Vec<Tval, Dim> >& _image) {

	cv::Mat_< cv::Vec<uchar, 3> > read_image_buffer;
	imread(_read_name, _image, read_image_buffer);
}

template <class Tval, int Dim>
void ocv::imread(const std::string& _read_name, cv::Mat_< cv::Vec<Tval, Dim> >& _image, cv::Mat_< cv::Vec<uchar, 3> >& _read_image_buffer) {

	_read_image_buffer = cv::imread(_read_name, cv::IMREAD_COLOR);

	if (!_read_image_buffer.empty()) {
		/*! Manage rescaling.*/
		ocv::convertTo(_read_image_buffer, _image, true);

		if (size_coef.width != 0 && size_coef.height != 0) {
			cv::resize(_image, _image, cv::Size(0, 0), size_coef.width, size_coef.height);
		}

	} else {
		_image.release();
		std::cout << "WARNING : " << _read_name << " is empty" << std::endl;
	}

}