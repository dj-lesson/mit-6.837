#include "TimeStepper.hpp"

///TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> c_state = particleSystem->getState();
	vector<Vector3f> state_f = particleSystem->evalF(c_state);
	for (int i = 0; i < particleSystem->m_numParticles; i++) {
		c_state[i] = c_state[i]+stepSize * state_f[i];
	}
	particleSystem->setState(c_state);
}

///TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> c_state = particleSystem->getState();
	vector<Vector3f> state_f_0 = particleSystem->evalF(c_state);
	vector<Vector3f> state_f_1=c_state;
	for (int i = 0; i < particleSystem->m_numParticles; i++) {
		state_f_1[i] = c_state[i] + (stepSize * state_f_0[i]);
	}
	state_f_1 = particleSystem->evalF(state_f_1);
	for (int i = 0; i < particleSystem->m_numParticles; i++) {
		c_state[i] = c_state[i] + (stepSize / 2)*(state_f_0[i] + state_f_1[i]);
	}
	particleSystem->setState(c_state);
}

void RungeKutta4::takeStep(ParticleSystem * particleSystem, float stepSize)
{
	vector<Vector3f> c_state = particleSystem->getState();
	vector<Vector3f> state_f1 = particleSystem->evalF(c_state);
	vector<Vector3f> state_f2 = c_state;
	vector<Vector3f> state_f3 = c_state;
	vector<Vector3f> state_f4 = c_state;
	Vector3f k1, k2, k3, k4;
	for (int i = 0; i < particleSystem->m_numParticles; i++) {
		//k1 = stepSize*(state_f1[i]);
		//state_f2[i] = state_f2[i] + (0.5)*k1;
		//k2 = stepSize*(particleSystem->evalF(state_f2)[i]);
		//state_f3[i] = state_f3[i] + (0.5)*k2;
		//k3 = stepSize*(particleSystem->evalF(state_f3)[i]);
		//state_f4[i] = state_f4[i] + k3;
		//k4 = stepSize * (particleSystem->evalF(state_f4)[i]);
		//c_state[i] = c_state[i] + (1.0 / 6.0)*(k1 + 2 * k2 + 2 * k3 + k4);

		k1 = (state_f1[i]);
		state_f2[i] = state_f2[i] + (0.5f*stepSize)*k1;
		k2 = (particleSystem->evalF(state_f2)[i]);
		state_f3[i] = state_f3[i] + (0.5f*stepSize)*k2;
		k3 = (particleSystem->evalF(state_f3)[i]);
		state_f4[i] = state_f4[i] +stepSize* k3;
		k4 = (particleSystem->evalF(state_f4)[i]);
		c_state[i] = c_state[i] + (1.0f / 6.0f)*stepSize*(k1 + 2.0f * k2 + 2.0f * k3 + k4);
	}
	particleSystem->setState(c_state);
}
