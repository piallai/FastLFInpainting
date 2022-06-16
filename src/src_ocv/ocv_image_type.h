/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

namespace ocv {

	/*! Structure in charge of modifying value type (ex float) of an image.*/
	template <class Timg_, class Tvalue>
	struct ImageTypeValue {
		typedef cv::Mat_< cv::Vec<Tvalue, Timg_::value_type::channels> > Timg;
		typedef cv::Vec<Tvalue, Timg_::value_type::channels> Tvec;
	};

	/*! Structure in charge of modifying number of channels of image while keeping its value type.*/
	template <class Timg_, int Dim>
	struct ImageTypeChannels {
		typedef cv::Mat_< cv::Vec<typename Timg_::value_type::value_type, Dim> > Timg;
	};

	/*! Structure in charge of providing correct number of channels ot obtain a proper colormap Timg. Takes into account potential alpha channel.*/
	template <class Timg>
	struct ColorMapImageType;

	/*! Grayscale will turn to color.*/
	template <class Tvalue>
	struct ColorMapImageType< cv::Mat_< cv::Vec<Tvalue, 1> > > {
		/* Default image type in colormap dimension with same data type.*/
		typedef cv::Mat_< cv::Vec<Tvalue, 3> > Timg;
		static const int channels = 3;
		/* Image type in colormap dimension with template data type. Convenient to call colormap type and just specifying data type as template argument.*/
		template <class _Tvalue>
		using Timg_ = cv::Mat_< cv::Vec<_Tvalue, channels> >;
	};

#ifdef ENABLE_OPACITY_1C
	/*! Grayscale will turn to color. Alpha channel is kept.*/
	template <class Tvalue>
	struct ColorMapImageType< cv::Mat_< cv::Vec<Tvalue, 2> > > {
		typedef cv::Mat_< cv::Vec<Tvalue, 4> > Timg;
		static const int channels = 4;
		/* Image type in colormap dimension with template data type. Convenient to call colormap type and just specifying data type as template argument.*/
		template <class _Tvalue>
		using Timg_ = cv::Mat_< cv::Vec<_Tvalue, channels> >;
	};
#endif

	/*! Color will stay as color.*/
	template <class Tvalue>
	struct ColorMapImageType< cv::Mat_< cv::Vec<Tvalue, 3> > > {
		typedef cv::Mat_< cv::Vec<Tvalue, 3> > Timg;
		static const int channels = 3;
		/* Image type in colormap dimension with template data type. Convenient to call colormap type and just specifying data type as template argument.*/
		template <class _Tvalue>
		using Timg_ = cv::Mat_< cv::Vec<_Tvalue, channels> >;
	};

	/*! Color + alpha will stay as color + alpha.*/
	template <class Tvalue>
	struct ColorMapImageType< cv::Mat_< cv::Vec<Tvalue, 4> > > {
		typedef cv::Mat_< cv::Vec<Tvalue, 4> > Timg;
		static const int channels = 4;
		/* Image type in colormap dimension with template data type. Convenient to call colormap type and just specifying data type as template argument.*/
		template <class _Tvalue>
		using Timg_ = cv::Mat_< cv::Vec<_Tvalue, channels> >;
	};

	/*! Structure to obtain a colormap Timg, and asigning its type with Tvalue_out. Ie : ColorMapImageType + ImageTypeValue*/
	template <class Timg_, class Tvalue_out>
	struct ColorMapImageTypeValue {
		typedef typename ImageTypeValue<typename ColorMapImageType<Timg_>::Timg, Tvalue_out>::Timg Timg;
		typedef typename ImageTypeValue<typename ColorMapImageType<Timg_>::Timg, Tvalue_out>::Tvec Tvec;
	};
}