#include"GameObject.h"
#include"Component.h"
#include"Math/Transform.h"

GameObject::GameObject()
	: transform(),
	id(0)
{
	children = vector<GameObject*>();
	components = vector<Component*>();
	transform.AttachToGameObject(this);
}

void GameObject::SetID(uint32 id)
{
	id = id;
}

void GameObject::AddComponent(Component* component)
{
	component->AttachToGameObject(this);
	components.push_back(component);
}

void GameObject::AddChild(GameObject* child)
{
	children.push_back(child);
	Transform* childTransform = child->GetTransform();
	//childTransform->SetParent(&transform);
}


/*
void GameObject::InsertChild(GameObject* child, uint32 position)
{
	if (position < 0 || position > children.size())
	{
		throw BengineException("Array Index Out of Bounds: cannot insert at this location.");
	}

	children.insert(children.begin() + position, child);
	Transform* childTransform = child->getTransform();
	childTransform->setParent(&transform);
}*/

void GameObject::RemoveChild(uint32 index)
{
	if (index >= 0 && index < children.size())
	{
		GameObject* child = children[index];
		delete child;
		children.erase(children.begin() + index);
	}
}

uint32 GameObject::GetID() const
{
	return id;
}

uint32 GameObject::GetNumComponents() const
{
	return components.size();
}


uint32 GameObject::GetNumChildren() const
{
	return children.size();
}

GameObject* GameObject::GetChild(uint32 index)
{
	return children[index];
}

Transform* GameObject::GetTransform()
{
	return &transform;
}

void GameObject::Start()
{
	//Start all of my Components
	vector<Component*>::iterator comp;
	for (comp = components.begin(); comp != components.end(); comp++)
	{
		(*comp)->start();
	}


	//Start all of my children
	vector<GameObject*>::iterator child;
	for (child = children.begin(); child != children.end(); child++)
	{
		(*child)->Start();
	}
}

void GameObject::Update()
{
	//Update all of my Components
	vector<Component*>::iterator comp;
	for (comp = components.begin(); comp != components.end(); comp++)
	{
		(*comp)->update();
	}

	//Update all of my children
	vector<GameObject*>::iterator child;
	for (child = children.begin(); child != children.end(); child++)
	{
		(*child)->Update();
	}
}

GameObject::~GameObject()
{
	//Must delete all Components
	/*for (uint32 i = 0; i < components.size(); i++)
	{
		if (components[i] != NULL)
		{
			delete components[i];
		}
	}*/
	components.clear();

	//Must delete all children GameObjects
/*
	for (uint32 i = 0; i < children.size(); i++)
	{
		if (children[i] != 0)
		{
			delete children[i];
		}
	}
	children.clear();*/
}



