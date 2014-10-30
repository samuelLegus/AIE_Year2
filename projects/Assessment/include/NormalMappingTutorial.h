#include "Assessment.h"

struct NormalMappedModel
{
	FBXFile * fbx;
	glm::mat4 modelViewProjection;
	glm::mat4 modelView;
	glm::mat3 normalMatrix;
	glm::vec3 lightPosition;
	glm::vec3 lightColor;
	glm::vec3 lightDirection;
	glm::vec4 ambientLightColor;
	//float specularPower;
	Texture normalMap; // ideally these would be part of the FBX file
	Texture specularMap; // ... but we have to load them externally for now. 
};

class NormalMappingTutorial : public Assessment
{
private:

	unsigned int m_program;
	NormalMappedModel m_model;
	
	void BuildProgram();

	bool onCreate(int a_argc, char* a_argv[]);

	void onUpdate(float a_deltaTime);

	void onDraw();

	void onDestroy();

public:

	NormalMappingTutorial();

	virtual ~NormalMappingTutorial();

	void InitFBXSceneResource(FBXFile *a_pScene);

	void DestroyFBXSceneResource(FBXFile *a_pScene);

	void UpdateFBXSceneResource(FBXFile *a_pScene);

	void RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_view, glm::mat4 a_projection);

};