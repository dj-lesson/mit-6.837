#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"
#include "pendulumSystem.h"

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem();
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();

	void addSpring(Spring spring);

	// get spring force\velocity\posiion by i,j
	Vector3f getSpringForce(int i, int j,vector<Vector3f> state);
	Vector3f getvelocity(int i, int j,vector<Vector3f> state);
	Vector3f getPosition(int i, int j,vector<Vector3f> state);
	//get spring force\velocity\posiion by total_index
	Vector3f getvelocity(int total_index, vector<Vector3f> state);
	Vector3f getPosition(int total_index, vector<Vector3f> state);
	Vector3f getSpringForce(int total_index, vector<Vector3f> state);
	// set position/velocity
	void setPosition(int total_index,Vector3f v);
	void setVelocity(int total_index, Vector3f v);
	
	// transform the (i,j) to total_index
	int indexOf(int i,int j);
	// draw springs/cloth
	void drawSprings();
	void drawclothes();
private:
	float mass;
	float k4f;
	float k4Spring;
	float len_static_Spring;

	vector<Spring> m_springs;
	vector<vector<int>> p2p_spring;
	vector<Vector3f> m_face_normals;
	Vector3f ball_r_vector;
	float ball_r;
};


#endif
