#ifndef _Sphere_h
#define _Sphere_h

#include "RTRTC_api.h"
#include "DeviceViewable.h"
#include "DVVector.h"
#include "rtrtc_built_in.h"

class RT_RTC_API DVSphere : public DeviceViewable
{
public:
	DVSphere(fvec3 center, float radius, Material material = { Material::lamertian, {0.5f, 0.5f, 0.5f} }, fvec3 velocity = { 0.0f, 0.0f, 0.0f });

	virtual std::string name_view_cls() const;
	virtual ViewBuf view() const;

private:
	fvec3 m_center;
	float m_radius;
	Material m_material;
	fvec3 m_velocity;

};

class RT_RTC_API DVMultiSpheres : public DeviceViewable
{
public:
	DVMultiSpheres(const std::vector<const DVSphere*>& spheres);

	virtual std::string name_view_cls() const;
	virtual ViewBuf view() const;

private:
	DVVector m_spheres;
};

#endif
