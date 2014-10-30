#include "Assessment.h"

class ShadowTutorial : public Assessment
{
private:

	unsigned int m_shader;
	unsigned int m_depthShader;
	unsigned int m_shadowFramebuffer;
	unsigned int m_shadowTexture;
	
	int m_shadowWidth;
	int m_shadowHeight;

	glm::vec4 m_lightDirection;
	glm::mat4 m_depthViewMatrix;
	glm::mat4 m_depthProjectionMatrix;
	glm::mat4 m_shadowProjectionViewMatrix;

	//stuff to render the 3d plane
	unsigned int m_planeShader;
	Texture m_planeTexture;
	unsigned int m_planeVBO, m_planeVAO, m_planeIBO;

	/*struct DirectionalLight
	{
		glm::vec4 direction;
		glm::vec4 color;
	} m_directionalLight;*/

	FBXFile * m_fbx;

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