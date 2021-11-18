#include "SceneNode.h"

SceneNode::SceneNode(Shader* sh, Mesh* mesh, Vector4 colour) {
	this->shader = sh;
	this->mesh = mesh;
	this->colour = colour;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
}

SceneNode::~SceneNode() {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode* s) {
	if (this == s) return;
	children.push_back(s);
	s->parent = this;
}

void SceneNode::RemoveChild(SceneNode* s) {
	for (auto itr = children.begin(); itr != children.end(); ++itr) {
		if (s == *(itr)) {
			children.erase(itr);
			return;
		}
	}
}

void   SceneNode::Draw(const  OGLRenderer& r) {
	if (mesh) { mesh->Draw(); }
}

void   SceneNode::Update(float dt) {
	if (parent) { //This  node  has a parent ...
		worldTransform = parent->worldTransform * transform;
	}
	else { //Root node , world  transform  is  local  transform!
		worldTransform = transform;
	}
	for (vector <SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(dt);
	}
}