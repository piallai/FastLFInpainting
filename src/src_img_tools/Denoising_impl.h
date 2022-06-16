/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "ocv_convert.h"
#include <opencv2/photo.hpp>

template <class Tvalue, int Dim>
void Denoising::denoise(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _input, cv::Mat_< cv::Vec<Tvalue, Dim> >& _output, Buffer<Dim>& _buffer) {

	if (Misc::l_verbose_low) std::cout << "Denoising" << std::endl;

	if (parameters.denoising_type == DenoisingType::TVL1 || parameters.denoising_type == DenoisingType::NLMeans || parameters.denoising_type == DenoisingType::NLMeansColored) {

		if (Dim == 3 || parameters.denoising_type != DenoisingType::NLMeansColored) {

			/*! Convert input image to 8bits.*/
			ocv::Tscaling scaling = ocv::convertTo(_input, _buffer.image_converted, true, false);

			if (parameters.denoising_type == DenoisingType::TVL1) {

				if (parameters.Niterations > 0) {

					if (Dim > 1) {
						cv::split(_buffer.image_converted, _buffer.splits_input);
						_buffer.splits_output.resize(Dim);
						for (unsigned int d = 0; d < Dim; d++) {
							/*! Insert image_converted in an image vector (argument needed by cv::denoise_TVl1).*/
							std::vector<cv::Mat> images_input(1, _buffer.splits_input[d]);
							/*! Denoise image.*/
							cv::denoise_TVL1(images_input, _buffer.splits_output[d], parameters.lambda, parameters.Niterations);
						}
						cv::merge(_buffer.splits_output, _buffer.image_denoised);
					} else {
						/*!Insert image_converted in an image vector(argument needed by cv::denoise_TVl1).*/
						std::vector<cv::Mat> images_input(1, _buffer.image_converted);
						/*! Denoise image.*/
						cv::denoise_TVL1(images_input, _buffer.image_denoised, parameters.lambda, parameters.Niterations);
					}

				} else {
					_buffer.image_converted.copyTo(_buffer.image_denoised);
				}

			} else if (parameters.denoising_type == DenoisingType::NLMeans) {

				cv::fastNlMeansDenoising(_buffer.image_converted, _buffer.image_denoised, parameters.h, parameters.templateWindowSize, parameters.searchWindowSize);

			} else if (parameters.denoising_type == DenoisingType::NLMeansColored) {

				cv::fastNlMeansDenoisingColored(_buffer.image_converted, _buffer.image_denoised, parameters.h, parameters.hColor, parameters.templateWindowSize, parameters.searchWindowSize);
			}

			/*! Convert back to original type.*/
			ocv::convertTo(_buffer.image_denoised, _output, false);
			/*! Apply scaling produced by 8bits conversion.*/
			ocv::rescale_inv(_output, scaling);

		} else {
			std::cout << "Denoising::denoise : Can't apply color method" << std::endl;
		}

	} else if (parameters.denoising_type == DenoisingType::NLMeansMulti || parameters.denoising_type == DenoisingType::NLMeansColoredMulti) {

		std::cout << "Denoising::denoise : Can't apply multi method using a single image" << std::endl;

	} else {

		std::cout << "Denoising::denoise : Invalid denoising type" << std::endl;
	}

}


template <class Tvalue, int Dim>
void Denoising::denoise(const std::vector< cv::Mat_< cv::Vec<Tvalue, Dim> > >& _inputs, cv::Mat_< cv::Vec<Tvalue, Dim> >& _output, Buffer<Dim>& _buffer) {

	if (Misc::l_verbose_low) std::cout << "Denoising" << std::endl;

	if (parameters.denoising_type == DenoisingType::TVL1) {


		/*! Convert input image to 8bits.*/
		_buffer.images_converted.resize(_inputs.size());
		std::vector<ocv::Tscaling> scalings(_inputs.size());
		ocv::Tscaling scaling_mean(0, 0);
		for (unsigned int i = 0; i < _inputs.size(); i++) {
			scalings[i] = ocv::convertTo(_inputs[i], _buffer.images_converted[i], true);
			scaling_mean.first += scalings[i].first;
			scaling_mean.second += scalings[i].second;
		}
		scaling_mean.first /= _inputs.size();
		scaling_mean.second /= _inputs.size();



		if (parameters.Niterations > 0) {

			if (Dim > 1) {

				_buffer.splits_vector_input.resize(Dim);
				for (unsigned int d = 0; d < Dim; d++) {
					_buffer.splits_vector_input.resize(_inputs.size());
				}

				for (unsigned int i = 0; i < _inputs.size(); i++) {
					cv::split(_buffer.images_converted[i], _buffer.splits_input);
					for (unsigned int d = 0; d < Dim; d++) {
						_buffer.splits_vector_input[d][i] = _buffer.splits_input[d];
					}
				}

				cv::split(_buffer.image_converted, _buffer.splits_input);
				_buffer.splits_output.resize(Dim);
				for (unsigned int d = 0; d < Dim; d++) {
					/*! Insert image_converted in an image vector (argument needed by cv::denoise_TVl1).*/
					std::vector<cv::Mat> images_input(_buffer.splits_vector_input[d]);
					/*! Denoise image.*/
					cv::denoise_TVL1(images_input, _buffer.splits_output[d], parameters.lambda, parameters.Niterations);
				}
				cv::merge(_buffer.splits_output, _buffer.image_denoised);

			} else {


				/*!Insert image_converted in an image vector(argument needed by cv::denoise_TVl1).*/
				std::vector<cv::Mat> images_input(_buffer.images_converted);
				/*! Denoise image.*/
				cv::denoise_TVL1(images_input, _buffer.image_denoised, parameters.lambda, parameters.Niterations);
			}

		} else {
			if (_inputs.size() > 0) {
				_buffer.images_converted[0].copyTo(_buffer.image_denoised);
			}
			std::cout << "Denoising::denoise : No iterations for TVL1" << std::endl;
		}



		/*! Convert back to original type.*/
		ocv::convertTo(_buffer.image_denoised, _output, false);
		/*! Apply scaling produced by 8bits conversion.*/
		ocv::rescale_inv(_output, scaling_mean);

	} else {
		std::cout << "Denoising::denoise : method only applies to TVL1 denoising" << std::endl;
	}

}

template <class Tvalue, int Dim>
void Denoising::denoise(const std::vector< cv::Mat_< cv::Vec<Tvalue, Dim> > >& _inputs, const unsigned int _image_index, cv::Mat_< cv::Vec<Tvalue, Dim> >& _output, Buffer<Dim>& _buffer) {

	if (Misc::l_verbose_low) std::cout << "Denoising" << std::endl;


	if (Dim == 3 || (parameters.denoising_type != DenoisingType::NLMeansColored && parameters.denoising_type != DenoisingType::NLMeansColoredMulti)) {

		if (parameters.denoising_type != DenoisingType::NLMeansMulti && parameters.denoising_type != DenoisingType::NLMeansColoredMulti) {

			denoise(_inputs[_image_index], _output, _buffer);

		} else {


			/*! Convert input image to 8bits.*/
			_buffer.images_converted.resize(_inputs.size());
			std::vector<ocv::Tscaling> scalings(_inputs.size());
			for (unsigned int i = 0; i < _inputs.size(); i++) {
				scalings[i] = ocv::convertTo(_inputs[i], _buffer.images_converted[i], true);
			}

			if (parameters.denoising_type == DenoisingType::NLMeans) {

				cv::fastNlMeansDenoisingMulti(_buffer.images_converted, _buffer.image_denoised, _image_index, parameters.temporalWindowSize, parameters.h, parameters.templateWindowSize, parameters.searchWindowSize);

			} else if (parameters.denoising_type == DenoisingType::NLMeansColored) {

				cv::fastNlMeansDenoisingColoredMulti(_buffer.image_converted, _buffer.image_denoised, _image_index, parameters.temporalWindowSize, parameters.h, parameters.hColor, parameters.templateWindowSize, parameters.searchWindowSize);
			}

			/*! Convert back to original type.*/
			ocv::convertTo(_buffer.image_denoised, _output, false);
			/*! Apply scaling produced by 8bits conversion.*/
			ocv::rescale_inv(_output, scalings[_image_index]);

		}


	} else {
		std::cout << "Denoising::denoise : Can't apply color method" << std::endl;
	}

	

}

template <class Tvalue, int Dim>
void Denoising::denoise(const std::vector< cv::Mat_< cv::Vec<Tvalue, Dim> > >& _inputs, std::vector< cv::Mat_< cv::Vec<Tvalue, Dim> > >& _outputs, Buffer<Dim>& _buffer) {

	if (_inputs.size() == _outputs.size()) {

		for (unsigned int i = 0; i < _inputs.size(); i++) {
			denoise(_inputs, i, _outputs[i], _buffer);
		}

	} else {
		std::cout << "Denoising::denoise _inputs and _outputs have different sizes" << std::endl;
	}

}


/*! Static case for direct use without Parameter structure.*/

template <class Tvalue, int Dim>
void Denoising::denoiseTVL1(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _input, cv::Mat_< cv::Vec<Tvalue, Dim> >& _output, const double _lambda, const unsigned int _Niterations) {

	Buffer<Dim> buffer;
	denoiseTVL1(_input, _output, buffer, _lambda, _Niterations);
}

template <class Tvalue, int Dim>
void Denoising::denoiseTVL1(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _input, cv::Mat_< cv::Vec<Tvalue, Dim> >& _output, Buffer<Dim>& _buffer, const double _lambda, const unsigned int _Niterations) {

	Parameters parameters;
	parameters.denoising_type = DenoisingType::TVL1;
	parameters.lambda = _lambda;
	parameters.Niterations = _Niterations;

	Denoising denoising;
	denoising.set_parameters(parameters);

	denoising.denoise(_input, _output, _buffer);

}