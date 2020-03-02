#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"


struct Spring;
class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles);
	
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();

	// add a spring.
	void addSpring(Spring spring);

	// get the sum of springs' force of a point.
	Vector3f getSpringForce(int i, vector<Vector3f> state);
	// get the velocity of a point.
	Vector3f getvelocity(int i, vector<Vector3f> state);
	// get the position of a point.
	Vector3f getPosition(int i, vector<Vector3f> state);
	// draw all springs.
	void drawSprings();
private:
	// points' mass
	float mass;
	// k for viscous drag
	float k4f;
	// k for all springs
	float k4Spring;
	// the static length of spring.
	float len_static_Spring;

	// vector which stores all springs.
	vector<Spring> m_springs;
};
struct Spring
{
	int leftnode;
	int rightnode;
	float static_length;
	float k_spring;
	Spring(int leftnode, int rightnode, float static_length, float k_spring) {
		this->leftnode = leftnode;
		this->rightnode = rightnode;
		this->static_length = static_length;
		this->k_spring = k_spring;
	}
};

#endif
