
#include "pendulumSystem.h"

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	m_numParticles = numParticles;
	this->mass = 0.1f;
	this->k4f = 0.01f;
	this->k4Spring = 3.0f;
	this->len_static_Spring = 0.2f;

	// add default start point(0,0,0)
	m_vVecState.push_back(Vector3f(0,0,0));
	m_vVecState.push_back(Vector3f(0, 0, 0));

	for (int i = 1; i < m_numParticles; i++) {
		// for this system, we care about the position and the velocity
		// all of the points' velocity are (0,0,0)
		// create a spring between point_i and point_(i-1)
		m_vVecState.push_back(Vector3f(i * len_static_Spring,0, 0));
		m_vVecState.push_back(Vector3f(0, 0, 0));
		m_springs.push_back(Spring(i,i-1,len_static_Spring,k4Spring));
	}
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	// half of f store the velocity of states
	// the other store the a=F(x,v)
	vector<Vector3f> f;

	for (int i = 0; i < 2 * m_numParticles; i++) {
		// push the velocity
		if (i % 2 == 0) { f.push_back(state[i+1]); continue;}

		// make the first point's force zero.
		if (i == 1) { 
			f.push_back(Vector3f::ZERO); 
			continue;
		}

		// F = G + f + spring_force
		// f(X,t)=(1/mass)*F
		// 1.add G
		Vector3f Force = (-mass) * Vector3f(0, 9.8f, 0);
		// 2.add f
		Force = ((-k4f)* state[i]) + Force;
		// 3.add spring force
		Force = Force + getSpringForce(i/2,state);
		// 4.divide by mass
		Force = (1 / mass)*Force;
		f.push_back(Force);
	}
	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos=m_vVecState[2*i] ;//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
	}
	drawSprings();
}

void PendulumSystem::addSpring(Spring spring)
{
	m_springs.push_back(spring);
}

Vector3f PendulumSystem::getSpringForce(int i, vector<Vector3f> state){
	// visit all springs
	//get the sum of the springs' force which links the i-th point.
	Vector3f spring_force = Vector3f::ZERO;
	for (int j = 0; j < m_springs.size(); j++) {
		if (m_springs[j].leftnode == i) {
			Vector3f d = getPosition(i,state) - getPosition(m_springs[j].rightnode,state);
			spring_force = spring_force+(-m_springs[j].k_spring)*(d.abs() - m_springs[j].static_length)*(d.normalized());
		}
		else if (m_springs[j].rightnode == i) {
			Vector3f d = getPosition(i,state) - getPosition(m_springs[j].leftnode, state);
			spring_force = spring_force + ((-m_springs[j].k_spring)*(d.abs() - m_springs[j].static_length))*(d.normalized());
		}
	}
	return spring_force;
}

Vector3f PendulumSystem::getvelocity(int i, vector<Vector3f> state)
{
	return state[2*i+1];
}

Vector3f PendulumSystem::getPosition(int i, vector<Vector3f> state)
{
	return state[2*i];
}

void PendulumSystem::drawSprings()
{
	glLineWidth(10);
	glBegin(GL_LINES);
	
	for (int i=0;i<m_springs.size();i++)
	{
		glVertex3f(getPosition(m_springs[i].leftnode,getState()).x(), getPosition(m_springs[i].leftnode, getState()).y(), getPosition(m_springs[i].leftnode, getState()).z());
		glVertex3f(getPosition(m_springs[i].rightnode, getState()).x(), getPosition(m_springs[i].rightnode, getState()).y(), getPosition(m_springs[i].rightnode, getState()).z());
	}
	glEnd();
}
