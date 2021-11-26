#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include <fstream>

class Camera {
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
		//file.open("camMeta.txt", std::fstream::in);
		file.open("camMeta.txt", std::fstream::in | std::fstream::binary);
	}

	Camera(float pitch, float yaw, Vector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
		//file.open("camMeta.txt", std::fstream::in);
		file.open("camMeta.txt", std::fstream::in | std::fstream::binary);
	}

	~Camera(void) { file.close(); }

	void UpdateCamera(float dt = 1.0f);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }

	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }

protected:
	float yaw;
	float pitch;
	float prevYaw;
	float prevPitch;
	Vector3 position;
	Vector3 prevPosition;

	std::fstream file;
};
