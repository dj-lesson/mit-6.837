#include "particleSystem.h"
ParticleSystem::ParticleSystem(int nParticles):m_numParticles(nParticles),open_wind(false),draw_spring(false),move_open(false),move_velocity(-2.0),move_distance(4.0){
	//default value
	//wind: off
	//draw: cloth
	//move: off
	//move velocity :-2.0
	//move distance: 4.0
}
