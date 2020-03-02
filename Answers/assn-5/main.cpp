#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>
#include "RayTracer.h"
#include <time.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
  // Fill in your implementation here.

	SceneParser * sceneParser;
	Image* image=NULL;
	int base_image_width;
	int base_image_height;
	Image* depth_image=NULL;
	Image* normal_image = NULL;
	char* output_image_name=NULL;
	char* depth_image_name=NULL;
	char* normal_image_name = NULL;
	int depth_min = 0;
	int depth_max = 0;
	int bounces = 0;
	bool jittered = false;
	bool filter = false;
	bool shadows = false;
  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
	//a5 -input scene06_bunny_1k.txt -size 300 300 -output 6.bmp -shadows -bounces 4 -jitter -filter 
	//a5 -input scene10_sphere.txt -size 300 300 -output 10.bmp -shadows -bounces 4 -jitter -filter
	//a5 -input scene11_cube.txt -size 300 300 -output 11.bmp -shadows -bounces 4 -jitter -filter
    //a5 -input scene12_vase.txt -size 300 300 -output 12.bmp -shadows -bounces 4 -jitter -filter

  for( int argNum = 1; argNum < argc; ++argNum )
    {
      std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
	  if (!strcmp(argv[argNum], "-input")) {
		  sceneParser = new SceneParser(argv[argNum + 1]);
	  }
	  else if (!strcmp(argv[argNum], "-size")) {
		  base_image_width = atoi(argv[argNum + 1]);
		  base_image_height = atoi(argv[argNum + 2]);
	  }
	  else if (!strcmp(argv[argNum], "-output")) {
		  output_image_name = argv[argNum + 1];
	  }
	  else if (!strcmp(argv[argNum], "-depth")) {
		  depth_min = atoi(argv[argNum + 1]);
		  depth_max = atoi(argv[argNum + 2]);
		  depth_image_name = argv[argNum + 3];
	  }
	  else if (!strcmp(argv[argNum], "-normal")) {
		  normal_image_name = argv[argNum+1];
		  normal_image = new Image(image->Width(),image->Height());
	  }
	  else if (!strcmp(argv[argNum], "-bounces")) {
		  bounces = atoi(argv[argNum + 1]);
	  }
	  else if (!strcmp(argv[argNum], "-jitter")) {
		  jittered = true;
	  }
	  else if (!strcmp(argv[argNum], "-filter")) {
		  filter = true;
	  }
	  else if (!strcmp(argv[argNum], "-shadows")) {
		  shadows = true;
	  }
    }
    RayTracer rayTracer(sceneParser, bounces, shadows);
	
    
  // First, parse the scene using SceneParser.
  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene.  Write the color at the intersection to that
  // pixel in your output image.
	Camera *camera = sceneParser->getCamera();
	Group* group = sceneParser->getGroup();
	if (!jittered) {
		image = new Image(base_image_width, base_image_height);
		for (int i = 0; i < image->Width(); i++) {
			for (int j = 0; j < image->Height(); j++) {
				Ray ray = camera->generateRay(Vector2f(i*(1.0000001f / base_image_width), j*(1.0000001f / base_image_height)));
				Hit hit;
				image->SetPixel(i, j, rayTracer.traceRay(ray, camera->getTMin(), 0, 1.0, hit));
			}
		}
	}else{
		int width = 3 * base_image_width;
		int height = 3 * base_image_height;
		srand(time(0));
		Vector3f** sub_pixels = new Vector3f*[width];
		for (int i = 0; i < width; i++) {
			sub_pixels[i] = new Vector3f[height];
		}
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				Ray ray = camera->generateRay(Vector2f((i + rand() % 100 / (double)101)*(1.0000001f / width), (j + rand() % 100 / (double)101)*(1.0000001f / height)));
				Hit hit;
				sub_pixels[i][j] = rayTracer.traceRay(ray, camera->getTMin(), 0, 1.0, hit);
			}
		}
		if (!filter) {
			image = new Image(width, height);
			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					image->SetPixel(i, j, sub_pixels[i][j]);
				}
			}
		}else {
			Vector3f** pixels = new Vector3f*[base_image_width];
			for (int i = 0; i < base_image_height; i++) {
				pixels[i] = new Vector3f[base_image_height];
			}
			Vector3f** gaussian_pixels = new Vector3f*[width];
			for (int i = 0; i < width; i++) {
				gaussian_pixels[i] = new Vector3f[height];
			}
			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					gaussian_pixels[i][j] = 0.1201f*sub_pixels[i][(j - 2) > 0 ? j - 2 : 0] + 0.2339f*sub_pixels[i][(j - 1) > 0 ? j - 1 : 0] + 0.2931f*sub_pixels[i][j]
						+ 0.2339f*sub_pixels[i][(j + 1) % height] + 0.1201*sub_pixels[i][(j + 2) % height];
				}
			}
			for (int i = 0; i < base_image_width; i++) {
				for (int j = 0; j < base_image_height; j++) {
					pixels[i][j] = (gaussian_pixels[3 * i][3 * j] + gaussian_pixels[3 * i + 1][3 * j] + gaussian_pixels[3 * i + 2][3 * j] +
						gaussian_pixels[3 * i][3 * j + 1] + gaussian_pixels[3 * i + 1][3 * j + 1] + gaussian_pixels[3 * i + 2][3 * j + 1] +
						gaussian_pixels[3 * i][3 * j + 2] + gaussian_pixels[3 * i + 1][3 * j + 2] + gaussian_pixels[3 * i + 2][3 * j + 2]) / 9.0f;
				}
			}
			image = new Image(base_image_width, base_image_height);
			for (int i = 0; i < image->Width(); i++)
			{
				for (int j = 0; j < image->Height(); j++)
				{
					image->SetPixel(i, j, pixels[i][j]);
				}
			}
		}
	}
	image->SaveImage(output_image_name);
  return 0;
}

