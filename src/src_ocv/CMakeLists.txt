# This is the Cmake file for OCV library
# author : Pierre Allain
# see the accompanying license for info

PROJECT(OCV)

set(OCV_INCLUDE_DIR	
			${Boost_INCLUDE_DIR}
			${UTILS_SOURCE_DIR}
			${OpenCV_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(${OCV_INCLUDE_DIR})

ADD_LIBRARY(OCV STATIC

	ocv.h
	ocv.cpp
	ocv_onMouse_spec.h
	ocv_image_type.h
	ocv_convert.h
	ocv_minmax.h
	ocv_minmax.cpp
	ocv_rw.h
	ocv_rw.cpp
	ocv_utils.h 
	ocv_derivative.h
	ocv_derivative.cpp
)

SOURCE_GROUP(Headers REGULAR_EXPRESSION "[.]h$")
