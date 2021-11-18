#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "SceneNode.h"

class  Renderer : public  OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void  UpdateScene(float dt) override;

	void  RenderScene()  override;
	void  Eliminate();
protected:
	void   DrawNode(SceneNode* n);

	void  RenderHeightmap();

	HeightMap* tropicalIsland;
	GLuint	   terrainTex;
	GLuint	   grassTex;
	GLuint	   areaMapTex;

	SceneNode* root;
	Camera* camera;
	Shader* sceneShader;
	Shader* heightMapShader;
};