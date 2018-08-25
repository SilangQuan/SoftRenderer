
//#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <vector>
//#include "3rdParty/SDL2/include/SDL.h"

#include "Base/Csdl2.h"
#include "Base/Color.h"
#include "TestCases.h"
#include "RenderSystem/Rasterizer.h"
#include "RenderSystem/RenderSystem.h"
#include "Shape/Line2d.h"
#include "Math/Random.h"
#include "Base/Vertex.h"
#include "Base/Mesh.h"
#include "Base/Camera.h"
#include "Base/Model.h"
#include "RenderSystem/Shader.h"
#include "TypeDef.h"
#include "Light/Light.h"
#include "Base/Time.h"
#include "Base/Input.h"

//#include "SDL_test_common.h"
void handleKeyEvent(SDL_Keycode keyCode)
{
	switch (keyCode)
	{
	case SDLK_ESCAPE:
		SDL_Quit();
		/* Exit program. */
		exit(0);
		break;
	case SDLK_SPACE:
		break;
	case SDLK_F1:
		//toggle_fullscreen();
		break;

	case SDLK_w:

	default:
		break;
	}
}

void handleEvents()
{
	// Our SDL event placeholder.  
	SDL_Event event;
	//Grab all the events off the queue.  
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			// Handle key Event  
			handleKeyEvent(event.key.keysym.sym);
			break;
		case SDL_QUIT:
			// Handle quit requests (like Ctrl-c).  
			SDL_Quit();
			/* Exit program. */
			exit(0);
			break;
		}
	}
}


int main(int argc, char* args[])
{

	Time time;
	time.Start();
	Input input;
	InitDesc initDesc;
	initDesc.height = 360;
	initDesc.width = 640;

	//initDesc.height = 720;
	//initDesc.width = 1280;

	initDesc.isFullScreen = false;
	initDesc.title ="Soft Renderer by SilangQuan";
	RenderSystem* renderSystem = new RenderSystem(initDesc);

	qDebug()<<"RenderSystem Init OK!";

	//float widthInv = 1.0 / (sdl->screenWidth);
	//float heightInv = 1.0 / (sdl->screenHeight);

	//For 2.Test draw line
	//Line2d* line = new Line2d(Vector2(0, 0), Vector2(550, 200));

	//For 5.Test draw mesh
	/*
	Mesh* mesh = new Mesh();
	mesh->Vertices.push_back(Vector3(0,-55,0));
	mesh->Vertices.push_back(Vector3(25,0,0));
	mesh->Vertices.push_back(Vector3(5,15,0));
	mesh->Triangles.push_back(0);
	mesh->Triangles.push_back(1);
	mesh->Triangles.push_back(2);

	Camera* cam = new Camera(Vector3(0,0,17), Vector3::zero, Vector3::up);
	cam->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 0.3, 1000, true);

	Matrix4x4 modelMatrix = Matrix4x4::identity;*/


	//For 6.Test load mesh and draw
	//Model* model = new Model("./Assets/monkey.obj");
	//Model* model = new Model("./Assets/Bigmax_White_OBJ.obj");
	
	//Model* model = new Model("./Assets/ColorSquare.obj");
	Model* model = new Model("./Assets/sponza.obj");
	//Model* model = new Model("./Assets/wall.obj");
	//Model* model = new Model("./Assets/lion.obj");
	//Model* model = new Model("./Assets/banner.obj");
	//Model* model = new Model("./Assets/ColorSquare.obj");
	//Model* model = new Model("./Assets/sponza_floor.obj");
	//Model* model = new Model("./Assets/collection.obj");
	// /Model* model = new Model("./Assets/triangle.obj");
	//Model* model = new Model("./Assets/teapot.obj");
	//Model* model = new Model("./Assets/sphere.obj");
	//Model* model = new Model("./Assets/box.obj");

	//Custome Vertex Color
	std::vector<Mesh*> meshes = model->GetMeshes();
	for(int i=0; i< meshes.size(); i++)
	{
		meshes[i]->Colors.resize(meshes[i]->Vertices.size());
		for(int j =0; j< meshes[i]->Vertices.size(); j++)
		{
			switch(j%4)
			{
				case 0:
					meshes[i]->Colors[j] = Vector4(Color::red);
					break;
				case 1:
					meshes[i]->Colors[j] = Vector4(Color::green);
					break;
				case 2:
					meshes[i]->Colors[j] = Vector4(Color::blue);
					break;
				case 3:
					meshes[i]->Colors[j] = Vector4(Color::black);
					break;
			}
		}
	}

	Light* light = new Light();
	light->SetType(LightType::kLightDirectional);
	light->direction = Vector3(1,1,1);
	light->intensity = 1.0f;

	//VertexColorShader shader;
	//VertexColorShader* shader = new VertexColorShader();
	//SimpleTextureShader* shader =  new SimpleTextureShader();
	//GouraudShader shader;
	//BlinnPhongShader shader;

	//shader->DirectionLightDir = &(light->direction);
	//shader->DirectionLightIntensity = &(light->intensity);

	vector<Material*>& mats = model->GetMaterials();
		
	for(int i=0; i< mats.size(); i++)
	{
		//SimpleTextureShader* shader = new SimpleTextureShader();
		//VertexColorShader* shader = new VertexColorShader();
		//BlinnPhongShader* shader = new BlinnPhongShader();

		if(mats[i]->AlphaTex != NULL)
		{
			AlphaTestShader* shader  = new AlphaTestShader();
			mats[i]->Shader = shader;
			mats[i]->Shader->TextureUnit0 = mats[i]->DiffuseTex;
			mats[i]->Shader->TextureUnit1 = mats[i]->AlphaTex;
			shader->DirectionLightDir = &(light->direction);
			shader->DirectionLightIntensity = &(light->intensity);
		}
		else
		{
			GouraudShader* shader = new GouraudShader();
			mats[i]->Shader = shader;
			//mats[i]->Shader->TextureUnit0 = mats[i]->DiffuseTex;
			mats[i]->Shader->TextureUnit0 = mats[i]->DiffuseTex;
			shader->DirectionLightDir = &(light->direction);
			shader->DirectionLightIntensity = &(light->intensity);
		}
	}

	//Camera* cam = new Camera(Vector3(-40,120,-3), Vector3(-40,120.5,0), Vector3::up);
	Camera* cam = new Camera(Vector3(0,5,-13), Vector3(0,5.2,-12), Vector3::up);
	//Camera* cam = new Camera(Vector3(0,0,-3), Vector3(0,0,0), Vector3::up);
	cam->SetDepthRange(0,1);
	//cam->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 0.3, 1000, true);
	cam->SetPespective(60, 16.0/9, 0.3, 2000);
	cam->SetViewPort(0,0,initDesc.width, initDesc.height, true);
	//cam->SetViewPort(0,initDesc.height,initDesc.width, 0, true);
	Matrix4x4 modelMatrix = Matrix4x4::identity;

	qDebug() <<"ProjectionMatri: " << cam->projectionMaxtrix;  
	qDebug() << "ViewMatrix: " << cam->viewMatrix;
	qDebug() << "ModelMatrix: " << modelMatrix;  
	qDebug() << "MVP: " << cam->projectionMaxtrix * cam->viewMatrix * modelMatrix;  

	//modelMatrix.rotateY(90);
	//modelMatrix.rotateY(100);
	//modelMatrix.rotateY(180);
	//modelMatrix.rotate(20, Vector3::one);
	
	renderSystem->SetCurCamera(cam);
	//renderSystem->SetRenderMode(RenderMode::DepthMode);
	//renderSystem->SetRenderMode(RenderMode::WireframeMode);

	std::map<Mesh*, Material*>::iterator meshIter = model->GetMeshMatMap().begin();


	float showFpsDelta = 5;
	float showFpsMark = time.time;

	Color clearCol(0.212,0.212,0.212);
	while (true)
	{
		time.Update();
		input.Update();

		if(time.time - showFpsMark > showFpsMark)
		{
			qDebug()<<"FPS: "<<time.fps;
		}

		handleEvents();
		//sdl->Clean(&renderContext, &(Color::white));
		renderSystem->Clear(clearCol);

		if(input.GetKeyDown(KeyCode::KEY_SPACE))
		{
			//modelMatrix.rotateX(4 * time.deltaTime);
			modelMatrix.rotate(20 * time.deltaTime, Vector3::one);
		}

		if(input.GetKeyDown(KeyCode::KEY_UP))
		{
			//modelMatrix.rotateX(4 * time.deltaTime);
			modelMatrix.rotateX(20 * time.deltaTime);
		}
		if(input.GetKeyDown(KeyCode::KEY_DOWN))
		{
			//modelMatrix.rotateX(4 * time.deltaTime);
			modelMatrix.rotateX(-20 * time.deltaTime);
		}

		if(input.GetKeyDown(KeyCode::KEY_LEFT))
		{
			//modelMatrix.rotateX(4 * time.deltaTime);
			modelMatrix.rotateY(-20 * time.deltaTime);
		}

		if(input.GetKeyDown(KeyCode::KEY_RIGHT))
		{
			modelMatrix.rotateY(20 * time.deltaTime);
		}

		if(input.GetKeyDown(KeyCode::KEY_W))
		{
			cam->transform.position += 0.5f * Vector3::forward;
			cam->target += 0.5f * Vector3::forward;
		}

		if(input.GetKeyDown(KeyCode::KEY_A))
		{
			cam->transform.position += Vector3::left;
			cam->target += Vector3::left;
		}

		if(input.GetKeyDown(KeyCode::KEY_D))
		{
			cam->transform.position += Vector3::right;
			cam->target += Vector3::right;
		}

		if(input.GetKeyDown(KeyCode::KEY_S))
		{
			cam->transform.position += Vector3::back;
			cam->target += Vector3::back;
		}

		if(input.GetKeyDown(KeyCode::KEY_E))
		{
			cam->transform.position += 0.5f * Vector3::up;
			cam->target += 0.5f * Vector3::up;
		}

		if(input.GetKeyDown(KeyCode::KEY_Q))
		{
			cam->transform.position += 0.5f * Vector3::down;
			cam->target += 0.5f * Vector3::down;
		}

		cam->RefreshViewMatrix();

		//for(int i=0; i< meshes.size(); i++)
		///{
			//renderSystem->RenderMesh(meshes[i],&modelMatrix, mats[i]);
		//	renderSystem->RenderMesh(meshes[i],&modelMatrix, mats[0]);
		//}
		meshIter = model->GetMeshMatMap().begin();

	
		//renderSystem->RenderMesh(meshIter->first, &modelMatrix, meshIter->second);

		int count =0;
		while(meshIter != model->GetMeshMatMap().end())
		{
			renderSystem->RenderMesh(meshIter->first, &modelMatrix, meshIter->second);
			meshIter++;
			++count;
		}

		//renderSystem->RenderMesh(meshes[0],&modelMatrix, mats[0]);
		//renderSystem->DrawPoint(10,10, Color::green);
		
		renderSystem->SwapBuffer();


		//1.Test draw point
		/*for (int i = 0; i < 100; i++)
		{
			rasterizer.DrawPixel(widowWidth * Random::Value(), windowHeight * Random::Value(), Color::red);
		}*/

		//2.Test draw line
		/*for (int i = 0; i < 100; i++)
		{
			line->start.x = widowWidth * Random::Value();
			line->start.y = windowHeight * Random::Value();

			line->end.x = widowWidth * Random::Value();
			line->end.y = windowHeight * Random::Value();

			rasterizer.DrawOneLine(line, Color::RandomColor());
		}*/

		//3.Test draw lines with clip
	/*	for (int i = 0; i < 100; i++)
		{
			line->start.x =  widowWidth * Random::Range(-2.0f, 2.0f);
			line->start.y = windowHeight * Random::Range(-2.0f, 2.0f);

			line->end.x = widowWidth * Random::Range(-2.0f, 2.0f);
			line->end.y = windowHeight * Random::Range(-2.0f, 2.0f);

			rasterizer.DrawOneLine(line, Color::RandomColor());
		}*/

		//4.Test draw triangles 
		// for (int i = 0; i < 10; i++)
		// {
		// 	Vector2 v0(widowWidth * Random::Value(), windowHeight * Random::Value());
		// 	Vector2 v1(widowWidth * Random::Value(), windowHeight * Random::Value());
		// 	Vector2 v2(widowW//th * Random::Value(), windowHeight * Random::Value());
		// 	Color col0 = Color::RandomColor();
		// 	Color col1 = Color::RandomColor();
		// 	Color col2 = Color::RandomColor();
		// 	rasterizer.DrawTriangle2D(Vertex2D(v0, col0), Vertex2D(v1, col1), Vertex2D(v2, col2));
		// }

		//5.Test draw mesh//

		//modelMatrix.rotateY(2);
		// Matrix4x4 MVP = cam->projectionMaxtrix * cam->viewMatrix * modelMatrix;


		// for(int i=0; i< mesh->Triangles.size() /3; i++)
		// {
		// 	Vector4 clipPos0 = MVP * Vector4(mesh->Vertices[ mesh->Triangles[i]].x, mesh->Vertices[ mesh->Triangles[i]].y, mesh->Vertices[ mesh->Triangles[i]].z, 1);  
		// 	Vector4 clipPos1 = MVP * Vector4(mesh->Vertices[ mesh->Triangles[i+1]].x, mesh->Vertices[ mesh->Triangles[i+1]].y, mesh->Vertices[ mesh->Triangles[i+1]].z, 1);  
		// 	Vector4 clipPos2 = MVP * Vector4(mesh->Vertices[ mesh->Triangles[i+2]].x, mesh->Vertices[ mesh->Triangles[i+2]].y, mesh->Vertices[ mesh->Triangles[i+2]].z, 1);  


		// 	Vector3 ndcPos0 = Vector3(clipPos0.x / clipPos0.w, clipPos0.y / clipPos0.w, clipPos0.z / clipPos0.w);  
		// 	Vector3 ndcPos1 = Vector3(clipPos1.x / clipPos1.w, clipPos1.y / clipPos1.w, clipPos1.z / clipPos1.w);  
		// 	Vector3 ndcPos2 = Vector3(clipPos2.x / clipPos2.w, clipPos2.y / clipPos2.w, clipPos2.z / clipPos2.w);  

		// 	int x = 0, y = 0, w = widowWidth,  h= windowHeight, n = 0.3, f =1000;  O
		// 	Vector3 screenPos0(w * 0.5f * ndcPos0.x + x + w * 0.5f, h* 0.5f * ndcPos0.y + y + h *0.5f, 0.5f *(f - n) * ndcPos0.z + 0.5f * (f + n));  
		// 	Vector3 screenPos1(w * 0.5f * ndcPos1.x + x + w * 0.5f, h* 0.5f * ndcPos1.y + y + h *0.5f, 0.5f *(f - n) * ndcPos1.z + 0.5f * (f + n));  
		// 	Vector3 screenPos2(w * 0.5f * ndcPos2.x + x + w * 0.5f, h* 0.5f * ndcPos2.y + y + h *0.5f, 0.5f *(f - n) * ndcPos2.z + 0.5f * (f + n));  


		// 	Color col0 = Color::RandomColor();

		// 	rasterizer.DrawTriangle2D(Vertex2D(Vector2(screenPos0.x, screenPos0.y), col0), Vertex2D(Vector2(screenPos1.x, screenPos1.y), col0), Vertex2D(Vertex2D(Vector2(screenPos2.x, screenPos2.y), col0)));
		// }

		//5.Draw Model
		// modelMatrix.rotateY(2);
		// Matrix4x4 MVP = cam->projectionMaxtrix * cam->viewMatrix * modelMatrix;
		// for(int i=0; i< model->GetMeshes().size(); i++)
		// {
		// 	Mesh* mesh =  model->GetMeshes()[i];
		// 	for(int j =0; j<mesh->Vertices.size(); j++)
		// 	{
		// 		cout << mesh->Vertices[j].x << " " << mesh->Vertices[j].y << " " << mesh->Vertices[j].z<<endl;  
				
		// 		Vector4 clipPos0 = MVP * Vector4(mesh->Vertices[ mesh->Triangles[i]].x, mesh->Vertices[ mesh->Triangles[i]].y, mesh->Vertices[ mesh->Triangles[i]].z, 1);  
		// 		Vector4 clipPos1 = MVP * Vector4(mesh->Vertices[ mesh->Triangles[i+1]].x, mesh->Vertices[ mesh->Triangles[i+1]].y, mesh->Vertices[ mesh->Triangles[i+1]].z, 1);  
		// 		Vector4 clipPos2 = MVP * Vector4(mesh->Vertices[ mesh->Triangles[i+2]].x, mesh->Vertices[ mesh->Triangles[i+2]].y, mesh->Vertices[ mesh->Triangles[i+2]].z, 1);  


		// 		Vector3 ndcPos0 = Vector3(clipPos0.x / clipPos0.w, clipPos0.y / clipPos0.w, clipPos0.z / clipPos0.w);  
		// 		Vector3 ndcPos1 = Vector3(clipPos1.x / clipPos1.w, clipPos1.y / clipPos1.w, clipPos1.z / clipPos1.w);  
		// 		Vector3 ndcPos2 = Vector3(clipPos2.x / clipPos2.w, clipPos2.y / clipPos2.w, clipPos2.z / clipPos2.w);  

		// 		int x = 0, y = 0, w = widowWidth,  h= windowHeight, n = 0.3, f =1000;  
		// 		Vector3 screenPos0(w * 0.5f * ndcPos0.x + x + w * 0.5f, h* 0.5f * ndcPos0.y + y + h *0.5f, 0.5f *(f - n) * ndcPos0.z + 0.5f * (f + n));  
		// 		Vector3 screenPos1(w * 0.5f * ndcPos1.x + x + w * 0.5f, h* 0.5f * ndcPos1.y + y + h *0.5f, 0.5f *(f - n) * ndcPos1.z + 0.5f * (f + n));  
		// 		Vector3 screenPos2(w * 0.5f * ndcPos2.x + x + w * 0.5f, h* 0.5f * ndcPos2.y + y + h *0.5f, 0.5f *(f - n) * ndcPos2.z + 0.5f * (f + n));  

		// 		Color col0 = Color::RandomColor();
		//  		rasterizer.DrawTriangle2D(Vertex2D(Vector2(screenPos0.x, screenPos0.y), col0), Vertex2D(Vector2(screenPos1.x, screenPos1.y), col0), Vertex2D(Vertex2D(Vector2(screenPos2.x, screenPos2.y), col0)));

		// 	}
		// }



		//rasterizer.DrawPixel(100, 100, Color::red);
		//rasterizer.DrawOneLine(line, Color::green);
		/*sdl->Clean(Color::black);
		for (int i = 0; i < sdl->screenWidth; i++)
			for (int j = 0; j < sdl->screenHeight; j++)
			{
				tmpColor.r = i * widthInv;
				tmpColor.g = j * heightInv;
				//std::cout << tmpColor << std::endl;
				sdl->SetDrawColor(tmpColor);
				sdl->DrawPixel(i, j);
			}
		sdl->Render();*/

		
		//sdl->SwapBuffer(&renderContext);
	}


	getchar();
	return 0;
}

