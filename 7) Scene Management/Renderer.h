#pragma once

#include "../nclgl/OGLRenderer.h"
#include "Frustum.h"
#include "SceneNode.h"
class Camera;
class SceneNode;
class Mesh;
class Shader;

class  Renderer : public  OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void  UpdateScene(float dt) override;
	void  RenderScene()  override;

protected:
	void	BuildNodeLists(SceneNode* from);
	void	SortNodeLists();
	void	ClearNodeLists();
	void	DrawNodes();
	void	DrawNode(SceneNode* n);

	SceneNode* root;
	Camera* camera;
	Mesh* quad;
	Mesh* cube;
	Shader* shader;
	GLuint	texture[5];

	Frustum		frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};