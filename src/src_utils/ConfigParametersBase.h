/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/


#pragma once

#include "ConfigBase.h"

/*! Template class for containing parameters. Using a different class along with ConfigBase allows the last to remain non-template.*/
template <class Tparameters>
class ConfigParametersBase : public ConfigBase {

protected :

	Tparameters parameters;

public :

	ConfigParametersBase() {}
	~ConfigParametersBase() {}

	const Tparameters& get_parameters() const {
		return parameters;
	}
};