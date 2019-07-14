#include <Python.h>
#include <stdio.h>
#include <string.h>
#include "DVImage.h"
#include "rtrtc_built_in.h"
#include "DVSphere.h"
#include "RayTracer.h"
#include "functor.h"

static PyObject* n_dvimage_create(PyObject* self, PyObject* args)
{
	int width = (int)PyLong_AsLong(PyTuple_GetItem(args, 0));
	int height = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
	return PyLong_FromVoidPtr(new DVImage("u8vec3", width, height));
}

static PyObject* n_dvimage_size(PyObject* self, PyObject* args)
{
	DVImage* img = (DVImage*)PyLong_AsVoidPtr(PyTuple_GetItem(args, 0));
	PyObject* ret = PyTuple_New(2);
	PyTuple_SetItem(ret, 0, PyLong_FromLong((long)img->width()));
	PyTuple_SetItem(ret, 1, PyLong_FromLong((long)img->height()));
	return ret;
}

static PyObject* n_dvimage_to_bytes(PyObject* self, PyObject* args)
{
	DVImage* img = (DVImage*)PyLong_AsVoidPtr(PyTuple_GetItem(args, 0));
	int width = img->width();
	int height = img->height();
	PyObject* dataBuf = PyBytes_FromStringAndSize(nullptr, sizeof(u8vec3)*width * height);
	char* p;
	ssize_t size;
	PyBytes_AsStringAndSize(dataBuf, &p, &size);
	img->to_host(p);
	return dataBuf;
}

static fvec3 PyTuple_As_FVec3(PyObject* tuple)
{
	fvec3 ret;
	ret.x = (float)PyFloat_AsDouble(PyTuple_GetItem(tuple, 0));
	ret.y = (float)PyFloat_AsDouble(PyTuple_GetItem(tuple, 1));
	ret.z = (float)PyFloat_AsDouble(PyTuple_GetItem(tuple, 2));
	return ret;
}

static PyObject* n_dvsphere_create(PyObject* self, PyObject* args)
{
	fvec3 center = PyTuple_As_FVec3(PyTuple_GetItem(args, 0));
	float radius = (float)PyFloat_AsDouble(PyTuple_GetItem(args, 1));
	PyObject* py_material = PyTuple_GetItem(args, 2);
	Material material;
	PyObject* py_mat_type = PyDict_GetItemString(py_material, "type");
	if (py_mat_type != nullptr)
	{
		const char* str_type = PyUnicode_AsUTF8(py_mat_type);
		if (strcmp(str_type, "lamertian") == 0)
			material.type = Material::lamertian;
		else if (strcmp(str_type, "metal") == 0)
			material.type = Material::metal;
		else if (strcmp(str_type, "dielectric") == 0)
			material.type = Material::dielectric;
	}
	PyObject* py_mat_color = PyDict_GetItemString(py_material, "color");
	if (py_mat_color != nullptr)
		material.color = PyTuple_As_FVec3(py_mat_color);
	PyObject* py_mat_fuzz = PyDict_GetItemString(py_material, "fuzz");
	if (py_mat_fuzz != nullptr)
	{
		material.fuzz = (float)PyFloat_AsDouble(py_mat_fuzz);
	}
	PyObject* py_mat_ref_idx = PyDict_GetItemString(py_material, "ref_idx");
	if (py_mat_ref_idx != nullptr)
		material.ref_idx = (float)PyFloat_AsDouble(py_mat_ref_idx);
	
	return PyLong_FromVoidPtr(new DVSphere(center, radius, material));
}

static PyObject* n_dv_multisphres_create(PyObject* self, PyObject* args)
{
	PyObject* py_spheres = PyTuple_GetItem(args, 0);
	ssize_t count = PyList_Size(py_spheres);
	std::vector<const DVSphere*> spheres((size_t)count);
	for (ssize_t i = 0; i < count; i++)
		spheres[i] = (const DVSphere*)PyLong_AsVoidPtr(PyList_GetItem(py_spheres, i));
	return PyLong_FromVoidPtr(new DVMultiSpheres(spheres));
}

static PyObject* n_raytracer_create(PyObject* self, PyObject* args)
{
	DVImage* target = (DVImage*)PyLong_AsVoidPtr(PyTuple_GetItem(args, 0));
	int rays_per_pixels = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
	return PyLong_FromVoidPtr(new RayTracer(*target, rays_per_pixels));
}

static PyObject* n_raytracer_destroy(PyObject* self, PyObject* args)
{
	RayTracer* raytracer = (RayTracer*)PyLong_AsVoidPtr(PyTuple_GetItem(args, 0));
	delete raytracer;
	return PyLong_FromLong(0);
}

static PyObject* n_raytracer_set_camera(PyObject* self, PyObject* args)
{
	RayTracer* raytracer = (RayTracer*)PyLong_AsVoidPtr(PyTuple_GetItem(args, 0));
	fvec3 lookfrom = PyTuple_As_FVec3(PyTuple_GetItem(args, 1));
	fvec3 lookat = PyTuple_As_FVec3(PyTuple_GetItem(args, 2));
	fvec3 vup = PyTuple_As_FVec3(PyTuple_GetItem(args, 3));
	float vfov = (float)PyFloat_AsDouble(PyTuple_GetItem(args, 4));
	float aperture = (float)PyFloat_AsDouble(PyTuple_GetItem(args, 5));
	float focus_dist = (float)PyFloat_AsDouble(PyTuple_GetItem(args, 6));
	raytracer->set_camera(lookfrom, lookat, vup, vfov, aperture, focus_dist);
	return PyLong_FromLong(0);
}

static PyObject* n_raytracer_trace(PyObject* self, PyObject* args)
{
	RayTracer* raytracer = (RayTracer*)PyLong_AsVoidPtr(PyTuple_GetItem(args, 0));
	DeviceViewable* hitable = (DeviceViewable*)PyLong_AsVoidPtr(PyTuple_GetItem(args, 1));
	PyObject* py_skybox = PyTuple_GetItem(args, 2);
	Functor* skybox = nullptr;
	if (py_skybox != Py_None)
		skybox = (Functor*)PyLong_AsVoidPtr(py_skybox);
	return PyLong_FromLong(raytracer->trace(*hitable, skybox)?1:0);
}

static PyMethodDef s_Methods[] = {
	{ "n_dvimage_create", n_dvimage_create, METH_VARARGS, "" },
	{ "n_dvimage_size", n_dvimage_size, METH_VARARGS, "" },
	{ "n_dvimage_to_bytes", n_dvimage_to_bytes, METH_VARARGS, "" },
	{ "n_dvsphere_create", n_dvsphere_create, METH_VARARGS, "" },
	{ "n_dv_multisphres_create", n_dv_multisphres_create, METH_VARARGS, "" },
	{ "n_raytracer_create", n_raytracer_create, METH_VARARGS, "" },
	{ "n_raytracer_destroy", n_raytracer_destroy, METH_VARARGS, "" },
	{ "n_raytracer_set_camera", n_raytracer_set_camera, METH_VARARGS, "" },
	{ "n_raytracer_trace", n_raytracer_trace, METH_VARARGS, "" },
	0
};

static struct PyModuleDef cModPyDem = { PyModuleDef_HEAD_INIT, "RTRTC_module", "", -1, s_Methods };

PyMODINIT_FUNC PyInit_PyRTRTC(void)
{
	return PyModule_Create(&cModPyDem);
}

