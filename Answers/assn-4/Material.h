#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"
///TODO:
///Implement Shade function that uses ambient, diffuse, specular and texture
class Material
{
public:
	
 Material( const Vector3f& d_color ,const Vector3f& s_color=Vector3f::ZERO, float s=0):
  diffuseColor( d_color),specularColor(s_color), shininess(s)
  {
        	
  }

  virtual ~Material()
    {

    }

  virtual Vector3f getDiffuseColor() const 
  { 
    return  diffuseColor;
  }
    

  Vector3f Shade( const Ray& ray, const Hit& hit,
                  const Vector3f& dirToLight, const Vector3f& lightColor ) {
	  
	  float diffuse_temp = (Vector3f::dot(hit.getNormal().normalized(), dirToLight))>0? Vector3f::dot(hit.getNormal().normalized(), dirToLight):0;
	  //Vector3f diffuse_color(diffuse_temp*lightColor.x()*diffuseColor.x(), diffuse_temp*lightColor.y()*diffuseColor.y(), diffuse_temp*lightColor.z()*diffuseColor.z());
	  Vector3f diffuse_color;
	  if (t.valid()) {
		  Vector3f texture = t(hit.texCoord[0], hit.texCoord[1]);
		  diffuse_color= diffuse_temp * lightColor*texture;
	  }else{diffuse_color= diffuse_temp * lightColor*diffuseColor;}
	   

	  float costheta = Vector3f::dot(hit.getNormal().normalized(), dirToLight);
	  Vector3f r = 2 * costheta*hit.getNormal();
	  r=r-dirToLight;
	  //r.normalize();
	  Vector3f l = ray.getDirection();
	  l.negate();
	  float specular_temp = Vector3f::dot(l, r);
	  specular_temp = specular_temp > 0 ? specular_temp : 0;
	  specular_temp = pow(specular_temp, shininess);

	  //float specular_temp = Vector3f::dot(-ray.getDirection(), (Vector3f::dot(2*Vector3f::dot(hit.getNormal(), dirToLight),hit.getNormal())- dirToLight).normalized());
	  //specular_temp = specular_temp>0? pow(specular_temp, shininess):0;
	  //Vector3f specular_color(specular_temp*lightColor.x()*specularColor.x(), specular_temp*lightColor.y()*specularColor.y(), specular_temp*lightColor.z()*specularColor.z());
	  Vector3f specular_color = specularColor * specular_temp*lightColor;
      
	  return diffuse_color+specular_color; 
  }

  void loadTexture(const char * filename){
    t.load(filename);
  }
 protected:
  Vector3f diffuseColor;
  Vector3f specularColor;
  float shininess;
  Texture t;
};



#endif // MATERIAL_H
