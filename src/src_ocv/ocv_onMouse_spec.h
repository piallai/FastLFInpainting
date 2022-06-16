/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

namespace ocv {
	/*! Used by ocv::imshow. Private methods.*/
	template <class Tvalue>
	struct onMouseDim_struct {
		static void display(const Tvalue& _value) {
			std::cout << "I = " << onMouseVal_struct<Tvalue>::convert(_value) << ", value = " << onMouseVal_struct<Tvalue>::direct(_value);
		}
	};
#define Tvec cv::Vec<Tvalue, 1>
	template <class Tvalue>
	struct onMouseDim_struct<Tvec> {
		static void display(const Tvec& _vec) {
			std::cout << "I = " << onMouseVal_struct<Tvalue>::convert(_vec[0]) << ", value = " << onMouseVal_struct<Tvalue>::direct(_vec[0]);
		}
	};
#undef Tvec
#define Tvec cv::Vec<Tvalue, 2>
	template <class Tvalue>
	struct onMouseDim_struct<Tvec> {
		static void display(const Tvec& _vec) {
			std::cout << "I1 = " << onMouseVal_struct<Tvalue>::convert(_vec[0]);
			std::cout << ", I2 = " << onMouseVal_struct<Tvalue>::convert(_vec[1]);
		}
	};
#undef Tvec
#define Tvec cv::Vec<Tvalue, 3>
	template <class Tvalue>
	struct onMouseDim_struct<Tvec> {
		static void display(const Tvec& _vec) {
			std::cout << "B = " << onMouseVal_struct<Tvalue>::convert(_vec[0]);
			std::cout << ", G = " << onMouseVal_struct<Tvalue>::convert(_vec[1]);
			std::cout << ", R = " << onMouseVal_struct<Tvalue>::convert(_vec[2]);
		}
	};
#undef Tvec
#define Tvec cv::Vec<Tvalue, 4>
	template <class Tvalue>
	struct onMouseDim_struct<Tvec> {
		static void display(const Tvec& _vec) {
			std::cout << "B = " << onMouseVal_struct<Tvalue>::convert(_vec[0]);
			std::cout << ", G = " << onMouseVal_struct<Tvalue>::convert(_vec[1]);
			std::cout << ", R = " << onMouseVal_struct<Tvalue>::convert(_vec[2]);
			std::cout << ", A = " << onMouseVal_struct<Tvalue>::convert(_vec[3]);
		}
	};
#undef Tvec

	template <class Tvalue>
	struct onMouseVal_struct {
		static Tvalue convert(const Tvalue& _value) {
			return _value;
		}
		static Tvalue direct(const Tvalue& _value) {
			return _value;
		}
	};

#define Tvalue uchar
	template <>
	struct onMouseVal_struct<Tvalue> {
		static unsigned int convert(const Tvalue& _value) {
			return _value;
		}
		static unsigned int direct(const Tvalue& _value) {
			return _value;
		}
	};
#undef Tvalue
#define Tvalue float
	template <>
	struct onMouseVal_struct<Tvalue> {
		static Tvalue convert(const Tvalue& _value) {
			Tvalue result = _value;
			//result *= 255.;
			return result;
		}
		static Tvalue direct(const Tvalue& _value) {
			return _value;
		}
	};
#undef Tvalue
#define Tvalue double
	template <>
	struct onMouseVal_struct<Tvalue> {
		static Tvalue convert(const Tvalue& _value) {
			Tvalue result = _value;
			return result;
			//return _value*255.;
		}
		static Tvalue direct(const Tvalue& _value) {
			return _value;
		}
	};
#undef Tvalue
}