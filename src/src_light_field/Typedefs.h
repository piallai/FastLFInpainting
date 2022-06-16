/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include <utility>

typedef std::pair<int, int> Ipair;
typedef std::pair<unsigned int, unsigned int> UIpair;
typedef std::pair<double, double> Fpair;
typedef std::pair<unsigned int, unsigned int> Upair;
typedef std::pair<unsigned int, unsigned int> XYindices;
typedef std::pair<unsigned int, unsigned int> UVindices;

#include <array>
/*! Array of values related to Left, Up, Right, and Bottom edges of a 2D rectangle area.*/
template <class T>
using Leupribo = std::array<T, 4>;