#include "Assessment.h"

class ShadowTutorial : public Assessment
{
private:

	unsigned int m_shader;
	unsigned int m_depthShader;
	unsigned int m_shadowFramebuffer;

	int m_shadowWidth;
	int m_shadowHeight;

	glm::vec4 m_lightDirection;
	glm::mat4 m_depthViewMatrix;
	glm::mat4 m_depthProjectionMatrix;
	glm::mat4 m_depthMVP;
	glm::mat4 m_depthBiasMatrix;

	FBXFile * m_fbx;

	GLuint m_uDepthMVP;
	GLuint m_uDepthBiasMVP;
	GLuint m_uMVP;
	GLuint m_uShadowMap;
	GLuint m_uTexture;

	bool onCreate(int a_argc, char* a_argv[]);

	void onUpdate(float a_deltaTime);

	void onDraw();

	void onDestroy();

public:

	ShadowTutorial();

	virtual ~ShadowTutorial();

	void BuildProgram();
	void BuildDepthProgram();
	void CreateShadowBuffer();
	void CalculateLightAndShadowMatrix();

	void InitFBXSceneResource(FBXFile *a_pScene);

	void DestroyFBXSceneResource(FBXFile *a_pScene);

	void UpdateFBXSceneResource(FBXFile *a_pScene);

	void RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_view, glm::mat4 a_projection);

};