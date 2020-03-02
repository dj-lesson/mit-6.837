#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere: public Object3D
{
public:
	Sphere(){ 
		//unit ball at the center
		center = Vector3f(0, 0, 0);
		radius = 1.0f;
		material = NULL;
	}

	Sphere( Vector3f center , float radius , Material* material ):Object3D(material){
		this->center = center;
		this->radius = radius;
		this->material = material;
	}
	
	~Sphere(){
		if (material != NULL) delete material;
	}

	bool intersect( const Ray& r , Hit& h , float tmin){
		float a = Vector3f::dot(r.getDirection(), r.getDirection());
		float b = 2*Vector3f::dot(r.getDirection(), r.getOrigin()-center);
		float c = Vector3f::dot(r.getOrigin()-center, r.getOrigin()-center) - radius * radius;
		float delta = b * b - 4 * a*c;
		float t = FLT_MAX;
		if (delta == 0) t = (-b) / (2 * a);
		else if (delta > 0) {
			float t1 = (-b - sqrt(delta)) / (2 * a);
			float t2 = (-b + sqrt(delta)) / (2 * a);
			if (t1 >= tmin) t = t1;
			else if (t1<tmin && t2 >=tmin)  t = t2;
		}
		if (t < h.getT()) {
		h.set(t, material, (r.pointAtParameter(t) - center).normalized());
		return true;
		}
			
		return false;
	}

protected:
	Vector3f center;
	float radius;
	Material* material;
};


#endif
