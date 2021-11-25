#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/MeshAnimation.h"
#include <algorithm>
#include <random>
#include <vector>

std::random_device                  rand_dev;
std::mt19937                        generator(rand_dev());
std::vector<int> randomRot, randomDelta, randomDelta1;
bool random_values_set = false;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

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

	if (!terrainTex || !grassTex || !areaMapTex || !bumpmap || !waterTex || !treeTex || !boatTex) {
		return;
	}

	SetTextureRepeating(terrainTex, true);
	SetTextureRepeating(grassTex, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(bumpmap, true);

	skyboxShader = new  Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	reflectShader = new  Shader("reflectVertex.glsl", "reflectFragment.glsl");
	objectShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	animShader = new  Shader("SkinningVertex.glsl", "texturedFragment.glsl");
	if (!skyboxShader->LoadSuccess() || !reflectShader->LoadSuccess() || !objectShader->LoadSuccess() || !animShader->LoadSuccess()) {
		return;
	}
	////////////////////////////////////////////////////////////////////

	// Adding Light
	////////////////////////////////////////////////////////////////////
	Vector3 heightmapSize = tropicalIsland->GetHeightmapSize();
	light = new  Light(heightmapSize * Vector3(0.5f, 1.5f, 0.5f),
		Vector4(1, 1, 1, 1), heightmapSize.x * 0.5f);
	
	lightShader = new  Shader("bumpvertex.glsl", "bumpfragment.glsl");
	if (!lightShader->LoadSuccess()) {
		return;
	}
	////////////////////////////////////////////////////////////////////
	auto heightmapLight = new SceneNode(lightShader, tropicalIsland, light->GetColour(), "heightmaplight");
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

	// Adding Water
	heightmapLight->AddChild(new SceneNode(reflectShader, Mesh::GenerateQuad(), Vector4(1, 1, 1, 0.3), "water"));

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	waterRotate = 0.0f;
	waterCycle = 0.0f;

	currentFrame = 0;
	frameTime = 0.0f;

	init = true;
}

Renderer ::~Renderer(void) {
	delete root;
	delete tropicalIsland;
	delete light;
	delete camera;
	delete skyboxShader;
	delete lightShader;
}

void Renderer::Eliminate() {
	//root->RemoveChild(enemy);
}

void  Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f; //2 degrees a second
	waterCycle += dt * 0.5f; //0.25f;//10  units a second
	frameTime -= dt;//*0.1;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0f / anim->GetFrameRate();
	}
	root->Update(dt);
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	Mesh::GenerateQuad()->Draw();

	glDepthMask(GL_TRUE);
}

void  Renderer::RenderCharacter(const SceneNode& scene) {

	auto shader = scene.GetShader();
	auto mesh = scene.GetMesh();

	BindShader(shader);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);

	auto hSize = tropicalIsland->GetHeightmapSize();

	modelMatrix.ToIdentity();
	modelMatrix = modelMatrix
		* Matrix4::Translation(Vector3(hSize.x * (0.57), hSize.y * 0.06, hSize.z * (0.385)))
		* Matrix4::Scale(Vector3(1.0, 1.0, 1.0) * hSize.y / 10.0);
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

	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&scene.GetColour());

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);

	auto hSize = tropicalIsland->GetHeightmapSize();
	float sign = +1;

	if (scene.GetSceneName() == "treeObject") {
		int index = 0;
		glBindTexture(GL_TEXTURE_2D, treeTex);
		std::uniform_int_distribution<int>  distr(0, 360), distr1(10,100), distr2(-100,100);
		for (size_t j = 0; j < 26; ++j) {
			if (j >= 5 && j <= 21) continue;
			float jstep = j * 0.03;
			if (j == 22) jstep = j * 0.0295;
			for (size_t i = 1; i < 24; ++i) {
				float istep = i * 0.02;
				modelMatrix.ToIdentity();
				if (!random_values_set) {
					randomRot.push_back(distr(generator));
					randomDelta.push_back(distr1(generator));
					randomDelta1.push_back((distr2(generator)));
				}
				modelMatrix = modelMatrix
					* Matrix4::Translation(Vector3(hSize.x * (0.25 + istep + randomDelta1[index] / 10000.0), 0.0, hSize.z * (0.1 + jstep + sign * randomDelta[index] / 10000.0)))
					* Matrix4::Scale(Vector3(1.0, 1.0, 1.0) * hSize.y / 22.0)
					* Matrix4::Rotation(randomRot[index], Vector3(0,1,0));
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
			* Matrix4::Translation(Vector3(hSize.x * (0.106), -hSize.y*0.05, hSize.z * (0.418)))
			* Matrix4::Scale(Vector3(1.0, 1.0, 1.0) * hSize.y / 8)
			* Matrix4::Rotation(180, Vector3(0, 1, 0));
		textureMatrix.ToIdentity();

		UpdateShaderMatrices();

		scene.GetMesh()->Draw();
	}
}

void Renderer::RenderHeightmapWithLight(const SceneNode& scene) {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = scene.GetShader();
	BindShader(shader);

	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

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

void Renderer::DrawWater(const SceneNode& scene) {
	auto shader = scene.GetShader();
	BindShader(shader);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());
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

	Vector3  hSize = tropicalIsland->GetHeightmapSize();

	modelMatrix =
		Matrix4::Translation(Vector3(hSize.x*0.5,hSize.y*0.005, hSize.z*0.5)) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	// SetShaderLight (* light); //No  lighting  in this  shader!
	scene.GetMesh()->Draw();
}

void   Renderer::BuildNodeLists(SceneNode* from) {
	if (true){
	//if (frameFrustum.InsideFrustum(*from)) {
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
	/*glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox(*Mesh::GenerateQuad());
	DrawNode(root);
	*/
	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox();
	DrawNodes();
	//DrawWater(*Mesh::GenerateQuad());
	ClearNodeLists();
}

void   Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {

		if (n->GetSceneName() == "heightmaplight") RenderHeightmapWithLight(*n);
		else if (n->GetSceneName() == "water") DrawWater(*n);
		else if (n->GetSceneName().find("Object") != std::string::npos) DrawObject(*n);
		else if (n->GetSceneName() == "character") RenderCharacter(*n);
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