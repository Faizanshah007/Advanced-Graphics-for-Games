#include "CubeRobot.h"

CubeRobot::CubeRobot(Mesh* cube) {
	//SetMesh(cube); // Uncomment  if you  want a local  origin  marker!

	auto* individual_shader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	auto* individual_shader_new = new Shader("SceneVertex.glsl", "SceneFragmentNew.glsl");

	SceneNode* body = new  SceneNode(individual_shader, cube, Vector4(1, 0, 0, 1));    //Red!
	body->SetModelScale(Vector3(10, 15, 5));
	body->SetTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	AddChild(body);

	head = new SceneNode(individual_shader_new, cube, Vector4(0, 1, 0, 1));	//Green!
	head->SetModelScale(Vector3(5, 5, 5));
	head->SetTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	body->AddChild(head);

	leftArm = new SceneNode(individual_shader, cube, Vector4(0, 0, 1, 1));    //Blue!
	leftArm->SetModelScale(Vector3(3, -18, 3));
	leftArm->SetTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	body->AddChild(leftArm);

	rightArm = new  SceneNode(individual_shader, cube, Vector4(0, 0, 1, 1));           //Blue!
	rightArm->SetModelScale(Vector3(3, -18, 3));
	rightArm->SetTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	body->AddChild(rightArm);

	SceneNode* hip = new  SceneNode(individual_shader, cube, Vector4(0.5, 0.1, 0.2, 1));
	hip->SetModelScale(Vector3(10, 5, 2));
	hip->SetTransform(Matrix4::Translation(Vector3(0, 0, -7)));
	body->AddChild(hip);

	SceneNode* leftLeg = new  SceneNode(individual_shader, cube, Vector4(0, 0, 1, 1));   //Blue!
	leftLeg->SetModelScale(Vector3(3, -17.5, 3));
	leftLeg->SetTransform(Matrix4::Translation(Vector3(-8, 0, 7)));
	hip->AddChild(leftLeg);

	SceneNode* rightLeg = new  SceneNode(individual_shader, cube, Vector4(0, 0, 1, 1)); //Blue!
	rightLeg->SetModelScale(Vector3(3, -17.5, 3));
	rightLeg->SetTransform(Matrix4::Translation(Vector3(8, 0, 7)));
	hip->AddChild(rightLeg);
}

void  CubeRobot::Update(float dt) {
	transform = transform * Matrix4::Rotation(30.0f * dt, Vector3(0, 1, 0));

	head->SetTransform(head->GetTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(0, 1, 0)));
	leftArm->SetTransform(leftArm->GetTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(1, 0, 0)));
	rightArm->SetTransform(rightArm->GetTransform() * Matrix4::Rotation(30.0f * dt, Vector3(1, 0, 0)));

	SceneNode::Update(dt);
}

void CubeRobot::Scale(Vector3 scale) {
	for (auto child : this->children) {
		child->SetTransform(child->GetTransform() * Matrix4::Scale(scale));
	}
}