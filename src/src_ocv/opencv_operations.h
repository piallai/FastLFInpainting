/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include <opencv2/core.hpp>

/*! Following overloads are related to the ones provided in opencv2/core/operations.hpp when using macro CV_MAT_AUG_OPERATOR_TN.*/
/*! The following modifications are needed for opencv version after April 8 2019.*/

/*! For opencv 3, versions above 3.4.5 have the problematic modifications.*/
#if CV_MAJOR_VERSION == 3 && ((CV_MINOR_VERSION == 4 && CV_SUBMINOR_VERSION > 5) || CV_MINOR_VERSION > 4)
#define CV_OVERLOAD_PROBLEM
#endif

/*! For opencv 4, versions above 4.1.0 have the problematic modifications.*/
#if CV_MAJOR_VERSION == 4 && CV_MINOR_VERSION > 0
#define CV_OVERLOAD_PROBLEM
#endif

#ifdef CV_OVERLOAD_PROBLEM

#define CV_MAT_AUG_OPERATOR1(op, cvop, A, B) \
    static inline A& operator op (A& a, const B& b) {cvop; return a; }

#define CV_MAT_AUG_OPERATOR(op, cvop, A, B)   \
    CV_MAT_AUG_OPERATOR1(op, cvop, A, B)      \
    CV_MAT_AUG_OPERATOR1(op, cvop, const A, B)

#define CV_MAT_AUG_OPERATOR_T(op, cvop, A, B)                   \
    template<typename _Tp> CV_MAT_AUG_OPERATOR1(op, cvop, A, B) \
    template<typename _Tp> CV_MAT_AUG_OPERATOR1(op, cvop, const A, B)

CV_MAT_AUG_OPERATOR_T(+=, cv::add(a, cv::Scalar(b), a), cv::Mat_<_Tp>, _Tp)
CV_MAT_AUG_OPERATOR_T(-=, cv::subtract(a, cv::Scalar(b), a), cv::Mat_<_Tp>, _Tp)
CV_MAT_AUG_OPERATOR_T(&=, cv::bitwise_and(a, cv::Scalar(b), a), cv::Mat_<_Tp>, _Tp)
CV_MAT_AUG_OPERATOR_T(|=, cv::bitwise_or(a, cv::Scalar(b), a), cv::Mat_<_Tp>, _Tp)
CV_MAT_AUG_OPERATOR_T(^=, cv::bitwise_xor(a, cv::Scalar(b), a), cv::Mat_<_Tp>, _Tp)

#undef CV_MAT_AUG_OPERATOR_T
#undef CV_MAT_AUG_OPERATOR
#undef CV_MAT_AUG_OPERATOR1

#endif

#undef CV_OVERLOAD_PROBLEM