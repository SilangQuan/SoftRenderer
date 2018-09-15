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

class ClipInHomogeneous: public AppBase
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
            initDesc.title ="Soft Renderer by SilangQuan - ClipInHomogeneous";
            mRenderSystem = new RenderSystem(initDesc);
            qDebug()<<"RenderSystem Init OK!";
            
            mModel = new Model("./Assets/ColorSquare.obj");

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

            vector<Material*>& mats = mModel->GetMaterials();
            
            for(int i=0; i< mats.size(); i++)
            {
                VertexColorShader* shader = new VertexColorShader();
                //SimpleTextureShader* shader = new SimpleTextureShader();
                mats[i]->Shader = shader;
                mats[i]->Shader->TextureUnit0 = mats[i]->DiffuseTex;
            }

            mCamera = new Camera(Vector3(0,0,2), Vector3(0,0,0), Vector3::up);
            mCamera->SetDepthRange(0,1);
            mCamera->SetPespective(60, 16.0/9, 0.3, 1000);
            mCamera->SetViewPort(0,0,initDesc.width, initDesc.height, true);
            mModelMatrix = Matrix4x4::identity;
            mRenderSystem->SetCurCamera(mCamera);

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
             if(mInput->GetKeyDown(KeyCode::KEY_ESCAPE))
            {
                return false;
            }


            handleEvents();
             if(mInput->GetKeyDown(KeyCode::KEY_UP))
            {
                mModelMatrix.rotateX(4 * mTime->deltaTime);
            }
            if(mInput->GetKeyDown(KeyCode::KEY_DOWN))
            {
                mModelMatrix.rotateX(4 * mTime->deltaTime);
            }

            if(mInput->GetKeyDown(KeyCode::KEY_LEFT))
            {
                mModelMatrix.rotateX(4 * mTime->deltaTime);
            }

            if(mInput->GetKeyDown(KeyCode::KEY_RIGHT))
            {
                mModelMatrix.rotateY(20 * mTime->deltaTime);
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

            mRenderSystem->Clear(mClearColor);

            std::map<Mesh*, Material*>::iterator meshIter = mModel->GetMeshMatMap().begin();

            mRenderSystem->RenderMesh(meshIter->first,&mModelMatrix, meshIter->second, true);
            
            mRenderSystem->SwapBuffer();

            return true;
        }

        void Release()
        {
            delete mModel;
            delete mCamera;
            delete mRenderSystem;
        }

    private:
        Time* mTime;
        Input* mInput;
        Camera* mCamera;
        Model* mModel;
        RenderSystem* mRenderSystem;
        Color mClearColor;
        float mShowFPSMark;
        Matrix4x4 mModelMatrix;
};