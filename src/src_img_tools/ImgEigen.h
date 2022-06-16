/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "ImgMatrix.h"

template <class Timg>
class ImgEigen {

	Timg eigen_value1;
	Timg eigen_value2;
	ocv::Vec<Timg> eigen_vector1;
	ocv::Vec<Timg> eigen_vector2;
	Timg coherence;
	Timg confidence;

public :

	typedef typename Timg::value_type value_type;

	struct Buffer {
		Timg buff1,buff2,delta,norm,epsilon;
		/*! Buffers for static methods. For example, only eigen_vector2 can be asked as output.*/
		Timg eigen_value1, eigen_value2;
		ocv::Vec<Timg> eigen_vector1;
		ocv::Vec<Timg> eigen_vector2;
		void release() {
			buff1.release();
			buff2.release();
			delta.release();
			norm.release();
			epsilon.release();
			eigen_value1.release(); eigen_value2.release();
			eigen_vector1.first.release(); eigen_vector1.second.release();
			eigen_vector2.first.release(); eigen_vector2.second.release();
		}
	};

public:

	ImgEigen();
	ImgEigen(const ImgMatrix<Timg>& _matrix);
	~ImgEigen();

	cv::Size size() const;

	void set(const ImgMatrix<Timg>& _matrix, bool _l_switch_vector_computing=false);
	void set(const ImgMatrix<Timg>& _matrix, bool _l_switch_vector_computing, Buffer& _buffer);
	void set(const ImgMatrix<Timg>& _matrix, Buffer& _buffer);

	const Timg& get_eigen_value1() const;
	const Timg& get_eigen_value2() const;

	const ocv::Vec<Timg>& get_eigen_vector1() const;
	const ocv::Vec<Timg>& get_eigen_vector2() const;

	const Timg& get_coherence() const;
	const Timg& get_confidence() const;


	static void get_eigenvalues(const ImgMatrix<Timg>& _matrix, Timg& _eigen_value1, Timg& _eigen_value2, Buffer& _buffer);
	static void get_eigenvectors(const ImgMatrix<Timg>& _matrix, const Timg& _eigen_value1, const Timg& _eigen_value2, ocv::Vec<Timg>& _eigen_vector1, ocv::Vec<Timg>& _eigen_vector2, Buffer& _buffer, bool _l_switch_vector_computing = false);
	/*! To obtain only eigenvector of second eigenvalue along with coherence.
	Convenient to save memory and some computations.*/
	static void get_eigenvector2_coherence(const ImgMatrix<Timg>& _matrix, ocv::Vec<Timg>& _eigen_vector2, Timg& coherence, Buffer& _buffer);
	static void get_eigenvector2_coherence_confidence(const ImgMatrix<Timg>& _matrix, ocv::Vec<Timg>& _eigen_vector2, Timg& coherence, Timg& _confidence, Buffer& _buffer);
};



template <class Timg>
ImgEigen<Timg>::ImgEigen() {

}

template <class Timg>
ImgEigen<Timg>::ImgEigen(const ImgMatrix<Timg>& _matrix) {

	set(_matrix);
}

template <class Timg>
ImgEigen<Timg>::~ImgEigen() {

}

template <class Timg>
cv::Size ImgEigen<Timg>::size() const {

	return eigen_value1.size();
}

template <class Timg>
const Timg& ImgEigen<Timg>::get_eigen_value1() const {

	return eigen_value1;
}

template <class Timg>
const Timg& ImgEigen<Timg>::get_eigen_value2() const {

	return eigen_value2;
}

template <class Timg>
const ocv::Vec<Timg>& ImgEigen<Timg>::get_eigen_vector1() const {

	return eigen_vector1;
}

template <class Timg>
const ocv::Vec<Timg>& ImgEigen<Timg>::get_eigen_vector2() const {

	return eigen_vector2;
}

template <class Timg>
const Timg& ImgEigen<Timg>::get_coherence() const {

	return coherence;
}

template <class Timg>
const Timg& ImgEigen<Timg>::get_confidence() const {

	return confidence;
}

template <class Timg>
void ImgEigen<Timg>::set(const ImgMatrix<Timg>& _matrix, bool _l_switch_vector_computing) {

	Buffer buffer;
	set(_matrix, _l_switch_vector_computing, buffer);

}

template <class Timg>
void ImgEigen<Timg>::set(const ImgMatrix<Timg>& _matrix, Buffer& _buffer) {

	set(_matrix, false, _buffer);
}

template <class Timg>
void ImgEigen<Timg>::set(const ImgMatrix<Timg>& _matrix, bool _l_switch_vector_computing, Buffer& _buffer) {

	get_eigenvalues(_matrix, eigen_value1, eigen_value2, _buffer);
	get_eigenvectors(_matrix, eigen_value1, eigen_value2, eigen_vector1, eigen_vector2, _buffer, _l_switch_vector_computing);

	//coherence and confidence
	
	cv::subtract(eigen_value1, eigen_value2, _buffer.buff1);
	cv::add(eigen_value1, eigen_value2, confidence);
	cv::divide(_buffer.buff1, confidence, coherence);
	coherence.mul(coherence);

}

template <class Timg>
void ImgEigen<Timg>::get_eigenvalues(const ImgMatrix<Timg>& _matrix, Timg& _eigen_value1, Timg& _eigen_value2, Buffer& _buffer) {

	//http://people.csail.mit.edu/bkph/articles/Eigenvectors.pdf

	cv::subtract(_matrix(0, 0), _matrix(1, 1), _buffer.buff1);
	cv::multiply(_buffer.buff1, _buffer.buff1, _buffer.buff1);

	cv::multiply(_matrix(0, 1), _matrix(1, 0), _buffer.buff2);
	_buffer.buff2 *= 4.;

	cv::add(_buffer.buff1, _buffer.buff2, _buffer.delta);
	cv::sqrt(_buffer.delta, _buffer.delta);
	_buffer.delta *= 0.5;

	cv::add(_matrix(0, 0), _matrix(1, 1), _buffer.buff1);
	_buffer.buff1 *= 0.5;

	cv::add(_buffer.buff1, _buffer.delta, _eigen_value1);
	cv::subtract(_buffer.buff1, _buffer.delta, _eigen_value2);

}

template <class Timg>
void ImgEigen<Timg>::get_eigenvectors(const ImgMatrix<Timg>& _matrix, const Timg& _eigen_value1, const Timg& _eigen_value2, ocv::Vec<Timg>& _eigen_vector1, ocv::Vec<Timg>& _eigen_vector2, Buffer& _buffer, bool _l_switch_vector_computing) {

	cv::add(_matrix(0, 1), _matrix(1, 0), _buffer.buff2);
	_buffer.buff2 *= -0.5;

	/*! Eigen vector computation has two possible expressions. In case matrix(0, 0) >>> matrix(1, 1) (or oppositely), it is sometimes convenient to compute these vectors using the other way to avoid noisy results.*/
	if (!_l_switch_vector_computing) {
		_buffer.buff2.copyTo(_eigen_vector1.first);
		_buffer.buff2.copyTo(_eigen_vector2.first);

		cv::subtract(_matrix(0, 0), _eigen_value1, _eigen_vector1.second);
		cv::subtract(_matrix(0, 0), _eigen_value2, _eigen_vector2.second);
	} else {
		_buffer.buff2.copyTo(_eigen_vector1.second);
		_buffer.buff2.copyTo(_eigen_vector2.second);

		cv::subtract(_matrix(1, 1), _eigen_value1, _eigen_vector1.first);
		cv::subtract(_matrix(1, 1), _eigen_value2, _eigen_vector2.first);
	}

}

template <class Timg>
void ImgEigen<Timg>::get_eigenvector2_coherence(const ImgMatrix<Timg>& _matrix, ocv::Vec<Timg>& _eigen_vector2, Timg& _coherence, Buffer& _buffer) {

	get_eigenvector2_coherence_confidence(_matrix, _eigen_vector2, _coherence, _buffer.buff2, _buffer);

}

template <class Timg>
void ImgEigen<Timg>::get_eigenvector2_coherence_confidence(const ImgMatrix<Timg>& _matrix, ocv::Vec<Timg>& _eigen_vector2, Timg& _coherence, Timg& _confidence, Buffer& _buffer) {


	get_eigenvalues(_matrix, _buffer.eigen_value1, _buffer.eigen_value2, _buffer);

	get_eigenvectors(_matrix, _buffer.eigen_value1, _buffer.eigen_value2, _buffer.eigen_vector1, _eigen_vector2, _buffer);

	ocv::Vec<Timg> vector_buffer(_buffer.buff1, _buffer.buff2);
	ocv::normalize(_eigen_vector2, vector_buffer);

	//coherence and confidence

	cv::subtract(_buffer.eigen_value1, _buffer.eigen_value2, _buffer.buff1);
	cv::add(_buffer.eigen_value1, _buffer.eigen_value2, _confidence);
	cv::divide(_buffer.buff1, _confidence, _coherence);
	_coherence.mul(_coherence);

}