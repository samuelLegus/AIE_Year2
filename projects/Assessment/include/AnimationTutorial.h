#include "Assessment.h"
#include "FBXFile.h"

class AnimationTutorial : public Assessment
{
private:

	unsigned int m_shader;

	unsigned int m_vbo;
	unsigned int m_ibo;
	unsigned int m_vao;

	FBXFile * m_fbx; 

	void BuildProgram();

	bool onCreate(int a_argc, char* a_argv[]);

	void onUpdate(float a_deltaTime);

	void onDraw();

	void onDestroy();

public:

	AnimationTutorial();

	virtual ~AnimationTutorial();

	void InitFBXSceneResource(FBXFile *a_pScene);

	void DestroyFBXSceneResource(FBXFile *a_pScene);

	void UpdateFBXSceneResource(FBXFile *a_pScene);

	void RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_view, glm::mat4 a_projection);

};