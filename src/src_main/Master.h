/******************************************************************/
/***                Fast Inpainting, INRIA license				  */
/******************************************************************/

#pragma once

#include "SubaperturesLoader.h"
#include "SubaperturesInpainting.h"


class Master {

public :


	struct Parameters {

		/*! Light field loader parameters.*/
		SubaperturesLoader::Parameters LF_loader_parameters;
		/*! Inpainting parameters.*/
		SubaperturesInpainting::Parameters inpainting_parameters;
		/*! Path where data are written.*/
		std::string data_path;
	};
private :

	Parameters parameters;

public:
	Master();
	~Master();

	void set_parameters(const Parameters& _parameters);
	const Parameters& get_parameters() const;

};

#include "Master_Config.h"

