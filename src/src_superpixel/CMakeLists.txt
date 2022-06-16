# This is the Cmake file for SuperPixel library
# author : Pierre Allain
# see the accompanying license for info

PROJECT(SUPERPIXEL)

set(SUPERPIXEL_INCLUDE_DIR	
			${Boost_INCLUDE_DIR}
			${UTILS_SOURCE_DIR}
			${OpenCV_INCLUDE_DIRS}
			${OCV_SOURCE_DIR}
			${IMG_TOOLS_SOURCE_DIR}
)

INCLUDE_DIRECTORIES(${SUPERPIXEL_INCLUDE_DIR})

ADD_LIBRARY(SuperPixel STATIC
	
	SuperPixelSegmentation.h
	SuperPixelSegmentation.cpp
	SuperPixelSegmentation_Config.h
	SuperPixelSegmentation_Config.cpp

	SpsMaskMerge.h
	SpsMaskMerge.cpp
	SpsMaskMerge_Config.h
	SpsMaskMerge_Config.cpp
	
	SpsInterpolation.h
	SpsInterpolation.cpp
	SpsInterpolation_Config.h
	SpsInterpolation_Config.cpp
)

SOURCE_GROUP(Headers REGULAR_EXPRESSION "[.]h$")
SOURCE_GROUP(Config\\Headers REGULAR_EXPRESSION "_Config[.]h$")
SOURCE_GROUP(Config\\Sources REGULAR_EXPRESSION "_Config[.]cpp$")
