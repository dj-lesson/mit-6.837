#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include <vecmath.h>

using namespace std;

class ParticleSystem
{
public:

	ParticleSystem(int numParticles=0);

	int m_numParticles;
	
	
	// for a given state, evaluate derivative f(X,t)
	virtual vector<Vector3f> evalF(vector<Vector3f> state) = 0;
	
	// getter method for the system's state
	vector<Vector3f> getState(){ return m_vVecState; };
	
	// setter method for the system's state
	void setState(const vector<Vector3f>  & newState) { m_vVecState = newState; };
	
	virtual void draw() = 0;

	// switch on or switch off the wind. default is off.
	void switch_wind() { open_wind = !open_wind; printf("%s\n", "switch wind"); }
	// draw spring or cloth. default is cloth.
	void switch_draw_spring() { draw_spring = !draw_spring; printf("%s\n", "switch draw spring or clothes");}
	// switch moving the spring or cloth. default is off.
	void switch_move_open() { move_open = !move_open; printf("%s\n", "switch open move");}
	// change the velocity's direction to the negative one.
	void move_velocity_negative() { move_velocity = -move_velocity; }
	
protected:

	vector<Vector3f> m_vVecState;
	bool open_wind;
	bool draw_spring;
	bool move_open;
	float move_velocity;
	float move_distance;
	
};

#endif
