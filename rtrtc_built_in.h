#ifndef _rtrtc_built_in_h
#define _rtrtc_built_in_h

#include "built_in.h"
#include "curandrtc_built_in.h"

#ifndef PI
#define PI 3.1415926f
#endif

template<class _T>
struct ImageView
{
	typedef _T value_t;
	typedef _T& ref_t;

	value_t* _data;
	int _width;
	int _height;

#ifdef DEVICE_ONLY
	__device__ inline int width() const
	{
		return _width;
	}

	__device__ inline int height() const
	{
		return _height;
	}

	__device__ inline ref_t pix(int x, int y)
	{
		return _data[x + y*_width];
	}
#endif
};

struct fvec3
{
	float x;
	float y;
	float z;
};

struct u8vec3
{
	unsigned char x;
	unsigned char y;
	unsigned char z;
};

#ifdef DEVICE_ONLY
template<typename T>
__device__ inline T from_fvec3(const fvec3& v);

// no conversion
template<>
__device__ inline fvec3 from_fvec3(const fvec3& v)
{
	return v;
}

__device__ inline fvec3 to_fvec3(const fvec3& v)
{
	return v;
}

// u8vec3 conversion

template<>
__device__ inline u8vec3 from_fvec3(const fvec3& v)
{
	return  { (unsigned char)(v.x*255.0f + 0.5f),  (unsigned char)(v.y*255.0f + 0.5f),  (unsigned char)(v.z*255.0f + 0.5f) };
}

__device__ inline fvec3 to_fvec3(const u8vec3& v)
{
	return { (float)v.x / 255.0f, (float)v.y / 255.0f, (float)v.z / 255.0f };
}

// fvec3 operations
__device__ inline fvec3 operator + (const fvec3& v1, const fvec3& v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

__device__ inline fvec3 operator - (const fvec3& v1, const fvec3& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

__device__ inline fvec3 operator * (const fvec3& v1, const fvec3& v2)
{
	return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

__device__ inline fvec3 operator / (const fvec3& v1, const fvec3& v2)
{
	return { v1.x / v2.x, v1.y / v2.y, v1.z / v2.z };
}

__device__ inline fvec3 operator * (const fvec3& v, float s)
{
	return { v.x*s, v.y*s, v.z*s };
}

__device__ inline fvec3 operator * (float s, const fvec3& v)
{
	return { v.x*s, v.y*s, v.z*s };
}

__device__ inline fvec3 operator / (const fvec3& v, float s)
{
	return { v.x/s, v.y/s, v.z/s };
}

__device__ inline float d_dot(const fvec3& v1, const fvec3& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
}

__device__ inline float d_sqrlen(const fvec3& v)
{
	return v.x*v.x + v.y * v.y + v.z* v.z;
}

__device__ inline fvec3 d_normalize(const fvec3& v)
{
	float len = sqrtf(d_sqrlen(v));
	return { v.x / len, v.y / len, v.z / len };
}
#endif


struct Ray
{
	fvec3 origin;
	fvec3 direction;
	float time;

#ifdef DEVICE_ONLY
	__device__ inline fvec3 point_at_parameter(float t) const
	{ 
		return origin + t * direction;
	}
#endif
};

struct Material
{
	enum Type
	{
		lamertian,
		metal,
		dielectric
	};

	Type type;
	fvec3 color;
	float fuzz;
	float ref_idx;
};

struct HitRecord
{
	float t;
	fvec3 point;
	fvec3 normal;
	Material material;
};

#ifdef DEVICE_ONLY
__device__ inline fvec3 rand_in_unit_sphere(RNGState& rstate)
{
	fvec3 ret;
	do{
		ret = { rstate.rand01()*2.0f - 1.0f, rstate.rand01()*2.0f - 1.0f, rstate.rand01()*2.0f - 1.0f };
	} while (d_sqrlen(ret) > 1.0f);
	return ret;
}

__device__ inline fvec3 rand_in_unit_disk(RNGState& rstate)
{
	fvec3 ret;
	do {
		ret = { rstate.rand01()*2.0f - 1.0f, rstate.rand01()*2.0f - 1.0f, 0.0f};
	} while (d_sqrlen(ret) > 1.0f);
	return ret;
}


__device__ inline fvec3 reflect(const fvec3& v, const fvec3& n)
{
	return  v - 2.0f*d_dot(v, n)*n;
}

__device__ inline bool refract(const fvec3& v, const fvec3& n, float ref_idx, fvec3& refracted)
{
	float dt = d_dot(v, n);
	float discriminant = 1.0f - ref_idx * ref_idx * (1.0f - dt * dt);
	if (discriminant > 0.0f)
	{
		refracted = ref_idx * (v - n * dt) - n * sqrtf(discriminant);
		return true;
	}
	else
		return false;
}

__device__ inline float schlick(float cosine, float ref_idx)
{
	float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0)*powf(1.0f - cosine, 5.0f);
}

struct DefaultSky
{
	__device__ inline fvec3 operator()(const fvec3& dir)
	{
        float t = 0.5f*dir.y + 1.0f;
        fvec3 ret = { 1.0f - 0.5f*t, 1.0f - 0.3f*t, 1.0f };
        return ret;
	}
};

#endif


struct RayTracerView
{
	RNGState* rng_states;
	int rays_per_pixels;
	fvec3 origin;
	fvec3 upper_left;
	fvec3 ux;
	fvec3 uy;
	float lens_radius;
	float t0;
	float t1;

#ifdef DEVICE_ONLY
	template<class _TObj, class _TSky>
	__device__ inline void trace_pix(_TObj& obj, _TSky& sky, ImageView<fvec3>& image, int x, int y)
	{
		RNGState& rstate = rng_states[x + y * image.width()];

		// generate ray
		float fx = (float)x + rstate.rand01();
		float fy = (float)y + rstate.rand01();
		fvec3 pos_pix = upper_left + fx * ux + fy * uy;

		Ray r;
		r.origin = origin;	

		if (lens_radius > 0.0f)
		{
			fvec3 u = d_normalize(ux);
			fvec3 v = d_normalize(uy);
			fvec3 rd = lens_radius * rand_in_unit_disk(rstate);
			r.origin = r.origin + u * rd.x + v * rd.y;
		}

		r.direction = pos_pix - r.origin;
		r.direction = d_normalize(r.direction);
		r.time = t0;
		if (t1!=t0) r.time+=(t1-t0)*rstate.rand01();

#if 0
		fvec3 f_col;
		HitRecord record;
		if (obj.hit(r, record))
		{
			f_col = { 0.5f*(record.normal.x + 1.0f), 0.5f*(record.normal.y + 1.0f), 0.5f*(record.normal.z + 1.0f) };
		}
		else
		{
			//float t = 0.5f*r.direction.y + 1.0f;
			//f_col = { 1.0f - 0.5f*t, 1.0f - 0.3f*t, 1.0f };
			f_col = sky(r.direction);
		}
#else

		fvec3 f_att = { 1.0f, 1.0f, 1.0f };
		int depth = 0;
		while (f_att.x > 0.0001f || f_att.y > 0.0001f || f_att.z > 0.0001f)
		{
			if (depth >= 10)
			{
				f_att = { 0.0f, 0.0f, 0.0f };
				break;
			}
			HitRecord record;
			if (obj.hit(r, record))
			{
				r.origin = record.point;
				fvec3 fuzz_dir = record.normal;

				if (record.material.type == Material::lamertian || record.material.type == Material::metal)
				{
					f_att = f_att * record.material.color;					
				}

				if (record.material.type == Material::lamertian)
				{
					r.direction = d_normalize(rand_in_unit_sphere(rstate) + record.normal);
				}
				else if (record.material.type == Material::metal)
				{
					r.direction = reflect(r.direction, record.normal);
				}
				else if (record.material.type == Material::dielectric)
				{
					fvec3 refracted;
					float reflect_prob = 1.0f;

					if (refract(r.direction, record.normal, record.material.ref_idx, refracted))
					{
						float cosine = -d_dot(r.direction, record.normal);
						reflect_prob = schlick(cosine, record.material.ref_idx);
					}

					if (rstate.rand01() < reflect_prob)
					{
						r.direction = reflect(r.direction, record.normal);						
					}
					else
					{
						r.direction = refracted;
						fuzz_dir = { -fuzz_dir.x, -fuzz_dir.y, -fuzz_dir.z };
					}

					if (record.material.ref_idx > 1.0f)
					{
						f_att.x *= powf(record.material.color.x, record.t);
						f_att.y *= powf(record.material.color.y, record.t);
						f_att.z *= powf(record.material.color.z, record.t);
					}
				}
				if (record.material.fuzz > 0.0f && (record.material.type == Material::metal || record.material.type == Material::dielectric))
				{
					float f = record.material.fuzz; if (f > 1.0f) f = 1.0f;
					r.direction = r.direction + f * rand_in_unit_sphere(rstate);
					if (d_dot(r.direction, fuzz_dir) <= 0.0f)
					{
						f_att = { 0.0f, 0.0f, 0.0f };
						break;
					}
					r.direction = d_normalize(r.direction);
				}
			}
			else break;
			depth++;
		}

		fvec3 f_col = sky(r.direction) * f_att;

#endif	
		// accumulate pixel
		image.pix(x, y) = image.pix(x, y) + f_col;
	}

	template<class _TImg>
	__device__ inline void finalize_pix(ImageView<fvec3>& image_acc, _TImg& target, int x, int y)
	{
		fvec3 pix = image_acc.pix(x, y);
		pix = pix / (float)rays_per_pixels;

		// gamma correction
		pix.x = sqrtf(pix.x);
		pix.y = sqrtf(pix.y);
		pix.z = sqrtf(pix.z);
		if (pix.x < 0.0f) pix.x = 0.0f;
		if (pix.x > 1.0f) pix.x = 1.0f;
		if (pix.y < 0.0f) pix.y = 0.0f;
		if (pix.y > 1.0f) pix.y = 1.0f;
		if (pix.z < 0.0f) pix.z = 0.0f;
		if (pix.z > 1.0f) pix.z = 1.0f;

		target.pix(x, y) = from_fvec3<typename _TImg::value_t>(pix);
	}

#endif
};

struct Sphere
{
	fvec3 center;
	fvec3 velocity;
	float radius;
	Material material;

#ifdef DEVICE_ONLY
	__device__ inline bool hit(const Ray& r, HitRecord& record)
	{
		fvec3 center_now = center + velocity * r.time;
		fvec3 oc = center_now - r.origin;
		float proj = d_dot(oc, r.direction);
		float discriminant = radius*radius - (d_sqrlen(oc) - proj*proj);
		if (discriminant < 0.0f) return false;
		
		record.material = material;

		record.t = proj - sqrtf(discriminant);
		if (record.t > 0.001f)
		{
			record.point = r.point_at_parameter(record.t);
			fvec3 d = record.point - center_now;
			record.normal = d_normalize(d);
			record.material.ref_idx = 1.0f / material.ref_idx;
		}
		else
		{
			record.t = proj + sqrtf(discriminant);
			if (record.t > 0.0001f)
			{
				record.point = r.point_at_parameter(record.t);
				fvec3 d = center_now - record.point;
				record.normal = d_normalize(d);
			}
			else return false;
		}			
		
		return true;
	
	}
#endif

};

struct MultiSpheres
{
	VectorView<Sphere> spheres;

#ifdef DEVICE_ONLY
	__device__ inline bool hit(const Ray& r, HitRecord& record)
	{
		record.t = FLT_MAX;
		bool has_hit = false;
		for (size_t i = 0; i < spheres.size(); i++)
		{
			HitRecord record_i;
			if (spheres[i].hit(r, record_i))
			{
				if (record_i.t < record.t)	record = record_i;
				has_hit = true;
			}
		}
		return has_hit;
	}

#endif
	
};


#endif

