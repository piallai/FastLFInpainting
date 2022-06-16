/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once 

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <utility>//pair
#include "misc_funcs.h"
#include "opencv_operations.h"

/*! Add missing typedefs in opencv.*/
namespace cv {

	typedef Vec<uchar, 1> Vec1b;
	typedef Vec<short, 1> Vec1s;
	typedef Vec<ushort, 1> Vec1w;
	typedef Vec<int, 1> Vec1i;
	typedef Vec<float, 1> Vec1f;
	typedef Vec<double, 1> Vec1d;

}

/*! Namespace containing convenient functions to use OpenCV.*/
namespace ocv {

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//// Type definitions /////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Precision type for OpenCV images to deal with for common floating point operations.*/
	typedef float Tvalue;
	/*! Type at at pixel's image.*/
	typedef cv::Vec<Tvalue, 3> Tvec;//Vec3f
	/*! Type of images.*/
	typedef cv::Mat_<Tvec> Timg;
	typedef cv::Vec<Tvalue, 1> Tvec1;
	typedef cv::Mat_< cv::Vec<Tvalue, 1> > Timg1;
	typedef cv::Mat_< cv::Vec<Tvalue, 2> > Timg2;
	typedef cv::Mat_< cv::Vec<Tvalue, 3> > Timg3;
	typedef cv::Mat_< cv::Vec<Tvalue, 4> > Timg4;

	typedef cv::Vec<int, 1> Tvec1i;//CV_32SC1

	template <class Timg>
	using Vec = std::pair<Timg, Timg>;

	typedef Vec<Timg1> VecImg;

	typedef int64 Timg_ptr_type;
	typedef cv::Mat_<cv::Vec<Timg_ptr_type, 1> > Timg_ptr;

	typedef cv::Mat_< cv::Vec<uchar, 1> > Tmask;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	enum Orientation { horizontal, vertical };

	/*! Converts Blue Green Red index to Red Green Blue index. Convenient for passing RGB coordinates to OpenCV BGR standard.*/
	unsigned int RGBtoBGR_index(unsigned int chan);
	/*! Converts a RGB std::vector into a cv::Scalar for OpenCV.*/
	cv::Scalar to_color_cvScalar(const std::vector<unsigned int>& _rgb_vector);

	/*! Default value.*/
	EXTERN_CELF_API double textFontScale;
	/*! Default value.*/
	EXTERN_CELF_API cv::Scalar textColor;
	/*! Default value.*/
	EXTERN_CELF_API int textFont;
	/*! Static value defining whether algorithms run in a low memory mode or not.*/
	EXTERN_CELF_API  bool l_low_memory;

	/*! Inserts a text into \p _image.*/
	void putText(cv::Mat& _image, const std::string& _text, const double _font_scale = textFontScale, cv::Scalar _color = textColor, int _thickness = 2);
	void putText(cv::Mat& _image, const std::string& _text, const cv::Point& _point, const double _font_scale = textFontScale, cv::Scalar _color = textColor, int _thickness = 2);

	/*! Function displaying pixel information. Used in #imshow.*/
	template <class Tvec>
	void onMouse(int event, int x, int y, int flags, void* param);
	template <class Tvec>
	struct onMouseDim_struct;
	template <class Tvalue>
	struct onMouseVal_struct;

	EXTERN_CELF_API unsigned int imshow_wait_time;
	EXTERN_CELF_API unsigned int imshow_wait_time_animation;

	/*! Shows an image with label \p _image_name. Same as cv::imshow but with position/color feedback.*/
	void imshow(const cv::Mat& _image, std::string _image_name = "image", unsigned int _wait_time_ms = imshow_wait_time, bool _l_keep_window=false);
	/*! Shows an image with label \p _image_name. \p _wait_time_ms stands for the delay before continuing program execution.*/
	template <class T>
	bool imshow(const cv::Mat_<T>& _image, std::string _image_name = "image", unsigned int _wait_time_ms = imshow_wait_time, bool _l_keep_window = false, const cv::WindowFlags _window_flag= cv::WINDOW_AUTOSIZE);
	/*! Shows a sequence of images.*/
	template <class T>
	void imshow(const std::vector< cv::Mat_<T> >& _images, std::string _image_name = "images", unsigned int _wait_time_ms = imshow_wait_time, const cv::WindowFlags _window_flag = cv::WINDOW_AUTOSIZE);
	/*! Shows two images in a Vec<Timg> side by side.*/
	template <class Timg>
	bool imshow(const ocv::Vec<Timg>& _vec, std::string _image_name = "image", unsigned int _wait_time_ms = imshow_wait_time, bool _l_keep_window = false, const cv::WindowFlags _window_flag = cv::WINDOW_AUTOSIZE);

	template <class T>
	void imshow_colormap(const cv::Mat_<T>& _image, std::string _image_name = "image", cv::ColormapTypes _colormap_type=cv::COLORMAP_JET, unsigned int _wait_time_ms = imshow_wait_time);

	EXTERN_CELF_API const Tvalue epsilon;
	EXTERN_CELF_API uchar mask_value;
	/*! Factor for resizing read images. Usefull to speed up computation.*/
	EXTERN_CELF_API cv::Size_<float> size_coef;

	template <class Tvalue>
	using Range = std::pair<Tvalue, Tvalue>;

	/*! Filter values which are neither 0 neither ocv::mask_value.*/
	void mask_filter(ocv::Tmask& _mask);
	/*! Inverse values of mask.*/
	void mask_inverse(const ocv::Tmask& _mask_input, ocv::Tmask& _mask_output, bool _l_strong=false);
	/*! Merge two masks. Kept values are those being : _mask_value AND _mask_value.*/
	void mask_merge_and(const ocv::Tmask& _mask1, const ocv::Tmask& _mask2, ocv::Tmask& _mask_output, const uchar _mask_value=ocv::mask_value);

	/*! Get 1D position of a Mat.*/
	int get_position(const cv::Point& _point, const cv::Size& _size);

	/*! Checks if two images have same size. Can display a warning message if not.*/
	template <class Timg1, class Timg2>
	bool check_images_size(const Timg1& _image1, const Timg2& _image2, bool _l_message = true);

	//misc
	/*! Converts a cv::Scalar into a cv::Vec.*/
	template <class Tval>
	cv::Vec<Tval, 1>& assign(cv::Vec<Tval, 1>& _vec, const cv::Scalar& _scalar);
	/*! Converts a cv::Scalar into a cv::Vec.*/
	template <class Tval>
	cv::Vec<Tval, 2>& assign(cv::Vec<Tval, 2>& _vec, const cv::Scalar& _scalar);
	/*! Converts a cv::Scalar into a cv::Vec.*/
	template <class Tval>
	cv::Vec<Tval, 3>& assign(cv::Vec<Tval, 3>& _vec, const cv::Scalar& _scalar);
	/*! Converts a cv::Scalar into a cv::Vec.*/
	template <class Tval>
	cv::Vec<Tval, 4>& assign(cv::Vec<Tval, 4>& _vec, const cv::Scalar& _scalar);
	/*! Converts a cv::Scalar into a cv::Vec which is a value.*/
	template <class Tvec>
	Tvec& assign(Tvec& _vec, const cv::Scalar& _scalar);

	template <class Tval, class Top>
	cv::Vec<Tval, 1>& mul(cv::Vec<Tval, 1>& _vec, const Top& _value);
	template <class Tval, class Top>
	cv::Vec<Tval, 3>& mul(cv::Vec<Tval, 3>& _vec, const Top& _value);
	template <class Tval>
	cv::Vec<Tval, 1>& mul(cv::Vec<Tval, 1>& _vec, const cv::Vec<Tval, 1>& _vec_op);
	template <class Tval>
	cv::Vec<Tval, 3>& mul(cv::Vec<Tval, 3>& _vec, const cv::Vec<Tval, 3>& _vec_op);
	template <class Tval>
	cv::Vec<Tval, 3>& mul(cv::Vec<Tval, 3>& _vec, const cv::Vec<Tval, 1>& _vec_op);

	template <class Tval>
	cv::Vec<Tval, 1>& divide(cv::Vec<Tval, 1>& _vec1, const cv::Vec<Tval, 1>& _vec2);
	template <class Tval>
	cv::Vec<Tval, 3>& divide(cv::Vec<Tval, 3>& _vec1, const cv::Vec<Tval, 3>& _vec2);

	typedef std::pair<double, double> Tscaling;

	/*! Compute scaling needed to go from a value range to an other.*/
	template <class Tval1, class Tval2>
	Tscaling get_scaling(const ocv::Range<Tval1>& _range_input, const ocv::Range<Tval2>& _range_output);
	/*! Rescale an image.*/
	template <class Tval, int Dim>
	void rescale(cv::Mat_< cv::Vec<Tval, Dim> >& _image, const Tscaling& _scaling);
	/*! Rescale back an image.*/
	template <class Tval, int Dim>
	void rescale_inv(cv::Mat_< cv::Vec<Tval, Dim> >& _image, const Tscaling& _scaling);
	

	template <class Tvec = ocv::Tvec>
	class Video;
	template <class Tvec = ocv::Tvec>
	class VideoLoader;

	//computing
	/*Function used in shiftFrame.*/
	template <class T>
	T shift_maxmin(T _value1, T _value2);
	/*Function used in shiftFrame.*/
	template <class T>
	T shift_minmax(T _value1, T _value2);
	/*! Shift an image by a integer number of pixels. Cartesian convention for x and y.*/
	Timg& shiftFrame(const Timg& _image, int _pixels_x, int _pixels_y, Timg& _image_output);
	/*! Shift an image by a floating point number of pixels. Uses supersampling. Cartesian convention for x and y.*/
	Timg shiftFrame(Timg _frame, float pixels_x, float pixels_y, Timg& _frame_output, Timg _super_frame1, Timg _super_frame2, unsigned int _Nsuper_resolution = 2);

	/*! Shift an image by floating point number of pixels. Found at https://stackoverflow.com/questions/40551281/translate-image-by-very-small-step-by-opencv-in-c. Way faster and accurate than # shiftFrame.*/
	void shift(const cv::Mat& src, cv::Mat& dst, cv::Point2f delta, int fill = cv::BORDER_CONSTANT, cv::Scalar value = cv::Scalar(0, 0, 0, 0));

	/*! Extract an area from an image using float coordinates.*/
	template <class Timg>
	void rect(const Timg& _image, const cv::Point2f& _position, const cv::Size& _size, Timg& _result);

	/*! Rescales \p _image linearily so that its maximum value is maximum color range.*/
	template <class Tvalue, int Dim>
	double rescale_max(cv::Mat_< cv::Vec<Tvalue,Dim> >& _image);

	/*! Crops \p _image left, up, right, bottom). Do not use same image as input and output. */
	template <class Timg>
	bool crop(const Timg& _image, Timg& _image_result, const std::array<unsigned int, 4>& _Ncrop_pixels);

	/*! Resize imae with highed size. Reimplementation because opencv's resize with nearest is not accurate.*/
	template <class Timg>
	void upsize_nearest(const Timg& _image, Timg& _image_result, const cv::Size& _size);

	/*! Computes absolute difference of \p _image1 and \p _image2. Divide it by \p _image1 and returns result in \p _RMS_output.*/
	void get_diff_normalized(const Timg& _image1, const Timg& _image2, Timg& _RMS_output);
	/*! Computes absolute difference of \p _image1 and \p _image2. Divide it by \p _image1 and returns result in \p _RMS_output.*/
	void get_diff_normalized(const Timg1& _image1, const Timg1& _image2, Timg1& _RMS_output);

	/*! Check if images are equal.*/
	bool is_equal(const cv::Mat& _image1, const cv::Mat& _image2);


	/*! Set minimum value in image.*/
	template <class Tvec>
	void bound_min(const cv::Mat_<Tvec>& _input, cv::Mat_<Tvec>& _output, const Tvec _value_min);
	/*! Set maximum value in image.*/
	template <class Tvec>
	void bound_max(const cv::Mat_<Tvec>& _input, cv::Mat_<Tvec>& _output, const Tvec _value_max);
	/*! Set minimum/maximum value in image.*/
	template <class Tvec>
	void bound(const cv::Mat_<Tvec>& _input, cv::Mat_<Tvec>& _output, const Tvec _value_min, const Tvec _value_max);

	/*! Reduce channels of image and write result in 1 channel image.*/
	cv::Mat reduce_channels(const cv::Mat& _input, const int _rtype = cv::REDUCE_SUM);

	/*! Compute norm of \p _vector and writes result in \p _norm.*/
	template <class Timg>
	void get_norm(const ocv::Vec<Timg>& _vector, Timg& _norm);
	template <class Timg>
	void get_norm(const ocv::Vec<Timg>& _vector, Timg& _norm, ocv::Vec<Timg>& _buffer);
	/*! Normalize \p _vector.*/
	template <class Timg>
	void normalize(ocv::Vec<Timg>& _vector);
	/*! Normalize \p _vector.*/
	template <class Timg>
	void normalize(ocv::Vec<Timg>& _vector, Timg& _buff);
	/*! Normalize \p _vector.*/
	template <class Timg>
	void normalize(ocv::Vec<Timg>& _vector, ocv::Vec<Timg>& _buff);

	/*! Get mean image of a set of images.*/
	template <class Timg>
	void get_mean(const std::vector<Timg>& _images, Timg& _result);
	/*! Get standard deviation image of a set of images.*/
	template <class Timg>
	void get_std(const std::vector<Timg>& _images, Timg& _result);
	
	/*! Get covariance of two images.*/
	template <class Tval, int Dim>
	cv::Vec<double, Dim> get_covariance(const cv::Mat_< cv::Vec<Tval, Dim> >& _image1, const cv::Mat_< cv::Vec<Tval, Dim> >& _image2);
	template <class Tval, int Dim>
	cv::Vec<double, Dim> get_covariance(const cv::Mat_< cv::Vec<Tval, Dim> >& _image1, const cv::Mat_< cv::Vec<Tval, Dim> >& _image2, const cv::Vec<Tval, Dim>& _mean1, const cv::Vec<Tval, Dim>& _mean2);

	/*! Get mean Vec of image.*/
	template <class Timg>
	void get_mean(const Timg& _image, typename Timg::value_type& _mean);
	/*! Get mean scalar value of image.*/
	template <class Timg>
	void get_mean(const Timg& _image, typename Timg::value_type::value_type& _mean);

	
	/*! Wether \p _image is valid.*/
	bool is_valid(const cv::Mat& _image);

	void quiver(const Timg& _image, const Timg2& _vectors, float _scale=1., cv::Point _offset=cv::Point(5,5));
	void quiver(const Timg& _image, const Timg1& _vectors_x, const Timg1& _vectors_y, float _scale=1., cv::Point _offset = cv::Point(5, 5));
	void quiver(const Timg& _image, const VecImg& _vectors, float _scale = 1., cv::Point _offset = cv::Point(5, 5));

	

	void imshow_norm(const VecImg& _vector, std::string _image_name = "image norm", unsigned int _wait_time_ms = imshow_wait_time, bool _l_keep_window = false);
	void imshow_quiver(const VecImg& _vector, std::string _image_name = "image quiver", float _scale = 1., cv::Point _offset = cv::Point(5, 5), unsigned int _wait_time_ms = imshow_wait_time, bool _l_keep_window = false);
	void imshow_quiver(const cv::Mat& _image, const VecImg& _vector, std::string _image_name = "image quiver", float _scale = 1., cv::Point _offset = cv::Point(5, 5), unsigned int _wait_time_ms = imshow_wait_time, bool _l_keep_window = false);
	void imshow_quiver(const cv::Mat& _image, const VecImg& _vector, const VecImg& _vector2, std::string _image_name = "image quiver", float _scale = 1., cv::Point _offset = cv::Point(5, 5), unsigned int _wait_time_ms = imshow_wait_time, bool _l_keep_window = false);

	/*! Whether _image has a NaN value. If _info is set, also displays location of NaN.*/
	bool has_nan(const cv::Mat& _image, const std::string _info="");

	template <class T>
	std::pair<T, T> pointToPair(const cv::Point_<T>& _point);
	template <class T>
	cv::Point_<T> pairToPoint(const std::pair<T, T>& _pair);

	/*! Check if _point is inside _image sizes.*/
	template <class Timg>
	bool check_point(const cv::Point& _point, const Timg& _image);

}


/*! Specialization of cv::DataType to manage images containing pointers. This is a copy of 'double' specialization.*/
namespace cv {
	template<> class DataType<ocv::Timg_ptr_type> {
	public:
		typedef double      value_type;
		typedef value_type  work_type;
		typedef value_type  channel_type;
		typedef value_type  vec_type;
		enum {
			generic_type = 0,
			depth = CV_64F,
			channels = 1,
			fmt = (int)'d',
			type = CV_MAKETYPE(depth, channels)
		};
	};
}


#include "ocv_onMouse_spec.h"

template <class Tvec>
void ocv::onMouse(int event, int x, int y, int flags, void* param) {

	if (event != cv::EVENT_LBUTTONDOWN)
		return;

	if (flags == cv::EVENT_FLAG_CTRLKEY) {
		/*! Just to use flags and remove warning.*/
	}

	cv::Point pt = cv::Point(x, y);
	cv::Mat_<Tvec>* mat = static_cast<cv::Mat_<Tvec>*>(param);
	std::cout << "Mouse event : mat->size() = " << mat->size() << std::endl;
	if (x < mat->cols && y < mat->rows) {
		Tvec value = mat->template at<Tvec>(y, x);

		std::cout << "x=" << pt.x << ", y=" << pt.y << ",   ";
		onMouseDim_struct<Tvec>::display(value);
		double min, max;
		cv::minMaxLoc(*mat, &min, &max);
		std::cout << " ### min=" << min << ", max=" << max;
		assign(value, cv::mean(*mat));
		std::cout << ", mean=" << value;
		std::cout << std::endl;

	}

}


template <class Timg1, class Timg2>
bool ocv::check_images_size(const Timg1& _image1, const Timg2& _image2, bool _l_message) {

	if (_image1.size == _image2.size) {
		return true;
	} else {
		if (_l_message) {
			std::cout << "Different size for image1 and image2" << std::endl;
		}
		return false;
	}

}


template <class T>
bool ocv::imshow(const cv::Mat_<T>& _image, std::string _image_name, unsigned int _wait_time_ms, bool _l_keep_window, const cv::WindowFlags _window_flag) {

	if (is_valid(_image)) {
		if (!_l_keep_window) {
			cv::namedWindow(_image_name, cv::WINDOW_GUI_EXPANDED | _window_flag);//cv::WINDOW_GUI_NORMAL, cv::WINDOW_GUI_EXPANDED
																						 //cv::setWindowProperty(_image_name, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		} else {
			cv::namedWindow(_image_name, cv::WINDOW_GUI_EXPANDED);// | _window_flag);
		}
		cv::setMouseCallback(_image_name, ocv::onMouse<T>, (void*)&_image);
		cv::imshow(_image_name, _image);
		cv::waitKey(_wait_time_ms);
		if (!_l_keep_window) {
			cv::destroyWindow(_image_name);
		}
		return true;
	} else {
		std::cout << "image : " << _image_name << ", is invalid for imshow" << std::endl;
		return false;
	}

}

template <class T>
void ocv::imshow(const std::vector< cv::Mat_<T> >& _images, std::string _image_name, unsigned int _wait_time_ms, const cv::WindowFlags _window_flag) {

	bool l_display=false;
	for (unsigned int i = 0; i < _images.size(); i++) {
		if (ocv::imshow(_images[i], _image_name, _wait_time_ms, true, _window_flag)) {
			l_display = true;
		}
	}
	if (l_display) {
		cv::destroyWindow(_image_name);
	}
}

template <class T>
void ocv::imshow_colormap(const cv::Mat_<T>& _image, std::string _image_name, cv::ColormapTypes _colormap_type, unsigned int _wait_time_ms) {

	cv::Mat_<cv::Vec3b> image_colored;
	get_colormap(_image, image_colored, _colormap_type);
	ocv::imshow(image_colored, _image_name);

}

template <class Timg>
bool ocv::imshow(const ocv::Vec<Timg>& _vec, std::string _image_name, unsigned int _wait_time_ms, bool _l_keep_window, const cv::WindowFlags _window_flag) {

	if (_vec.first.size() == _vec.second.size()) {

		cv::Size size = _vec.first.size();

		unsigned int offset = 2;

		Timg image(cv::Size(size.width * 2 + offset, size.height));
		_vec.first.copyTo(image(cv::Rect(0, 0, size.width, size.height)));
		_vec.second.copyTo(image(cv::Rect(size.width+offset, 0, size.width, size.height)));

		ocv::imshow(image, _image_name, _wait_time_ms, _l_keep_window, _window_flag);

	} else {
		std::cout << "ocv::imshow(const ocv::Vec<Timg>& _vec) : the two images must have same size" << std::endl;
		return false;
	}

}


template <class Tvalue, int Dim>
double ocv::rescale_max(cv::Mat_< cv::Vec<Tvalue, Dim> >& _image) {

	double minVal;
	double maxVal;
	cv::Point minLoc;
	cv::Point maxLoc;

	cv::minMaxIdx(_image, &minVal, &maxVal);

	if (maxVal != 0.) {
		_image /= maxVal;
	}

	return maxVal;

}


template <class Timg>
bool ocv::crop(const Timg& _image, Timg& _image_result, const std::array<unsigned int, 4>& _Ncrop_pixels) {

	if ((unsigned int)_image.size().width >= _Ncrop_pixels[0] + _Ncrop_pixels[2] && (unsigned int)_image.size().height >= _Ncrop_pixels[1] + _Ncrop_pixels[3]) {
		_image_result = _image(cv::Rect(_Ncrop_pixels[0], _Ncrop_pixels[1], _image.size().width - _Ncrop_pixels[0] - _Ncrop_pixels[2], _image.size().height - _Ncrop_pixels[1] - _Ncrop_pixels[3]));
		return true;
	} else {
		//std::cout << "SubaperturesData<Timg>::crop_spatially : Image cropping not applied. Too many pixels to crop. _Ncrop_pixels = " << std::vector<unsigned int>(_Ncrop_pixels.begin(), _Ncrop_pixels.end()) << ", image size = " << _image.size() << std::endl;
		return false;
	}

}

template <class Timg>
void ocv::upsize_nearest(const Timg& _image, Timg& _image_result, const cv::Size& _ratio) {

	if (is_valid(_image)) {

		cv::Size upsize;
		upsize.width = (_image.size().width - 1)*_ratio.width + 1;
		upsize.height = (_image.size().height - 1)*_ratio.height + 1;

		_image_result.create(upsize);

		cv::Size2f half_ratio;
		half_ratio.width = float(_ratio.width) / (float)2.;
		half_ratio.height = float(_ratio.height) / (float)2.;

		int count = 0;
		int ptr_delta;
		cv::Size modulo;
		cv::Point coordinates_res(0, 0);
		cv::Point coordinates_inp(0, 0);
		typename Timg::const_iterator it_inp = _image.begin();
		for (typename Timg::iterator it_res = _image_result.begin(); it_res != _image_result.end(); ++it_res) {

			ptr_delta = 0;

			modulo.width = coordinates_res.x % _ratio.width;
			if (modulo.width < half_ratio.width) {
				//backward
			} else {
				//forward
				ptr_delta++;
			}

			modulo.height = coordinates_res.y % _ratio.height;
			if (modulo.height < half_ratio.height) {
				//backward
			} else {
				//forward
				ptr_delta += _image.size().width;
			}

			it_inp += ptr_delta;
			*it_res = *it_inp;
			it_inp -= ptr_delta;

			coordinates_res.x++;
			if (coordinates_res.x == upsize.width) {
				coordinates_res.y++;
				coordinates_res.x = 0;

				if (coordinates_res.y % _ratio.height == 0) {
					it_inp += _image.size().width;
				}
				it_inp -= _image.size().width - 1;
			}

			if (coordinates_res.x % _ratio.width == 0 && coordinates_res.x != 0) {
				++it_inp;
				count++;
			}

		}

	} else {
		std::cout << "ocv::upsize_nearest : invalid image" << std::endl;
	}

}

template <class T>
T ocv::shift_maxmin(T _value1, T _value2) {

	return std::max(std::min(_value1, _value2), -_value2);
}

template <class T>
T ocv::shift_minmax(T _value1, T _value2) {

	return std::max(std::min(_value1, -_value2), _value2);
}

template <class Tval>
cv::Vec<Tval, 1>& ocv::assign(cv::Vec<Tval, 1>& _vec, const cv::Scalar& _scalar) {
	_vec[0] = (Tval)_scalar[0];
	return _vec;
}

template <class Tval>
cv::Vec<Tval, 2>& ocv::assign(cv::Vec<Tval, 2>& _vec, const cv::Scalar& _scalar) {
	_vec[0] = (Tval)_scalar[0];
	_vec[1] = (Tval)_scalar[1];
	return _vec;
}

template <class Tval>
cv::Vec<Tval, 3>& ocv::assign(cv::Vec<Tval, 3>& _vec, const cv::Scalar& _scalar) {
	_vec[0] = (Tval)_scalar[0];
	_vec[1] = (Tval)_scalar[1];
	_vec[2] = (Tval)_scalar[2];
	return _vec;
}

template <class Tval>
cv::Vec<Tval, 4>& ocv::assign(cv::Vec<Tval, 4>& _vec, const cv::Scalar& _scalar) {
	_vec[0] = (Tval)_scalar[0];
	_vec[1] = (Tval)_scalar[1];
	_vec[2] = (Tval)_scalar[2];
	_vec[3] = (Tval)_scalar[3];
	return _vec;
}

template <class Tvec>
Tvec& ocv::assign(Tvec& _vec, const cv::Scalar& _scalar) {
	_vec = (Tvec)_scalar[0];
	return _vec;
}

template <class Tval, class Top>
cv::Vec<Tval, 1>& ocv::mul(cv::Vec<Tval, 1>& _vec, const Top& _value) {

	_vec[0] *= _value;
	return _vec;
}

template <class Tval, class Top>
cv::Vec<Tval, 3>& ocv::mul(cv::Vec<Tval, 3>& _vec, const Top& _value) {

	_vec[0] *= _value;
	_vec[1] *= _value;
	_vec[2] *= _value;
	return _vec;
}

template <class Tval>
cv::Vec<Tval, 1>& ocv::mul(cv::Vec<Tval, 1>& _vec, const cv::Vec<Tval, 1>& _vec_op) {

	_vec[0] *= _vec_op[0];
	return _vec;
}

template <class Tval>
cv::Vec<Tval, 3>& ocv::mul(cv::Vec<Tval, 3>& _vec, const cv::Vec<Tval, 3>& _vec_op) {

	_vec[0] *= _vec_op[0];
	_vec[1] *= _vec_op[1];
	_vec[2] *= _vec_op[2];
	return _vec;
}

template <class Tval>
cv::Vec<Tval, 3>& ocv::mul(cv::Vec<Tval, 3>& _vec, const cv::Vec<Tval, 1>& _vec_op) {

	ocv::mul(_vec, _vec_op[0]);
	return _vec;
}

template <class Tval>
cv::Vec<Tval, 1>& ocv::divide(cv::Vec<Tval, 1>& _vec1, const cv::Vec<Tval, 1>& _vec2) {

	_vec1[0] /= _vec2[0];
	return _vec1;
}

template <class Tval>
cv::Vec<Tval, 3>& ocv::divide(cv::Vec<Tval, 3>& _vec1, const cv::Vec<Tval, 3>& _vec2) {

	_vec1[0] /= _vec2[0];
	_vec1[1] /= _vec2[1];
	_vec1[2] /= _vec2[2];
	return _vec1;
}

template <class Tval1, class Tval2>
ocv::Tscaling ocv::get_scaling(const ocv::Range<Tval1>& _range_input, const ocv::Range<Tval2>& _range_output) {

	Tscaling scaling;

	/*! Differences are converted to double because range can be out of range. Ex: char : -128 to 127, range=255 : can't be stored in a char*/
	double diff_input = (double)_range_input.second - (double)_range_input.first;
	double diff_output = (double)_range_output.second - (double)_range_output.first;

	double scale;
	if (diff_output != 0) {
		scale = (double)diff_output / (double)diff_input;
	} else {
		scale = 1.;
	}

	scaling.first = scale;
	scaling.second = -scale * (double)_range_input.first + (double)_range_output.first;

	return scaling;

}

template <class Tval, int Dim>
void ocv::rescale(cv::Mat_< cv::Vec<Tval, Dim> >& _image, const Tscaling& _scaling) {

	_image *= _scaling.first;
	_image += cv::Vec<Tval, Dim>::all(Tval(_scaling.second));
}

template <class Tval, int Dim>
void ocv::rescale_inv(cv::Mat_< cv::Vec<Tval, Dim> >& _image, const Tscaling& _scaling) {

	_image -= cv::Vec<Tval, Dim>::all(Tval(_scaling.second));
	_image /= _scaling.first;
}


template <class Timg>
void ocv::get_norm(const ocv::Vec<Timg>& _vector, Timg& _norm) {

	VecImg vector_buffer;
	get_norm(_vector, _norm, vector_buffer);

}

template <class Timg>
void ocv::get_norm(const ocv::Vec<Timg>& _vector, Timg& _norm, ocv::Vec<Timg>& _buffer) {

	cv::multiply(_vector.first, _vector.first, _buffer.first);
	cv::multiply(_vector.second, _vector.second, _buffer.second);
	cv::add(_buffer.first, _buffer.second, _norm);
	cv::sqrt(_norm, _norm);

}

template <class Timg>
void ocv::normalize(ocv::Vec<Timg>& _vector) {

	Timg buff;
	normalize(_vector, buff);
}

template <class Timg>
void ocv::normalize(ocv::Vec<Timg>& _vector, Timg& _buff) {

	get_norm(_vector, _buff);
	_buff += Timg::value_type::all(ocv::epsilon);
	cv::divide(_vector.first, _buff, _vector.first);
	cv::divide(_vector.second, _buff, _vector.second);
}

template <class Timg>
void ocv::normalize(ocv::Vec<Timg>& _vector, ocv::Vec<Timg>& _buff) {

	/*! _buff.first is used as norm output.*/
	get_norm(_vector, _buff.first, _buff);
	_buff.first += Timg::value_type::all(ocv::epsilon);
	cv::divide(_vector.first, _buff.first, _vector.first);
	cv::divide(_vector.second, _buff.first, _vector.second);
}

template <class Timg>
void ocv::get_mean(const std::vector<Timg>& _images, Timg& _result) {

	if (_images.size() > 0) {
		_images[0].copyTo(_result);
		for (unsigned int i = 1; i < _images.size(); i++) {
			cv::add(_result, _images[i], _result);
		}
		_result /= _images.size();
	}

}

template <class Timg>
void ocv::get_std(const std::vector<Timg>& _images, Timg& _result) {

	Timg mean;
	get_mean(_images, mean);

	_result.create(mean.size());
	_result = 0.;

	Timg buff;
	for (unsigned int i = 0; i < _images.size(); i++) {
		cv::subtract(_images[i], mean, buff);
		cv::multiply(buff, buff, buff);
		cv::add(_result, buff, _result);
	}

	cv::sqrt(_result, _result);
}

template <class Tval, int Dim>
cv::Vec<double, Dim> ocv::get_covariance(const cv::Mat_< cv::Vec<Tval, Dim> >& _image1, const cv::Mat_< cv::Vec<Tval, Dim> >& _image2) {

	cv::Vec<Tval, Dim> mean1;
	cv::Scalar buff_mean;
	buff_mean = cv::mean(_image1);
	ocv::assign(mean1, buff_mean);

	cv::Vec<Tval, Dim> mean2;
	buff_mean = cv::mean(_image2);
	ocv::assign(mean2, buff_mean);

	return get_covariance(_image1, _image2, mean1, mean1);
}

template <class Tval, int Dim>
cv::Vec<double, Dim> ocv::get_covariance(const cv::Mat_< cv::Vec<Tval, Dim> >& _image1, const cv::Mat_< cv::Vec<Tval, Dim> >& _image2, const cv::Vec<Tval, Dim>& _mean1, const cv::Vec<Tval, Dim>& _mean2) {

	cv::Vec<double, Dim> covariance = cv::Vec<double, Dim>::all(0);

	if (check_images_size(_image1, _image2)) {


		typename cv::Mat_< cv::Vec<Tval, Dim> >::const_iterator it1 = _image1.begin();
		typename cv::Mat_< cv::Vec<Tval, Dim> >::const_iterator it2 = _image2.begin();

		for (; it1 != _image1.end(); ++it1, ++it2) {

			covariance += ((*it1) - _mean1)*((*it2) - _mean2);

		}

		covariance /= (int)_image1.total();
	}

	return covariance;

}

template <class Timg>
void ocv::get_mean(const Timg& _image, typename Timg::value_type& _mean) {

	cv::Scalar mean = cv::mean(_image);
	ocv::assign(_mean, mean);

}

template <class Timg>
void ocv::get_mean(const Timg& _image, typename Timg::value_type::value_type& _mean) {

	typename Timg::value_type mean;
	get_mean(_image, mean);
	_mean = 0;
	for (unsigned int d = 0; d < Timg::value_type::channels; d++) {
		_mean += mean[d];
	}
	_mean /= Timg::value_type::channels;
}

template <class Tvec>
void ocv::bound_min(const cv::Mat_<Tvec>& _input, cv::Mat_<Tvec>& _output, const Tvec _value_min) {

	bound(_input, _output, _value_min, Tvec::all(std::numeric_limits<ocv::Tvalue>::min()));
}

template <class Tvec>
void ocv::bound_max(const cv::Mat_<Tvec>& _input, cv::Mat_<Tvec>& _output, const Tvec _value_max) {

	bound(_input, _output, Tvec::all(-std::numeric_limits<ocv::Tvalue>::max()), _value_max);
}

template <class Tvec>
void ocv::bound(const cv::Mat_<Tvec>& _input, cv::Mat_<Tvec>& _output, const Tvec _value_min, const Tvec _value_max) {

	_output.create(_input.size());
	typename cv::Mat_<Tvec>::iterator it_out = _output.begin();
	unsigned int dim;
	for (typename cv::Mat_<Tvec>::const_iterator it_in = _input.begin(); it_in != _input.end(); ++it_in, ++it_out) {

		for (dim = 0; dim < Tvec::channels; dim++) {
			if ((*it_in)[dim] < _value_min[dim]) {
				(*it_out)[dim] = _value_min[dim];
			} else {
				(*it_out)[dim] = (*it_in)[dim];
			}

			if ((*it_in)[dim] > _value_max[dim]) {
				(*it_out)[dim] = _value_max[dim];
			} else {
				(*it_out)[dim] = (*it_in)[dim];
			}
		}

	}

}

template <class T>
std::pair<T, T> ocv::pointToPair(const cv::Point_<T>& _point) {

	std::pair<T, T> pair(_point.x, _point.y);
	return pair;
}

template <class T>
cv::Point_<T> ocv::pairToPoint(const std::pair<T, T>& _pair) {

	cv::Point_<T> point(_pair.first, _pair.second);
	return point;
}

template <class Timg>
bool ocv::check_point(const cv::Point& _point, const Timg& _image) {

	return (_point.x < _image.size().width && _point.y < _image.size().height);

}

template <class Timg>
void ocv::rect(const Timg& _image, const cv::Point2f& _position, const cv::Size& _size, Timg& _result) {

	cv::Point2d position_integer;
	cv::Point2f position_frac;
	position_frac.x = (float)std::modf(_position.x, &position_integer.x);
	position_frac.y = (float)std::modf(_position.y, &position_integer.y);

	cv::Size increment;
	increment.width = (int)(position_frac.x != 0.);
	increment.height = (int)(position_frac.y != 0.);

	cv::Rect rect((int)_position.x, (int)_position.y, _size.width + increment.width, _size.height + increment.height);

	Timg subimage = _image(rect);

	cv::Point2f shift(-position_frac.x, -position_frac.y);

	ocv::shift(subimage, _result, shift, cv::BORDER_REPLICATE);

	rect.x = 0;
	rect.y = 0;
	rect.width = _size.width;
	rect.height = _size.height;

	_result = _result(rect);

}
