# RTRTC

This is the 3rd of the series of GPU libraries based on the 
["NVRTC + dynamic-instantiation" paradigm](https://fynv.github.io/ProgrammingGPUAcrossTheLaunguageBoundaries.html).

This is just a toy GPU ray-tracer, which is basically a implementation of 
Peter Shirley's [Ray Tracing in a Weekend](https://github.com/petershirley/raytracinginoneweekend),
using my CUDA runtime compilation tools.

Ray-tracing Peter Shirley's "spheres" scene in 800x600 resolution, 1000 rays/pixels:
![alt text](docs/scene1_1000.tga)

This project depends on the first 2 of the series:

* [ThrustRTC](https://github.com/fynv/ThrustRTC), for infrastructures;
* [CURandRTC](https://github.com/fynv/CURandRTC), for random number generation.

## Installation

### Install from Source Code

You will need first get the source code of ThrustRTC and CURandRTC, and compile them.

Since RTRTC only targets for C++ and python, you only need to build for these 2 lauguages.

ThrustRTC:

```
$ git clone https://github.com/fynv/ThrustRTC.git
$ cd ThrustRTC
$ mkdir build_cpp
$ cd build_cpp
$ cmake ../cpp
$ make
$ make install
$ cd ..
$ mkdir build_python
$ cd build_python
$ cmake ../python
$ make
$ make install
$ cd ..
$ cd ..
```

CURandRTC:
```
$ git clone https://github.com/fynv/CURandRTC.git
$ cd CURandRTC
$ cp -r ../ThrustRTC/install .
$ mkdir build_cpp
$ cd build_cpp
$ cmake ../cpp
$ make
$ make install
$ cd ..
$ mkdir build_python
$ cd build_python
$ cmake ../python
$ make
$ make install
$ cd ..
$ cd ..
```

Then you can build RTRTC:
```
$ git clone https://github.com/fynv/RTRTC.git
$ RTRTC
$ cp -r ../CURandRTC/install .
$ mkdir build_cpp
$ cd build_cpp
$ cmake ../cpp -DBUILD_TESTS=true
$ make
$ make install
$ cd ..
$ mkdir build_python
$ cd build_python
$ cmake ../python -INCLUDE_TESTS=true
$ make
$ make install
$ cd ..
$ cd ..
```

You will get the library headers, binaries and examples in the "install" directory.


### Install RTRTC for Python from Pypi

Builds for Win64/Linux64 + Python 3.7 are available from [Pypi](https://pypi.org/project/RTRTC/)
If your environment matches, you can try:

```
$ pip3 install RTRTC
```
You will not get the C++ library, headers as well as all the test programs using this installation method.


### GitHub Release

Zip packages are available at:

[https://github.com/fynv/RTRTC/releases](https://github.com/fynv/RTRTC/releases)

These libraries should be used together with the ThrustRTC and CURandRTC libraries in the correpsonding languages.


## Examples of Usage

In C++:

```cpp
#include <stdio.h>
#include "DVImage.h"
#include "rtrtc_built_in.h"
#include "DVSphere.h"
#include "RayTracer.h"

int main()
{
	DVImage img("u8vec3", 800, 400);
	DVSphere sphere1({ 0.0f, 0.0f, -1.0f }, 0.5f, { Material::lamertian, {0.1f, 0.2f, 0.5f} });
	DVSphere sphere2({ 0.0f, -100.5f, -1.0f }, 100.0f, { Material::lamertian, {0.8f, 0.8f, 0.0f} });
	DVSphere sphere3({ 1.0f, 0.0f, -1.0f }, 0.5f, { Material::metal, {0.8f, 0.6f, 0.2f}, 0.3f });
	DVSphere sphere4({ -1.0f, 0.0f, -1.0f }, 0.5f, { Material::dielectric, {1.0f, 1.0f, 1.0f}, 0.0f, 1.5f });
	DVSphere sphere5({ -1.0f, 0.0f, -1.0f }, 0.45f, { Material::dielectric, {1.0f, 1.0f, 1.0f}, 0.0f, 1.0f/1.5f });
	DVMultiSpheres mul_sph({ &sphere1, &sphere2, &sphere3, &sphere4, &sphere5 });
	RayTracer raytracer(img);
	raytracer.set_camera({ 3, 3, 2 }, { 0, 0, -1 }, { 0,1,0 }, 20.0f, 0.5f, 5.2f);
	raytracer.trace(mul_sph);

	u8vec3 *h_img = new u8vec3[800 * 400];
	img.to_host(h_img);

	FILE *fp = fopen("rgb.raw", "wb");
	fwrite(h_img, sizeof(u8vec3), 800 * 400, fp);
	fclose(fp);

	delete[] h_img;

	return 0;
}
```

In Python:
```python
import RTRTC as rtrtc

img = rtrtc.DVImage(800, 400)

spheres = []
spheres.append(rtrtc.DVSphere ((0.0, 0.0, -1.0), 0.5, { "type": "lamertian", "color": (0.1, 0.2, 0.5) }))
spheres.append(rtrtc.DVSphere ((0.0, -100.5, -1.0), 100.0, { "type": "lamertian", "color": (0.8, 0.8, 0.0) }))

spheres.append(rtrtc.DVSphere ((1.0, 0.0, -1.0), 0.5, { "type": "metal", "color": (0.8, 0.6, 0.2), "fuzz": 0.3 }))
spheres.append(rtrtc.DVSphere ((-1.0, 0.0, -1.0), 0.5, { "type": "dielectric", "color": (1.0, 1.0, 1.0), "fuzz": 0.0, "ref_idx": 1.5 }))
spheres.append(rtrtc.DVSphere ((-1.0, 0.0, -1.0), 0.45, { "type": "dielectric", "color": (1.0, 1.0, 1.0), "fuzz": 0.0, "ref_idx": 1.0/1.5 }))

mul_sph = rtrtc.DVMultiSpheres(spheres)

raytracer = rtrtc.RayTracer(img)
raytracer.set_camera( (3.0, 3.0, 2.0), (0.0, 0.0, -1.0), (0.0, 1.0, 0.0), 20.0, 0.5, 5.2);
raytracer.trace(mul_sph);

himg = img.to_host()
himg.save("scene0.tga")

```

## License 

I've decided to license this project under ['"Anti 996" License'](https://github.com/996icu/996.ICU/blob/master/LICENSE)

Basically, you can use the code any way you like unless you are working for a 996 company.

[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)

