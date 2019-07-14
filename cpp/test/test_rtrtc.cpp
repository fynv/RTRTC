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