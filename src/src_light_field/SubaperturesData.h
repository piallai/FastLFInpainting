/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "ocv.h"
#include "SubapertureBundle.h"
#include "SubaperturesLoader.h"
#include "Images4D.h"
#include "SubaperturesDataBase.h"

/*! Class representing a light field as a set of subaperture images. Most common way of using a light field.*/
template <class Timg=ocv::Timg>
class SubaperturesData : public SubaperturesDataBase, public Images4D<Timg> {

public:


	/*! Type of sub-apertures.*/
	typedef TsubApT<Timg> TsubAp;
	/*! Basic structure for a strip of subapertures.*/
	struct TsubApStrip {
		std::vector<Timg> images;
		double baseline;
		void copyTo(TsubApStrip& _strip) const;
	};

	/*! Defined for clarity of declarations in the methods.*/
	typedef typename Timg::value_type Tvec;
	/*! Defined for clarity of declarations in the methods.*/
	typedef typename Tvec::value_type Tvalue;
	/*! Usefull when need to access value_type of Timg in functions applying to a Timg. So Images4D inherihted class can also be applied.*/
	typedef Tvec value_type;

private:

	/*! Contains images of light field.*/
	TsubAp subaperture_images;

public:

	SubaperturesData();
	SubaperturesData(const SubaperturesData<Timg>& _subapertures);
	template <class Timg_arg>
	SubaperturesData(const SubaperturesData<Timg_arg>& _subapertures);
	~SubaperturesData();

	/*! Clear memory. Ie, clear subapertures.*/
	void clear();
	/*! Return static name of class for inherited Images4D in a virtual fashion.*/
	std::string get_class_name() const;

	SubaperturesData<Timg> copy() const;
	void copyTo(SubaperturesData<Timg>& _subapertures) const;
	template <class Timg_arg>
	void copyTo(SubaperturesData<Timg_arg>& _subapertures) const;
	template <class Timg_arg>
	void copyPropertiesTo(SubaperturesData<Timg_arg>& _subapertures) const;

	unsigned int get_Nu() const;
	unsigned int get_Nv() const;
	cv::Size size() const;

	const Fpair get_center_coordinates() const;
	/*! Returns indices in the range of Nu and Nv, and closest as possible as get_center_coordinates().*/
	const UVindices get_center_indices() const;

	void resize(const unsigned int _Nu, const unsigned int _Nv, const cv::Size& _image_size);
	void resize(const unsigned int _Nu, const unsigned int _Nv);
	void resize(const UVindices& _Nuv, const cv::Size& _image_size);
	void resize(const cv::Size& _subapertures_size, const cv::Size& _image_size);
	void resize_images(const cv::Size& _image_size);
	void resize(const SubaperturesData<Timg>& _subapertures);
	template <class Timg2>
	void resize(const SubaperturesData<Timg2>& _subapertures);
	const Timg& operator() (const unsigned int u, const unsigned int v) const;
	Timg& operator() (const unsigned int u, const unsigned int v);
	const Timg& operator[] (const UVindices& _indices) const;
	Timg& operator[] (const UVindices& _indices);

	/*! Checks whether \p _uv_indices is in range of subapertures. */
	bool check_uv_indices(const UVindices& _uv_indices) const;

	Timg get_center_image() const;

	const Timg& get_image_original_coordinates(const unsigned int u, const unsigned int v) const;
	Timg& get_image_original_coordinates(const unsigned int u, const unsigned int v);
	UVindices get_original_coordinates(const UVindices& _uv) const;
	

	const TsubApStrip get_strip(unsigned int _index, const ocv::Orientation _orientation) const;
	TsubApStrip get_strip(unsigned int _index, const ocv::Orientation _orientation);
	void set_strip(const TsubApStrip& _subapertures_strip, unsigned int _index, const ocv::Orientation _orientation);

	/*! Applies image conversion.*/
	template <class Timg_input>
	SubaperturesData<Timg>& operator=(const SubaperturesData<Timg_input>& _subapertures);

	SubaperturesData<Timg>& copyFrom(const Timg& _image);
	SubaperturesData<Timg>& operator+=(const SubaperturesData<Timg>& _subapertures);
	SubaperturesData<Timg>& operator-=(const SubaperturesData<Timg>& _subapertures);
	SubaperturesData<Timg>& operator*=(const SubaperturesData<Timg>& _subapertures);
	template <class Timg_input>
	SubaperturesData<Timg>& operator*=(const SubaperturesData<Timg_input>& _subapertures);
	SubaperturesData<Timg>& operator/=(const SubaperturesData<Timg>& _subapertures);
	SubaperturesData<Timg>& operator=(const Timg& _image);
	SubaperturesData<Timg>& operator=(const Tvec& _value);
	SubaperturesData<Timg>& operator+=(const Tvec& _value);
	SubaperturesData<Timg>& operator*=(const Tvalue& _value);
	SubaperturesData<Timg>& operator/=(const Tvalue& _value);
	SubaperturesData<Timg>& inverse();
	SubaperturesData<Timg>& sqrt();
	SubaperturesData<Timg>& pow(const Tvalue& _power);
	SubaperturesData<Timg>& log();
	SubaperturesData<Timg>& log(const Tvalue& _base);
	SubaperturesData<Timg>& abs();
	SubaperturesData<Timg>& exp();


	/*! Add subapertures inbetween existing ones. Argument \p _n_images stands for number of subapertures introduced between each one.*/
	void refine(const unsigned int& _n_images);
	void refine(const Upair& _n_images);
	void get_refined(SubaperturesData<Timg>& _subapertures, const unsigned int& _n_images) const;
	void get_refined(SubaperturesData<Timg>& _subapertures, const Upair& _n_images) const;

	/*! Remove subapertures inbetween existing ones.*/
	bool get_coarsened(SubaperturesData<Timg>& _subapertures, const Upair& _n_images) const;

	/*! Add/remove subapertures at angular boundaries. With \p _offsets (left, up, right, bottom). Positive=>add, negative=>remove (like angular cropping).*/
	void angular_resize(const Leupribo<unsigned int>& _offsets);
	void angular_resize(const Leupribo<int>& _offsets);
	void get_angular_resized(SubaperturesData<Timg>& _subapertures, const Leupribo<unsigned int>& _offsets) const;
	void get_angular_resized(SubaperturesData<Timg>& _subapertures, const Leupribo<int>& _offsets) const;

	/*! Flip subapertures angularly along u direction if _orientation is ocv::horizontal, along v direction if _orientation is ocv::vertical.*/
	void flip_angular(const ocv::Orientation& _orientation);
	void transpose_angular();

	cv::Size get_image_size() const;

	/*! Same as get_image_size()*/
	cv::Size get_size_spatial() const;
	UVindices get_size_angular() const;

	/*! Spatial size in 4D structure*/
	cv::Size get_inner_size() const;
	/*! Angular size in 4D structure*/
	cv::Size get_outer_size() const;
private:
	bool is_inverted() const;
public:

	std::pair<bool, bool> get_full_image_transposes() const;

	/*! Crops subapertures left, up, right, bottom). */
	void crop_spatially(const Leupribo<unsigned int>& _Ncrop_pixels);
	void crop_angular(const Leupribo<unsigned int>& _Ncrop_suabpertures);
	void crop_angular_spatially(const Leupribo<unsigned int>& _Ncrop_angular, const Leupribo<unsigned int>& _Ncrop_spatial);

	void rescale_spatially(const Fpair& _image_scale);

	/*! Apply various transformations stores in DimensionsTransforms.*/
	void apply_transforms(const SubaperturesDataBase::DimensionsTransforms& _transforms);

	///// Projection on single image //////
	/*! To get sum of all subapertures.*/
	void get_sum_image(Timg& _result) const;
	/*! To get mean of all subapertures.*/
	void get_mean_image(Timg& _result) const;
	void get_mean_image_weighted(Timg& _result, const SubaperturesData<ocv::Timg1>& _weights) const;
	void get_mean_image_weighted(Timg& _result, const SubaperturesData<ocv::Timg1>& _weights, Timg& _buffer, ocv::Timg1& _buffer2) const;
	/*! Projects on a single image values with highest weight. Also returns highest weights.*/
	void get_highest_image_weighted(Timg& _result, ocv::Timg1& _weights_highest, const SubaperturesData<ocv::Timg1>& _weights) const;
	////////////////////////////////////////

	/*! Value above/under _threshold_value are replaced by _replacement_value.*/
	void threshold(const Tvalue& _threshold_value, const Tvec& _replacement_value, bool _l_inferior);
	/*! Values under _threshold_value are labelled in _result.*/
	void threshold_mask(const Tvalue& _threshold_value, SubaperturesData<ocv::Tmask>& _result) const;


	void reduce_channels(SubaperturesData< cv::Mat_< cv::Vec<Tvalue, 1> > >& _subapertures, const int _rtype) const;

	/*! Sets channel number defined by _nchannel as _value for every subaperture.*/
	void set_channel(int _nchannel, Tvalue _value);
	/*! Sets channel number defined by _nchannel as _subapertures1C.*/
	void set_channel(int _nchannel, const SubaperturesData< cv::Mat_< cv::Vec<Tvalue, 1> > >& _subapertures1C);


	bool has_nan() const;

	/*! Checks wether u and v are in range of Nu, Nv.*/
	bool is_in_uv_range(const unsigned int u, const unsigned int v) const;

	/*! Rescale images from min max values range to Tvalue range. Convenient if values are out of range of [0, 1] for floating point Tvalue for instance.*/
	void rescale_min_max();

	std::vector<Tvalue> calcHist(cv::Mat& _histogram, const unsigned int _Nvalues = 100, const ocv::Range<Tvalue> _range = { 0,0 }) const;
	std::vector<Tvalue> calcHist_minmax(cv::Mat& _histogram, const unsigned int _Nvalues = 100) const;

	std::vector<Tvec> get_medians(const unsigned int _Nmedians=1, const unsigned int _Nvalues=100) const;

	/*! Loads subapertures thanks to loader.*/
	bool load(const SubaperturesLoader& _loader);
	/*! Loads subapertures thanks to loader and a precomputed SubapertureBundle. Bundle contains standard loading information (path, etc..). Loader contains preprocessing parameters (angular cropping, etc).*/
	bool load(const SubaperturesLoader& _loader, const SubapertureBundle& _subapertures_bundle);

	static SubapertureBundle get_subapertures_bundle(const SubaperturesLoader& _loader);

private:

	/*! Modifications on \p _images depending one parameters \p _cfg.*/
	void preprocessing(const SubaperturesLoader::Parameters& _loader);

	/*! Apply various transforms contained in _loader. Is called by loading functions.*/
	void loader_assign(const SubaperturesLoader& _loader);
	/*! Loads light field according to informations precomputed in _subapertures_bundle and angular transforms.*/
	void load_SubapertureBundle(const SubapertureBundle& _subapertures_bundle, const std::array<unsigned int, 4>& _subaperture_offsets, const Upair& _angular_modulo, const Fpair& _image_scale);

	/*! Checks wether u and v are in range of Nu, Nv, and _subaperture_offsets.*/
	bool is_in_uv_range(const unsigned int u, const unsigned int v, const std::array<unsigned int, 4>& _subapertures_offsets) const;

};

