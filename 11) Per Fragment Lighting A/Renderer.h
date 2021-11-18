#pragma once

#include "../nclgl/OGLRenderer.h"

class HeightMap;
class Camera;
class Light; //Predeclare our new class type...
class Shader;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer();

	void RenderScene()			override;
	void UpdateScene(float dt)	override;

protected:
	HeightMap* heightMap;
	Shader* shader;
	Camera* camera;
	Light* light[4]; //A new thing!
	GLuint	texture;
};