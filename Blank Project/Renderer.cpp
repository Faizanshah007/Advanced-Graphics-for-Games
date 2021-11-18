#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	root = new  SceneNode();
	camera = new  Camera();

	// Initializing tropicalIsland
	////////////////////////////////////////////////////////////////////
	tropicalIsland = new  HeightMap(TEXTUREDIR"noiseTexture.png");

	Vector3  dimensions = tropicalIsland->GetHeightmapSize();
	camera->SetPosition(dimensions * Vector3(0.5, 2, 0.5));

	heightMapShader = new Shader("TexturedVertex.glsl",
		"TexturedFragment.glsl");
	if (!heightMapShader->LoadSuccess()) {
		return;
	}

	terrainTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	grassTex = SOIL_load_OGL_texture(TEXTUREDIR"grass1.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	areaMapTex = SOIL_load_OGL_texture(TEXTUREDIR"areaMap.PNG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!terrainTex || !grassTex || !areaMapTex) {
		return;
	}

	SetTextureRepeating(terrainTex, true);
	SetTextureRepeating(grassTex, true);
	////////////////////////////////////////////////////////////////////
	root->AddChild(new SceneNode(heightMapShader, tropicalIsland, Vector4(1, 0, 0, 1)));

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	
	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer ::~Renderer(void) {
	delete  root;
	delete  heightMapShader;
	delete  camera;
}

void Renderer::Eliminate() {
	//root->RemoveChild(enemy);
}

void  Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(dt);
}

void Renderer::RenderHeightmap() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(heightMapShader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(heightMapShader->GetProgram(),"diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTex);

	glUniform1i(glGetUniformLocation(heightMapShader->GetProgram(), "grassTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	glUniform1i(glGetUniformLocation(heightMapShader->GetProgram(), "areaMapTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, areaMapTex);

	tropicalIsland->Draw();
}

void  Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	/*BindShader(shader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 1);*/
	DrawNode(root);
}

void   Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		RenderHeightmap();
	}
	for (vector <SceneNode*>::const_iterator
		i = n->GetChildIteratorStart();
		i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}