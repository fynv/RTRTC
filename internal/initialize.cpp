#include "TRTCContext.h"
#include "cuda_inline_headers.hpp"

#include "initialize.h"
#include "cuda_wrapper.h"
#include "DVRNG.h"

void rtrtc_init()
{
	static bool s_initialized = false;
	if (!s_initialized)
	{
		init_cuda();
		DVRNG::singlton();

		TRTC_Add_Built_In_Header(s_name_headers[0], s_content_headers[0]);
		TRTC_Add_Inlcude_Filename(s_name_headers[0]);
		s_initialized = true;
	}
}

