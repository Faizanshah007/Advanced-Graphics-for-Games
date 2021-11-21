#pragma once

#include "../nclgl/Matrix4.h"
#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/Mesh.h"
#include <vector>

class  SceneNode {
public:
	SceneNode(Shader* sh = NULL, Mesh* m = NULL, Vector4 colour = Vector4(0, 0, 0, 1), std::string name = "");
	~SceneNode(void);

	std::string name;

	void   SetTransform(const  Matrix4& matrix) { transform = matrix; }
	const  Matrix4& GetTransform()        const { return  transform; }
	Matrix4 GetWorldTransform()  const { return  worldTransform; }
	Vector4 GetColour()           const { return  colour; }
	void              SetColour(Vector4 c) { colour = c; }
	Vector3 GetModelScale()              const { return  modelScale; }
	void              SetModelScale(Vector3 s) { modelScale = s; }
	Mesh* GetMesh()              const { return  mesh; }
	void              SetMesh(Mesh* m) { mesh = m; }
	Shader* GetShader()			 const { return shader; }
	void			  SetShader(Shader* sh) { shader = sh; }
	void              AddChild(SceneNode* s);
	void			  RemoveChild(SceneNode* s);


	virtual  void    Update(float dt);
	virtual  void    Draw(const  OGLRenderer& r);
	std::vector <SceneNode*>::const_iterator  GetChildIteratorStart() {
		return  children.begin();
	}
	std::vector <SceneNode*>::const_iterator  GetChildIteratorEnd() {
		return  children.end();
	}
protected:
	SceneNode* parent;
	Mesh* mesh;
	Shader* shader;
	Matrix4
		worldTransform;
	Matrix4
		transform;
	Vector3
		modelScale;
	Vector4
		colour;
	std::vector <SceneNode*>     children;
};