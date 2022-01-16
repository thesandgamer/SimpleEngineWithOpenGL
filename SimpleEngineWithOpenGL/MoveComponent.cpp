#include "MoveComponent.h"
#include "Maths.h"
#include "Actor.h"
#include "Window.h"

MoveComponent::MoveComponent(Actor* ownerP, int updateOrderP) :
	Component(ownerP, updateOrderP), forwardSpeed(0.0f),angularSpeed(0.0f)
{

}

void MoveComponent::setForwardSpeed(float forwardSpeedP)
{
	forwardSpeed = forwardSpeedP;
}

void MoveComponent::setAngularSpeed(float angularSpeedP)
{
	angularSpeed = angularSpeedP;
}

void MoveComponent::update(float dt)
{
	if (!Maths::nearZero(angularSpeed))//Rotation
	{
		Quaternion newRotation = owner.getRotation();
		float angle = angularSpeed * dt;
		Quaternion increment(Vector3::unitZ, angle);
		owner.setRotation(newRotation);
	}
	if (!Maths::nearZero(forwardSpeed))//Mouvement
	{
		Vector3 newPosition = owner.getPosition() + owner.getForward()* forwardSpeed * dt;
		owner.setPosition(newPosition);
	}

}
