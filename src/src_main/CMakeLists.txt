# This is the Cmake file for main project of CelfExec
# author : Pierre Allain
# see the accompanying license for info

PROJECT(FastLFInpainting)

set(${PROJECT_NAME}_PROJECT_SRCS 
	main.cpp
	Master.h
	Master.cpp
	Master_Config.h
	Master_Config.cpp
	version.h
	SubaperturesData_inst.cpp
)

SOURCE_GROUP(Headers REGULAR_EXPRESSION "[.]h$")
SOURCE_GROUP(Config\\Headers REGULAR_EXPRESSION "_Config[.]h$")
SOURCE_GROUP(Config\\Sources REGULAR_EXPRESSION "_Config[.]cpp$")

set(FAST_INPAINTING_INCLUDE_DIR	
			${Boost_INCLUDE_DIR}
			${UTILS_SOURCE_DIR}
			${OpenCV_INCLUDE_DIRS}
			${OCV_SOURCE_DIR}
			${IMG_TOOLS_SOURCE_DIR}
			${SUPERPIXEL_SOURCE_DIR}
			${LIGHT_FIELD_SOURCE_DIR}
			${CORE_SOURCE_DIR}
)

INCLUDE_DIRECTORIES(${FAST_INPAINTING_INCLUDE_DIR})


ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME}_PROJECT_SRCS})
IF(MSVC)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
ENDIF(MSVC)

set(FAST_INPAINTING_LINK_LIBRARIES
	Core
	SuperPixel
	LightField
	ImgTools
	OCV
	${OpenCV_LIBS} 
	Utils
	${Boost_LIBRARIES}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${FAST_INPAINTING_LINK_LIBRARIES})
