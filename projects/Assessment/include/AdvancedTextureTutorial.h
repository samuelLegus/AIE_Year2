#include "Assessment.h"

class AdvancedTextureTutorial : public Assessment
{
private:

	unsigned int m_program;
	FBXFile * m_fbx;
	float m_decayValue;
	Texture m_decayTexture;
	Texture m_metallicTexture;
	bool m_useSecondaryTexture;

	void BuildProgram();

	bool onCreate(int a_argc, char* a_argv[]);

	void onUpdate(float a_deltaTime);

	void onDraw();

	void onDestroy();

public:

	AdvancedTextureTutorial();

	virtual ~AdvancedTextureTutorial();

	void InitFBXSceneResource(FBXFile *a_pScene);

	void DestroyFBXSceneResource(FBXFile *a_pScene);

	void UpdateFBXSceneResource(FBXFile *a_pScene);
	
	void RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_view, glm::mat4 a_projection);
	
};