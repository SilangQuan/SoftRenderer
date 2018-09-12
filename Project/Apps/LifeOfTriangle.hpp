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

class LifeOfTriangle: public AppBase
{
    public:
        void Init()
        {   
            InitDesc initDesc;
            initDesc.height = 360;
            initDesc.width = 640;

            //initDesc.height = 720;
            //initDesc.width = 1280;

            initDesc.isFullScreen = false;
            initDesc.title ="Soft Renderer by SilangQuan - LifeOfTriangle";
            mRenderSystem = new RenderSystem(initDesc);
            qDebug()<<"RenderSystem Init OK!";
            
            mModel = new Model("./Assets/LifeOfTriangle.obj");

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
                VertexColorTextureShader* shader = new VertexColorTextureShader();
                //SimpleTextureShader* shader = new SimpleTextureShader();
                mats[i]->Shader = shader;
                mats[i]->Shader->TextureUnit0 = mats[i]->DiffuseTex;
            }

            mCamera = new Camera(Vector3(0,0,3), Vector3(0,0,0), Vector3::up);
            mCamera->SetDepthRange(0,1);
            mCamera->SetPespective(60, 16.0/9, 0.3, 1000);
            mCamera->SetViewPort(0,0,initDesc.width, initDesc.height, true);
            Matrix4x4 modelMatrix = Matrix4x4::identity;

            qDebug() <<"ProjectionMatri: " << mCamera->projectionMaxtrix;  
            qDebug() << "ViewMatrix: " << mCamera->viewMatrix;
            qDebug() << "ModelMatrix: " << modelMatrix;  
            qDebug() << "MVP: " << mCamera->projectionMaxtrix * mCamera->viewMatrix * modelMatrix;  
            qDebug() << "ViewPortMatrix: " << mCamera->viewportMaxtrix;  

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
            Matrix4x4 modelMatrix = Matrix4x4::identity;

            handleEvents();
            //sdl->Clean(&renderContext, &(Color::white));
            mRenderSystem->Clear(mClearColor);

            std::map<Mesh*, Material*>::iterator meshIter = mModel->GetMeshMatMap().begin();

            mRenderSystem->RenderMesh(meshIter->first,&modelMatrix, meshIter->second);
            
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
        Camera* mCamera;
        Model* mModel;
        RenderSystem* mRenderSystem;
        Color mClearColor;
        float mShowFPSMark;
};