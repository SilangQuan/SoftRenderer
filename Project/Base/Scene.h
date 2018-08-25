#pragma once

class GameObject;
class Camera;

class Scene
{
public:

	Scene();
	virtual ~Scene();

	void Initialize();
	void Shutdown();

	virtual void Update();	//TODO make this non-virtual. Have everything done in custom GameComponents

							//Don't override
	void Start();

	void AddGameObject(GameObject* gameObject);

	virtual GameObject* GetRoot();
	//ResourceManager* getResourceManager();

	Camera* GetActiveCamera();
	Camera* GetDefaultCamera();
	void SetActiveCamera(Camera* camera);

private:
	GameObject* rootObject;
	//ResourceManager m_resourceManager;
	Camera* defaultCamera;
	Camera* activeCamera;

};