#include "ParticleSystem.h"

ParticleEmitter::ParticleEmitter() :
	m_particles(nullptr),
	m_position(0, 0, 0),
	m_VAO(0), m_VBO(0), m_IBO(0),
	m_vertexData(nullptr), m_indexData(nullptr)
{

}

ParticleEmitter::~ParticleEmitter()
{
	delete[] m_particles;
	delete[] m_indexData;
	delete[] m_vertexData;

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);
}

void ParticleEmitter::Initialize(	unsigned int a_maxParticles, unsigned int a_emitRate,
									float a_lifetimeMin, float a_lifetimeMax,
									float a_velocityMin, float a_velocityMax,
									float a_startSize, float a_endSize,
									const glm::vec4& a_startColour, const glm::vec4& a_endColour)
{
	//setup emit timers
	m_emitTimer = 0;
	m_emitRate = 1.0f / a_emitRate;

	//store the variables that were passed in
	m_startColor = a_startColour;
	m_endColor = a_endColour;
	m_startSize = a_startSize;
	m_endSize = a_endSize;
	m_minVel = a_velocityMin;
	m_maxVel = a_velocityMax;
	m_minLifespan = a_lifetimeMin;
	m_maxLifespan = a_lifetimeMax;

	//create particle array and store them in the "dead" pool - hahahah badum tiss
	m_particles = new Particle[a_maxParticles];
	for (unsigned int i = 0; i < a_maxParticles; ++i)
	{
		m_deadParticles.push_back(&m_particles[i]);
	}

	m_vertexData = new ParticleVertex[a_maxParticles * 4];

	// create the index buffeer data for the particles
	// 6 indices per quad of 2 triangles
	// fill it now as it never changes
	m_indexData = new unsigned int[a_maxParticles * 6];
	for (unsigned int i = 0; i < a_maxParticles; ++i)
	{
		m_indexData[i * 6 + 0] = i * 4 + 0;
		m_indexData[i * 6 + 1] = i * 4 + 1;
		m_indexData[i * 6 + 2] = i * 4 + 2;

		m_indexData[i * 6 + 3] = i * 4 + 0;
		m_indexData[i * 6 + 4] = i * 4 + 2;
		m_indexData[i * 6 + 5] = i * 4 + 3;
	}

	// create opengl buffers
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, a_maxParticles * 4 * sizeof(ParticleVertex), m_vertexData, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_maxParticles * 6 * sizeof(unsigned int), m_indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // colour
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + sizeof(glm::vec4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ParticleEmitter::Emit()
{
	// No more dead particles to recycle? Break out of function.
	if (m_deadParticles.empty())
		return;

	// Remove particle from dead pool.
	Particle * particle = m_deadParticles.back();
	m_deadParticles.pop_back();

	// Starting location of the particle is equal to the emitters position
	particle->pos = m_position;

	// Randomize a lifespan for the particle
	particle->lifetime = 0;
	particle->lifespan = (rand() / (float)RAND_MAX) * (m_maxLifespan - m_minLifespan) + m_minLifespan;

	// Starting size and color of the particle are equal to the variables contained in our class

	particle->color = m_startColor;
	particle->size = m_startSize;

	// Randomizing a direction and strength of the particle's velocity
	float velocityStrength = (rand() / (float)RAND_MAX * (m_maxVel - m_minVel) + m_minVel);
	particle->vel.x = (rand() / (float)RAND_MAX) * 2 - 1;
	particle->vel.y = (rand() / (float)RAND_MAX) * 2 - 1;
	particle->vel.z = (rand() / (float)RAND_MAX) * 2 - 1;
	particle->vel = glm::normalize(particle->vel) * velocityStrength;

	m_aliveParticles.push_back(particle);
}

void ParticleEmitter::Update(float a_deltaTime, const glm::mat4& a_cameraTransform)
{
	// spawn particles
	m_emitTimer += a_deltaTime;
	while (m_emitTimer > m_emitRate)
	{
		Emit();
		m_emitTimer -= m_emitRate;
	}

	auto iter = m_aliveParticles.begin();
	unsigned int quad = 0;

	while (iter != m_aliveParticles.end())
	{
		Particle * particle = (*iter);

		particle->lifetime += a_deltaTime;
		if (particle->lifetime >= particle->lifespan)
		{
			m_deadParticles.push_back(particle);
			iter = m_aliveParticles.erase(iter); // The element AFTER the one that got erased.
		}
		else
		{
			particle->pos	+= particle->vel * a_deltaTime;
			particle->size	= glm::mix(m_startSize, m_endSize, particle->lifetime / particle->lifespan);
			particle->color = glm::mix(m_startColor, m_endColor, particle->lifetime / particle->lifespan); 

			float halfSize = particle->size * 0.5f;

			m_vertexData[quad * 4 + 0].pos = glm::vec4(halfSize, halfSize, 0, 1);
			m_vertexData[quad * 4 + 0].color = particle->color;
			
			m_vertexData[quad * 4 + 1].pos = glm::vec4(-halfSize, halfSize, 0, 1);
			m_vertexData[quad * 4 + 1].color = particle->color;

			m_vertexData[quad * 4 + 2].pos = glm::vec4(-halfSize, -halfSize, 0, 1);
			m_vertexData[quad * 4 + 2].color = particle->color;

			m_vertexData[quad * 4 + 3].pos = glm::vec4(halfSize, -halfSize, 0, 1);
			m_vertexData[quad * 4 + 3].color = particle->color;

			//fucking billboarding 
			glm::vec3 zAxis = glm::normalize(a_cameraTransform[3].xyz - particle->pos);
			glm::vec3 xAxis = glm::cross(glm::vec3(a_cameraTransform[1].xyz), zAxis);
			glm::vec3 yAxis = glm::cross(zAxis, xAxis);
			glm::mat3 billboard(xAxis, yAxis, zAxis);

			//FUCKING MMATTTTHHHHH
			m_vertexData[quad * 4 + 0].pos.xyz = billboard * m_vertexData[quad * 4 + 0].pos.xyz + particle->pos;
			m_vertexData[quad * 4 + 1].pos.xyz = billboard * m_vertexData[quad * 4 + 1].pos.xyz + particle->pos;
			m_vertexData[quad * 4 + 2].pos.xyz = billboard * m_vertexData[quad * 4 + 2].pos.xyz + particle->pos;
			m_vertexData[quad * 4 + 3].pos.xyz = billboard * m_vertexData[quad * 4 + 3].pos.xyz + particle->pos;

			++quad;
			++iter;
		}
	}

}

void ParticleEmitter::Draw()
{
	// sync the particle vertex buffer based on how many alive particles there are

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_aliveParticles.size() * 4 * sizeof(ParticleVertex), m_vertexData);

	// draw particles
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)m_aliveParticles.size() * 6, GL_UNSIGNED_INT, 0);
}
