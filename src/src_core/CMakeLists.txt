# This is the Cmake file for Core library
# author : Pierre Allain
# see the accompanying license for info

PROJECT(CORE)

set(CORE_INCLUDE_DIR	
			${Boost_INCLUDE_DIR}
			${UTILS_SOURCE_DIR}
			${OpenCV_INCLUDE_DIRS}
			${OCV_SOURCE_DIR}
			${IMG_TOOLS_SOURCE_DIR}
			${SUPERPIXEL_SOURCE_DIR}
			${LIGHT_FIELD_SOURCE_DIR}
)

INCLUDE_DIRECTORIES(${CORE_INCLUDE_DIR})

SET(CORE_SOURCES

	SubaperturesInpainting.h
	SubaperturesInpainting.cpp
	SubaperturesInpainting_Config.h
	SubaperturesInpainting_Config.cpp

	InpaintingAngular.h
	InpaintingAngular.cpp
	InpaintingAngular_Config.h
	InpaintingAngular_Config.cpp
	
	DisparityFastGradient.h
	DisparityFastGradient.cpp
	DisparityFastGradient_Config.h
	DisparityFastGradient_Config.cpp

	ShiftSubapertures.h
)

ADD_LIBRARY(Core STATIC
	${CORE_SOURCES}
)


SOURCE_GROUP(Headers REGULAR_EXPRESSION "[.]h$")
SOURCE_GROUP(Config\\Headers REGULAR_EXPRESSION "_Config[.]h$")
SOURCE_GROUP(Config\\Sources REGULAR_EXPRESSION "_Config[.]cpp$")
