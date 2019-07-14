cmake_minimum_required (VERSION 3.0)

SET(CMAKE_INSTALL_RPATH ../lib)

add_executable(test_rtrtc test_rtrtc.cpp)

if (WIN32) 
target_compile_definitions(test_rtrtc PUBLIC THRUST_RTC_DLL_IMPORT CURAND_RTC_DLL_IMPORT CURAND_RTC_DLL_IMPORT)
endif()

target_link_libraries(test_rtrtc ThrustRTC CURandRTC RTRTC)

install(TARGETS test_rtrtc RUNTIME DESTINATION test_cpp)

