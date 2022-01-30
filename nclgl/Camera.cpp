#include "Camera.h"
#include "Window.h"
#include <algorithm>

void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	file.read((char*)& position.x, sizeof(float));
	file.read((char*)& position.y, sizeof(float));
	file.read((char*)& position.z, sizeof(float));
	file.read((char*)& pitch, sizeof(float));
	file.read((char*)& yaw, sizeof(float));

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0.0f) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	float speed = 500.0f * dt;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position -= right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position += right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y += speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y -= speed;
	}
	/*if (!(prevPosition == position && prevYaw == yaw && prevPitch == pitch)) {
		file.write((char*)& position.x, sizeof(float));
		file.write((char*)& position.y, sizeof(float));
		file.write((char*)& position.z, sizeof(float));
		file.write((char*)& pitch, sizeof(float));
		file.write((char*)& yaw, sizeof(float));
	}

	prevPosition = position;
	prevYaw = yaw;
	prevPitch = pitch;*/
}

Matrix4 Camera::BuildViewMatrix() {
	/*Matrix4::Rotation(-roll, Vector3(0, 0, 1))*/
	return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
}