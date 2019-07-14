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

