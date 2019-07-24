#ifndef _RayTracer_h
#define _RayTracer_h

#include "RTRTC_api.h"
#include "rtrtc_built_in.h"

class DeviceViewable;
class DVImage;
class DVRayTracer;
class Functor;

class RT_RTC_API RayTracer
{
public:
	RayTracer(DVImage& target, int rays_per_pixels = 100);
	~RayTracer();

	void set_camera(fvec3 lookfrom, fvec3 lookat, fvec3 vup, float vfov, float aperture = 0.0f, float focus_dist = 1.0f);
	void set_shutter(float t0, float t1);

	bool trace(DeviceViewable& hitable, Functor* sky_box = nullptr);

private:
	DVImage& m_target;
	DVImage* m_acc;
	int m_rays_per_pixels;
	DVRayTracer* m_dv_ray_tracer;
};

#endif


