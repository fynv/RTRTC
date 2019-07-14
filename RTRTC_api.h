#ifndef _RTRTC_API_h
#define _RTRTC_API_h

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma warning( disable: 4275 )
#pragma warning( disable: 4251 )
#pragma warning( disable: 4530 )
#if defined RT_RTC_DLL_EXPORT
#define RT_RTC_API __declspec(dllexport)
#elif defined RT_RTC_DLL_IMPORT
#define RT_RTC_API __declspec(dllimport)
#endif
#endif

#ifndef RT_RTC_API
#define RT_RTC_API
#endif

#endif

