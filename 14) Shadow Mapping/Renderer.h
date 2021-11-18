#pragma once

#include "../nclgl/OGLRenderer.h"

class Mesh;
class Camera;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer();

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void			DrawShadowScene();
	void			DrawMainScene();

	GLuint			shadowTex;
	GLuint			shadowFBO;

	GLuint			sceneDiffuse;
	GLuint			sceneBump;
	float			sceneTime;

	Shader* sceneShader;
	Shader* shadowShader;

	vector<Mesh*>	sceneMeshes;
	vector<Matrix4>	sceneTransforms;

	Camera* camera;
	Light* light;
};