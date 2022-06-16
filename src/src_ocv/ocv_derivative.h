/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "ocv.h"

namespace ocv {

	EXTERN_CELF_API int derivative_borderType;

	template <class Tval>
	struct Buffer_derivative {
		std::vector< cv::Mat_< cv::Vec<Tval, 1> > > channels, channel_derivatives;
		void clear() {
			channels.clear();
			channel_derivatives.clear();
		}
	};
	/*! Derivative of multi channel image. Copied on cv::Sobel.*/
	template <class Tval, int Dim>
	void derivative(const cv::Mat_< cv::Vec<Tval, Dim> >& _image, cv::Mat_< cv::Vec<Tval, Dim> >& _result, int dx, int dy, int ksize = 3, double scale = 1., double delta = 0., int borderType = ocv::derivative_borderType);
	/*! Derivative of multi channel image. Copied on cv::Sobel.*/
	template <class Tval, int Dim>
	void derivative(const cv::Mat_< cv::Vec<Tval, Dim> >& _image, cv::Mat_< cv::Vec<Tval, Dim> >& _result, int dx, int dy, Buffer_derivative<Tval>& _buffer,
		int ksize = 3, double scale = 1., double delta = 0., int borderType = ocv::derivative_borderType);
	/*! Derivative of multi channel image. Copied on cv::Sobel.*/
	template <class Tval>
	void derivative(const cv::Mat_< cv::Vec<Tval, 1> >& _image, cv::Mat_< cv::Vec<Tval, 1> >& _result, int dx, int dy, Buffer_derivative<Tval>& _buffer,
		int ksize = 3, double scale = 1., double delta = 0., int borderType = ocv::derivative_borderType);
	/*! Derivative of multi channel image. Copied on cv::Sobel. Overload for 1D.*/
	template <class Tval>
	void derivative(const cv::Mat_< cv::Vec<Tval, 1> >& _image, cv::Mat_< cv::Vec<Tval, 1> >& _result, int dx, int dy, int ksize = 3, double scale = 1., double delta = 0., int borderType = ocv::derivative_borderType);

	template <class T>
	void imshow_grad(const cv::Mat_<T> _image, float _scale = 1., cv::Point _offset = cv::Point(5, 5), std::string _image_name = "image grad", unsigned int _wait_time_ms = imshow_wait_time);

}

template <class Tval, int Dim>
void ocv::derivative(const cv::Mat_< cv::Vec<Tval, Dim> >& _image, cv::Mat_< cv::Vec<Tval, Dim> >& _result, int dx, int dy, int ksize, double scale, double delta, int borderType) {

	Buffer_derivative<Tval> buffer;
	ocv::derivative(_image, _result, dx, dy, buffer, ksize, scale, delta, borderType);

}

template <class Tval>
void ocv::derivative(const cv::Mat_< cv::Vec<Tval, 1> >& _image, cv::Mat_< cv::Vec<Tval, 1> >& _result, int dx, int dy, int ksize, double scale, double delta, int borderType) {

	Buffer_derivative<Tval> buffer;
	ocv::derivative(_image, _result, dx, dy, buffer, ksize, scale, delta, borderType);
}

template <class Tval, int Dim>
void ocv::derivative(const cv::Mat_< cv::Vec<Tval, Dim> >& _image, cv::Mat_< cv::Vec<Tval, Dim> >& _result, int dx, int dy, Buffer_derivative<Tval>& _buffer,
	int ksize, double scale, double delta, int _borderType) {

	cv::split(_image, _buffer.channels);
	_buffer.channel_derivatives.resize(Dim);
	for (unsigned int d = 0; d < Dim; d++) {

		ocv::derivative(_buffer.channels[d], _buffer.channel_derivatives[d], dx, dy, _buffer, ksize, scale, delta, _borderType);

	}

	cv::merge(_buffer.channel_derivatives, _result);

}

template <class Tval>
void ocv::derivative(const cv::Mat_< cv::Vec<Tval, 1> >& _image, cv::Mat_< cv::Vec<Tval, 1> >& _result, int dx, int dy, Buffer_derivative<Tval>& _buffer,
	int ksize, double scale, double delta, int _borderType) {

	int borderType;
	if (_borderType == cv::BORDER_TRANSPARENT) {
		/*! Because opencv doesn't manage this case, a design is porposed.*/
		borderType = cv::BORDER_REPLICATE;
	} else {
		borderType = _borderType;
	}

	cv::Mat kx_;
	cv::Mat ky_;
	cv::Mat_<cv::Vec1f> kx;
	cv::Mat_<cv::Vec1f> ky;
	/*! getDerivKernels returns only one derivativr scheme, but smoothed depending on ksize. Both directions. There is no accuracy management.*/
	getDerivKernels(kx_, ky_, dx, dy, ksize, true, CV_32F);
	typedef float Tkernel;

	kx_.copyTo(kx);
	ky_.copyTo(ky);

	/*! Make sure no orthogonal smoothing is applied.*/
	if (dx == 0) {
		kx = cv::Mat_<cv::Vec1f>(cv::Size(1, 1));
		kx(0, 0) = cv::Vec1f(1.);
	}
	if (dy == 0) {
		ky = cv::Mat_<cv::Vec1f>(cv::Size(1, 1));
		ky(0, 0) = cv::Vec1f(1.);
	}

	cv::sepFilter2D(_image, _result, -1, kx, ky, cv::Point(-1, -1), delta, borderType);

	/*! The following is an attempt to implement a transparent border condition.*/
	if (_borderType == cv::BORDER_TRANSPARENT) {

		int width = _image.size().width;
		int height = _image.size().height;

		unsigned int k_d1_O2_center_size = 3;
		cv::Mat_<cv::Vec1f> k_d1_O2_center(k_d1_O2_center_size, 1);
		k_d1_O2_center.at<Tkernel>(0, 0) = Tkernel(-1. / 2.);
		k_d1_O2_center.at<Tkernel>(1, 0) = Tkernel(0.);
		k_d1_O2_center.at<Tkernel>(2, 0) = Tkernel(1. / 2.);

		unsigned int k_d1_O1_backward_size = 2;
		cv::Mat_<cv::Vec1f> k_d1_O1_backward(k_d1_O1_backward_size, 1);
		k_d1_O1_backward.at<Tkernel>(0, 0) = Tkernel(-1.);
		k_d1_O1_backward.at<Tkernel>(1, 0) = Tkernel(1.);

		unsigned int k_d1_O2_backward_size = 3;
		cv::Mat_<cv::Vec1f> k_d1_O2_backward(k_d1_O2_backward_size, 1);
		k_d1_O2_backward.at<Tkernel>(0, 0) = Tkernel(1. / 2.);
		k_d1_O2_backward.at<Tkernel>(1, 0) = Tkernel(-2.);
		k_d1_O2_backward.at<Tkernel>(2, 0) = Tkernel(3. / 2.);

		unsigned int k_d1_O6_backward_size = 7;
		cv::Mat_<cv::Vec1f> k_d1_O6_backward(k_d1_O6_backward_size, 1);
		k_d1_O6_backward.at<Tkernel>(0, 0) = Tkernel(1. / 6.);
		k_d1_O6_backward.at<Tkernel>(1, 0) = Tkernel(-6. / 5.);
		k_d1_O6_backward.at<Tkernel>(2, 0) = Tkernel(15. / 4.);
		k_d1_O6_backward.at<Tkernel>(3, 0) = Tkernel(-20. / 3.);
		k_d1_O6_backward.at<Tkernel>(4, 0) = Tkernel(15. / 2.);
		k_d1_O6_backward.at<Tkernel>(5, 0) = Tkernel(-6.);
		k_d1_O6_backward.at<Tkernel>(6, 0) = Tkernel(49. / 20.);

		unsigned int k_d1_O1_forward_size = 2;
		cv::Mat_<cv::Vec1f> k_d1_O1_forward(k_d1_O1_forward_size, 1);
		k_d1_O1_forward.at<Tkernel>(0, 0) = Tkernel(-1.);
		k_d1_O1_forward.at<Tkernel>(1, 0) = Tkernel(1.);

		unsigned int k_d1_O2_forward_size = 3;
		cv::Mat_<cv::Vec1f> k_d1_O2_forward(k_d1_O2_forward_size, 1);
		k_d1_O2_forward.at<Tkernel>(0, 0) = Tkernel(-3. / 2.);
		k_d1_O2_forward.at<Tkernel>(1, 0) = Tkernel(2.);
		k_d1_O2_forward.at<Tkernel>(2, 0) = Tkernel(-1. / 2.);

		unsigned int k_d1_O6_forward_size = 7;
		cv::Mat_<cv::Vec1f> k_d1_O6_forward(k_d1_O6_forward_size, 1);
		k_d1_O6_forward.at<Tkernel>(0, 0) = Tkernel(-49. / 20.);
		k_d1_O6_forward.at<Tkernel>(1, 0) = Tkernel(6.);
		k_d1_O6_forward.at<Tkernel>(2, 0) = Tkernel(-15. / 2.);
		k_d1_O6_forward.at<Tkernel>(3, 0) = Tkernel(20. / 3.);
		k_d1_O6_forward.at<Tkernel>(4, 0) = Tkernel(-15. / 4.);
		k_d1_O6_forward.at<Tkernel>(5, 0) = Tkernel(6. / 5.);
		k_d1_O6_forward.at<Tkernel>(6, 0) = Tkernel(-1. / 6.);

		unsigned int k_d2_O1_backward_size = 3;
		cv::Mat_<cv::Vec1f> k_d2_O1_backward(k_d2_O1_backward_size, 1);
		k_d2_O1_backward.at<Tkernel>(0, 0) = Tkernel(1.);
		k_d2_O1_backward.at<Tkernel>(1, 0) = Tkernel(-2.);
		k_d2_O1_backward.at<Tkernel>(2, 0) = Tkernel(1.);

		unsigned int k_d2_O1_forward_size = 3;
		cv::Mat_<cv::Vec1f> k_d2_O1_forward(k_d2_O1_forward_size, 1);
		k_d2_O1_forward.at<Tkernel>(0, 0) = Tkernel(1.);
		k_d2_O1_forward.at<Tkernel>(1, 0) = Tkernel(-2.);
		k_d2_O1_forward.at<Tkernel>(2, 0) = Tkernel(1.);

		// First order

		if (dx == 1 && dy == 0 && width > 2) {

			cv::Mat_<cv::Vec1f> kx_border;
			cv::Mat_<cv::Vec1f> ky_border(1, 1);
			ky_border.at<Tkernel>(0, 0) = Tkernel(1.);

			cv::Mat_< cv::Vec<Tval, 1> > input;
			cv::Mat_< cv::Vec<Tval, 1> > output;

			//BOTTOM
			bool l_O1 = (ksize == 1);
			int k_size;
			if (l_O1) {
				kx_border = k_d1_O1_forward;
			} else {
				kx_border = k_d1_O2_forward;
			}
			k_size = kx_border.size().height;

			input = _image(cv::Rect(0, 0, k_size, height));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(0, -1), delta);
			output(cv::Rect(0, 0, 1, height)).copyTo(_result(cv::Rect(0, 0, 1, height)));

			//UP
			if (l_O1) {
				kx_border = k_d1_O1_backward;
			} else {
				kx_border = k_d1_O2_backward;
			}
			k_size = kx_border.size().height;

			input = _image(cv::Rect(width - k_size, 0, k_size, height));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(k_size - 1, -1), delta);
			output(cv::Rect(k_size - 1, 0, 1, height)).copyTo(_result(cv::Rect(width - 1, 0, 1, height)));


		} else if (dx == 1 && dy == 0 && width > 1) {
			// centered scheme to uncentered order 1 has factor coefficient
			_result.col(0) *= 2.;
			_result.col(width - 1) *= 2.;
		}

		if (dy == 1 && dx == 0 && height > 2) {

			cv::Mat_<cv::Vec1f> kx_border(1, 1);
			cv::Mat_<cv::Vec1f> ky_border;
			kx_border.at<Tkernel>(0, 0) = Tkernel(1.);

			cv::Mat_< cv::Vec<Tval, 1> > input;
			cv::Mat_< cv::Vec<Tval, 1> > output;

			//BOTTOM
			bool l_O1 = (ksize == 1);
			int k_size;
			if (l_O1) {
				ky_border = k_d1_O1_forward;
			} else {
				ky_border = k_d1_O2_forward;
			}
			k_size = ky_border.size().height;

			input = _image(cv::Rect(0, 0, width, k_size));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(-1, 0), delta);
			output(cv::Rect(0, 0, width, 1)).copyTo(_result(cv::Rect(0, 0, width, 1)));

			//UP
			if (l_O1) {
				ky_border = k_d1_O1_backward;
			} else {
				ky_border = k_d1_O2_backward;
			}
			k_size = ky_border.size().height;

			input = _image(cv::Rect(0, height - k_size, width, k_size));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(-1, k_size - 1), delta);
			output(cv::Rect(0, k_size - 1, width, 1)).copyTo(_result(cv::Rect(0, height - 1, width, 1)));


		} else if (dy == 1 && dx == 0 && height > 1) {
			// centered scheme to uncentered order 1 has factor coefficient
			_result.row(0) *= 2.;
			_result.row(height - 1) *= 2.;
		}

		// First order multi

		if (dx == 1 && dy == 1 && width > 2) {

			cv::Mat_<cv::Vec1f> kx_border;
			cv::Mat_<cv::Vec1f> ky_border;

			cv::Mat_< cv::Vec<Tval, 1> > input;
			cv::Mat_< cv::Vec<Tval, 1> > output;

			//LEFT
			kx_border = k_d1_O1_forward;
			ky_border = k_d1_O2_center;

			input = _image(cv::Rect(0, 0, k_d1_O1_forward_size, height));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(0, -1), delta);
			output(cv::Rect(0, 0, 1, height)).copyTo(_result(cv::Rect(0, 0, 1, height)));

			//RIGHT
			kx_border = k_d1_O1_backward;
			ky_border = k_d1_O2_center;

			input = _image(cv::Rect(width - k_d1_O1_backward_size, 0, k_d1_O1_backward_size, height));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(k_d1_O1_backward_size - 1, -1), delta);
			output(cv::Rect(k_d1_O1_backward_size - 1, 0, 1, height)).copyTo(_result(cv::Rect(width - 1, 0, 1, height)));

			//BOTTOM
			kx_border = k_d1_O2_center;
			ky_border = k_d1_O1_forward;

			input = _image(cv::Rect(0, 0, width, k_d1_O1_forward_size));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(-1, 0), delta);
			output(cv::Rect(0, 0, width, 1)).copyTo(_result(cv::Rect(0, 0, width, 1)));

			//UP
			kx_border = k_d1_O2_center;
			ky_border = k_d1_O1_backward;

			input = _image(cv::Rect(0, height - k_d1_O1_backward_size, width, k_d1_O1_backward_size));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(-1, k_d1_O1_backward_size - 1), delta);
			output(cv::Rect(0, k_d1_O1_backward_size - 1, width, 1)).copyTo(_result(cv::Rect(0, height - 1, width, 1)));

			//LEFT-BOTTOM
			kx_border = k_d1_O1_forward;
			ky_border = k_d1_O1_forward;

			input = _image(cv::Rect(0, 0, k_d1_O1_forward_size, k_d1_O1_forward_size));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(0, 0), delta);
			output(cv::Rect(0, 0, 1, 1)).copyTo(_result(cv::Rect(0, 0, 1, 1)));

			//RIGHT-BOTTOM
			kx_border = k_d1_O1_backward;
			ky_border = k_d1_O1_forward;

			input = _image(cv::Rect(width - k_d1_O1_backward_size, 0, k_d1_O1_backward_size, k_d1_O1_forward_size));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(k_d1_O1_backward_size - 1, 0), delta);
			output(cv::Rect(k_d1_O1_backward_size - 1, 0, 1, 1)).copyTo(_result(cv::Rect(width - 1, 0, 1, 1)));

			//LEFT-UP
			kx_border = k_d1_O1_forward;
			ky_border = k_d1_O1_backward;

			input = _image(cv::Rect(0, height - k_d1_O1_backward_size, k_d1_O1_forward_size, k_d1_O1_backward_size));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(0, k_d1_O1_backward_size - 1), delta);
			output(cv::Rect(0, k_d1_O1_backward_size - 1, 1, 1)).copyTo(_result(cv::Rect(0, height - 1, 1, 1)));

			//RIGHT-UP
			kx_border = k_d1_O1_backward;
			ky_border = k_d1_O1_backward;

			input = _image(cv::Rect(width - k_d1_O1_backward_size, height - k_d1_O1_backward_size, k_d1_O1_backward_size, k_d1_O1_backward_size));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(k_d1_O1_backward_size - 1, k_d1_O1_backward_size - 1), delta);
			output(cv::Rect(k_d1_O1_backward_size - 1, k_d1_O1_backward_size - 1, 1, 1)).copyTo(_result(cv::Rect(width - 1, height - 1, 1, 1)));

		}

		// Second order

		if (dx == 2 && dy == 0 && width > 2) {

			cv::Mat_<cv::Vec1f> kx_border;
			cv::Mat_<cv::Vec1f> ky_border(1, 1);
			ky_border.at<Tkernel>(0, 0) = Tkernel(1.);

			cv::Mat_< cv::Vec<Tval, 1> > input;
			cv::Mat_< cv::Vec<Tval, 1> > output;

			//LEFT
			kx_border = k_d2_O1_forward;

			input = _image(cv::Rect(0, 0, k_d2_O1_forward_size, height));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(0, -1), delta);
			output(cv::Rect(0, 0, 1, height)).copyTo(_result(cv::Rect(0, 0, 1, height)));

			//RIGHT
			kx_border = k_d2_O1_backward;

			input = _image(cv::Rect(width - k_d2_O1_backward_size, 0, k_d2_O1_backward_size, height));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(k_d2_O1_backward_size - 1, -1), delta);
			output = 0.;
			output(cv::Rect(k_d2_O1_backward_size - 1, 0, 1, height)).copyTo(_result(cv::Rect(width - 1, 0, 1, height)));


		} else if (dx == 2 && dy == 0 && width > 1) {
			//assumes there is no second order derivative
			_result.col(0) = 0.;
			_result.col(width - 1) = 0.;
		}

		if (dy == 2 && dx == 0 && height > 2) {

			cv::Mat_<cv::Vec1f> kx_border(1, 1);
			cv::Mat_<cv::Vec1f> ky_border;
			kx_border.at<Tkernel>(0, 0) = Tkernel(1.);

			cv::Mat_< cv::Vec<Tval, 1> > input;
			cv::Mat_< cv::Vec<Tval, 1> > output;

			//BOTTOM
			ky_border = k_d2_O1_forward;

			input = _image(cv::Rect(0, 0, width, k_d2_O1_forward_size));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(-1, 0), delta);
			output(cv::Rect(0, 0, width, 1)).copyTo(_result(cv::Rect(0, 0, width, 1)));

			//UP
			ky_border = k_d2_O1_backward;

			input = _image(cv::Rect(0, height - k_d2_O1_backward_size, width, k_d2_O1_backward_size));
			cv::sepFilter2D(input, output, -1, kx_border, ky_border, cv::Point(-1, k_d2_O1_backward_size - 1), delta);
			output = 0.;
			output(cv::Rect(0, k_d2_O1_backward_size - 1, width, 1)).copyTo(_result(cv::Rect(0, height - 1, width, 1)));


		} else if (dy == 2 && dx == 0 && height > 1) {
			_result.row(0) = 0.;
			_result.row(height - 1) = 0.;
		}
	}

	if ((dy + 1) % 2 == 0) {
		_result *= -1.;
	}

}


template <class T>
void ocv::imshow_grad(const cv::Mat_<T> _image, float _scale, cv::Point _offset, std::string _image_name, unsigned int _wait_time_ms) {

	cv::Mat_<T> image_clone = _image.clone();
	ocv::Timg1 grad_x, grad_y;
	ocv::Timg1 image_gray;
	cvtColor(image_clone, image_gray, cv::COLOR_BGR2GRAY);
	ocv::derivative(image_gray, grad_x, image_gray.type(), 1, 0, 3);
	ocv::derivative(image_gray, grad_y, image_gray.type(), 0, 1, 3);
	ocv::quiver(image_clone, grad_x, grad_y, _scale, _offset);
	ocv::imshow(image_clone, _image_name, _wait_time_ms);

}