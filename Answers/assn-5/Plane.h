#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
using namespace std;
///TODO: Implement Plane representing an infinite plane
///choose your representation , add more fields and fill in the functions
class Plane: public Object3D
{
public:
	Plane(){}
	Plane( const Vector3f& normal , float d , Material* m):Object3D(m){
		this->normal = normal;
		this->d = -d;
		this->m = m;
	}
	~Plane(){}
	bool intersect( const Ray& r , Hit& h , float tmin){
		float t = -(d + Vector3f::dot(normal, r.getOrigin())) / Vector3f::dot(normal, r.getDirection());
		if (t > tmin && t < h.getT()) {
			h.set(t, m, normal);
			return true;
		}
		return false;
	}

protected:
	Vector3f normal;
	float d;
	Material* m;
};
#endif //PLANE_H
		

