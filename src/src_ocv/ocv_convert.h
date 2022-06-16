/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "ocv.h"
#include "ocv_image_type.h"
#include "ocv_minmax.h"

namespace ocv {

	/*! Convenient method to automatically convert an image in a different color space.*/
	template <class Tval1, class Tval2, int Dim>
	Tscaling convertTo_auto(const cv::Mat_< cv::Vec<Tval1, Dim> >& _image_original, cv::Mat_< cv::Vec<Tval2, Dim> >& _image_converted);
	/*! Reciprocal method to convert back to original color space.*/
	template <class Tval2, class Tval1, int Dim>
	void convertTo_auto(const cv::Mat_< cv::Vec<Tval2, Dim> >& _image_converted, cv::Mat_< cv::Vec<Tval1, Dim> >& _image_original, const Tscaling& _scaling);

	// Low level methods //

	/*! Converts images of different precision type.*/
	template <class Tval1, class Tval2, int Dim>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, Dim> >& _image_input, cv::Mat_< cv::Vec<Tval2, Dim> >& _image_output, bool _l_auto = false, bool _l_image_range_saturate = false);
	/*! Converts images of different precision type. Specify input range.*/
	template <class Tval1, class Tval2, int Dim>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, Dim> >& _image_input, cv::Mat_< cv::Vec<Tval2, Dim> >& _image_output, const ocv::Range<Tval1>& _range_input);

	template <class Tval1, int Dim1, class Tval2, int Dim2>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, Dim1> >& _image_input, cv::Mat_< cv::Vec<Tval2, Dim2> >& _image_output, bool _l_auto = false, bool _l_image_range_saturate = false);
	template <class Tval1, int Dim1, class Tval2, int Dim2>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, Dim1> >& _image_input, cv::Mat_< cv::Vec<Tval2, Dim2> >& _image_output, const ocv::Range<Tval1>& _range_input);
#ifdef ENABLE_OPACITY_1C
	template <class Tval1, class Tval2>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, 1> >& _image_input, cv::Mat_< cv::Vec<Tval2, 2> >& _image_output, bool _l_auto = false, bool _l_image_range_saturate = false);
	template <class Tval1, class Tval2>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 1> >& _image_output, bool _l_auto = false, bool _l_image_range_saturate = false);
	template <class Tval1, class Tval2>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 2> >& _image_output, bool _l_auto = false, bool _l_image_range_saturate = false);
	template <class Tval1, class Tval2>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 4> >& _image_output, bool _l_auto = false, bool _l_image_range_saturate = false);
	template <class Tval1, class Tval2>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, 1> >& _image_input, cv::Mat_< cv::Vec<Tval2, 2> >& _image_output, const ocv::Range<Tval1>& _range_input);
	template <class Tval1, class Tval2>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 1> >& _image_output, const ocv::Range<Tval1>& _range_input);
	template <class Tval1, class Tval2>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 2> >& _image_output, const ocv::Range<Tval1>& _range_input);
	template <class Tval1, class Tval2>
	Tscaling convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 4> >& _image_output, const ocv::Range<Tval1>& _range_input);
#endif


	/*! Converts images of different precision type. Also manage range.*/
	template <class Tval2, int Dim>
	Tscaling convertTo(const cv::Mat& _image_input, cv::Mat_< cv::Vec<Tval2, Dim> >& _image_output, bool _l_auto = false, bool _l_image_range_saturate = false);

}

template <class Tval1, class Tval2, int Dim>
ocv::Tscaling ocv::convertTo_auto(const cv::Mat_< cv::Vec<Tval1, Dim> >& _image_original, cv::Mat_< cv::Vec<Tval2, Dim> >& _image_converted) {

	if (std::is_floating_point<Tval1>::value && std::is_floating_point<Tval2>::value) {
		return ocv::convertTo(_image_original, _image_converted, false);
	} else {
		return ocv::convertTo(_image_original, _image_converted, true);
	}

}

/*! Reciprocal method to convert back to original color space.*/
template <class Tval2, class Tval1, int Dim>
void ocv::convertTo_auto(const cv::Mat_< cv::Vec<Tval2, Dim> >& _image_converted, cv::Mat_< cv::Vec<Tval1, Dim> >& _image_original, const Tscaling& _scaling) {

	/*! If Tval2 has a shortest discretization range (assumed integer type), or if Tval2 a floating point type.*/
	if (sizeof(Tval2) < sizeof(Tval1) || (std::is_floating_point<Tval2>::value && std::is_floating_point<Tval1>::value)) {
		ocv::convertTo(_image_converted, _image_original, false);
		ocv::rescale_inv(_image_original, _scaling);
	} else {
		/* Uses color range of Tval1 for converting back to original.*/
		ocv::convertTo(_image_converted, _image_original, true);
	}
	
}

// Low level methods //

template <class Tval1, class Tval2, int Dim>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, Dim> >& _image_input, cv::Mat_< cv::Vec<Tval2, Dim> >& _image_output, bool _l_auto, bool _l_image_range_saturate) {

	ocv::Tscaling scaling;

	if (_l_auto) {

		/*! Differences are converted to double because range can be out of range. Ex: char : -128 to 127, range=255 : can't be stored in a char*/
		ocv::Range<Tval1> range_input = get_minmax_range(_image_input, _l_image_range_saturate);
		scaling = convertTo(_image_input, _image_output, range_input);

		/*! If dimension 4, alpha channel must be treated differently. Scaling applied to color channels (min/max) doesn't mean the same for the opacity channel.*/
		if (Dim == 4) {
			std::vector< cv::Mat_< cv::Vec<Tval1, 1> > > channels_input;
			/*! Extract alpha channel first and auto convert it independantly.*/
			cv::split(_image_input, channels_input);
			cv::Mat_< cv::Vec<Tval2, 1> > alpha_channel_converted;
			ocv::convertTo(channels_input[3], alpha_channel_converted, true);

			std::vector< cv::Mat_< cv::Vec<Tval2, 1> > > channels_output;
			cv::split(_image_output, channels_output);
			channels_output[3] = alpha_channel_converted;
			/*! Merge it to result.*/
			cv::merge(channels_output, _image_output);
		}

	} else {
		scaling.first = 1.;
		scaling.second = 0.;
		_image_input.convertTo(_image_output, _image_output.type());
	}

	return scaling;
}

template <class Tval1, class Tval2, int Dim>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, Dim> >& _image_input, cv::Mat_< cv::Vec<Tval2, Dim> >& _image_output, const ocv::Range<Tval1>& _range_input) {

	Tscaling scaling;
	ocv::Range<Tval2> range_output = get_minmax_range(cv::Mat_< cv::Vec<Tval2, Dim> >());
	scaling = ocv::get_scaling(_range_input, range_output);
	_image_input.convertTo(_image_output, _image_output.type(), scaling.first, scaling.second);

	return scaling;
}

/*! Converts images of different precision type. Also manage range.*/
template <class Tval2, int Dim>
ocv::Tscaling ocv::convertTo(const cv::Mat& _image_input, cv::Mat_< cv::Vec<Tval2, Dim> >& _image_output, bool _l_auto, bool _l_image_range_saturate) {

	cv::Mat_< cv::Vec<Tval2, Dim> > image_dim;
	cv::Mat image_cvt;
	if (_image_input.channels() == Dim) {

		if (_image_input.depth() == 0) {
			cv::Mat_< cv::Vec<uint8_t, Dim> > image(_image_input);
			return convertTo(image, _image_output, _l_auto, _l_image_range_saturate);
		} else if (_image_input.depth() == 1) {
			cv::Mat_< cv::Vec<int8_t, Dim> > image(_image_input);
			return convertTo(image, _image_output, _l_auto, _l_image_range_saturate);
		} else if (_image_input.depth() == 2) {
			cv::Mat_< cv::Vec<uint16_t, Dim> > image(_image_input);
			return convertTo(image, _image_output, _l_auto, _l_image_range_saturate);
		} else if (_image_input.depth() == 3) {
			cv::Mat_< cv::Vec<int16_t, Dim> > image(_image_input);
			return convertTo(image, _image_output, _l_auto, _l_image_range_saturate);
		} else if (_image_input.depth() == 4) {
			cv::Mat_< cv::Vec<int32_t, Dim> > image(_image_input);
			return convertTo(image, _image_output, _l_auto, _l_image_range_saturate);
		} else if (_image_input.depth() == 5) {
			cv::Mat_< cv::Vec<float, Dim> > image(_image_input);
			return convertTo(image, _image_output, _l_auto, _l_image_range_saturate);
		} else if (_image_input.depth() == 6) {
			cv::Mat_< cv::Vec<double, Dim> > image(_image_input);
			return convertTo(image, _image_output, _l_auto, _l_image_range_saturate);
		} else {
			std::cout << "ocv::convertTo : unkown depth value " << _image_input.depth() << std::endl;
			DEBUG_BREAK;
			return ocv::Tscaling();
		}

	} else {
		cv::Mat image_cvt;
		if (_image_input.channels() == 3 && Dim == 1) {
			cvtColor(_image_input, image_cvt, cv::COLOR_BGR2GRAY);
		} else if (_image_input.channels() == 1 && Dim == 3) {
			cvtColor(_image_input, image_cvt, cv::COLOR_GRAY2BGR);
		}
#ifdef ENABLE_OPACITY_1C
		else if (_image_input.channels() == 2 && Dim == 1) {
			std::vector<cv::Mat> channels;
			cv::split(_image_input, channels);
			channels[0].copyTo(image_cvt);
		} else if (_image_input.channels() == 2 && Dim == 3) {
			std::vector<cv::Mat> channels;
			cv::split(_image_input, channels);
			cvtColor(channels[0], image_cvt, cv::COLOR_GRAY2BGR);
		}
#endif
		else {
			std::cout << "_image_input.channels() = " << _image_input.channels() << ", Dim = " << Dim << std::endl;
			DEBUG_BREAK;
		}

		/*! loops back to this same function.*/
		return convertTo(image_cvt, _image_output, _l_auto, _l_image_range_saturate);
	}

}

template <class Tval1, int Dim1, class Tval2, int Dim2>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, Dim1> >& _image_input, cv::Mat_< cv::Vec<Tval2, Dim2> >& _image_output, bool _l_auto, bool _l_image_range_saturate) {

	cv::Mat_< cv::Vec<Tval1, Dim2> > image_cvt;

	if (Dim1 == 3 && Dim2 == 1) {
		cvtColor(_image_input, image_cvt, cv::COLOR_BGR2GRAY);
	} else if (Dim1 == 1 && Dim2 == 3) {
		cvtColor(_image_input, image_cvt, cv::COLOR_GRAY2BGR);
	} else if (Dim1 == 1 && Dim2 == 4) {
		cvtColor(_image_input, image_cvt, cv::COLOR_GRAY2BGRA);
	} else if (Dim1 == 3 && Dim2 == 4) {
		cvtColor(_image_input, image_cvt, cv::COLOR_BGR2BGRA);
	} else if (Dim1 == 4 && Dim2 == 1) {
		cvtColor(_image_input, image_cvt, cv::COLOR_BGRA2GRAY);
	} else if (Dim1 == 4 && Dim2 == 3) {
		cvtColor(_image_input, image_cvt, cv::COLOR_BGRA2BGR);

	} else {
		std::cout << "Can't convert : Dim1 = " << Dim1 << ", Dim2 = " << Dim2 << std::endl;
		DEBUG_BREAK;
	}

	return convertTo(image_cvt, _image_output, _l_auto, _l_image_range_saturate);

}

template <class Tval1, int Dim1, class Tval2, int Dim2>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, Dim1> >& _image_input, cv::Mat_< cv::Vec<Tval2, Dim2> >& _image_output, const ocv::Range<Tval1>& _range_input) {

	cv::Mat_< cv::Vec<Tval1, Dim2> > image_cvt;

	if (Dim1 == 3 && Dim2 == 1) {
		cvtColor(_image_input, image_cvt, cv::COLOR_BGR2GRAY);
	} else if (Dim1 == 1 && Dim2 == 3) {
		cvtColor(_image_input, image_cvt, cv::COLOR_GRAY2BGR);
	} else if (Dim1 == 1 && Dim2 == 4) {
		cvtColor(_image_input, image_cvt, cv::COLOR_GRAY2BGRA);
	} else if (Dim1 == 3 && Dim2 == 4) {
		cvtColor(_image_input, image_cvt, cv::COLOR_BGR2BGRA);
	} else if (Dim1 == 4 && Dim2 == 1) {
		cvtColor(_image_input, image_cvt, cv::COLOR_BGRA2GRAY);
	} else if (Dim1 == 4 && Dim2 == 3) {
		cvtColor(_image_input, image_cvt, cv::COLOR_BGRA2BGR);

	} else {
		std::cout << "Can't convert : Dim1 = " << Dim1 << ", Dim2 = " << Dim2 << std::endl;
		DEBUG_BREAK;
	}

	return convertTo(image_cvt, _image_output, _range_input);

}

#ifdef ENABLE_OPACITY_1C
template <class Tval1, class Tval2>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, 1> >& _image_input, cv::Mat_< cv::Vec<Tval2, 2> >& _image_output, bool _l_auto, bool _l_image_range_saturate) {

	std::vector< cv::Mat_< cv::Vec<Tval2, 1> > > channels_output(2);

	ocv::Tscaling scaling;

	scaling = ocv::convertTo(_image_input, channels_output[0], _l_auto, _l_image_range_saturate);
	channels_output[1].create(_image_input.size());
	channels_output[1] = ocv::MaxRange<Tval2>::get();//default value for alpha channel
	cv::merge(channels_output, _image_output);

	return scaling;
}

template <class Tval1, class Tval2>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 1> >& _image_output, bool _l_auto, bool _l_image_range_saturate) {

	std::vector< cv::Mat_< cv::Vec<Tval1, 1> > > channels_input;

	cv::split(_image_input, channels_input);

	ocv::Tscaling scaling;
	scaling = ocv::convertTo(channels_input[0], _image_output, _l_auto, _l_image_range_saturate);

	return scaling;
}

template <class Tval1, class Tval2>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 2> >& _image_output, bool _l_auto, bool _l_image_range_saturate) {

	std::vector< cv::Mat_< cv::Vec<Tval1, 1> > > channels_input;
	cv::split(_image_input, channels_input);

	cv::Mat_< cv::Vec<Tval2, 1> > alpha_channel_output;
	ocv::convertTo(channels_input[1], alpha_channel_output, true);//different specific options for alpha channel could apply

	std::vector< cv::Mat_< cv::Vec<Tval2, 1> > > channels_output;

	ocv::Tscaling scaling;

	cv::Mat_< cv::Vec<Tval2, 1> > image_output;
	scaling = ocv::convertTo(channels_input[0], image_output, _l_auto, _l_image_range_saturate);
	channels_output.push_back(image_output);

	channels_output.push_back(alpha_channel_output);
	cv::merge(channels_output, _image_output);

	return scaling;

}

template <class Tval1, class Tval2>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 4> >& _image_output, bool _l_auto, bool _l_image_range_saturate) {

	std::vector< cv::Mat_< cv::Vec<Tval1, 1> > > channels_input;
	cv::split(_image_input, channels_input);

	cv::Mat_< cv::Vec<Tval2, 1> > alpha_channel_output;
	ocv::convertTo(channels_input[1], alpha_channel_output, true);//different specific options for alpha channel could apply

	std::vector< cv::Mat_< cv::Vec<Tval2, 1> > > channels_output;

	ocv::Tscaling scaling;

	cv::Mat_< cv::Vec<Tval2, 3> > image_output;
	scaling = ocv::convertTo(channels_input[0], image_output, _l_auto, _l_image_range_saturate);
	cv::split(image_output, channels_output);

	channels_output.push_back(alpha_channel_output);
	cv::merge(channels_output, _image_output);

	return scaling;

}

template <class Tval1, class Tval2>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, 1> >& _image_input, cv::Mat_< cv::Vec<Tval2, 2> >& _image_output, const ocv::Range<Tval1>& _range_input) {

	std::vector< cv::Mat_< cv::Vec<Tval2, 1> > > channels_output(2);

	ocv::Tscaling scaling;

	scaling = ocv::convertTo(_image_input, channels_output[0], _range_input);
	channels_output[1].create(_image_input.size());
	channels_output[1] = ocv::MaxRange<Tval2>::get();//default value for alpha channel
	cv::merge(channels_output, _image_output);

	return scaling;
}

template <class Tval1, class Tval2>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 1> >& _image_output, const ocv::Range<Tval1>& _range_input) {

	std::vector< cv::Mat_< cv::Vec<Tval1, 1> > > channels_input;

	cv::split(_image_input, channels_input);

	ocv::Tscaling scaling;
	scaling = ocv::convertTo(channels_input[0], _image_output, _range_input);

	return scaling;
}

template <class Tval1, class Tval2>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 2> >& _image_output, const ocv::Range<Tval1>& _range_input) {

	std::vector< cv::Mat_< cv::Vec<Tval1, 1> > > channels_input;
	cv::split(_image_input, channels_input);

	cv::Mat_< cv::Vec<Tval2, 1> > alpha_channel_output;
	ocv::convertTo(channels_input[1], alpha_channel_output, true);//different specific options for alpha channel could apply

	std::vector< cv::Mat_< cv::Vec<Tval2, 1> > > channels_output;

	ocv::Tscaling scaling;

	cv::Mat_< cv::Vec<Tval2, 1> > image_output;
	scaling = ocv::convertTo(channels_input[0], image_output, _range_input);
	channels_output.push_back(image_output);

	channels_output.push_back(alpha_channel_output);
	cv::merge(channels_output, _image_output);

	return scaling;

}

template <class Tval1, class Tval2>
ocv::Tscaling ocv::convertTo(const cv::Mat_< cv::Vec<Tval1, 2> >& _image_input, cv::Mat_< cv::Vec<Tval2, 4> >& _image_output, const ocv::Range<Tval1>& _range_input) {

	std::vector< cv::Mat_< cv::Vec<Tval1, 1> > > channels_input;
	cv::split(_image_input, channels_input);

	cv::Mat_< cv::Vec<Tval2, 1> > alpha_channel_output;
	ocv::convertTo(channels_input[1], alpha_channel_output, true);//different specific options for alpha channel could apply

	std::vector< cv::Mat_< cv::Vec<Tval2, 1> > > channels_output;

	ocv::Tscaling scaling;

	cv::Mat_< cv::Vec<Tval2, 3> > image_output;
	scaling = ocv::convertTo(channels_input[0], image_output, _range_input);
	cv::split(image_output, channels_output);

	channels_output.push_back(alpha_channel_output);
	cv::merge(channels_output, _image_output);

	return scaling;

}
#endif
