/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

#include "ocv.h"
/*! https://docs.opencv.org/2.4/modules/photo/doc/denoising.html */

class Denoising {

public :

	static const std::string directory_name();

	enum DenoisingType { TVL1, NLMeans, NLMeansColored, NLMeansMulti, NLMeansColoredMulti};

	struct Parameters {
		/*! Type of image denoising.*/
		DenoisingType denoising_type = DenoisingType::TVL1;
		/*! TVL1 : Lambda for denoising.*/
		double lambda = 0.5;
		/*! TVL1 : Number of iterations for denoising.*/
		unsigned int Niterations = 10;
		/*! All NLMeans : Parameter regulating filter strength. Big h value perfectly removes noise but also removes image details, smaller h value preserves details but also preserves some noise*/
		float h;
		/*! All NLMeans : Size in pixels of the template patch that is used to compute weights. Should be odd. Recommended value 7 pixels.*/
		unsigned int templateWindowSize = 7;
		/*! All NLMeans : Size in pixels of the window that is used to compute weighted average for given pixel. Should be odd. Affect performance linearly: greater searchWindowsSize - greater denoising time. Recommended value 21 pixels.*/
		int searchWindowSize = 21;
		/*! Colored NLMeans : The same as h but for color components. For most images value equals 10 will be enough to remove colored noise and do not distort colors*/
		float hColor = 10;
		/*! Multi NLMeans : Target image to denoise index in srcImgs sequence.*/
		//int imgToDenoiseIndex;
		/*! Multi NLMeans : Number of surrounding images to use for target image denoising. Should be odd. Images from imgToDenoiseIndex - temporalWindowSize / 2 to imgToDenoiseIndex - temporalWindowSize / 2 from srcImgs will be used to denoise srcImgs[imgToDenoiseIndex] image.*/
		int temporalWindowSize = 3;
	};



	template <int Dim>
	struct Buffer {
		cv::Mat_< cv::Vec<uchar, Dim> > image_converted;
		cv::Mat_< cv::Vec<uchar, Dim> > image_denoised;
		std::vector< cv::Mat_< cv::Vec<uchar, 1> > > splits_input;
		std::vector< cv::Mat_< cv::Vec<uchar, 1> > > splits_output;
		std::vector< cv::Mat_< cv::Vec<uchar, Dim> > > images_converted;
		std::vector< cv::Mat_< cv::Vec<uchar, Dim> > > images_denoised;
		std::vector< std::vector< cv::Mat_< cv::Vec<uchar, 1> > > > splits_vector_input;
		std::vector< std::vector< cv::Mat_< cv::Vec<uchar, 1> > > > splits_vector_output;
	};

private:

	Parameters parameters;

public:
	Denoising();
	Denoising(const Parameters& _parameters);
	~Denoising();

	void set_parameters(const Parameters& _parameters);
	const Parameters& get_parameters() const;

	std::string get_method_name() const;

	//*! Denoise an image. Algorithms need 8bits images and can generate loss.*/
	template <class Tvalue, int Dim>
	void denoise(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _input, cv::Mat_< cv::Vec<Tvalue, Dim> >& _output, Buffer<Dim>& _buffer);
	/*! Denoise vector of images to obtain a denoised single one.*/
	template <class Tvalue, int Dim>
	void denoise(const std::vector< cv::Mat_< cv::Vec<Tvalue, Dim> > >& _inputs, cv::Mat_< cv::Vec<Tvalue, Dim> >& _output, Buffer<Dim>& _buffer);
	/*! Denoise vector of images to obtain a denoised single one.*/
	template <class Tvalue, int Dim>
	void denoise(const std::vector< cv::Mat_< cv::Vec<Tvalue, Dim> > >& _inputs, const unsigned int _image_index, cv::Mat_< cv::Vec<Tvalue, Dim> >& _output, Buffer<Dim>& _buffer);
	/*! Denoise vector of images to obtain a denoised vector of images*/
	template <class Tvalue, int Dim>
	void denoise(const std::vector< cv::Mat_< cv::Vec<Tvalue, Dim> > >& _inputs, std::vector< cv::Mat_< cv::Vec<Tvalue, Dim> > >& _outputs, Buffer<Dim>& _buffer);

	/*! Static case for direct use without Parameter structure.*/
	template <class Tvalue, int Dim>
	static void denoiseTVL1(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _input, cv::Mat_< cv::Vec<Tvalue, Dim> >& _output, const double _lambda = 0.5, const unsigned int _Niterations = 100);
	template <class Tvalue, int Dim>
	static void denoiseTVL1(const cv::Mat_< cv::Vec<Tvalue, Dim> >& _input, cv::Mat_< cv::Vec<Tvalue, Dim> >& _output, Buffer<Dim>& _buffer, const double _lambda=0.5, const unsigned int _Niterations=100);

};

#include "Denoising_impl.h"
