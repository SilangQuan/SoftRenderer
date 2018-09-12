#include "AppBase.h"
#include "Base/Time.h"
#include "Base/Input.h"
#include "TypeDef.h"
#include "RenderSystem/RenderSystem.h"
#include "Base/Model.h"
#include "Base/Camera.h"
#include "Light/Light.h"
#include "Base/Mesh.h"
#include "RenderSystem/Shader.h"

class Sponza: public AppBase
{
    public:

        void Init()
        {   
            mInput = new Input();
            mTime = new Time();
            mTime->Start();
            InitDesc initDesc;
            initDesc.height = 360;
            initDesc.width = 640;

            initDesc.isFullScreen = false;
            initDesc.title ="Soft Renderer by SilangQuan - Sponza";
            mRenderSystem = new RenderSystem(initDesc);
            qDebug()<<"RenderSystem Init OK!";
            
            mModel = new Model("./Assets/sponza.obj");

            std::vector<Mesh*> meshes = mModel->GetMeshes();
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

            vector<Material*>& mats = mModel->GetMaterials();
            
            for(int i=0; i< mats.size(); i++)
            {
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

            //mCamera= new Camera(Vector3(-40,120,-3), Vector3(-40,120.5,0), Vector3::up);
            mCamera = new Camera(Vector3(-2,5,-13), Vector3(-2,5,-12.5), Vector3::up);
            //Camera* cam = new Camera(Vector3(0,0,-3), Vector3(0,0,0), Vector3::up);
            mCamera->SetDepthRange(0,1);
            //mCamera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 0.3, 1000, true);
            mCamera->SetPespective(60, 16.0/9, 0.3, 2000);
            mCamera->SetViewPort(0,0,initDesc.width, initDesc.height, true);
            //mCamera->SetViewPort(0,initDesc.height,initDesc.width, 0, true);
            Matrix4x4 modelMatrix = Matrix4x4::identity;

            qDebug() <<"ProjectionMatri: " << mCamera->projectionMaxtrix;  
            qDebug() << "ViewMatrix: " << mCamera->viewMatrix;
            qDebug() << "ModelMatrix: " << modelMatrix;  
            qDebug() << "MVP: " << mCamera->projectionMaxtrix * mCamera->viewMatrix * modelMatrix;  

            mRenderSystem->SetCurCamera(mCamera);

            float showFpsDelta = 5;
            float showFpsMark = mTime->time;

            mClearColor = Color(0.212,0.212,0.212);
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

        bool Run()
        {
            mTime->Update();
		    mInput->Update();

            Matrix4x4 modelMatrix = Matrix4x4::identity;
            float showFpsDelta = 5;
            if(mTime->time - mShowFPSMark > showFpsDelta)
            {
                qDebug()<<"FPS: "<<mTime->fps;
                mShowFPSMark = mTime->time;
            }

            handleEvents();
            //sdl->Clean(&renderContext, &(Color::white));
            mRenderSystem->Clear(mClearColor);

            if(mInput->GetKeyDown(KeyCode::KEY_ESCAPE))
            {
                return false;
            }

            if(mInput->GetKeyDown(KeyCode::KEY_SPACE))
            {
                //modelMatrix.rotateX(4 * mTime->deltaTime);
                modelMatrix.rotate(20 * mTime->deltaTime, Vector3::one);
            }

            if(mInput->GetKeyDown(KeyCode::KEY_UP))
            {
                //modelMatrix.rotateX(4 * mTime->deltaTime);
                modelMatrix.rotateX(20 * mTime->deltaTime);
            }
            if(mInput->GetKeyDown(KeyCode::KEY_DOWN))
            {
                //modelMatrix.rotateX(4 * mTime->deltaTime);
                modelMatrix.rotateX(-20 * mTime->deltaTime);
            }

            if(mInput->GetKeyDown(KeyCode::KEY_LEFT))
            {
                //modelMatrix.rotateX(4 * mTime->deltaTime);
                modelMatrix.rotateY(-20 * mTime->deltaTime);
            }

            if(mInput->GetKeyDown(KeyCode::KEY_RIGHT))
            {
                modelMatrix.rotateY(20 * mTime->deltaTime);
            }

            if(mInput->GetKeyDown(KeyCode::KEY_W))
            {
                mCamera->transform.position += 0.5f * Vector3::forward;
                mCamera->target += 0.5f * Vector3::forward;
            }

            if(mInput->GetKeyDown(KeyCode::KEY_A))
            {
                mCamera->transform.position += Vector3::left;
                mCamera->target += Vector3::left;
            }

            if(mInput->GetKeyDown(KeyCode::KEY_D))
            {
                mCamera->transform.position += Vector3::right;
                mCamera->target += Vector3::right;
            }

            if(mInput->GetKeyDown(KeyCode::KEY_S))
            {
                mCamera->transform.position += Vector3::back;
                mCamera->target += Vector3::back;
            }

            if(mInput->GetKeyDown(KeyCode::KEY_E))
            {
                mCamera->transform.position += 0.5f * Vector3::up;
                mCamera->target += 0.5f * Vector3::up;
            }

            if(mInput->GetKeyDown(KeyCode::KEY_Q))
            {
                mCamera->transform.position += 0.5f * Vector3::down;
                mCamera->target += 0.5f * Vector3::down;
            }

            mCamera->RefreshViewMatrix();

            std::map<Mesh*, Material*>::iterator meshIter = mModel->GetMeshMatMap().begin();

            int count =0;
            while(meshIter != mModel->GetMeshMatMap().end())
            {
                mRenderSystem->RenderMesh(meshIter->first, &modelMatrix, meshIter->second);
                meshIter++;
                ++count;
            }
            
            mRenderSystem->SwapBuffer();
            return true;

        }

        void Release()
        {

        }

    private:
        Time* mTime;
        Input* mInput;
        Camera* mCamera;
        Model* mModel;
        RenderSystem* mRenderSystem;
        Color mClearColor;
        float mShowFPSMark;

};