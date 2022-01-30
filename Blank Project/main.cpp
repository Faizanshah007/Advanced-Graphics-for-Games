#include "../nclgl/window.h"
#include "Renderer.h"
#include <chrono>

int main() {
	Window w("Loading!!!", 1280, 720, false); //Set true for fullscreen - check compatiblity
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	auto start_time = std::chrono::steady_clock::now();
	int frameCount = 0;

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_0)) {
			renderer.TogglePostEffect(0); // No effect
		}

		else if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			renderer.TogglePostEffect(1); // My Special
		}

		else if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			renderer.TogglePostEffect(2); // Blur
		}

		else if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
			renderer.TogglePostEffect(3); // Bloom
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F)) {
			renderer.ToggleFireSmoke(); // Fire / Smoke
		}

		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		renderer.SwapBuffers();
		++frameCount;
		float dt = (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time).count());
		if (dt >= 1.0) {
			w.SetTitle("Tropical Island - " + std::to_string(int(round(frameCount / dt))) + " fps"); frameCount = 0;
			start_time = std::chrono::steady_clock::now();
		}

	}

	return 0;
}