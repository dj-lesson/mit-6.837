#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;
///TODO: implement this class.
///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
class Triangle: public Object3D
{
public:
	Triangle();
        ///@param a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m):Object3D(m){
		this->a = a;
		this->b = b;
		this->c = c;
		material = m;
          hasTex = false;
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin){
		Matrix3f A (
			a.x() - b.x(),a.x() - c.x(),ray.getDirection().x(),
			a.y() - b.y(),a.y() - c.y(),ray.getDirection().y(),
			a.z() - b.z(),a.z() - c.z(),ray.getDirection().z()
		);
		//A.transpose();
		Matrix3f T (
			a.x() - b.x(),a.x() - c.x(),a.x() - ray.getOrigin().x(),
			a.y() - b.y(),a.y() - c.y(),a.y() - ray.getOrigin().y(),
			a.z() - b.z(),a.z() - c.z(),a.z() - ray.getOrigin().z()
		);
		//T.transpose();
		Matrix3f Beta (
			a.x() - ray.getOrigin().x(),a.x() - c.x(),ray.getDirection().x(),
			a.y() - ray.getOrigin().y(),a.y() - c.y(),ray.getDirection().y(),
			a.z() - ray.getOrigin().z(),a.z() - c.z(),ray.getDirection().z()
		);
		//Beta.transpose();
		Matrix3f Gama(
			a.x() - b.x(),a.x() - ray.getOrigin().x(),ray.getDirection().x(),
			a.y() - b.y(),a.y() - ray.getOrigin().y(),ray.getDirection().y(),
			a.z() - b.z(),a.z() - ray.getOrigin().z(),ray.getDirection().z()
		);
		//Gama.transpose();
		float t = T.determinant() / A.determinant();
		float beta = Beta.determinant() / A.determinant();
		float gama = Gama.determinant() / A.determinant();
		float alpha = 1 - beta - gama;
		
		if (t > tmin && t < hit.getT()&& alpha>=0 && beta>=0 && gama>=0) {
			hit.set(t, material, (normals[0]*alpha+normals[1]*beta+normals[2]*gama).normalized());
			hit.setTexCoord(alpha * texCoords[0] + beta * texCoords[1] + gama * texCoords[2]);
			return true;
		}
		return false;
	}
	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];
protected:
	Vector3f a,b,c;
};

#endif //TRIANGLE_H
