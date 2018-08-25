#pragma once
#include <vector>
#include "CTypes.h"
#include "Math/Transform.h"

class Component;
class GameObject
{
	uint32 id;
	

	vector<GameObject*> children;         //GameObject has ownership of its children
	vector<Component*> components;    //GameObject has ownership of its GameComponents

public:
	Transform transform;
	GameObject();
	virtual ~GameObject();

	void SetID(uint32 id);
	void AddComponent(Component* component);
	void AddChild(GameObject* child);
	/*void InsertChild(GameObject* child, uint32 position);*/
	void RemoveChild(uint32 index);

	uint32 GetID() const;

	uint32 GetNumComponents() const;
	uint32 GetNumChildren() const;

	template<class T>
	T* GetComponent()
	{
		T* ret = 0;
		for (uint32 i = 0; i < components.size(); i++)
		{
			ret = dynamic_cast<T*>(components[i]);
			if (ret != 0)
				break;
		}
		return ret;
	}

	template<class T>
	void RemoveComponent()
	{
		T* comp = 0;
		for (uint32 i = 0; i < components.size(); i++)
		{
			comp = dynamic_cast<T*>(components[i]);
			if (comp != 0)
			{
				delete comp;
				components.erase(components.begin() + i);
				return;
			}
		}
	}

	GameObject* GetChild(uint32 index);
	Transform* GetTransform();

	//Do not override. Instead, make a custom GameComponent
	void Start();
	void Update();

protected:
private:
};