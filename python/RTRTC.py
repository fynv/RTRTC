import PyRTRTC as native
import ThrustRTC as trtc
from PIL import Image

class DVImage(trtc.DeviceViewable):
	def __init__(self, width, height):
		self.m_cptr = native.n_dvimage_create(width, height)

	def size(self):
		return native.n_dvimage_size(self.m_cptr)

	def to_host(self):
		data = native.n_dvimage_to_bytes(self.m_cptr)
		return Image.frombytes('RGB', self.size(), data)

class DVSphere(trtc.DeviceViewable):
	def __init__(self, center, radius, material={"type": "lamertian", "color":(0.5, 0.5, 0.5)}, velocity = (0.0, 0.0, 0.0)):
		self.m_cptr = native.n_dvsphere_create(center, radius, material, velocity)

class DVMultiSpheres(trtc.DeviceViewable):
	def __init__(self, spheres):
		self.m_spheres = spheres
		cptr_spheres = [item.m_cptr for item in spheres]
		self.m_cptr = native.n_dv_multisphres_create(cptr_spheres)

class RayTracer:
	def __init__(self, target, rays_per_pixels = 100):
		self.m_target = target
		self.m_cptr = native.n_raytracer_create(target.m_cptr, rays_per_pixels)

	def __del__(self):
		native.n_raytracer_destroy(self.m_cptr)

	def set_camera(self, lookfrom, lookat, vup, vfov, aperture = 0.0, focus_dist = 1.0):
		native.n_raytracer_set_camera(self.m_cptr, lookfrom, lookat, vup, vfov, aperture, focus_dist)

	def set_shutter(self, t0, t1):
		native.n_raytracer_set_shutter(self.m_cptr, t0, t1)

	def trace(self, hitable, skybox = None):
		cptr_skybox = None
		if skybox != None:
			cptr_skybox = skybox.m_cptr
		native.n_raytracer_trace(self.m_cptr, hitable.m_cptr, cptr_skybox)




