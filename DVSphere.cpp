#include <vector>
#include "initialize.h"
#include "DVSphere.h"

DVSphere::DVSphere(fvec3 center, float radius, Material material, fvec3 velocity)
{
	rtrtc_init();
	m_center = center;
	m_radius = radius;
	m_material = material;
	m_velocity = velocity;
	m_name_view_cls = "Sphere";
}


ViewBuf DVSphere::view() const
{
	ViewBuf ret(sizeof(Sphere));
	*(Sphere*)ret.data() = { m_center, m_velocity, m_radius, m_material };
	return ret;
}

class VecSphereView : public std::vector<Sphere>
{
public:
	VecSphereView(const std::vector<const DVSphere*>& spheres)
	{
		resize(spheres.size());
		for (size_t i = 0; i < spheres.size(); i++)
			at(i) = *(Sphere*)spheres[i]->view().data();
	}

};

DVMultiSpheres::DVMultiSpheres(const std::vector<const DVSphere*>& spheres)
	: m_spheres("Sphere", spheres.size(), VecSphereView(spheres).data())
{
	rtrtc_init();
	m_name_view_cls = "MultiSpheres";
}

ViewBuf DVMultiSpheres::view() const
{
	return m_spheres.view();
}
