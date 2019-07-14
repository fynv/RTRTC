#include <cmath>
#include "RayTracer.h"
#include "DVImage.h"
#include "DVVector.h"
#include "initialize.h"
#include "DVRNG.h"
#include "TRTCContext.h"
#include "functor.h"

inline fvec3 operator + (const fvec3& v1, const fvec3& v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

inline fvec3 operator - (const fvec3& v1, const fvec3& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

inline fvec3 operator * (const fvec3& v1, const fvec3& v2)
{
	return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

inline fvec3 operator / (const fvec3& v1, const fvec3& v2)
{
	return { v1.x / v2.x, v1.y / v2.y, v1.z / v2.z };
}

inline fvec3 operator * (const fvec3& v, float s)
{
	return { v.x*s, v.y*s, v.z*s };
}

inline fvec3 operator * (float s, const fvec3& v)
{
	return { v.x*s, v.y*s, v.z*s };
}

inline fvec3 operator / (const fvec3& v, float s)
{
	return { v.x / s, v.y / s, v.z / s };
}

inline float dot(const fvec3& v1, const fvec3& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
}

inline fvec3 cross(const fvec3& v1, const fvec3& v2)
{
	return { v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x * v2.z, v1.x*v2.y - v1.y*v2.x };
}

inline float sqrlen(const fvec3& v)
{
	return v.x*v.x + v.y * v.y + v.z* v.z;
}

inline fvec3 normalize(const fvec3& v)
{
	float len = sqrtf(sqrlen(v));
	return { v.x / len, v.y / len, v.z / len };
}

class DVRayTracer : public DeviceViewable
{
public:
	DVRayTracer(int image_width, int image_height, int rays_per_pixels)
		: m_rng_states("RNGState", (size_t)image_width*(size_t)image_height)
	{
		rtrtc_init();
		DVRNG& rng = DVRNG::singlton();
		TRTC_For ker({ "rng", "states" }, "idx",
			"    rng.state_init(1234, idx, 0, states[idx]);\n"
		);
		const DeviceViewable* args[] = { &rng, &m_rng_states };
		ker.launch_n(m_rng_states.size(), args);

		m_image_width = image_width;
		m_image_height = image_height;
		m_rays_per_pixels = rays_per_pixels;

		set_camera({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f, 0.0f, 1.0f);
	}

	void set_camera(fvec3 lookfrom, fvec3 lookat, fvec3 vup, float vfov, float aperture, float focus_dist)
	{
		m_origin = lookfrom;
		
		float theta = vfov * PI / 180.0f;
		float half_height = tanf(theta*0.5f)*focus_dist;
		float size_pix = half_height * 2.0f / (float)m_image_height;
		float half_width = size_pix * (float)m_image_width*0.5f;

		fvec3 axis_z = normalize(lookfrom - lookat);
		fvec3 axis_x = normalize(cross(vup, axis_z));
		fvec3 axis_y = cross(axis_z, axis_x);

		fvec3 plane_center = lookfrom - axis_z* focus_dist;
		m_upper_left = plane_center - axis_x * half_width + axis_y * half_height;
		m_ux = size_pix * axis_x;
		m_uy = -size_pix * axis_y;

		m_lens_radius = aperture * 0.5f;

	}

	virtual std::string name_view_cls() const
	{
		return "RayTracerView";
	}

	virtual ViewBuf view() const
	{
		ViewBuf buf(sizeof(RayTracerView));
		RayTracerView *pview = (RayTracerView*)buf.data();
		pview->rng_states = (RNGState*)m_rng_states.data();
		pview->rays_per_pixels = m_rays_per_pixels;
		pview->origin = m_origin;
		pview->upper_left = m_upper_left;
		pview->ux = m_ux;
		pview->uy = m_uy;
		pview->lens_radius = m_lens_radius;
		return buf;
	}


private:
	DVVector m_rng_states;
	int m_image_width;
	int m_image_height;
	int m_rays_per_pixels;

	fvec3 m_origin;
	fvec3 m_upper_left;
	fvec3 m_ux;
	fvec3 m_uy;
	float m_lens_radius;

};


RayTracer::RayTracer(DVImage& target, int rays_per_pixels)
	: m_target(target)
{
	m_dv_ray_tracer = new DVRayTracer(target.width(), target.height(), rays_per_pixels);
	m_rays_per_pixels = rays_per_pixels;
	m_acc = new DVImage("fvec3", target.width(), target.height());

	m_sky = new Functor({}, { "dir" },
		"        float t = 0.5f*dir.y + 1.0f;\n"
		"        fvec3 ret = { 1.0f - 0.5f*t, 1.0f - 0.3f*t, 1.0f };\n"
		"        return ret;\n"
	);
}

RayTracer::~RayTracer()
{
	delete m_sky;
	delete m_acc;
	delete m_dv_ray_tracer;
}

void RayTracer::set_camera(fvec3 lookfrom, fvec3 lookat, fvec3 vup, float vfov, float aperture, float focus_dist)
{
	m_dv_ray_tracer->set_camera(lookfrom, lookat, vup, vfov, aperture, focus_dist);
}

bool RayTracer::trace(DeviceViewable& hitable, Functor* sky_box)
{
	{
		TRTC_Kernel ker({ "acc", "raytracer", "obj", "sky" },
			"    unsigned x = threadIdx.x + blockIdx.x*blockDim.x;\n"
			"    unsigned y = threadIdx.y + blockIdx.y*blockDim.y;\n"
			"    if (x>=acc.width() || y>=acc.height()) return;\n"
			"    raytracer.trace_pix(obj, sky, acc, x,y);\n"
		);

		if (sky_box == nullptr) sky_box = m_sky;

		const DeviceViewable* args[] = { m_acc, m_dv_ray_tracer, &hitable, sky_box};
		unsigned int block_x = (m_acc->width() + 15) / 16;
		unsigned int block_y = (m_acc->height() + 15) / 16;
		for (int i = 0; i < m_rays_per_pixels; i++)
			if (!ker.launch({ block_x, block_y, 1 }, { 16, 16, 1 }, args)) return false;
	}
	{
		TRTC_Kernel ker({ "acc", "target", "raytracer" },
			"    unsigned x = threadIdx.x + blockIdx.x*blockDim.x;\n"
			"    unsigned y = threadIdx.y + blockIdx.y*blockDim.y;\n"
			"    if (x>=target.width() || y>=target.height()) return;\n"
			"    raytracer.finalize_pix(acc, target, x,y);\n"
		);
		const DeviceViewable* args[] = { m_acc, &m_target, m_dv_ray_tracer };
		unsigned int block_x = (m_target.width() + 15) / 16;
		unsigned int block_y = (m_target.height() + 15) / 16;
		return ker.launch({ block_x, block_y, 1 }, { 16, 16, 1 }, args);
	}
}
