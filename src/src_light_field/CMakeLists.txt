# This is the Cmake file for LightField library
# author : Pierre Allain
# see the accompanying license for info

PROJECT(LIGHT_FIELD)

set(LIGHT_FIELD_INCLUDE_DIR	
			${Boost_INCLUDE_DIR}
			${UTILS_SOURCE_DIR}
			${OpenCV_INCLUDE_DIRS}
			${OCV_SOURCE_DIR}
			${IMG_TOOLS_SOURCE_DIR}
)

INCLUDE_DIRECTORIES(${LIGHT_FIELD_INCLUDE_DIR})

SET(LIGHT_FIELD_SOURCES

	Typedefs.h
	DirectoryParser.h
	DirectoryParser.cpp
	SubapertureBundle.h
	SubapertureBundle.cpp
	SubapertureBundle_Config.h
	SubapertureBundle_Config.cpp
	
	SubaperturesDataBase.h
	SubaperturesDataBase.cpp

	SubaperturesData.h
	SubaperturesData_impl.h
	SubaperturesData_inst.cpp
	SubaperturesData_ocv.h
	
	SubaperturesLoader.h
	SubaperturesLoader.cpp
	SubaperturesLoader_Config.h
	SubaperturesLoader_Config.cpp

	Images4D_base.h
	Images4D_base_impl.h
	Images4D_base.cpp
	Images4D.h
	Images4D_impl.h

)

ADD_LIBRARY(LightField STATIC
	${LIGHT_FIELD_SOURCES}
)


SOURCE_GROUP(Headers REGULAR_EXPRESSION "[.]h$")
SOURCE_GROUP(Config\\Headers REGULAR_EXPRESSION "_Config[.]h$")
SOURCE_GROUP(Config\\Sources REGULAR_EXPRESSION "_Config[.]cpp$")
