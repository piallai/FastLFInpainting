/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once
#include "ocv.h"
#include "Typedefs.h"
#include "Images4D_base.h"

class SubaperturesDataBase : virtual public Images4D_base {

protected :

	/*! Whether angular coordinates are inverted or not when light field is loaded.*/
	bool l_invert_uv;
	/*! Baseline of light field. Has to always be absolute value.*/
	Fpair baseline;
	/*! Center coordinates of subapertures. Use get_center_coordinates() to access it.*/
	Fpair center_coordinates;

public:

	/*! Type of sub-apertures.*/
	template <class Timg>
	using TsubApT = Tarray<Timg>;

	SubaperturesDataBase();
	~SubaperturesDataBase();

	void clear();

	bool is_uv_inverted() const;
	void set_uv_inverted(bool _l_invert_uv);
	/*! Returns baseline in horizontal and vertical direction.*/
	const Fpair& get_baseline() const;
	/*! Returns baseline ratio baseline_vertical/baseline_horizontal*/
	double get_baseline_ratio() const;
	//double get_baseline_ratio_abs() const;
	void set_baseline(const Fpair& _baseline);
	void set_baseline_ratio(const double& _baseline_ratio);
	virtual const Fpair get_center_coordinates() const=0;
	void set_center_coordinates();
	void set_center_coordinates(const Fpair _center_coordinates);

	/*! General rule of averaging values (for instance disparity) in case baseline is different in horizontal and vertical direction :
	D = 0.5 * (D_horizontal + D_vertical * (baseline_vertical / baseline_horizontal))
	That is : horizontal value is used as reference.
	*/

	/*! Method computing average of two values, weighted by baseline of light field.
	Convenient for averaging disparities for instance.*/
	template <class Tvalue>
	void average_values_baseline_weighted(const ocv::Vec<Tvalue>& _values, Tvalue& _result) const;
	template <class Tvalue>
	static void average_values_baseline_weighted(const ocv::Vec<Tvalue>& _values, const double& _baseline_ratio, Tvalue& _result);
	static void average_values_baseline_weighted(const cv::Point2f& _values, const double& _baseline_ratio, float& _result);

	/*! Method computing average of two images, weighted by absolute baseline of light field.
	Convenient for averaging disparities for instance.*/
	template <class Timg>
	void average_baseline_weighted(const ocv::Vec<Timg>& _images, Timg& _result) const;
	template <class Timg>
	static void average_baseline_weighted(const ocv::Vec<Timg>& _images, const double& _baseline_ratio, Timg& _result);

	/*! Method computing average of two images, weighted by absolute baseline of light field, and another pair of images (such as coherence).
	Convenient for averaging disparities for instance.*/
	template <class Timg>
	void average_baseline_weighted(const ocv::Vec<Timg>& _images, const ocv::Vec<Timg>& _weight_images, Timg& _result) const;
	template <class Timg>
	void average_baseline_weighted(const ocv::Vec<Timg>& _images, const ocv::Vec<Timg>& _weight_images, Timg& _result, ocv::Vec<Timg>& _buffer_images) const;
	template <class Timg>
	static void average_baseline_weighted(const ocv::Vec<Timg>& _images, const ocv::Vec<Timg>& _weight_images, const double& _baseline_ratio, Timg& _result, ocv::Vec<Timg>& _buffer_images);

	/*! Overload in which weights are included as second member of pair structure. Also return average weights.*/
	template <class Timg>
	static void average_baseline_weighted(const std::pair<Timg, Timg>& _value1, const std::pair<Timg, Timg>& _value2, const double& _baseline_ratio, std::pair<Timg, Timg>& _result, Timg& _image_buffer);
	/*! Filter values having the highest weight. The result are the obtained values and the highest weights for each pixel. Also takes into account the baseline.
	If weight of _value1 is higher than the weight of _value2, then returns _value1 in _result. Weight is second argument of each pair.*/
	template <class Timg>
	static void weight_filtering_highest(const std::pair<Timg, Timg>& _value1, const std::pair<Timg, Timg>& _value2, const double& _baseline_ratio, std::pair<Timg, Timg>& _result);

	template <class Timg>
	static void resize(TsubApT<Timg>& _subap, const unsigned int& _Nu, const unsigned int& _Nv);
	template <class Timg>
	static void resize(TsubApT<Timg>& _subap, const cv::Size& _size);
	template <class Timg>
	static unsigned int get_Nu(const TsubApT<Timg>& _subap);
	template <class Timg>
	static unsigned int get_Nv(const TsubApT<Timg>& _subap);

	/*! Stores transformations applied on dimensions. Convenient to apply again exact same transforms.*/
	struct DimensionsTransforms;

protected :
	/*! Returns new angular dimensions and whether angular cropping is possible.*/
	static std::pair<UVindices, bool> is_valid_angular_cropping(const UVindices& _Nuv, const std::array<unsigned int, 4>& _subaperture_offsets);
	/*! Returns new angular dimensions and whether angular removing is possible.*/
	static std::pair<UVindices, bool> is_valid_angular_modulo(const UVindices& _Nuv, const Upair& _angular_modulo);
public :
	/*! Get final angular dimensions according to original dimensions _Nuv and angular transforms (cropping and views removal). Returned value: (first : obtained angular dimensions), (second : whether transforms are possible)*/
	static std::pair<UVindices, bool> get_Nuv_transforms(const UVindices& _Nuv, const std::array<unsigned int, 4>& _subaperture_offsets, const Upair& _angular_modulo);
protected :
	/*! Converts angular indices _uv expressed on dimensions _Nuv, into coordinates on dimensions transformed by _subapertures_offsets and _angular_modulo. Angular modulo can used or ignored.*/
	static UVindices get_uv_transforms(const UVindices& _uv, const UVindices& _Nuv, const std::array<unsigned int, 4>& _subaperture_offsets, const Upair& _angular_modulo, bool _l_apply_angular_modulo);

	/*! Checks wether u and v are in range of Nu, Nv, and _subaperture_offsets.*/
	static bool is_in_uv_range(const unsigned int u, const unsigned int v, const unsigned int Nu, const unsigned int Nv, const std::array<unsigned int, 4>& _subapertures_offsets);
};

template <class Tvalue>
void SubaperturesDataBase::average_values_baseline_weighted(const ocv::Vec<Tvalue>& _values, Tvalue& _result) const {

	average_baseline_weighted(_values, get_baseline_ratio(), _result);
}

template <class Tvalue>
void SubaperturesDataBase::average_values_baseline_weighted(const ocv::Vec<Tvalue>& _values, const double& _baseline_ratio, Tvalue& _result) {

	_result = _values.first;
	_result /= (Tvalue)_baseline_ratio;
	_result += _values.second;
	_result *= (Tvalue)_baseline_ratio;
	_result *= 0.5;
}

template <class Timg>
void SubaperturesDataBase::average_baseline_weighted(const ocv::Vec<Timg>& _images, Timg& _result) const {

	average_baseline_weighted(_images, get_baseline_ratio(), _result);
}

template <class Timg>
void SubaperturesDataBase::average_baseline_weighted(const ocv::Vec<Timg>& _images, const double& _baseline_ratio, Timg& _result) {

	typedef typename Timg::value_type::value_type Tvalue;

	_images.first.copyTo(_result);
	_result /= (Tvalue)(_baseline_ratio);
	_result += _images.second;
	_result *= (Tvalue)(0.5 * _baseline_ratio);

}

template <class Timg>
void SubaperturesDataBase::average_baseline_weighted(const ocv::Vec<Timg>& _images, const ocv::Vec<Timg>& _weight_images, Timg& _result) const {

	ocv::Vec<Timg> buffer_images;
	average_baseline_weighted(_images, _weight_images, get_baseline_ratio(), _result, buffer_images);
}

template <class Timg>
void SubaperturesDataBase::average_baseline_weighted(const ocv::Vec<Timg>& _images, const ocv::Vec<Timg>& _weight_images, Timg& _result, ocv::Vec<Timg>& _buffer_images) const {

	average_baseline_weighted(_images, _weight_images, get_baseline(), _result, _buffer_images);
}

template <class Timg>
void SubaperturesDataBase::average_baseline_weighted(const ocv::Vec<Timg>& _images, const ocv::Vec<Timg>& _weight_images, const double& _baseline_ratio, Timg& _result, ocv::Vec<Timg>& _buffer_images) {

	_images.first.copyTo(_buffer_images.first);
	cv::multiply(_buffer_images.first, _weight_images.first, _buffer_images.first);
	_images.second.copyTo(_buffer_images.second);
	cv::multiply(_buffer_images.second, _weight_images.second, _buffer_images.second);
	_buffer_images.second *= _baseline_ratio;

	_buffer_images.first.copyTo(_result);
	cv::add(_result, _buffer_images.second, _result);

	_weight_images.first.copyTo(_buffer_images.first);
	_weight_images.second.copyTo(_buffer_images.second);
	cv::add(_buffer_images.first, _buffer_images.second, _buffer_images.first);

	cv::divide(_result, _buffer_images.first, _result);
}

template <class Timg>
void SubaperturesDataBase::average_baseline_weighted(const std::pair<Timg, Timg>& _value1, const std::pair<Timg, Timg>& _value2, const double& _baseline_ratio, std::pair<Timg, Timg>& _result, Timg& _image_buffer) {

	_result.first.create(_value1.first.size());
	_result.second.create(_value1.first.size());

	_value1.first.copyTo(_image_buffer);
	cv::multiply(_image_buffer, _value1.second, _image_buffer);
	_image_buffer.copyTo(_result.first);
	_value1.second.copyTo(_result.second);

	_value2.first.copyTo(_image_buffer);
	_image_buffer *= _baseline_ratio;
	cv::multiply(_image_buffer, _value2.second, _image_buffer);
	cv::add(_result.first, _image_buffer, _result.first);
	cv::add(_result.second, _value2.second, _result.second);

	cv::divide(_result.first, _result.second, _result.first);
	/*! Average weights :*/
	_result.second *= 0.5;

}

template <class Timg>
void SubaperturesDataBase::weight_filtering_highest(const std::pair<Timg, Timg>& _value1, const std::pair<Timg, Timg>& _value2, const double& _baseline_ratio, std::pair<Timg, Timg>& _result) {

	ocv::Timg1::const_iterator it_val1 = _value1.first.begin();
	ocv::Timg1::const_iterator it_w1 = _value1.second.begin();
	ocv::Timg1::const_iterator it_val2 = _value2.first.begin();
	ocv::Timg1::const_iterator it_w2 = _value2.second.begin();

	_result.first.create(_value1.first.size());
	_result.second.create(_value1.first.size());

	ocv::Timg1::iterator it_w = _result.second.begin();
	for (ocv::Timg1::iterator it_val = _result.first.begin(); it_val != _result.first.end(); ++it_val, ++it_w, ++it_val1, ++it_w1, ++it_val2, ++it_w2) {

		if ((*it_w1)[0] > (*it_w2)[0]) {
			*it_val = *it_val1;
			*it_w = *it_w1;
		} else {
			*it_val = *it_val2;
			*it_val *= _baseline_ratio;
			*it_w = *it_w2;
		}

	}

}

template <class Timg>
void SubaperturesDataBase::resize(TsubApT<Timg>& _subap, const unsigned int& _Nu, const unsigned int& _Nv) {

	Images4D_base::resize(_subap, _Nu, _Nv);

}

template <class Timg>
void SubaperturesDataBase::resize(TsubApT<Timg>& _subap, const cv::Size& _size) {

	Images4D_base::resize(_subap, _size.width, _size.height);

}

template <class Timg>
unsigned int SubaperturesDataBase::get_Nu(const TsubApT<Timg>& _subap) {

	return Images4D_base::get_N1(_subap);
}

template <class Timg>
unsigned int SubaperturesDataBase::get_Nv(const TsubApT<Timg>& _subap) {

	return Images4D_base::get_N2(_subap);

}

///////// DimensionsTransforms

/*! Stores transformations applied on dimensions. Convenient to apply again exact same transforms.
Order of transforms is related to SubaperturesData<Timg>::apply_transforms */
struct SubaperturesDataBase::DimensionsTransforms {

public :

	struct Transform;

private:

	std::vector<Transform> transforms;

public:

	/*! Get all transforms added.*/
	const std::vector<Transform>& get_transforms() const;

	void transpose_angular();
	void flip_angular(ocv::Orientation _orientation);
	/*! Remove _Nviews every _Nviews+1 views.*/
	void remove_angular(const Upair& _Nviews);
	void crop_angular(const Leupribo<unsigned int>& _Ncrop_angular);
	void crop_spatially(const Leupribo<unsigned int>& _Ncrop_spatial);
	void rescale_spatially(const Fpair& _image_scale);
	void set_baseline(const Fpair& _baseline);

};

/*! Stores only one transformation among possible ones.*/
struct SubaperturesDataBase::DimensionsTransforms::Transform {

public :

	enum Type { Null, TransposeAngular, FlipAngularHorizontal, FlipAngularVertical, RemoveAngular, CropAngular, CropSpatially, RescaleSpatially, Baseline };

private:

	bool l_set = false;
	Type transform_type = Type::Null;

	bool l_transpose_angular = false;
	/*! Horizontal and vertical.*/
	std::pair<bool, bool> l_flip_angular = { false, false };
	Leupribo<unsigned int> Ncrop = { {0, 0, 0, 0} };
	Fpair fpair = { 1., 1. };
	Upair upair = { (unsigned int)0, (unsigned int)0 };

public :

	/*! Obtain transform type.*/
	Type get_type() const;

	void transpose_angular();
	void flip_angular(ocv::Orientation _orientation);
	/*! Remove _Nviews every _Nviews+1 views.*/
	void remove_angular(const Upair& _Nviews);
	void crop_angular(const Leupribo<unsigned int>& _Ncrop_angular);
	void crop_spatially(const Leupribo<unsigned int>& _Ncrop_spatial);
	void rescale_spatially(const Fpair& _image_scale);
	void set_baseline(const Fpair& _baseline);

	const Upair& get_Nviews_removed() const;
	const Leupribo<unsigned int>& get_Ncrop_angular() const;
	const Leupribo<unsigned int>& get_Ncrop_spatial() const;
	const Fpair& get_image_scale() const;
	const Fpair& get_baseline() const;

};