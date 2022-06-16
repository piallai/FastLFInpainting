/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#include "SubaperturesData.h"
#include "SubaperturesData_impl.h"

template class SubaperturesData<ocv::Timg>;
template class SubaperturesData<ocv::Timg1>;
template class SubaperturesData< cv::Mat_< cv::Vec<unsigned char, 3> > >;
template class SubaperturesData< cv::Mat_< cv::Vec<unsigned char, 1> > >;

#ifdef ENABLE_OPACITY_1C
template class SubaperturesData<ocv::Timg2>;
#endif
