/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "Typedefs.h"
#include "SubaperturesData.h"
#include "InpaintingAngular.h"

class SubaperturesInpainting {

public :

	static const std::string directory_name();


	struct Parameters {
		/*! Path of reference inpaintd image. Central most of the time.*/
		std::string inpainted_subaperture_path = "";
		/*! Coordinates of inpainted subaperture in light field coordinates (u,v).*/
		UVindices subaperture_position = {(unsigned int)4, (unsigned int)4};
		/*! Path of mask.*/
		std::string mask_path = "";
		/*! Parameters of epipolar inpainting. Used if inpainting_type is set to total.*/
		InpaintingAngular::Parameters inpainting_angular_parameters;

	};

private :

	Parameters parameters;

public :

	SubaperturesInpainting();
	~SubaperturesInpainting();

	void set_parameters(const Parameters& _parameters);
	const Parameters& get_parameters() const;

	void inpaint(const SubaperturesData<>& _subapertures, SubaperturesData<>& _subapertures_output, const std::string _directory_name="") const;

};
