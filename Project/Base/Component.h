#pragma once

class GameObject;

class Component
{
public:
	Component();
	virtual ~Component();

	void AttachToGameObject(GameObject* gameObject);
	GameObject* GetGameObject() const;

	virtual void start();
	virtual void update();
	GameObject* gameObject;

protected:
private:
};