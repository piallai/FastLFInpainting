/******************************************************************/
/***                Fast Inpainting, INRIA license				  */
/******************************************************************/

#include "Master.h"

Master::Master() {}

Master::~Master() {}

void Master::set_parameters(const Parameters& _parameters) {

	parameters = _parameters;
}

const Master::Parameters& Master::get_parameters() const {

	return parameters;
}

