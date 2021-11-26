#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "SceneNode.h"

class Light;
class MeshMaterial;
class MeshAnimation;

class  Renderer : public  OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void  UpdateScene(float dt) override;

	void  RenderScene()  override;
	void  Eliminate();

	void ClearNodeLists();
	void DrawNodes();

	void TogglePostEffect(const int& val);
	void ToggleFireSmoke() { isFireOn = !isFireOn; }
protected:

	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	void DrawNode(SceneNode* n);

	void  RenderHeightmapWithLight(const SceneNode& scene);
	void  DrawSkybox();
	void  DrawWater(const SceneNode& scene);
	void  DrawObject(const SceneNode& scene);
	void  RenderCharacter(const SceneNode& scene);
	void  DrawGeom(const SceneNode& scene);

	void  BuildNodeLists(SceneNode* from);
	void  SortNodeLists();

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	HeightMap* tropicalIsland;
	Shader* heightMapShader;
	GLuint	   terrainTex;
	GLuint	   grassTex;

	GLuint	   waterTex;
	GLuint	   waterBumpmap;
	Shader* reflectShader;
	float	   waterRotate;
	float	   waterCycle;

	GLuint	   areaMapTex;
	GLuint	   bumpmap;

	Light* light[2];
	Shader* lightShader;

	Shader* skyboxShader;

	GLuint cubeMap;

	SceneNode* root;
	Camera* camera;

	Mesh* tree;
	GLuint treeTex;
	Mesh* boat;
	GLuint boatTex;
	Shader* objectShader;

	Mesh* animMesh;
	MeshAnimation* anim;
	MeshMaterial* material;
	vector<GLuint>	matTextures;
	Shader* animShader;
	int		currentFrame;
	float	frameTime;

	Shader* processShader;
	Shader* sceneShader;
	GLuint			heightTexture;
	GLuint			bufferFBO;
	GLuint			processFBO;
	GLuint			bufferColourTex[2];
	GLuint			bufferDepthTex;
	Mesh* quad;

	Mesh* bonfire;
	GLuint woodTex;
	Shader* geomShader;
	float fireTheta;

	int isFireOn = 1;
};