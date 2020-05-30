import os
import sys
import site
import ThrustRTC as trtc
import CURandRTC as rndrtc
from PIL import Image
from cffi import FFI

ffi  = FFI()

ffi.cdef("""
int n_rtrtc_try_init();
void* n_dvimage_create(int width, int height);
int n_dvimage_width(void* cptr);
int n_dvimage_height(void* cptr);
void n_dvimage_to_bytes(void* cptr, char* bytes);
void* n_vec3_create(float x, float y, float z);
void n_vec3_destroy(void* cptr);
void* n_material_create(const char* type, void* ptr_color, float fuzz, float ref_idx);
void n_material_destroy(void* cptr);
void* n_dvsphere_create(void* ptr_center, float radius, void* ptr_material, void* ptr_velocity);
void* n_dv_multisphres_create(unsigned long long size, const void* const* ptrs);
void* n_raytracer_create(void* ptr_target, int rays_per_pixels);
void n_raytracer_destroy(void* cptr);
void n_raytracer_set_camera(void* cptr, void* ptr_lookfrom, void* ptr_lookat, void* ptr_vup, float vfov, float aperture, float focus_dist);
void n_raytracer_set_shutter(void* cptr, float t0, float t1);
int n_raytracer_trace(void* cptr, void* ptr_hitable, void* ptr_skybox);
""")

if os.name == 'nt':
    fn_rtrtc = 'PyRTRTC.dll'
elif os.name == "posix":
    fn_rtrtc = 'libPyRTRTC.so'

path_rtrtc = os.path.dirname(__file__)+"/"+fn_rtrtc

native = ffi.dlopen(path_rtrtc)

if native.n_rtrtc_try_init()==0:
    raise ImportError('cannot import RTRTC')

class DVImage(trtc.DeviceViewable):
    def __init__(self, width, height):
        self.m_cptr = native.n_dvimage_create(width, height)

    def size(self):
        width = native.n_dvimage_width(self.m_cptr)
        height = native.n_dvimage_height(self.m_cptr)
        return (width, height)

    def to_host(self):
        (width, height) = self.size()
        data = b'\x00'*width*height*3
        native.n_dvimage_to_bytes(self.m_cptr, data)
        return Image.frombytes('RGB', self.size(), data)

class _Vec3:
    def __init__(self, t):
        self.m_cptr = native.n_vec3_create(t[0],t[1],t[2])
    def __del__(self):
        native.n_vec3_destroy(self.m_cptr)

class _Material:
    def __init__(self, dict_mat):
        p_mtype = ffi.NULL
        if 'type' in dict_mat:
            p_mtype = dict_mat['type'].encode('utf-8')
        p_color = ffi.NULL
        if 'color' in dict_mat:
            o_color = _Vec3(dict_mat['color'])
            p_color = o_color.m_cptr
        fuzz = 0.0
        if 'fuzz' in dict_mat:
            fuzz = dict_mat['fuzz']
        ref_idx = 0.0
        if 'ref_idx' in dict_mat:
            ref_idx = dict_mat['ref_idx']
        self.m_cptr = native.n_material_create(p_mtype, p_color, fuzz, ref_idx)
    def __del__(self):
        native.n_material_destroy(self.m_cptr)

class DVSphere(trtc.DeviceViewable):
    def __init__(self, center, radius, material={'type': 'lamertian', 'color':(0.5, 0.5, 0.5)}, velocity = (0.0, 0.0, 0.0)):
        o_mat = _Material(material)
        o_center = _Vec3(center)
        o_velocity = _Vec3(velocity)
        self.m_cptr = native.n_dvsphere_create(o_center.m_cptr, radius, o_mat.m_cptr, o_velocity.m_cptr)

class DVMultiSpheres(trtc.DeviceViewable):
    def __init__(self, spheres):
        self.m_spheres = spheres
        cptr_spheres = [item.m_cptr for item in spheres]
        self.m_cptr = native.n_dv_multisphres_create(len(cptr_spheres), cptr_spheres)

class RayTracer:
    def __init__(self, target, rays_per_pixels = 100):
        self.m_target = target
        self.m_cptr = native.n_raytracer_create(target.m_cptr, rays_per_pixels)

    def __del__(self):
        native.n_raytracer_destroy(self.m_cptr)

    def set_camera(self, lookfrom, lookat, vup, vfov, aperture = 0.0, focus_dist = 1.0):
        o_lookfrom = _Vec3(lookfrom)
        o_lookat = _Vec3(lookat)
        o_vup = _Vec3(vup)
        native.n_raytracer_set_camera(self.m_cptr, o_lookfrom.m_cptr, o_lookat.m_cptr, o_vup.m_cptr, vfov, aperture, focus_dist)

    def set_shutter(self, t0, t1):
        native.n_raytracer_set_shutter(self.m_cptr, t0, t1)

    def trace(self, hitable, skybox = None):
        cptr_skybox = ffi.NULL
        if skybox != None:
            cptr_skybox = skybox.m_cptr
        native.n_raytracer_trace(self.m_cptr, hitable.m_cptr, cptr_skybox)


