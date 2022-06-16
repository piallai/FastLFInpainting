/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "ocv.h"
#include "MatrixOcv2D.h"

template <class Timg>
class ImgMatrix;

#define Timg cv::Mat_<Tvec>
template <class Tvec>
class ImgMatrix<Timg> {

public :

	typedef typename Timg::value_type value_type;

private :

	bool l_symmetric;

	Timg img11;
	Timg* img12;
	Timg* img21;
	Timg img22;

public:

	ImgMatrix(bool _l_symmetric=true);
	~ImgMatrix();

	bool is_symmetric() const;
	void set_symmetric(bool _l_symmetric);

	cv::Size size() const;

	const Timg& operator() (unsigned int i, unsigned int j) const;
	Timg& operator() (unsigned int i, unsigned int j);

	MatrixOcv2D<ocv::Tvec1> operator() (const cv::Point& _point) const;
	void set(const cv::Point& _point, const MatrixOcv2D<ocv::Tvec1>& _matrix2D);

	void create(const cv::Size& _size);
	void copyTo(ImgMatrix<Timg>& _image_matrix, const cv::Rect _rect = cv::Rect()) const;
	void multiply_self(const Timg& _image);
	void multiply_self(const typename Tvec::value_type& _value);

	void add_self(const ImgMatrix<Timg>& _image_matrix);

	ImgMatrix<Timg>& operator=(const ImgMatrix<Timg>& _image_matrix);

	ImgMatrix<Timg>& operator+=(const ImgMatrix<Timg>& _image_matrix);
	ImgMatrix<Timg>& operator*=(const Timg& _image);
	ImgMatrix<Timg>& operator/=(const Timg& _image);
	ImgMatrix<Timg>& operator*=(const double& _value);
	ImgMatrix<Timg>& operator/=(const double& _value);

	MatrixOcv2D<ocv::Tvec1> sum() const;
	void sum(MatrixOcv2D<ocv::Tvec1>& _result) const;

	void smooth(unsigned int _w_kernel = 3);
	void smooth(const cv::Size& _kernel_size);

	void resize(const cv::Size& _size);
	void release();

	/*! Fill matrix with tensor product of two vectors.*/
	void tensor_product(const ocv::Vec<Timg>& _vec1, const ocv::Vec<Timg>& _vec2);
	/*! Fill matrix with tensor product of same vector.*/
	void tensor_product(const ocv::Vec<Timg>& _vec);
	/*! Fill matrix with tensor product of same vector.*/
	void tensor_product(const Timg& _img1, const Timg& _img2);

	/*! Display each image.*/
	void display() const;
};



template <class Tvec>
ImgMatrix<Timg>::ImgMatrix(bool _l_symmetric) {

	/*! Initialization*/
	l_symmetric = false;
	img12 = new Timg;
	img21 = 0;
	/*! Proper set.*/
	set_symmetric(_l_symmetric);

}

template <class Tvec>
ImgMatrix<Timg>::~ImgMatrix() {
	delete img12;
	if (!l_symmetric) {
		delete img21;
	}
}

template <class Tvec>
bool ImgMatrix<Timg>::is_symmetric() const {

	return l_symmetric;
}

template <class Tvec>
void ImgMatrix<Timg>::set_symmetric(bool _l_symmetric) {

	if (l_symmetric != _l_symmetric) {
		if (_l_symmetric) {
			if (img21) {
				delete img21;
			}
			img21 = img12;
		} else {
			img21 = new Timg;
			img12->copyTo(*img21);
		}
		l_symmetric = _l_symmetric;
	}
	
}

template <class Tvec>
cv::Size ImgMatrix<Timg>::size() const {

	return img11.size();
}

template <class Tvec>
const Timg& ImgMatrix<Timg>::operator() (unsigned int i, unsigned int j) const {

	return (*const_cast<ImgMatrix<Timg>*>(this))(i, j);
}

template <class Tvec>
Timg& ImgMatrix<Timg>::operator() (unsigned int i, unsigned int j) {

	switch (i) {
	case 0:
		switch (j) {
		case 0:
			return img11;
		case 1:
			return *img12;
		default:
			break;
		}
	case 1:
		switch (j) {
		case 0:
			return *img21;
		case 1:
			return img22;
		default:
			break;
		}
	default:
		DEBUG_BREAK;
		return img11;
		break;
	}
}

template <class Tvec>
MatrixOcv2D<ocv::Tvec1> ImgMatrix<Timg>::operator() (const cv::Point& _point) const {

	MatrixOcv2D<ocv::Tvec1> matrix2D;

	matrix2D(0, 0) = (*this)(0, 0)(_point);
	matrix2D(0, 1) = (*this)(0, 1)(_point);
	if (!is_symmetric()) {
		matrix2D(1, 0) = (*this)(1, 0)(_point);
	}
	matrix2D(1, 1) = (*this)(1, 1)(_point);

	return matrix2D;
}

template <class Tvec>
void ImgMatrix<Timg>::set(const cv::Point& _point, const MatrixOcv2D<ocv::Tvec1>& _matrix2D) {

	(*this)(0, 0)(_point) = _matrix2D(0, 0);
	(*this)(0, 1)(_point) = _matrix2D(0, 1);
	if (!is_symmetric()) {
		(*this)(1, 0)(_point) = _matrix2D(1, 0);
	}
	(*this)(1, 1)(_point) = _matrix2D(1, 1);
}

template <class Tvec>
void ImgMatrix<Timg>::create(const cv::Size& _size) {

	(*this)(0, 0).create(_size);
	(*this)(0, 1).create(_size);
	if (!is_symmetric()) {
		(*this)(1, 0).create(_size);
	}
	(*this)(1, 1).create(_size);
}

template <class Tvec>
void ImgMatrix<Timg>::copyTo(ImgMatrix<Timg>& _image_matrix, const cv::Rect _rect) const {

	if (_rect == cv::Rect()) {
		(*this)(0, 0).copyTo(_image_matrix(0, 0));
		(*this)(0, 1).copyTo(_image_matrix(0, 1));
		if (!is_symmetric()) {
			(*this)(1, 0).copyTo(_image_matrix(1, 0));
		}
		(*this)(1, 1).copyTo(_image_matrix(1, 1));
	} else {
		(*this)(0, 0)(_rect).copyTo(_image_matrix(0, 0));
		(*this)(0, 1)(_rect).copyTo(_image_matrix(0, 1));
		if (!is_symmetric()) {
			(*this)(1, 0)(_rect).copyTo(_image_matrix(1, 0));
		}
		(*this)(1, 1)(_rect).copyTo(_image_matrix(1, 1));
	}

}

template <class Tvec>
void ImgMatrix<Timg>::multiply_self(const Timg& _image) {

	cv::multiply((*this)(0, 0), _image, (*this)(0, 0));
	cv::multiply((*this)(0, 1), _image, (*this)(0, 1));
	if (!is_symmetric()) {
		cv::multiply((*this)(1, 0), _image, (*this)(1, 0));
	}
	cv::multiply((*this)(1, 1), _image, (*this)(1, 1));
}

template <class Tvec>
void ImgMatrix<Timg>::multiply_self(const typename Tvec::value_type& _value) {

	cv::MatIterator_<Tvec> it11;// = (*this)(0, 0).begin();
	cv::MatIterator_<Tvec> it12 = (*this)(0, 1).begin();
	cv::MatIterator_<Tvec> it21 = (*this)(1, 0).begin();
	cv::MatIterator_<Tvec> it22 = (*this)(1, 1).begin();
	for (it11 = (*this)(0, 0).begin(); it11 != (*this)(0, 0).end(); ++it11, ++it12, ++it22) {

		ocv::mul((*it11), _value);
		ocv::mul((*it12), _value);
		if (!is_symmetric()) {
			ocv::mul((*it21), _value);
			++it21;
		}
		ocv::mul((*it22), _value);
	}
}

template <class Tvec>
void ImgMatrix<Timg>::add_self(const ImgMatrix<Timg>& _image_matrix) {

	(*this) += _image_matrix;
}

template <class Tvec>
ImgMatrix<Timg>& ImgMatrix<Timg>::operator=(const ImgMatrix<Timg>& _image_matrix) {

	set_symmetric(_image_matrix.is_symmetric());

	img11 = _image_matrix.img11;
	*img12 = *_image_matrix.img12;
	if (!is_symmetric()) {
		*img21 = *_image_matrix.img21;
	}
	img22 = _image_matrix.img22;

	return *this;
}

template <class Tvec>
ImgMatrix<Timg>& ImgMatrix<Timg>::operator+=(const ImgMatrix<Timg>& _image_matrix) {

	if (!_image_matrix.is_symmetric()) {
		set_symmetric(false);
	}

	cv::add((*this)(0, 0), _image_matrix(0, 0), (*this)(0, 0));
	cv::add((*this)(0, 1), _image_matrix(0, 1), (*this)(0, 1));
	if (!is_symmetric()) {
		cv::add((*this)(1, 0), _image_matrix(1, 0), (*this)(1, 0));
	}
	cv::add((*this)(1, 1), _image_matrix(1, 1), (*this)(1, 1));

	return *this;
}

template <class Tvec>
ImgMatrix<Timg>& ImgMatrix<Timg>::operator*=(const Timg& _image) {

	cv::multiply((*this)(0, 0), _image, (*this)(0, 0));
	cv::multiply((*this)(0, 1), _image, (*this)(0, 1));
	if (!is_symmetric()) {
		cv::multiply((*this)(1, 0), _image, (*this)(1, 0));
	}
	cv::multiply((*this)(1, 1), _image, (*this)(1, 1));

	return *this;

}

template <class Tvec>
ImgMatrix<Timg>& ImgMatrix<Timg>::operator/=(const Timg& _image) {

	cv::divide((*this)(0, 0), _image, (*this)(0, 0));
	cv::divide((*this)(0, 1), _image, (*this)(0, 1));
	if (!is_symmetric()) {
		cv::divide((*this)(1, 0), _image, (*this)(1, 0));
	}
	cv::divide((*this)(1, 1), _image, (*this)(1, 1));

	return *this;

}

template <class Tvec>
ImgMatrix<Timg>& ImgMatrix<Timg>::operator*=(const double& _value) {

	(*this)(0, 0) *= _value;
	(*this)(0, 1) *= _value;
	if (!is_symmetric()) {
		(*this)(1, 0) *= _value;
	}
	(*this)(1, 1) *= _value;
}

template <class Tvec>
ImgMatrix<Timg>& ImgMatrix<Timg>::operator/=(const double& _value) {

	(*this)(0, 0) /= _value;
	(*this)(0, 1) /= _value;
	if (!is_symmetric()) {
		(*this)(1, 0) /= _value;
	}
	(*this)(1, 1) /= _value;
}

template <class Tvec>
MatrixOcv2D<ocv::Tvec1> ImgMatrix<Timg>::sum() const {
	MatrixOcv2D<ocv::Tvec1> matrix2D;
	sum(matrix2D);
	return matrix2D;
}

template <class Tvec>
void ImgMatrix<Timg>::sum(MatrixOcv2D<ocv::Tvec1>& _result) const {

	ocv::assign(_result(0, 0), cv::sum((*this)(0, 0)));
	ocv::assign(_result(0, 1), cv::sum((*this)(0, 1)));
	if (!is_symmetric()) {
		ocv::assign(_result(1, 0), cv::sum((*this)(1, 0)));
	}
	ocv::assign(_result(1, 1), cv::sum((*this)(1, 1)));

}

template <class Tvec>
void ImgMatrix<Timg>::smooth(unsigned int _w_kernel) {

	smooth(cv::Size(_w_kernel, _w_kernel));

}

template <class Tvec>
void ImgMatrix<Timg>::smooth(const cv::Size& _kernel_size) {

	cv::GaussianBlur((*this)(0, 0), (*this)(0, 0), _kernel_size, 0, 0);
	cv::GaussianBlur((*this)(0, 1), (*this)(0, 1), _kernel_size, 0, 0);
	if (!is_symmetric()) {
		cv::GaussianBlur((*this)(1, 0), (*this)(1, 0), _kernel_size, 0, 0);
	}
	cv::GaussianBlur((*this)(1, 1), (*this)(1, 1), _kernel_size, 0, 0);

}

template <class Tvec>
void ImgMatrix<Timg>::resize(const cv::Size& _size) {

	cv::resize((*this)(0, 0), (*this)(0, 0), _size);
	cv::resize((*this)(0, 1), (*this)(0, 1), _size);
	if (!is_symmetric()) {
		cv::resize((*this)(1, 0), (*this)(1, 0), _size);
	}
	cv::resize((*this)(1, 1), (*this)(1, 1), _size);

}

template <class Tvec>
void ImgMatrix<Timg>::release() {

	(*this)(0, 0).release();
	(*this)(0, 1).release();
	if (!is_symmetric()) {
		(*this)(1, 0).release();
	}
	(*this)(1, 1).release();

}

template <class Tvec>
void ImgMatrix<Timg>::tensor_product(const ocv::Vec<Timg>& _vec1, const ocv::Vec<Timg>& _vec2) {

	set_symmetric(false);

	cv::multiply(_vec1.first, _vec2.first, (*this)(0, 0));
	cv::multiply(_vec1.first, _vec2.second, (*this)(0, 1));
	cv::multiply(_vec1.second, _vec2.first, (*this)(1, 0));
	cv::multiply(_vec1.second, _vec2.second, (*this)(1, 1));
}

template <class Tvec>
void ImgMatrix<Timg>::tensor_product(const ocv::Vec<Timg>& _vec) {


	tensor_product(_vec.first, _vec.second);
}

template <class Tvec>
void ImgMatrix<Timg>::tensor_product(const Timg& _img1, const Timg& _img2) {

	set_symmetric(true);

	cv::multiply(_img1, _img1, (*this)(0, 0));
	cv::multiply(_img1, _img2, (*this)(0, 1));
	cv::multiply(_img2, _img2, (*this)(1, 1));

}

template <class Tvec>
void ImgMatrix<Timg>::display() const {

	Timg image_display;
	std::string label;
	for (unsigned int dim1 = 0; dim1<2; dim1++) {
		for (unsigned int dim2 = 0; dim2<2; dim2++) {
			label = "[" + Misc::to_string(dim1) + "," + Misc::to_string(dim2) + "]";
			(*this)(dim1, dim2).copyTo(image_display);
			ocv::putText(image_display, label);
			ocv::imshow(image_display, "ImgMatrix", 0, true);
		}
	}
	cv::destroyWindow("ImgMatrix");
}

#undef Timg