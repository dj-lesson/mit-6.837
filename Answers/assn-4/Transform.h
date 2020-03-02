#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
  Transform(){}
 Transform( const Matrix4f& m, Object3D* obj ):o(obj){
	 this->m = m;
  }
  ~Transform(){
  }
  bool intersect( const Ray& r , Hit& h , float tmin){
	  Matrix4f new_m = m.inverse();
	  Ray new_ray((new_m * Vector4f(r.getOrigin(), 1)).xyz(),( new_m * Vector4f(r.getDirection(), 0)).xyz());
	  if (o->intersect(new_ray, h, tmin)) {
		  h.set(h.getT(), h.getMaterial(), (new_m.transposed()*Vector4f(h.getNormal(), 0)).xyz().normalized());
		  return true;
	  }
	  return false;
  }

 protected:
  Object3D* o; //un-transformed object	
  Matrix4f m;
};

#endif //TRANSFORM_H
