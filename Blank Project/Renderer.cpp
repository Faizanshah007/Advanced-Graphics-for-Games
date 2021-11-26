#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/MeshAnimation.h"
#include <algorithm>
#include <random>
#include <vector>

const  int  POST_PASSES = 3;
bool post_effect_on = false;
int post_effect = 0;

std::random_device                  rand_dev;
std::mt19937                        generator(rand_dev());
std::vector<int> randomRot, randomDelta, randomDelta1;
bool random_values_set = false;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	quad = Mesh::GenerateQuad();

	root = new  SceneNode();
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
	tropicalIsland = new  HeightMap(TEXTUREDIR"noiseTexture.png", 3.0);

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

	waterBumpmap = SOIL_load_OGL_texture(
		TEXTUREDIR"waterbump.PNG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	bumpmap = SOIL_load_OGL_texture(
		TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	tree = Mesh::LoadFromMeshFile("Poplar_Tree.msh");

	treeTex = SOIL_load_OGL_texture(
		TEXTUREDIR"tree_diffuse.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	boat = Mesh::LoadFromMeshFile("Wood_BoatV1.msh");

	boatTex = SOIL_load_OGL_texture(
		TEXTUREDIR"Texture8x8.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	bonfire = Mesh::LoadFromMeshFile("Bonfire_B_Small_Fire.msh");

	woodTex = SOIL_load_OGL_texture(
		TEXTUREDIR"Texture, Wall rock and wood.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!terrainTex || !grassTex || !areaMapTex || !bumpmap || !waterTex || !treeTex || !boatTex || !woodTex) {
		return;
	}

	SetTextureRepeating(terrainTex, true);
	SetTextureRepeating(bumpmap, true);
	SetTextureRepeating(grassTex, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(waterBumpmap, true);

	skyboxShader = new  Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	reflectShader = new  Shader("reflectVertex.glsl", "reflectFragment.glsl");
	objectShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	animShader = new  Shader("SkinningVertex.glsl", "SceneFragment.glsl");
	sceneShader = new  Shader("TexturedVertex.glsl", "TexturedFragment1.glsl");
	processShader = new  Shader("TexturedVertex.glsl", "processfrag.glsl");
	geomShader = new Shader("pointVert.glsl", "TexturedFragment.glsl", "pointGeom.glsl");
	if (!skyboxShader->LoadSuccess() || !reflectShader->LoadSuccess() || !objectShader->LoadSuccess() || !animShader->LoadSuccess() || !processShader->LoadSuccess() || !sceneShader->LoadSuccess() || !geomShader->LoadSuccess()) {
		return;
	}
	////////////////////////////////////////////////////////////////////

	// Adding Lights
	////////////////////////////////////////////////////////////////////
	Vector3 heightmapSize = tropicalIsland->GetHeightmapSize();
	light[0] = new  Light(heightmapSize * Vector3(0.5f, 1.5f, 0.5f),
		Vector4(1, 1, 1, 1.5), heightmapSize.x * 0.5f);
	light[1] = new  Light(heightmapSize * Vector3(0.5f, 0.2f, 0.6f),
		Vector4(1, 0, 0, 2), heightmapSize.x * 0.07f);

	lightShader = new  Shader("bumpvertex.glsl", "bumpfragment.glsl");
	if (!lightShader->LoadSuccess()) {
		return;
	}
	////////////////////////////////////////////////////////////////////
	auto heightmapLight = new SceneNode(lightShader, tropicalIsland, light[0]->GetColour(), "heightmaplight");
	root->AddChild(heightmapLight);

	// Adding tree
	heightmapLight->AddChild(new SceneNode(objectShader, tree, Vector4(1, 0, 0, 1), "treeObject"));

	// Adding Boat
	heightmapLight->AddChild(new SceneNode(objectShader, boat, Vector4(0.588, 0.294, 0, 1), "boatObject"));

	animMesh = Mesh::LoadFromMeshFile("Ch39_nonPBR@Rumba Dancing.msh");
	anim = new MeshAnimation("Ch39_nonPBR@Rumba Dancing.anm");
	material = new MeshMaterial("Ch39_nonPBR@Rumba Dancing.mat");

	for (int i = 0; i < animMesh->GetSubMeshCount(); ++i) {
		const  MeshMaterialEntry* matEntry =
			material->GetMaterialForLayer(i);

		const  string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string  path = TEXTUREDIR + *filename;
		GLuint  texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}

	// Adding Anim Character
	heightmapLight->AddChild(new SceneNode(animShader, animMesh, Vector4(1, 1, 1, 1), "character"));

	// Adding Bornfire
	heightmapLight->AddChild(new SceneNode(geomShader, bonfire, Vector4(0.643f, 0.455, 0.286, 1), "bonfire"));

	// Adding Water
	heightmapLight->AddChild(new SceneNode(reflectShader, Mesh::GenerateQuad(), Vector4(1, 1, 1, 0.3), "water"));

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	// Generate  our  scene  depth  texture ...
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	//And  our  colour  texture ...
	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO); //We’ll  render  the  scene  into  this
	glGenFramebuffers(1, &processFBO);//And do post  processing  in this

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[0], 0);

	//We can  check  FBO  attachment  success  using  this  command!
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ||
		!bufferDepthTex || !bufferColourTex[0]) {
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	waterRotate = 0.0f;
	waterCycle = 0.0f;

	currentFrame = 0;
	frameTime = 0.0f;

	fireTheta = 0.0f;

	init = true;
}

Renderer ::~Renderer(void) {
	delete root;
	delete tropicalIsland;
	delete light[0];
	delete light[1];
	delete camera;
	delete skyboxShader;
	delete lightShader;
	delete reflectShader;
	delete objectShader;
	delete animShader;
	delete processShader;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
}

void Renderer::Eliminate() {
	//root->RemoveChild(enemy);
}

void  Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f; //2 degrees a second
	waterCycle += dt * 0.25f; //0.25//10  units a second
	fireTheta += dt * 2;
	frameTime -= dt;//*0.1;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0f / anim->GetFrameRate();
	}
	root->Update(dt); // Parent based transform included, Loops through child nodes!!
}

void Renderer::TogglePostEffect(const int& val) {
	if (val == 0) post_effect_on = false;
	else post_effect_on = true;
	post_effect = val;
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}

float big = 0.0;
void  Renderer::RenderCharacter(const SceneNode& scene) {

	auto shader = scene.GetShader();
	auto mesh = scene.GetMesh();

	BindShader(shader);
	SetShaderLight(*light[0]);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(),
		"cameraPos"), 1, (float*)& camera->GetPosition());

	auto hSize = tropicalIsland->GetHeightmapSize();

	modelMatrix.ToIdentity();
	modelMatrix = modelMatrix
		* Matrix4::Translation(Vector3(hSize.x * (0.546), hSize.y * 0.027, hSize.z * (0.377)))
		* Matrix4::Scale(Vector3(1.0, 1.0, 1.0) * hSize.y / (10.0 * 0.1) * std::clamp((sin(big) + 1) / 2, 0.1f, 0.23f));
	big += 0.01;
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	vector <Matrix4> frameMatrices;

	const  Matrix4* invBindPose = animMesh->GetInverseBindPose();
	const  Matrix4* frameData = anim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}

	int j = glGetUniformLocation(shader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false,
		(float*)frameMatrices.data());

	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		mesh->DrawSubMesh(i);
	}
}

void Renderer::DrawObject(const SceneNode& scene) {
	auto shader = scene.GetShader();
	BindShader(shader);

	SetShaderLight(*light[0]);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(),
		"cameraPos"), 1, (float*)& camera->GetPosition());

	//glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&scene.GetColour());

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);

	auto hSize = tropicalIsland->GetHeightmapSize();
	float sign = +1.0f;

	//Procedurally Generated Trees
	if (scene.GetSceneName() == "treeObject") {
		int index = 0;
		glBindTexture(GL_TEXTURE_2D, treeTex);
		std::uniform_int_distribution<int>  distr(0, 360), distr1(10, 100), distr2(-100, 100);
		for (size_t j = 0; j < 26; ++j) {
			if (j >= 5 && j <= 21) continue;
			float jstep = j * 0.03f;
			if (j == 22) jstep = j * 0.0295f;
			for (size_t i = 1; i < 24; ++i) {
				float istep = i * 0.02f;
				modelMatrix.ToIdentity();
				if (!random_values_set) {
					randomRot.push_back(distr(generator));
					randomDelta.push_back(distr1(generator));
					randomDelta1.push_back((distr2(generator)));
				}
				modelMatrix = modelMatrix
					* Matrix4::Translation(Vector3(hSize.x * (0.25 + istep + randomDelta1[index] / 10000.0f), 0.0f, hSize.z * (0.1f + jstep + sign * randomDelta[index] / 10000.0f)))
					* Matrix4::Scale(Vector3(1.0, 1.0, 1.0) * hSize.y / 22.0)
					* Matrix4::Rotation(randomRot[index], Vector3(0, 1, 0));
				++index;
				sign *= -1;
				textureMatrix.ToIdentity();

				UpdateShaderMatrices();

				scene.GetMesh()->Draw();
			}
		}
		random_values_set = true;
	}

	else if (scene.GetSceneName() == "boatObject") {
		glBindTexture(GL_TEXTURE_2D, boatTex);
		modelMatrix.ToIdentity();
		modelMatrix = modelMatrix
			* Matrix4::Translation(Vector3(hSize.x * (0.106), -hSize.y * 0.05, hSize.z * (0.418)))
			* Matrix4::Scale(Vector3(1.0, 1.0, 1.0) * hSize.y / 8)
			* Matrix4::Rotation(180, Vector3(0, 1, 0));
		textureMatrix.ToIdentity();

		UpdateShaderMatrices();

		scene.GetMesh()->Draw();
	}
}

void Renderer::RenderHeightmapWithLight(const SceneNode& scene) {
	auto shader = scene.GetShader();
	BindShader(shader);

	SetShaderLight(light);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(),
		"cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "grassTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "areaMapTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, areaMapTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "bumpTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bumpmap);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "waterTex"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	modelMatrix.ToIdentity();

	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	scene.GetMesh()->Draw();
}

void Renderer::DrawGeom(const SceneNode& scene) {
	auto shader = scene.GetShader();
	BindShader(shader);

	SetShaderLight(*light[0]);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(),
		"cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(
		shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTex);
	glUniform1i(glGetUniformLocation(
		shader->GetProgram(), "mode"), isFireOn);

	glUniform1f(glGetUniformLocation(
		shader->GetProgram(), "theta"), fireTheta);

	Vector3  hSize = tropicalIsland->GetHeightmapSize();

	modelMatrix.ToIdentity();

	modelMatrix = modelMatrix *
		Matrix4::Translation(Vector3(hSize.x * 0.25, hSize.y * 0.02, hSize.z * 0.6)) *
		Matrix4::Scale(Vector3(1, 1, 1) * hSize.y / 15.0f);

	textureMatrix.ToIdentity();

	UpdateShaderMatrices();
	scene.GetMesh()->Draw();
}

void Renderer::DrawWater(const SceneNode& scene) {
	auto shader = scene.GetShader();
	BindShader(shader);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(),
		"cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform1f(glGetUniformLocation(
		shader->GetProgram(), "clarity"), scene.GetColour().w);

	glUniform1i(glGetUniformLocation(
		shader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(
		shader->GetProgram(), "cubeTex"), 2);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "areaMapTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, areaMapTex);

	glUniform1i(glGetUniformLocation(
		shader->GetProgram(), "bumpTex"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, waterBumpmap);

	Vector3  hSize = tropicalIsland->GetHeightmapSize();

	modelMatrix =
		Matrix4::Translation(Vector3(hSize.x * 0.5, hSize.y * 0.005, hSize.z * 0.5)) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	scene.GetMesh()->Draw();
}

void   Renderer::BuildNodeLists(SceneNode* from) {
	if (true) {
		Vector3  dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}

	for (vector <SceneNode*>::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void   Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(),    //note  the r!
		transparentNodeList.rend(),      //note  the r!
		SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void   Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void  Renderer::RenderScene() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	DrawScene();
	if (post_effect_on) {
		DrawPostProcess();
		PresentScene();
	}
}

void  Renderer::DrawScene() {
	if (post_effect_on) glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);

	BuildNodeLists(root);
	SortNodeLists();
	DrawSkybox();
	DrawNodes();
	ClearNodeLists();
	textureMatrix.ToIdentity();
	if (post_effect_on) glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void  Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(processShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(
		processShader->GetProgram(), "sceneTex"), 0);
	glUniform1i(glGetUniformLocation(
		processShader->GetProgram(), "mode"), post_effect);
	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(processShader->GetProgram(),
			"isVertical"), 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		quad->Draw();
		//Now to swap  the  colour  buffers , and do the  second  blur  pass
		glUniform1i(glGetUniformLocation(processShader->GetProgram(),
			"isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void   Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(
		sceneShader->GetProgram(), "diffuseTex"), 0);
	Mesh::GenerateQuad()->Draw();
}

void   Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {

		if (n->GetSceneName() == "heightmaplight") RenderHeightmapWithLight(*n);
		else if (n->GetSceneName() == "water") DrawWater(*n);
		else if (n->GetSceneName().find("Object") != std::string::npos) DrawObject(*n);
		else if (n->GetSceneName() == "character") RenderCharacter(*n);
		else if (n->GetSceneName() == "bonfire") DrawGeom(*n);
	}
	for (vector <SceneNode*>::const_iterator
		i = n->GetChildIteratorStart();
		i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}