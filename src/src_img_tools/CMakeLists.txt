# This is the Cmake file for ImgTools library
# author : Pierre Allain
# see the accompanying license for info

PROJECT(IMG_TOOLS)

set(IMG_TOOLS_INCLUDE_DIR	
			${Boost_INCLUDE_DIR}
			${UTILS_SOURCE_DIR}
			${OpenCV_INCLUDE_DIRS}
			${OCV_SOURCE_DIR}
)

INCLUDE_DIRECTORIES(${IMG_TOOLS_INCLUDE_DIR})

ADD_LIBRARY(ImgTools STATIC

	Denoising.h
	Denoising_impl.h
	Denoising.cpp
	Denoising_Config.h
	Denoising_Config.cpp

	HistMatch.h
	HistMatch.cpp

	Contour.h
	Contour.cpp
	
	Inpainting.h
	
	ImgMatrix.h
	ImgEigen.h
	
	MatrixOcv2D.h

)

SOURCE_GROUP(Headers REGULAR_EXPRESSION "[.]h$")
SOURCE_GROUP(Config\\Headers REGULAR_EXPRESSION "_Config[.]h$")
SOURCE_GROUP(Config\\Sources REGULAR_EXPRESSION "_Config[.]cpp$")
