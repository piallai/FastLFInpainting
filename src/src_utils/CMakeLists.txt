# This is the Cmake file for Utils library
# author : Pierre Allain
# see the accompanying license for info

PROJECT(UTILS)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_LIBRARY(Utils STATIC
	misc_funcs.h
	misc_funcs.cpp
	Macros.h
	
	ConfigBase.h
	ConfigBase.cpp
	ConfigParametersBase.h
	ConfigParameters.h
	ConfigParameter.h
	ConfigReader.h
	
	ConfigParametersSpecializations.h
)

SOURCE_GROUP(Headers REGULAR_EXPRESSION "[.]h$")