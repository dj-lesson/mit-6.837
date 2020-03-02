#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

#define EPSILON 0.001

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming)
{
	return (incoming - Vector3f::dot(incoming, normal) * 2 * normal).normalized();
}

bool transmittedDirection( const Vector3f& normal, const Vector3f& incoming, 
        float index_n, float index_nt, 
        Vector3f& transmitted)
{
	float sqrt_root = 1 - (index_n*index_n*(1 - (Vector3f::dot(incoming, normal) * Vector3f::dot(incoming, normal)))) / (index_nt*index_nt);
	if (sqrt_root > 0) {
		//have transmitted light
		transmitted = (index_n * (incoming - normal * Vector3f::dot(incoming, normal)) / index_nt) - normal * sqrt(sqrt_root);
		return true;
	}
	else {
		//all reflection, no transmitted light
		return false;
	}
}

RayTracer::RayTracer( SceneParser * scene, int max_bounces ,bool shadows
  //more arguments if you need...
                      ) :
  m_scene(scene)

{
  g=scene->getGroup();
  m_maxBounces = max_bounces;
  this->shadows = shadows;
}

RayTracer::~RayTracer()
{
}

Vector3f RayTracer::traceRay( Ray& ray, float tmin, int bounces,
        float refr_index, Hit& hit ) const
{
	if (bounces > m_maxBounces) {
		return Vector3f(0, 0, 0);
	}
	int numLights = m_scene->getNumLights();
	if (g->intersect(ray, hit, tmin + EPSILON)) {
		Vector3f color = m_scene->getAmbientLight()* hit.getMaterial()->getDiffuseColor();

		//1. cast shadow lights, produce shadows
		if (shadows) {
			for (int i = 0; i < numLights; i++) {
				Vector3f dirToLight;
				Vector3f lightColor;
				float distanceToLight;
				m_scene->getLight(i)->getIllumination(ray.pointAtParameter(hit.getT()), dirToLight, lightColor, distanceToLight);
				Ray shadow_ray = Ray(ray.pointAtParameter(hit.getT()), dirToLight);
				//todo
				Hit shadow_hit = Hit(distanceToLight, NULL, NULL);
				if (!g->intersect(shadow_ray, shadow_hit, tmin + EPSILON)) {
					//add this light's contribution
					color += hit.getMaterial()->Shade(ray, hit, dirToLight, lightColor);
				}
			}
		}
		//2. add the reflection color
		Ray reflection_ray = Ray(ray.pointAtParameter(hit.getT()), mirrorDirection(hit.getNormal().normalized(), ray.getDirection().normalized()));
		Hit reflection_hit = Hit();
		Vector3f reflectionColor = traceRay(reflection_ray, tmin, bounces + 1, refr_index, reflection_hit)*hit.getMaterial()->getSpecularColor();

		//3. add the refraction color
		Vector3f light_from_dir = ray.getDirection();
		Vector3f normal = hit.getNormal();
		if (hit.getMaterial()->getRefractionIndex() > 0) {
			//there exists the refraction
			float refraction_index_nt=0;
			//judge the case: 3.1  refraction from object to air or 
			//                3.2  refraction from air to object
			if (Vector3f::dot(normal, light_from_dir) < 0) {
				//light from air to object
				refraction_index_nt = hit.getMaterial()->getRefractionIndex();
			}else {
				//light from object to air
				refraction_index_nt = 1.0;
				normal = -normal;
			}
			Vector3f refractionDirection;
			bool has_refraction = transmittedDirection(normal, light_from_dir, refr_index, refraction_index_nt, refractionDirection);
			float c = (refr_index >= refraction_index_nt) ? abs(Vector3f::dot(light_from_dir, normal)) : abs(Vector3f::dot(refractionDirection, normal));
			float Ro = pow((refraction_index_nt - refr_index) / (refraction_index_nt + refr_index), 2);
			float R = Ro + (1 - Ro)*pow(1 - c, 5);
			if (has_refraction) {
				// there exists the refraction ,not only reflection
				Ray refractionRay = Ray(ray.pointAtParameter(hit.getT()), refractionDirection.normalized());
				Hit refractionHit;
				Vector3f refractionColor = traceRay(refractionRay, tmin, bounces + 1, refraction_index_nt, refractionHit)*hit.getMaterial()->getSpecularColor();
				color += R * reflectionColor + (1 - R)*refractionColor;
			}
			else {
				// only reflection, no refraction
				color += reflectionColor;
			}
		}else {
			color += reflectionColor;
		}

		return color;
	}
	else {
		return m_scene->getBackgroundColor(ray.getDirection());
	}
}
