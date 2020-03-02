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

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
  // Fill in your implementation here.

	SceneParser * sceneParser;
	Image* image=NULL;
	Image* depth_image=NULL;
	Image* normal_image = NULL;
	char* output_image_name=NULL;
	char* depth_image_name=NULL;
	char* normal_image_name = NULL;
	int depth_min = 0;
	int depth_max = 0;
  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
	//-input scene01_plane.txt -size 200 200 -output 1.bmp 
	//-input scene02_cube.txt -size 200 200 -output 2.bmp 
	//-input scene03_sphere.txt -size 200 200 -output 3.bmp 
	//-input scene04_axes.txt -size 200 200 -output 4.bmp 
	// -input scene05_bunny_200.txt -size 200 200 -output 5.bmp 
	// -input scene06_bunny_1k.txt -size 200 200 -output 6.bmp 
	//-input scene07_shine.txt -size 200 200 -output 7.bmp
	//-input scene08_c.txt -size 200 200 -output 8.bmp 
	//-input scene09_s.txt -size 200 200 -output 9.bmp
  for( int argNum = 1; argNum < argc; ++argNum )
    {
      std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
	  if (!strcmp(argv[argNum], "-input")) {
		  sceneParser = new SceneParser(argv[argNum + 1]);
	  }
	  else if (!strcmp(argv[argNum], "-size")) {
		  image = new Image(atoi(argv[argNum + 1]), atoi(argv[argNum + 2]));
		  
		  depth_image = new Image(atoi(argv[argNum + 1]), atoi(argv[argNum + 2]));
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
    }
	
    
  // First, parse the scene using SceneParser.
  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene.  Write the color at the intersection to that
  // pixel in your output image.
	Camera *camera = sceneParser->getCamera();
	Group* group = sceneParser->getGroup();
	image->SetAllPixels(sceneParser->getBackgroundColor());
	int width = image->Width();
	int height = image->Height();
	for (int i = 0; i < image->Width(); i++) {
		for (int j = 0; j < image->Height(); j++) {
			Ray ray = camera->generateRay(Vector2f(i*(1.0f/width),j*(1.0f/height)));
			Hit hit;
			if (group->intersect(ray, hit, camera->getTMin())) {
				//image->SetPixel(i,j,Vector3f(1.0f, 1.0f, 1.0f));
				Vector3f color = sceneParser->getAmbientLight();
				for (int k = 0; k < sceneParser->getNumLights(); k++) {
					Vector3f lightdir;
					Vector3f lightcol;
					float a;
					sceneParser->getLight(k)->getIllumination(ray.pointAtParameter(hit.getT()), lightdir, lightcol, a);
					color = color + hit.getMaterial()->Shade(ray, hit, lightdir, lightcol);
				}

				image->SetPixel(i, j, color);
			}
			
			if (depth_image != NULL) {
				if (hit.getT() > depth_min && hit.getT() < depth_max) {
					float depth_degree = (hit.getT() - depth_min) / (depth_max - depth_min);
					depth_image->SetPixel(i, j, (depth_max - hit.getT()) / (depth_max - depth_min)*Vector3f(1.0f, 1.0f, 1.0f));
				}
			}
			if (normal_image != NULL) {
				
			}
			
		}
	}
	image->SaveImage(output_image_name);
	if (depth_image_name != NULL) {
		depth_image->SaveImage(depth_image_name);
	}
	

 
  ///TODO: below demonstrates how to use the provided Image class
  ///Should be removed when you start
 // Vector3f pixelColor (1.0f,0,0);
  ////width and height
  //Image image( 10 , 15 );
  //image.SetPixel( 5,5, pixelColor );
  //image.SaveImage("demo.bmp");
  return 0;
}

