from random import random
import RTRTC as rtrtc
import ThrustRTC as trtc

print("Initializing..")

img = rtrtc.DVImage(900, 600)

print("Generating a random scene..")

spheres = []
spheres.append(rtrtc.DVSphere ((0.0, -1000.0, 0.0), 1000.0, { 'type': 'lamertian', 'color': (0.5, 0.5, 0.5) }))
spheres.append(rtrtc.DVSphere ((0.0, 1.0, 0.0), 1.0, { 'type': 'dielectric', 'color': (1.0, 1.0, 1.0),  'fuzz': 0.0, 'ref_idx': 1.5}))
spheres.append(rtrtc.DVSphere ((-4.0, 1.0, 0.0), 1.0, { 'type': 'lamertian', 'color': (0.4, 0.2, 0.1)}))
spheres.append(rtrtc.DVSphere ((4.0, 1.0, 0.0), 1.0, { 'type': 'metal', 'color': (0.7, 0.6, 0.5),  'fuzz': 0.0}))

for a in range(-11, 11):
	for b in range (-11, 11):
		choose_mat = random()
		center = (a+0.9*random(), 0.2, b+0.9*random())
		dis = (center[0]-4.0)*(center[0]-4.0) + center[2]*center[2]
		if dis<1.0:
			continue
		dis = center[0]*center[0] + center[2]*center[2]
		if dis<1.0:
			continue
		if choose_mat<0.75:
			spheres.append(rtrtc.DVSphere (center, 0.2, { 'type': 'lamertian', 'color': (random()*random(), random()*random(), random()*random())}))
		elif choose_mat<0.90:
			spheres.append(rtrtc.DVSphere (center, 0.2, { 'type': 'metal', 'color': (0.5*(1.0+random()), 0.5*(1.0+random()), 0.5*(1.0+random())), 'fuzz': 0.0}))
		else:
			spheres.append(rtrtc.DVSphere (center, 0.2, { 'type': 'dielectric', 'color': (1.0, 1.0, 1.0),  'fuzz': 0.0, 'ref_idx': 1.5}))


mul_sph = rtrtc.DVMultiSpheres(spheres)

print("Rendering the scene..")

sky = trtc.Functor({}, ['dir'], 
	'''
	    float t = 0.5f*dir.y + 1.0f;
	    fvec3 ret = { 2.0f - 1.0f*t, 2.0f - 0.6f*t, 2.0f };
	    return ret;
	''')

raytracer = rtrtc.RayTracer(img)
raytracer.set_camera( (15.0, 3.0, 3.0), (0.0, 0.0, 0.0), (0.0, 1.0, 0.0), 20.0, 0.2, 12.0);
raytracer.trace(mul_sph, sky);

himg = img.to_host()
himg.save("scene1.tga")

print("Done.")
