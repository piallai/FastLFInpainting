/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#include "DirectoryParser.h"
#include "misc_funcs.h"
#include <iterator>     // std::distance

SubapertureBundle DirectoryParser::analyse(const std::string& _path, bool _l_recursive, unsigned int _time_index, const std::vector<std::string> _filter_strings) {

	if (Misc::l_verbose_high) std::cout << "Analyzing directory" << std::endl;

	/*! _path has to be a directory.*/
	if (Misc::is_directory(_path)) {

		/*! Gets all images' path in _path.*/
		std::vector<std::string> images_paths = Misc::get_images_in_path_recursive(_path, _l_recursive);

		Misc::filter_strings(images_paths, _filter_strings);

		/*! In this case, permutations of ConfigFileReading are computed. The one leading to a SubapertureBundle with highest likeliness coefficient is selected.*/

		/*! Look for an estimation of the number of numbers in file names.*/
		/*! Computes total number of numbers in file names.*/
		unsigned int Nnumbers_in_file_name = 0;
		for (unsigned int i = 0; i < images_paths.size(); i++) {
			Nnumbers_in_file_name += (unsigned int)Misc::get_numbers(images_paths[i]).size();
		}
		/*! Significant number of numbers in files. Only files matching this number of numbers will be used, because wrong index of reading will lead to -1 return and be ignored.*/
		unsigned int Nnumbers_average_rounded = (unsigned int)std::round((float)Nnumbers_in_file_name / (float)images_paths.size());

		std::vector<SubapertureBundle::Parameters> subapertures_bundle_parameters_vector;
		SubapertureBundle::Parameters subapertures_bundle_parameters;

		/*! Maximum number of coordinates.*/
		unsigned int Nuvt = 3;

		//unsigned int count = 0;

		/*! Global loop to find possible permutations of file_reading.uvt_position_in_name.*/
		/*! Boolean loop for single index for sub-apertures. Following code therefore works for cases [u,v] and [i].*/
		for (bool l_single_index_for_subapertures : { false, true }) {

			subapertures_bundle_parameters.l_single_index_for_subapertures = l_single_index_for_subapertures;

			for (bool l_begin0 : { false, true }) {

				subapertures_bundle_parameters.l_begin0 = l_begin0;

				/*! Npositions stands for the number of indices to look for. For instance [i]=>1, [u,v]=>2, [i,t]=>2, [u,v,t]=>3.
				Can't be superior to Nnumbers_average_rounded. If images contains only 2 numbers, they can't be [u,v,t].*/
				for (unsigned int Npositions = 1 + !subapertures_bundle_parameters.l_single_index_for_subapertures; Npositions <= std::min(Nuvt - subapertures_bundle_parameters.l_single_index_for_subapertures, Nnumbers_average_rounded); Npositions++) {
					/*! for instance :
					if file_reading.l_single_index_for_subapertures == false
					=> checks permutations for Npositions == 2 to 3 (u,v,t)
					if file_reading.l_single_index_for_subapertures == true
					=> checks permutations for Npositions == 1 to 2 (i,t).*/

					/*! permutation vector stands for the possible permutations of positions among number in files.
					For instance, ff number of numbers in image files is 4, there are multiple possibility for u,v or t indices to be located in.*/
					/*! Initalize permutation with simple 0,1,...,Nnumbers_average_rounded-1 vector.*/
					std::vector<unsigned int> permutation = Misc::get_count_vector(Nnumbers_average_rounded);
					/*! first permutation : 0 for u, 1 for v, 2 for t
					or : 0 for i, 1 for t.*/

					/*! Initialize uvt_position_in_name.*/
					subapertures_bundle_parameters.uvt_position_in_name = Misc::get_count_vector(Npositions);
					subapertures_bundle_parameters_vector.push_back(subapertures_bundle_parameters);

					/*! Add all possible permutations of uvt_position_in_name.*/
					/*! index at which u is located if i-loop==0, or v is located if i-loop==0, and so on.*/
					unsigned int position;
					/*! While next permutation i a new one.*/
					while (std::next_permutation(permutation.begin(), permutation.end())) {

						/*! Finds where is located i index in permutation vector.*/
						for (unsigned int i = 0; i < Npositions; i++) {
							//position = Misc::vector_find(permutation, i) - permutation.begin();
							position = (unsigned int)std::distance(permutation.begin(), Misc::vector_find(permutation, i));

							subapertures_bundle_parameters.uvt_position_in_name[i] = position;
						}
						/*! If file_reading defined in this loop doesn't already exist in file_readings, then add it to.*/
						if (!Misc::vector_find_bool(subapertures_bundle_parameters_vector, subapertures_bundle_parameters)) {
							/*file_reading.set_parameters(file_reading_parameters);
							file_readings.push_back(file_reading);*/
							subapertures_bundle_parameters_vector.push_back(subapertures_bundle_parameters);
						}

					}

				}/*! End of Npositions loop.*/

			}/*! End of l_begin0 loop*/

		}/*! End of boolean loop.*/

		 /*! Second pair parameter is to save correspond file_readings index. Needed to display the best one after sorting subaperture_bundles.*/
		std::vector< std::pair<SubapertureBundle, unsigned int> > subaperture_bundles;
		/*! Pair to push_back to subaperture_bundles.*/
		std::pair<SubapertureBundle, unsigned int> subapertures_with_file_reading_index;
		for (unsigned int i = 0; i < subapertures_bundle_parameters_vector.size(); i++) {
			/*! Analyses file_readings[i] for consistency.*/
			//file_readings[i].analyse();
			if (Misc::l_verbose_low) {
				std::cout << "######## Bundle parameters : i = " << i << std::endl;
				std::cout << "subapertures_bundle_parameters_vector[i] = " << subapertures_bundle_parameters_vector[i] << std::endl;
			}
			subapertures_with_file_reading_index.first.set_parameters(subapertures_bundle_parameters_vector[i]);
			/*! Read SubapertureBundle using file_readings[i].*/
			subapertures_with_file_reading_index.first.read_subapertures(images_paths, _path, _time_index);
			subapertures_with_file_reading_index.second = i;
			if (Misc::l_verbose_low) {
				std::cout << "## Read bundle : " << std::endl;
				displug(subapertures_with_file_reading_index.first);
			}
			subaperture_bundles.push_back(subapertures_with_file_reading_index);
		}
		if (Misc::l_verbose_low) {
			std::cout << "## Sort bundles by likeliness" << std::endl;
		}
		/*! Sorts all SubapertureBundle by likeliness (increasing) order. For std::pair, first parameter is used when sorting.*/
		std::sort(subaperture_bundles.begin(), subaperture_bundles.end());

		if (subaperture_bundles.size() > 0) {

			/*! In order to get the first one to match best score, needs to look backward. Because std::pair comparison uses ::second if ::first are equals.
			Therefore instead of simply use subaperture_bundles.back(), a lookup operation is performed.*/
			unsigned int i_match = (unsigned int)subaperture_bundles.size() - 1;
			/*! While sub-aperture_bundles are not equals.*/
			while (i_match > 1 && !(subaperture_bundles[i_match].first > subaperture_bundles[i_match - 1].first)) {
				i_match--;
			}

			if (Misc::l_verbose_low) {
				std::cout << "==============================" << std::endl;
				std::cout << "best SubapertureBundle parameters : " << subapertures_bundle_parameters_vector[subaperture_bundles[i_match].second] << std::endl;
				std::cout << "==============================" << std::endl;
			}

			return subaperture_bundles[i_match].first;

		} else {

			return SubapertureBundle();
		}

	} else {

		std::cout << _path << " is not a directory. Content analysis stoped." << std::endl;

		return SubapertureBundle();
	}

}
