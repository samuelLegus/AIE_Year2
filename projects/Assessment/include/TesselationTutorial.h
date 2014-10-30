#include "Assessment.h"

class TesselationTutorial : public Assessment
{
private:

	unsigned int m_program;
	Texture m_diffuseTexture;
	Texture m_displacementTexture;

	unsigned int m_vbo;
	unsigned int m_ibo;
	unsigned int m_vao;

	float m_displaceScale;
	float m_displaceLevel;

	void BuildProgram();

	bool onCreate(int a_argc, char* a_argv[]);

	void onUpdate(float a_deltaTime);

	void onDraw();

	void onDestroy();

public:

	TesselationTutorial();

	virtual ~TesselationTutorial();

	void InitFBXSceneResource(FBXFile *a_pScene);

	void DestroyFBXSceneResource(FBXFile *a_pScene);

	void UpdateFBXSceneResource(FBXFile *a_pScene);

	void RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_view, glm::mat4 a_projection);

};