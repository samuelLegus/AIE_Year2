#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <list>


struct Particle
{
	glm::vec3 pos;
	glm::vec3 vel;
	glm::vec4 color;
	float size;
	float lifetime;
	float lifespan;
};

struct ParticleVertex
{
	glm::vec4 pos;
	glm::vec4 color;
};

class ParticleEmitter
{
	public:

		ParticleEmitter();
		virtual ~ParticleEmitter();

		void Initialize(	unsigned int a_maxParticles, unsigned int a_emitRate,
							float a_lifetimeMin, float a_lifetimeMax,
							float a_velocityMin, float a_velocityMax,
							float a_startSize, float a_endSize,
							const glm::vec4& a_startColour, const glm::vec4& a_endColour);

		void Emit();
		void Update(float a_deltaTime, const glm::mat4& a_cameraTransform);
		void Draw();

	protected:

		std::list<Particle *>	m_aliveParticles;
		std::list<Particle *>	m_deadParticles;
		Particle *				m_particles;

		unsigned int			m_VBO;
		unsigned int			m_IBO;
		unsigned int			m_VAO;

		ParticleVertex *		m_vertexData;
		unsigned int *			m_indexData;

		glm::vec3				m_position;
		float					m_emitTimer;
		float					m_emitRate;
		float					m_maxLifespan;
		float					m_minLifespan;
		float					m_maxVel;
		float					m_minVel;
		float					m_startSize;
		float					m_endSize;
		glm::vec4				m_startColor;
		glm::vec4				m_endColor;
};
