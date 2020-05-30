#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define PY_RTRTC_API __declspec(dllexport)
#else
#define PY_RTRTC_API 
#endif

extern "C"
{
	PY_RTRTC_API int n_rtrtc_try_init();
	PY_RTRTC_API void* n_dvimage_create(int width, int height);
	PY_RTRTC_API int n_dvimage_width(void* cptr);
	PY_RTRTC_API int n_dvimage_height(void* cptr);
	PY_RTRTC_API void n_dvimage_to_bytes(void* cptr, char* bytes);
	PY_RTRTC_API void* n_vec3_create(float x, float y, float z);
	PY_RTRTC_API void n_vec3_destroy(void* cptr);
	PY_RTRTC_API void* n_material_create(const char* type, void* ptr_color, float fuzz, float ref_idx);
	PY_RTRTC_API void n_material_destroy(void* cptr);
	PY_RTRTC_API void* n_dvsphere_create(void* ptr_center, float radius, void* ptr_material, void* ptr_velocity);
	PY_RTRTC_API void* n_dv_multisphres_create(unsigned long long size, const void* const* ptrs);
	PY_RTRTC_API void* n_raytracer_create(void* ptr_target, int rays_per_pixels);
	PY_RTRTC_API void n_raytracer_destroy(void* cptr);
	PY_RTRTC_API void n_raytracer_set_camera(void* cptr, void* ptr_lookfrom, void* ptr_lookat, void* ptr_vup, float vfov, float aperture, float focus_dist);
	PY_RTRTC_API void n_raytracer_set_shutter(void* cptr, float t0, float t1);
	PY_RTRTC_API int n_raytracer_trace(void* cptr, void* ptr_hitable, void* ptr_skybox);
}

#include <stdio.h>
#include <string.h>
#include "initialize.h"
#include "DVImage.h"
#include "rtrtc_built_in.h"
#include "DVSphere.h"
#include "RayTracer.h"
#include "functor.h"

int n_rtrtc_try_init()
{
	return RTRTC_Try_Init() ? 1 : 0;
}

void* n_dvimage_create(int width, int height)
{
	return new DVImage("u8vec3", width, height);
}

int n_dvimage_width(void* cptr)
{
	DVImage* img = (DVImage*)cptr;
	return img->width();
}

int n_dvimage_height(void* cptr)
{
	DVImage* img = (DVImage*)cptr;
	return img->height();
}

void n_dvimage_to_bytes(void* cptr, char* bytes)
{
	DVImage* img = (DVImage*)cptr;
	img->to_host(bytes);
}

void* n_vec3_create(float x, float y, float z)
{
	return new fvec3({ x,y,z });
}

void n_vec3_destroy(void* cptr)
{
	delete (fvec3*)cptr;
}

void* n_material_create(const char* type, void* ptr_color, float fuzz, float ref_idx)
{
	Material* ret = new Material;
	if (type != nullptr)
	{
		if (strcmp(type, "lamertian") == 0)
			ret->type = Material::lamertian;
		else if (strcmp(type, "metal") == 0)
			ret->type = Material::metal;
		else if (strcmp(type, "dielectric") == 0)
			ret->type = Material::dielectric;
	}
	if (ptr_color != nullptr)
	{
		ret->color = *(fvec3*)ptr_color;
	}
	ret->fuzz = fuzz;
	ret->ref_idx = ref_idx;
	return ret;
}

void n_material_destroy(void* cptr)
{
	delete (Material*)cptr;
}


void* n_dvsphere_create(void* ptr_center, float radius, void* ptr_material, void* ptr_velocity)
{
	fvec3* center = (fvec3*)ptr_center;
	Material* material = (Material*)ptr_material;
	fvec3* velocity = (fvec3*)ptr_velocity;
	return new DVSphere(*center, radius, *material, *velocity);
}

void* n_dv_multisphres_create(unsigned long long size, const void* const* ptrs)
{
	std::vector<const DVSphere*> sphereArr(size);
	memcpy(sphereArr.data(), ptrs, sizeof(void*)*size);
	return new DVMultiSpheres(sphereArr);
}

void* n_raytracer_create(void* ptr_target, int rays_per_pixels)
{
	DVImage* target = (DVImage*)ptr_target;
	return new RayTracer(*target, rays_per_pixels);
}

void n_raytracer_destroy(void* cptr)
{
	RayTracer* raytracer = (RayTracer*)cptr;
	delete raytracer;
}

void n_raytracer_set_camera(void* cptr, void* ptr_lookfrom, void* ptr_lookat, void* ptr_vup, float vfov, float aperture, float focus_dist)
{
	RayTracer* raytracer = (RayTracer*)cptr;
	fvec3* lookfrom = (fvec3*)ptr_lookfrom;
	fvec3* lookat = (fvec3*)ptr_lookat;
	fvec3* vup = (fvec3*)ptr_vup;
	raytracer->set_camera(*lookfrom, *lookat, *vup, vfov, aperture, focus_dist);
}

void n_raytracer_set_shutter(void* cptr, float t0, float t1)
{
	RayTracer* raytracer = (RayTracer*)cptr;	
	raytracer->set_shutter(t0, t1);
}

int n_raytracer_trace(void* cptr, void* ptr_hitable, void* ptr_skybox)
{
	RayTracer* raytracer = (RayTracer*)cptr;
	DeviceViewable* hitable = (DeviceViewable*)ptr_hitable;
	Functor* skybox = (Functor*)ptr_skybox;
	return raytracer->trace(*hitable, skybox) ? 1 : 0;
}
