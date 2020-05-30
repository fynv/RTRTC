#include "TRTCContext.h"
#include "cuda_inline_headers.hpp"

#include "initialize.h"
#include "cuda_wrapper.h"
#include "DVRNG.h"

bool rtrtc_init(bool istrying)
{
	static bool s_initialized = false;
	if (!s_initialized)
	{
		if (!init_cuda())
		{
			printf("Cannot find CUDA driver. \n");
			if (istrying) return false;
			else exit(0);
		}
		DVRNG::singlton();
		TRTC_Add_Built_In_Header(s_name_headers[0], s_content_headers[0]);
		TRTC_Add_Inlcude_Filename(s_name_headers[0]);
		s_initialized = true;
	}
	return s_initialized;
}

bool RTRTC_Try_Init()
{
	return rtrtc_init(true);
}
