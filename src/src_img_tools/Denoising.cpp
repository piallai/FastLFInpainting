/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "Denoising.h"

const std::string Denoising::directory_name() {
	static const std::string _directory_name_ = "Denoising";
	return _directory_name_;
}

Denoising::Denoising() {}

Denoising::Denoising(const Parameters& _parameters) {
	set_parameters(_parameters);
}

Denoising::~Denoising() {}

void Denoising::set_parameters(const Parameters& _parameters) {

	parameters = _parameters;
}

const Denoising::Parameters& Denoising::get_parameters() const {

	return parameters;
}

std::string Denoising::get_method_name() const {

	std::string method_name;

	if (parameters.denoising_type == Denoising::DenoisingType::TVL1) {
		method_name = Misc::concat_paths(directory_name(), "TVL1");
	} else if (parameters.denoising_type == Denoising::DenoisingType::NLMeans) {
		method_name = Misc::concat_paths(directory_name(), "NLMeans");
	} else if (parameters.denoising_type == Denoising::DenoisingType::NLMeansColored) {
		method_name = Misc::concat_paths(directory_name(), "NLMeansColored");
	} else if (parameters.denoising_type == Denoising::DenoisingType::NLMeansMulti) {
		method_name = Misc::concat_paths(directory_name(), "NLMeansMulti");
	} else if (parameters.denoising_type == Denoising::DenoisingType::NLMeansColoredMulti) {
		method_name = Misc::concat_paths(directory_name(), "NLMeansColoredMulti");
	}

	return method_name;
}
