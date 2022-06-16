/******************************************************************/
/***                Fast Inpainting, INRIA license				  */
/******************************************************************/

#include <ctime>

#include "Master.h"
#include "ConfigReader.h"

#include "version.h"

void usage() {
  std::cout << "FastLFInpainting " << FI_VERSION_MAJOR<< "." << FI_VERSION_MINOR << "." << FI_VERSION_PATCH << " : " << std::endl;
  std::cout << "    Usage : ./FastLFInpainting <config-file>" << std::endl;
  std::cout << "    <config-file> full path to the configuration file providing access to light field parameters." << std::endl;
  exit(0);
}

int main(int argc, char** argv) {

	Master master;

	std::string directory_path;

	if (ConfigReader::read(master, argc, argv)) {

		Misc::data_path = master.get_parameters().data_path;
		std::cout << "Data path is set to : " << Misc::data_path << std::endl;

		SubaperturesLoader loader;

		loader.set_parameters(master.get_parameters().LF_loader_parameters);


		SubaperturesData<> subapertures;

		if (subapertures.load(loader) && subapertures.is_coherent()) {


			std::cout << "Dataset name : " << subapertures.get_name() << std::endl;

			clock_t start;
			double duration;

			SubaperturesData<> subapertures_result;


			std::cout << "Starting light field processing" << std::endl;
			start = clock();

			SubaperturesInpainting inpainting;
			inpainting.set_parameters(master.get_parameters().inpainting_parameters);
			inpainting.inpaint(subapertures, subapertures_result, directory_path);

			/*! Create output directory.*/
			directory_path = "";
			Misc::create_directory(directory_path);
			subapertures_result.imwrite(directory_path, 0., 1.);

			duration = (clock() - start) / (double)CLOCKS_PER_SEC;
			std::cout << "Method duration : " << (int)duration / 60 << " min, " << (long)duration % 60 << " s, " << (long)(duration*1000.) % 1000 << " ms" << std::endl;

		} else {
			std::cout << "Problem loading light field" << std::endl;
			if (!subapertures.is_coherent()) {
				std::cout << "=> light field is incoherent in size" << std::endl;
			}
		}

		std::cout << "End of program" << std::endl;

	} else {
	  usage();
	}

	return 0;
}


