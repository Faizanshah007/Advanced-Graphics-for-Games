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
protected:
	void   DrawNode(SceneNode* n);

	void  RenderHeightmapWithLight(const SceneNode& scene);
	void  DrawSkybox();
	void  DrawWater(const SceneNode& scene);
	void  DrawObject(const SceneNode& scene);
	void  RenderCharacter(const SceneNode& scene);

	void  BuildNodeLists(SceneNode* from);
	void  SortNodeLists();
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	HeightMap* tropicalIsland;
	Shader* heightMapShader;
	GLuint	   terrainTex;
	GLuint	   grassTex;

	GLuint	   waterTex;
	Shader* reflectShader;
	float	   waterRotate;
	float	   waterCycle;

	GLuint	   areaMapTex;
	GLuint	   bumpmap;

	Light* light;
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
};