#include "Renderer.h"
#include "../nclgl/Light.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	quad = Mesh::GenerateQuad();

	root = new  SceneNode();
	root->name = "root";
	camera = new  Camera();

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"TropicalSunnyDay_px.JPG", TEXTUREDIR"TropicalSunnyDay_nx.JPG",
		TEXTUREDIR"TropicalSunnyDay_py.JPG", TEXTUREDIR"TropicalSunnyDay_ny.JPG",
		TEXTUREDIR"TropicalSunnyDay_pz.JPG", TEXTUREDIR"TropicalSunnyDay_nz.JPG",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap) {
		return;
	}

	// Initializing tropicalIsland
	////////////////////////////////////////////////////////////////////
	tropicalIsland = new  HeightMap(TEXTUREDIR"noiseTexture.png");

	Vector3  dimensions = tropicalIsland->GetHeightmapSize();
	camera->SetPosition(dimensions * Vector3(0.5, 2, 0.5));

	terrainTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	grassTex = SOIL_load_OGL_texture(TEXTUREDIR"grass1.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	areaMapTex = SOIL_load_OGL_texture(TEXTUREDIR"areaMap.PNG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	bumpmap = SOIL_load_OGL_texture(
		TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!terrainTex || !grassTex || !areaMapTex || !bumpmap || !waterTex) {
		return;
	}

	SetTextureRepeating(terrainTex, true);
	SetTextureRepeating(grassTex, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(bumpmap, true);

	skyboxShader = new  Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	reflectShader = new  Shader("reflectVertex.glsl", "reflectFragment.glsl");
	if (!skyboxShader->LoadSuccess() || !reflectShader->LoadSuccess()) {
		return;
	}
	////////////////////////////////////////////////////////////////////
	root->AddChild(new SceneNode(nullptr, tropicalIsland, Vector4(1, 0, 0, 1), "heightmap"));

	// Adding Light
	////////////////////////////////////////////////////////////////////
	Vector3  heightmapSize = tropicalIsland->GetHeightmapSize();
	light = new  Light(heightmapSize * Vector3(0.5f, 1.5f, 0.5f),
		Vector4(1, 1, 1, 1), heightmapSize.x * 0.5f);
	
	lightShader = new  Shader("bumpvertex.glsl", "bumpfragment.glsl");
	if (!lightShader->LoadSuccess()) {
		return;
	}
	////////////////////////////////////////////////////////////////////
	root->AddChild(new SceneNode(lightShader, tropicalIsland, light->GetColour(), "heightmaplight"));

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	waterRotate = 0.0f;
	waterCycle = 0.0f;

	init = true;
}

Renderer ::~Renderer(void) {
	delete root;
	delete tropicalIsland;
	delete light;
	delete camera;
	delete skyboxShader;
	delete lightShader;
	delete quad;
}

void Renderer::Eliminate() {
	//root->RemoveChild(enemy);
}

void  Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f; //2 degrees a second
	waterCycle += dt * 0.25f; //10  units a second
	root->Update(dt);
}

void   Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}

void Renderer::RenderHeightmapWithLight() {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(lightShader);

	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "grassTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "areaMapTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, areaMapTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bumpmap);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	tropicalIsland->Draw();
}

void   Renderer::DrawWater() {
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "areaMapTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, areaMapTex);

	Vector3  hSize = tropicalIsland->GetHeightmapSize();

	modelMatrix =
		Matrix4::Translation(Vector3(hSize.x*0.5,hSize.y*0.001, hSize.z*0.5)) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	// SetShaderLight (* light); //No  lighting  in this  shader!
	quad->Draw();
}



void  Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox();
	DrawNode(root);
	DrawWater();
}

void   Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		if (n->name == "heightmaplight") RenderHeightmapWithLight();
	}
	for (vector <SceneNode*>::const_iterator
		i = n->GetChildIteratorStart();
		i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}