#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GL/glew.h>

struct BaseLight
{
	glm::vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;

	BaseLight()
	{
		Color = glm::vec3(0, 0, 0);
		AmbientIntensity = 0;
		DiffuseIntensity = 0;
	}
};

struct DirectionalLight : public BaseLight
{
	glm::vec3 Direction;

	DirectionalLight()
	{
		Direction = glm::vec3(0, 0, 0);
	}
};

struct PointLight : public BaseLight
{
	glm::vec3 Position;

	struct
	{
		float Constant;
		float Linear;
		float Exp;
	} Attenuation;

	PointLight()
	{
		Position = glm::vec3(0.0f, 0.0f, 0.0f);
		Attenuation.Constant = 1.0f;
		Attenuation.Linear = 0.0f;
		Attenuation.Exp = 0.0f;
	}

};

struct SpotLight : public PointLight
{
	glm::vec3 Direction;
	float Cutoff;

	SpotLight()
	{
		Direction = glm::vec3(0.0f, 0.0f, 0.0f);
		Cutoff = 0.0f;
	}
};

class Lighting 
{
public:

	static const unsigned int MAX_POINT_LIGHTS = 2;
	static const unsigned int MAX_SPOT_LIGHTS = 2;

	Lighting();

	virtual bool Init();

	void SetWVP(const glm::mat4& WVP);
	void SetWorldMatrix(const glm::mat4& WVP);
	void SetTextureUnit(unsigned int TextureUnit);
	void SetDirectionalLight(const DirectionalLight& Light);
	void SetPointLights(unsigned int NumLights, const PointLight* pLights);
	void SetSpotLights(unsigned int NumLights, const SpotLight* pLights);
	void SetEyeWorldPos(const glm::vec3& EyeWorldPos);
	void SetMatSpecularIntensity(float Intensity);
	void SetMatSpecularPower(float Power);

	GLuint GetUniformLocation(const char * Name);


private:

	GLuint m_shaderProg;

	GLuint m_WVPLocation;
	GLuint m_WorldMatrixLocation;
	GLuint m_samplerLocation;
	GLuint m_eyeWorldPosLocation;
	GLuint m_matSpecularIntensityLocation;
	GLuint m_matSpecularPowerLocation;
	GLuint m_numPointLightsLocation;
	GLuint m_numSpotLightsLocation;

	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Direction;
	} m_dirLightLocation;

	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Position;
		struct {
			GLuint Constant;
			GLuint Linear;
			GLuint Exp;
		} Atten;
	} m_pointLightsLocation[MAX_POINT_LIGHTS];

	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Position;
		GLuint Direction;
		GLuint Cutoff;
		struct {
			GLuint Constant;
			GLuint Linear;
			GLuint Exp;
		} Atten;
	} m_spotLightsLocation[MAX_SPOT_LIGHTS];
};
