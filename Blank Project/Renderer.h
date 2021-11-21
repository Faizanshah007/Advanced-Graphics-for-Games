#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "SceneNode.h"

class Light;

class  Renderer : public  OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void  UpdateScene(float dt) override;

	void  RenderScene()  override;
	void  Eliminate();
protected:
	void   DrawNode(SceneNode* n);

	void  RenderHeightmapWithLight();
	void  DrawSkybox();
	void  DrawWater();

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

	Mesh* quad;
};