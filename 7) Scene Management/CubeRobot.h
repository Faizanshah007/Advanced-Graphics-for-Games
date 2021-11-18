#pragma once

#include "SceneNode.h"

class  CubeRobot : public  SceneNode {
public:
	CubeRobot(Mesh* cube);
	~CubeRobot(void) {};
	void   Update(float dt) override;
	void   Scale(Vector3 scale);
protected:
	SceneNode* head;
	SceneNode* leftArm;
	SceneNode* rightArm;
};