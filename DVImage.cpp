#include "cuda_wrapper.h"
#include "initialize.h"
#include "TRTCContext.h"
#include "DVImage.h"
#include "rtrtc_built_in.h"

DVImageLike::DVImageLike(const char* elem_cls, const char* ref_type, int width, int height)
{
	rtrtc_init();
	m_elem_cls = elem_cls;
	m_ref_type = ref_type;
	m_elem_size = (int)TRTC_Size_Of(elem_cls);
	m_width = width;
	m_height = height;
}


DVImage::DVImage(const char* elem_cls, int width, int height, void* hdata)
	: DVImageLike(elem_cls, (std::string(elem_cls) + "&").c_str(), width, height)
{
	CUdeviceptr dptr;
	cuMemAlloc(&dptr, m_elem_size*m_width*m_height);
	m_data = (void*)dptr;
	if (hdata)
		cuMemcpyHtoD(dptr, hdata, m_elem_size*m_width*m_height);
	else
		cuMemsetD8(dptr, 0, m_elem_size*m_width*m_height);

	m_name_view_cls = std::string("ImageView<") + m_elem_cls + ">";
}

DVImage::~DVImage()
{
	cuMemFree((CUdeviceptr)m_data);
}


void DVImage::to_host(void* hdata) const
{
	cuMemcpyDtoH(hdata, (CUdeviceptr)m_data, m_elem_size*m_width*m_height);
}


ViewBuf DVImage::view() const
{
	ViewBuf buf(sizeof(ImageView<char>));
	ImageView<char> *pview = (ImageView<char>*)buf.data();
	pview->_data = (char*)m_data;
	pview->_width = m_width;
	pview->_height = m_height;
	return buf;
}

