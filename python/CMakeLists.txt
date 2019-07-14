cmake_minimum_required (VERSION 3.0)

project(PyRTRTC)
find_package(PythonLibs 3 REQUIRED)

add_executable(PackHeaders ../internal/pack_headers.cpp)
add_custom_target(Run_PackHeaders
COMMAND PackHeaders
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
COMMENT "Running PackHeaders in ${CMAKE_CURRENT_SOURCE_DIR}/.."
SOURCES ../internal/pack_headers.cpp
)

set(LIB_SOURCES
../internal/cuda_wrapper.cpp
../internal/initialize.cpp
../DVImage.cpp
../DVSphere.cpp
../RayTracer.cpp
)

set(LIB_HEADERS
../RTRTC_api.h
../rtrtc_built_in.h
../DVImage.h
../DVSphere.h
../RayTracer.h
)

set(INTERNAL_HEADERS
../internal/cuda_wrapper.h
../internal/initialize.h
)

set (SRC
PyRTRTC.cpp
)

set (INCLUDE_DIR 
${PYTHON_INCLUDE_DIRS}
.
..
../internal
../install/include
)


if (WIN32) 
set (DEFINES  ${DEFINES}
-D"_CRT_SECURE_NO_DEPRECATE"  
-D"_SCL_SECURE_NO_DEPRECATE" 
-D"_CRT_SECURE_NO_WARNINGS"
)
else()
add_definitions(-std=c++14)
add_compile_options(-fPIC)
endif()

include_directories(${INCLUDE_DIR})
add_definitions(${DEFINES})

if (WIN32) 
link_directories(../install/lib)
else()
add_library(PyThrustRTC SHARED IMPORTED)
set_property(TARGET PyThrustRTC PROPERTY IMPORTED_LOCATION ../install/test_python/PyThrustRTC.so)
add_library(PyCURandRTC SHARED IMPORTED)
set_property(TARGET PyCURandRTC PROPERTY IMPORTED_LOCATION ../install/test_python/PyCURandRTC.so)
endif()

SET(CMAKE_INSTALL_RPATH .)

add_library(PyRTRTC SHARED ${LIB_SOURCES} ${LIB_HEADERS} ${INTERNAL_HEADERS} ${SRC})

if (WIN32) 
target_compile_definitions(PyRTRTC PRIVATE RT_RTC_DLL_EXPORT)
target_compile_definitions(PyRTRTC PUBLIC THRUST_RTC_DLL_IMPORT CURAND_RTC_DLL_IMPORT)
endif()

target_link_libraries(PyRTRTC ${PYTHON_LIBRARIES} PyThrustRTC PyCURandRTC)

add_dependencies(Run_PackHeaders PackHeaders)
add_dependencies(PyRTRTC Run_PackHeaders)

if (WIN32) 
set_target_properties(PyRTRTC PROPERTIES SUFFIX ".pyd")
else()
set_target_properties(PyRTRTC PROPERTIES SUFFIX ".so")
set_target_properties(PyRTRTC PROPERTIES PREFIX "")
endif()

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX  ../install CACHE PATH "Install path" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

install(FILES ${LIB_HEADERS} DESTINATION include)

if (WIN32) 
install(TARGETS PyRTRTC 
		RUNTIME DESTINATION test_python
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib 
		)
else()
install(TARGETS PyRTRTC DESTINATION test_python)
endif()


set(PYTHON
	RTRTC.py
)

install(FILES ${PYTHON} DESTINATION test_python)

set(PYTHON_TEST
	test/test_rtrtc_scene0.py
	test/test_rtrtc_scene1.py
)

set(INCLUDE_TESTS false CACHE BOOL "Include tests")

if (INCLUDE_TESTS)
install(FILES ${PYTHON_TEST} DESTINATION test_python )
endif()
