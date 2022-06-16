/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "ocv.h"
#include <iostream>

uchar ocv::mask_value = std::numeric_limits<uchar>::max();

cv::Size_<float> ocv::size_coef = cv::Size_<float>(1, 1);

double ocv::textFontScale = 1.;
cv::Scalar ocv::textColor = cv::Scalar(200, 200, 250);
int ocv::textFont = cv::FONT_HERSHEY_SIMPLEX;

unsigned int ocv::imshow_wait_time = 0;
unsigned int ocv::imshow_wait_time_animation = 0;

bool ocv::l_low_memory = false;

unsigned int ocv::RGBtoBGR_index(unsigned int chan) {

	if (chan == 0) {
		return 2;
	} else if (chan == 1) {
		return 1;
	} else if (chan == 2) {
		return 0;
	} else {
		return -1;
	}
}

cv::Scalar ocv::to_color_cvScalar(const std::vector<unsigned int>& _rgb_vector) {

	cv::Scalar color;

	if (_rgb_vector.size() == 3) {
		color[0] = _rgb_vector[RGBtoBGR_index(0)];
		color[1] = _rgb_vector[RGBtoBGR_index(1)];
		color[2] = _rgb_vector[RGBtoBGR_index(2)];
	}

	return color;
}

void ocv::putText(cv::Mat& _image, const std::string& _text, const double _font_scale, cv::Scalar _color, int _thickness) {

	/*! Set default position of text bottom left.*/
	cv::Point point(20, _image.rows - 20);
	putText(_image, _text, point, _font_scale, _color, _thickness);
}

void ocv::putText(cv::Mat& _image, const std::string& _text, const cv::Point& _point, const double _font_scale, cv::Scalar _color, int _thickness) {

	if (_image.depth() == CV_16U) {
		_color *= 256;
	} else if (_image.depth() == CV_32F) {
		_color /= 256.;
	} else if (_image.depth() == CV_64F) {
		_color /= 256.;
	}

	cv::putText(_image, _text, _point, ocv::textFont, _font_scale, _color, _thickness, cv::LINE_AA, false);
}

bool ocv::is_valid(const cv::Mat& _image) {

	return _image.rows != 0 && _image.cols != 0;

}

void ocv::imshow(const cv::Mat& _image, std::string _image_name, unsigned int _wait_time_ms, bool _l_keep_window) {


	if (_image.type() == CV_8UC1) {
		imshow(cv::Mat_<cv::Vec1b>(_image), _image_name, _wait_time_ms, _l_keep_window);
	} else if (_image.type() == CV_16UC1) {
		imshow(cv::Mat_<cv::Vec1w>(_image), _image_name, _wait_time_ms, _l_keep_window);
	} else if (_image.type() == CV_32SC1) {
		imshow(cv::Mat_<cv::Vec1i>(_image), _image_name, _wait_time_ms, _l_keep_window);
	} else if (_image.type() == CV_32FC1) {
		imshow(cv::Mat_<cv::Vec1f>(_image), _image_name, _wait_time_ms, _l_keep_window);
	} else if (_image.type() == CV_64FC1) {
		imshow(cv::Mat_<cv::Vec1d>(_image), _image_name, _wait_time_ms, _l_keep_window);
	} else if (_image.type() == CV_8UC3) {
		imshow(cv::Mat_<cv::Vec3b>(_image), _image_name, _wait_time_ms, _l_keep_window);
	} else if (_image.type() == CV_16UC3) {
		imshow(cv::Mat_<cv::Vec3w>(_image), _image_name, _wait_time_ms, _l_keep_window);
	} else if (_image.type() == CV_32SC3) {
		imshow(cv::Mat_<cv::Vec3f>(_image), _image_name, _wait_time_ms, _l_keep_window);
	} else if (_image.type() == CV_32FC3) {
		imshow(cv::Mat_<cv::Vec3f>(_image), _image_name, _wait_time_ms, _l_keep_window);
	} else if (_image.type() == CV_64FC3) {
		imshow(cv::Mat_<cv::Vec3d>(_image), _image_name, _wait_time_ms, _l_keep_window);
	} else {
		std::cout << "not implemented for _image.type() = " << _image.type() << std::endl;
		DEBUG_BREAK;
	}
}


int ocv::get_position(const cv::Point& _point, const cv::Size& _size) {

	int pos;
	pos = _point.y;
	pos *= _size.width;
	pos += _point.x;

	return pos;
}

////////////Computing////////////


ocv::Timg& ocv::shiftFrame(const ocv::Timg& _image, int _pixels_x, int _pixels_y, ocv::Timg& _image_output) {

	_pixels_y *= -1;//cartesian coordinates

	if (_image.type() != _image_output.type()) {
		std::cout << "image types differ" << std::endl;
	}

	_image_output.create(_image.size());
	_image_output = 0;

	cv::Rect in_rect(shift_maxmin(0, _pixels_x), shift_maxmin(0, _pixels_y), _image.cols - std::abs(_pixels_x), _image.rows - std::abs(_pixels_y));

	cv::Rect out_rect(shift_minmax(0, _pixels_x), shift_minmax(0, _pixels_y), _image.cols - std::abs(_pixels_x), _image.rows - std::abs(_pixels_y));

	_image(in_rect).copyTo(_image_output(out_rect));

	return _image_output;
}

ocv::Timg ocv::shiftFrame(ocv::Timg _frame, float pixels_x, float pixels_y, ocv::Timg& _frame_output, ocv::Timg _super_frame1, ocv::Timg _super_frame2, unsigned int _Nsuper_resolution) {


	cv::resize(_frame, _super_frame1, cv::Size(), (float)_Nsuper_resolution, (float)_Nsuper_resolution, cv::INTER_LINEAR);

	pixels_x *= _Nsuper_resolution;
	pixels_y *= _Nsuper_resolution;

	_super_frame2 = shiftFrame(_super_frame1, (int)pixels_x, (int)pixels_y, _super_frame2);

	cv::resize(_super_frame2, _frame_output, cv::Size(), 1. / _Nsuper_resolution, 1. / _Nsuper_resolution, cv::INTER_AREA);

	return _frame_output;

}

void ocv::shift(const cv::Mat& src, cv::Mat& dst, cv::Point2f delta, int fill, cv::Scalar value) {

	// BSD License
	//https://stackoverflow.com/questions/40551281/translate-image-by-very-small-step-by-opencv-in-c

	// error checking
	//assert(fabs(delta.x) < src.cols && fabs(delta.y) < src.rows);

	// split the shift into integer and subpixel components
	cv::Point2i deltai((int)ceil(delta.x), (int)ceil(delta.y));
	cv::Point2f deltasub(fabs(delta.x - deltai.x), fabs(delta.y - deltai.y));

	// INTEGER SHIFT
	// first create a border around the parts of the Mat that will be exposed
	int t = 0, b = 0, l = 0, r = 0;
	if (deltai.x > 0) l = deltai.x;
	if (deltai.x < 0) r = -deltai.x;
	if (deltai.y > 0) t = deltai.y;
	if (deltai.y < 0) b = -deltai.y;
	cv::Mat padded;
	cv::copyMakeBorder(src, padded, t, b, l, r, fill, value);

	// SUBPIXEL SHIFT
	float eps = std::numeric_limits<float>::epsilon();
	if (deltasub.x > eps || deltasub.y > eps) {
		switch (src.depth()) {
		case CV_32F:
		{
			cv::Matx<float, 1, 2> dx(1 - deltasub.x, deltasub.x);
			cv::Matx<float, 2, 1> dy(1 - deltasub.y, deltasub.y);
			sepFilter2D(padded, padded, -1, dx, dy, cv::Point(0, 0), 0, cv::BORDER_CONSTANT);
			break;
		}
		case CV_64F:
		{
			cv::Matx<double, 1, 2> dx(1 - deltasub.x, deltasub.x);
			cv::Matx<double, 2, 1> dy(1 - deltasub.y, deltasub.y);
			sepFilter2D(padded, padded, -1, dx, dy, cv::Point(0, 0), 0, cv::BORDER_CONSTANT);
			break;
		}
		default:
		{
			cv::Matx<float, 1, 2> dx(1 - deltasub.x, deltasub.x);
			cv::Matx<float, 2, 1> dy(1 - deltasub.y, deltasub.y);
			padded.convertTo(padded, CV_32F);
			sepFilter2D(padded, padded, CV_32F, dx, dy, cv::Point(0, 0), 0, cv::BORDER_CONSTANT);
			break;
		}
		}
	}

	// construct the region of interest around the new matrix
	cv::Rect roi = cv::Rect(std::max(-deltai.x, 0), std::max(-deltai.y, 0), 0, 0) + src.size();
	dst = padded(roi);
}

void ocv::get_diff_normalized(const Timg& _image1, const Timg& _image2, Timg& _RMS_output) {

	cv::absdiff(_image1, _image2, _RMS_output);
	cv::divide(_RMS_output, _image1, _RMS_output);

}

void ocv::get_diff_normalized(const Timg1& _image1, const Timg1& _image2, Timg1& _RMS_output) {

	cv::absdiff(_image1, _image2, _RMS_output);
	cv::divide(_RMS_output, _image1, _RMS_output);

}

bool ocv::is_equal(const cv::Mat& _image1, const cv::Mat& _image2) {


	return (cv::sum(_image1 != _image2) == cv::Scalar(0, 0, 0, 0));

}


cv::Mat ocv::reduce_channels(const cv::Mat& _input, const int _rtype) {

	unsigned int rows = _input.rows;
	cv::Mat output = _input.reshape(1, rows*_input.cols);
	cv::reduce(output, output, 1, _rtype);
	output = output.reshape(1, rows);
	return output;
}


void ocv::quiver(const Timg& _image, const Timg2& _vectors, float _scale, cv::Point _offset) {

	cv::Point2f p, p_, p2;
	for (int i = 0; i < _image.rows; i += _offset.y) {
		for (int j = 0; j < _image.cols; j += _offset.x) {
			p.x = (float)j;
			p.y = (float)i;
			p_ = _vectors.at<cv::Point2f>(p);
			if (p_.x != 0 || p_.y != 0) {
				p_ *= _scale;
				p2 = p_;
				p2 += p;
				cv::arrowedLine(_image, p, p2, cv::Scalar(0, 0, 1.));// , 1.5, 8, 0, 0.1);
			}
		}
	}

}

void ocv::quiver(const Timg& _image, const Timg1& _vectors_x, const Timg1& _vectors_y, float _scale, cv::Point _offset) {

	std::vector<Timg1> channels = { _vectors_x, _vectors_y };
	Timg2 vectors;
	cv::merge(channels, vectors);
	quiver(_image, vectors, _scale, _offset);

}

void ocv::quiver(const Timg& _image, const VecImg& _vectors, float _scale, cv::Point _offset) {

	std::vector<Timg1> channels = { _vectors.first, _vectors.second };
	Timg2 vectors;
	cv::merge(channels, vectors);
	quiver(_image, vectors, _scale, _offset);
}

/*! Applies sqrt to be robust to square. Otherwise, will yield 0.*/
const ocv::Tvalue ocv::epsilon = std::sqrt(std::numeric_limits<ocv::Tvalue>::min());

void ocv::imshow_norm(const VecImg& _vector, std::string _image_name, unsigned int _wait_time_ms, bool _l_keep_window) {

	Timg1 norm;
	get_norm(_vector, norm);
	ocv::imshow(norm, _image_name, _wait_time_ms, _l_keep_window);

}

void ocv::imshow_quiver(const VecImg& _vector, std::string _image_name, float _scale, cv::Point _offset, unsigned int _wait_time_ms, bool _l_keep_window) {

	Timg norm(_vector.first.size());
	norm = 0.;
	imshow_quiver(norm, _vector, _image_name, _scale, _offset, _wait_time_ms, _l_keep_window);
}

void ocv::imshow_quiver(const cv::Mat& _image, const VecImg& _vector, std::string _image_name, float _scale, cv::Point _offset, unsigned int _wait_time_ms, bool _l_keep_window) {

	cv::Mat image;
	_image.copyTo(image);
	quiver(image, _vector, _scale, _offset);
	ocv::imshow(image, _image_name, _wait_time_ms, _l_keep_window);

}

void ocv::imshow_quiver(const cv::Mat& _image, const VecImg& _vector, const VecImg& _vector2, std::string _image_name, float _scale, cv::Point _offset, unsigned int _wait_time_ms, bool _l_keep_window) {

	cv::Mat image;
	_image.copyTo(image);
	quiver(image, _vector, _scale, _offset);
	quiver(image, _vector2, _scale, _offset);
	ocv::imshow(image, _image_name, _wait_time_ms, _l_keep_window);

}

bool ocv::has_nan(const cv::Mat& _image, const std::string _info) {

	bool l_has_nan = false;
	cv::Mat mask_nan = cv::Mat(_image != _image);
	std::vector<cv::Mat> channels;
	cv::split(mask_nan, channels);
	for (unsigned int c = 0; c < channels.size(); c++) {
		if (cv::countNonZero(channels[c]) > 0) {
			if (!_info.empty()) {
				std::cout << "c = " << c << std::endl;
				ocv::imshow(mask_nan, _info + " NaN at channel " + Misc::int_to_string(c));
			}
			l_has_nan = true;
		}
	}
	return l_has_nan;
}

void ocv::mask_filter(ocv::Tmask& _mask) {

	if (ocv::is_valid(_mask)) {
		for (ocv::Tmask::iterator it = _mask.begin(); it < _mask.end(); ++it) {
			if ((*it)[0] != 0 && (*it)[0] != ocv::mask_value) {
				(*it)[0] = ocv::mask_value;
			}
		}
	}

}

void ocv::mask_inverse(const ocv::Tmask& _mask_input, ocv::Tmask& _mask_output, bool _l_strong) {

	_mask_output.create(_mask_input.size());
	ocv::Tmask::iterator it_out = _mask_output.begin();
	for (ocv::Tmask::const_iterator it_in = _mask_input.begin(); it_in < _mask_input.end(); ++it_in, ++it_out) {
		if ((*it_in)[0] == 0) {
			(*it_out)[0] = ocv::mask_value;
		} else if ((*it_in)[0] == 255 || !_l_strong) {
			(*it_out)[0] = 0;
		} else {
			std::cout << "mask can't have value : " << (int)(*it_in)[0] << std::endl;
		}
	}

}

/*! Merge two masks. Kept values are those being : _mask_value AND _mask_value.*/
void ocv::mask_merge_and(const ocv::Tmask& _mask1, const ocv::Tmask& _mask2, ocv::Tmask& _mask_output, const uchar _mask_value) {

	if (_mask1.size() == _mask2.size()) {

		uchar mask_value_opposite;
		if (_mask_value == ocv::mask_value) {
			mask_value_opposite = 0;
		} else if (_mask_value == 0) {
			mask_value_opposite = ocv::mask_value;
		} else {
			mask_value_opposite = std::numeric_limits<uchar>::max();
			std::cout << "ocv::mask_merge_and : _mask_value is neither 0 or 255" << std::endl;
		}

		_mask_output.create(_mask1.size());

		ocv::Tmask::const_iterator it1 = _mask1.begin();
		ocv::Tmask::const_iterator it2 = _mask2.begin();
		for (ocv::Tmask::iterator it=_mask_output.begin(); it != _mask_output.end(); ++it, ++it1, ++it2) {

			if ((*it1)[0] == _mask_value && (*it2)[0] == _mask_value) {
				(*it)[0] = _mask_value;
			} else {
				(*it)[0] = mask_value_opposite;
			}

		}

	} else {
		std::cout << "ocv::mask_merge_and : wrong mask sizes. _mask1.size() = " << _mask1.size() << "_mask2.size() = " << _mask2.size() << std::endl;
	}

}

