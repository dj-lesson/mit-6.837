#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay( const Vector2f& point ) = 0 ; 
	
	virtual float getTMin() const = 0 ; 
	virtual ~Camera(){}
protected:
	Vector3f center; 
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;

};

///TODO: Implement Perspective camera
///Fill in functions and add more fields if necessary
class PerspectiveCamera: public Camera
{
public:
	PerspectiveCamera(const Vector3f& center, const Vector3f& direction,const Vector3f& up , float angle){
		this->center = center;
		this->horizontal = Vector3f::cross(direction, up).normalized();
		this->up = Vector3f::cross(horizontal, direction).normalized();
		this->direction = (0.5f/tanf(angle/2.0f))*(direction.normalized());
		this->angle = angle;
	}

	Ray generateRay( const Vector2f& point){
		float distance = 0.5f / tan(angle/2.0f);
		float bianchang = 2*tan(angle / 2.0f);
		//return Ray(center, direction + (point.x() - 0.5)*bianchang*horizontal + ((point.y() - 0.5)*bianchang*up));
		return Ray(center, (direction + (point.x() - 0.5)*horizontal + ((point.y() - 0.5)*up)).normalized());
	}

	virtual float getTMin() const { 
		return 0.0f;
	}

private:
	float angle;
};

#endif //CAMERA_H
