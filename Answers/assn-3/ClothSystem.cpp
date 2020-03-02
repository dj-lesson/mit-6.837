#include "ClothSystem.h"

//TODO: Initialize here
ClothSystem::ClothSystem()
{
	m_numParticles = 14;
	this->mass = 0.008f;
	this->k4f = 0.01f;
	this->k4Spring = 2.0f;
	this->len_static_Spring = 0.2f;
	float k4shear = k4Spring;
	float k4flex = 2*k4Spring;

	//this->mass = 0.05f;
	//this->k4f = 0.001f;
	//this->k4Spring = 30.0f;
	//this->len_static_Spring = 0.2f;

	ball_r_vector = Vector3f(1, -4, 0);
	ball_r = 1.5;

	//  1.  initialize every face's normal with (0,0,0)
	for (int i=0;i<m_numParticles*m_numParticles;i++)
	{
		m_face_normals.push_back(Vector3f(0));
	}

	//  2.  initialize the state. odd index stores the position, even index stores the velocity.
	for (int i=0;i<m_numParticles;i++)
	{
		for (int j=0;j<m_numParticles;j++)
		{
			//push position and velocity
			m_vVecState.push_back(Vector3f(j*len_static_Spring, 0, i*len_static_Spring));
			m_vVecState.push_back(Vector3f(0, 0, 0));
			//push p4p spring 
			p2p_spring.push_back(vector<int>());
		}
	}
	//  3.  add structural springs
	for (int i = 0; i < m_numParticles; i++) {
		for (int j = 0; j < m_numParticles-1; j++) {
			addSpring(Spring(indexOf(i,j),indexOf(i,j+1),len_static_Spring,k4Spring));
		}
	}
	for (int i = 0; i < m_numParticles; i++) {
		for (int j = 0; j < m_numParticles-1; j++) {
			addSpring(Spring(indexOf(j, i), indexOf(j + 1, i), len_static_Spring, k4Spring));
		}
	}
	//  4.  add shear springs
	for (int i = 0; i < m_numParticles - 1; i++)
	{
		for (int j = 0; j < m_numParticles - 1; j++)
		{
			addSpring(Spring(indexOf(i, j), indexOf(i + 1, j + 1), sqrt(2.0f)*len_static_Spring, k4shear));
			addSpring(Spring(indexOf(i + 1, j), indexOf(i, j + 1), sqrt(2.0f)*len_static_Spring, k4shear));
		}
	}
	//  5.  add flex springs
	for (int i = 0; i < m_numParticles; i++) {
		for (int j = 0; j < m_numParticles - 2; j++) {
			addSpring(Spring(indexOf(i, j), indexOf(i, j + 2), 2*len_static_Spring, k4flex));
		}
	}
	for (int i = 0; i < m_numParticles; i++) {
		for (int j = 0; j < m_numParticles - 2; j++) {
			addSpring(Spring(indexOf(j, i), indexOf(j + 2, i), 2*len_static_Spring, k4flex));
		}
	}
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
	// half of f store the velocity of states
	// the other store the a=F(x,v)
	vector<Vector3f> f;
	for (int i = 0; i < 2 * m_numParticles * m_numParticles; i++) {

		//move the top 2 point.
		if (move_open && (i == 0 || i == 2 * m_numParticles - 2)) {
			if (getPosition(i, state).z() < -move_distance || getPosition(i, state).z() > move_distance) move_velocity_negative();
			f.push_back(Vector3f(0,0,move_velocity));
			continue;
		}

		if (i % 2 == 0) { f.push_back(state[i + 1]); continue; }

		if (i == 1||i==2*m_numParticles-1) {
			f.push_back(Vector3f::ZERO);
			continue;
		}
		Vector3f Force = (-mass) * Vector3f(0, 9.8f, 0);
		Force = ((-k4f)* state[i]) + Force;
		Force = Force + getSpringForce(i / 2, state);
		
		//add wind force.
		int row = (i / 2) / m_numParticles;
		int column = (i / 2) % m_numParticles;
		if (open_wind && row>2 && row<m_numParticles-1 && column>0 && column < m_numParticles - 1) {
			Force = Force + Vector3f(0, 0, -0.04f+(rand()%3-1)*0.05f);
		}
		Force = (1 / mass)*Force;
		f.push_back(Force);
	}
	return f;
}

///TODO: render the system (ie draw the particles)
void ClothSystem::draw()
{
	// draw the ball
	for (int i = 0; i < m_numParticles*m_numParticles; i++) {
		if ((getPosition(i, m_vVecState) - ball_r_vector).abs() < ball_r) {
			setPosition(i, ball_r_vector + ((getPosition(i, m_vVecState) - ball_r_vector).normalized())*(ball_r + 0.1f));
			setVelocity(i, Vector3f(0, 0, 0));
		}
	}

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1.0, 1.0, 0.0);
	glPushMatrix();
	glTranslatef(1, -4, 0);
	glutSolidSphere(1.5, 40, 50);
	glPopMatrix();

	// draw spring or cloth.
	if (draw_spring) drawSprings();
	else drawclothes();

	glDisable(GL_COLOR_MATERIAL);
}
void ClothSystem::drawSprings()
{
	glLineWidth(1);
	glBegin(GL_LINES);

	//glColor3f(0,0,0);
	for (int i = 0; i < 2*m_numParticles*(m_numParticles-1); i++)
	{
		Vector3f v1=getPosition(m_springs[i].leftnode, getState());
		Vector3f v2=getPosition(m_springs[i].rightnode, getState());
		glVertex3f(v1.x(), v1.y(), v1.z());
		glVertex3f(v2.x(), v2.y(), v2.z());
	}
	glEnd();
}

void ClothSystem::drawclothes()
{
	
	glPushMatrix();
	glNormal3f(0, 0, 1);
	glColor3f(1.0f, 1.0f, 1.0f);
	Vector3f v1;
	Vector3f v2;
	Vector3f v3;
	Vector3f v4;
	Vector3f normal1;
	Vector3f normal2;
	

	for (int i=0;i<m_numParticles*m_numParticles;i++)
	{
		m_face_normals[i] = Vector3f(0);
	}
	for (int i=0;i<m_numParticles-1;i++)
	{
		for (int j=0;j<m_numParticles-1;j++)
		{
			v1 = getPosition(i, j, m_vVecState);
			v2 = getPosition(i, j + 1, m_vVecState);
			v3 = getPosition(i + 1, j + 1, m_vVecState);
			v4 = getPosition(i + 1, j, m_vVecState);
			
			normal1 = Vector3f::cross(v2 - v3,v1 - v2).normalized();
			normal2 = Vector3f::cross(v3 - v4,v1 - v3).normalized();

			m_face_normals[indexOf(i, j)] = m_face_normals[indexOf(i, j)] + normal1+normal2;
			m_face_normals[indexOf(i, j+1)] = m_face_normals[indexOf(i, j+1)] + normal1;
			m_face_normals[indexOf(i+1, j+1)] = m_face_normals[indexOf(i+1, j+1)] + normal1+normal2;
			m_face_normals[indexOf(i+1, j)] = m_face_normals[indexOf(i+1, j)] + normal2;
			
		}
	}
	for (int i = 0; i < m_numParticles*m_numParticles; i++)
	{
		m_face_normals[i].normalize();
	}

	glBegin(GL_TRIANGLES);
	for (int i=0;i<m_numParticles-1;i++)
	{
		for (int j=0;j<m_numParticles-1;j++)
		{
			v1 = getPosition(i, j,m_vVecState);
			v2 = getPosition(i, j + 1,m_vVecState);
			v3 = getPosition(i + 1, j + 1, m_vVecState);
			v4 = getPosition(i + 1, j, m_vVecState);
			glNormal3f(m_face_normals[indexOf(i, j)].x(), m_face_normals[indexOf(i, j)].y(), m_face_normals[indexOf(i, j)].z());
			glVertex3f(v1.x(), v1.y(), v1.z());
			glNormal3f(m_face_normals[indexOf(i, j+1)].x(), m_face_normals[indexOf(i, j+1)].y(), m_face_normals[indexOf(i, j+1)].z());
			glVertex3f(v2.x(), v2.y(), v2.z());
			glNormal3f(m_face_normals[indexOf(i+1, j+1)].x(), m_face_normals[indexOf(i+1, j+1)].y(), m_face_normals[indexOf(i+1, j+1)].z());
			glVertex3f(v3.x(), v3.y(), v3.z());
			glNormal3f(m_face_normals[indexOf(i, j)].x(), m_face_normals[indexOf(i, j)].y(), m_face_normals[indexOf(i, j)].z());
			glVertex3f(v1.x(), v1.y(), v1.z());
			glNormal3f(m_face_normals[indexOf(i + 1, j + 1)].x(), m_face_normals[indexOf(i + 1, j + 1)].y(), m_face_normals[indexOf(i + 1, j + 1)].z());
			glVertex3f(v3.x(), v3.y(), v3.z());
			glNormal3f(m_face_normals[indexOf(i+1, j)].x(), m_face_normals[indexOf(i+1, j)].y(), m_face_normals[indexOf(i+1, j)].z());
			glVertex3f(v4.x(), v4.y(), v4.z());
		}
	}
	glEnd();
	glPopMatrix();
	
}


void ClothSystem::addSpring(Spring spring)
{
	m_springs.push_back(spring);
	p2p_spring[spring.leftnode].push_back(m_springs.size()-1);
	p2p_spring[spring.rightnode].push_back(m_springs.size()-1);
}

Vector3f ClothSystem::getSpringForce(int i, int j,vector<Vector3f> state)
{
	return getSpringForce(indexOf(i, j), state);
}

Vector3f ClothSystem::getvelocity(int i, int j,vector<Vector3f> state)
{
	return state[2*indexOf(i,j)+1];
}

Vector3f ClothSystem::getPosition(int i,int j, vector<Vector3f> state)
{
	return state[2*indexOf(i,j)];
}

Vector3f ClothSystem::getvelocity(int total_index, vector<Vector3f> state)
{
	return state[2*total_index+1];
}

Vector3f ClothSystem::getPosition(int total_index, vector<Vector3f> state)
{
	return state[2*total_index];
}

void ClothSystem::setPosition(int total_index, Vector3f v)
{
	m_vVecState[2 * total_index] = v;
}

void ClothSystem::setVelocity(int total_index, Vector3f v)
{
	m_vVecState[2 * total_index + 1] = v;
}

Vector3f ClothSystem::getSpringForce(int total_index, vector<Vector3f> state)
{
	Vector3f spring_force = Vector3f::ZERO;
	//for (int j = 0; j < m_springs.size(); j++) {
	//	if (m_springs[j].leftnode == total_index) {
	//		Vector3f d = getPosition(total_index, state) - getPosition(m_springs[j].rightnode, state);
	//		spring_force = spring_force + (-m_springs[j].k_spring)*(d.abs() - m_springs[j].static_length)*(d.normalized());
	//	}
	//	else if (m_springs[j].rightnode == total_index) {
	//		Vector3f d = getPosition(total_index, state) - getPosition(m_springs[j].leftnode, state);
	//		spring_force = spring_force + ((-m_springs[j].k_spring)*(d.abs() - m_springs[j].static_length))*(d.normalized());
	//	}
	//}
	vector<int> p_vector_springs = p2p_spring[total_index];
	for (int j = 0; j <p_vector_springs.size(); j++)
	{
		Vector3f d;
		if (m_springs[p_vector_springs[j]].leftnode == total_index) {
			d = getPosition(total_index, state) - getPosition(m_springs[p_vector_springs[j]].rightnode, state);
		}
		else {
			d = getPosition(total_index, state) - getPosition(m_springs[p_vector_springs[j]].leftnode, state);
		}
		spring_force = spring_force + ((-m_springs[p_vector_springs[j]].k_spring)*(d.abs() - m_springs[p_vector_springs[j]].static_length))*(d.normalized());
	}
	return spring_force;
}

int ClothSystem::indexOf(int i, int j)
{
	return i*m_numParticles+j;
}

