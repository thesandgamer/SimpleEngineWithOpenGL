#include "Actor.h"
#include <algorithm>
#include "Game.h"
#include "Component.h"
#include "Maths.h"

Actor::Actor() :
	state(Actor::ActorState::Active), //Quand l'acteur est cr?er il est actif
	position(Vector2::zero),
	scale(1.0f),
	rotation(0.0f),
	game(Game::instance())
{
	game.addActor(this);
}

Actor::~Actor()
{
	game.removeActor(this);//Remove l'actor de la liste des acteurs du game
	while (!components.empty())
	{
		delete components.back();
	}
}

//Calcul le forward vector 
const Vector2 Actor::getForward() const
{
	return Vector2(Maths::cos(rotation),-Maths::sin(rotation));
}

void Actor::setPosition(Vector2 positionP)
{
	position = positionP;
}

void Actor::setScale(float scaleP)
{
	scale = scaleP;
}

void Actor::setRotation(float rotationP)
{
	rotation = rotationP;
}

void Actor::SetState(ActorState newState)
{
	state = newState;
}

void Actor::update(float dt)
{
	if (state == Actor::ActorState::Active)
	{
		updateComponents(dt);//Update ses components
		updateActor(dt);
	}
}

//On va r?cup?rer tous les components de l'acteur pour les update les uns ? la suite des autres
void Actor::updateComponents(float dt)
{
	for (auto component : components)
	{
		component->update(dt);
	}
}

void Actor::updateActor(float dt)
{

}


//Fonction pour ajouter un component ? l'acteur
void Actor::addComponent(Component* component)
{
	int myOrder = component->getUpdateOrder();//Get l'ordre d'update du componenent
	auto iter = begin(components);
	for (; iter != end(components); iter++)//On va parcourir tous les componenets
	{
		if (myOrder < (*iter)->getUpdateOrder()) //si l'ordre du compoenent x est sup?rieur ? celui du C ? ajouter
		{
			break;
		}
	}
	//Ins?re le component dans l'ordre d'?x?cution
	components.insert(iter, component);


}

void Actor::removeComponent(Component* component)
{
	auto iter = std::find(begin(components), end(components), component); //Cherche le componenent dans la liste
	if (iter != end(components))//Si on le trouve
	{
		components.erase(iter);//On l'erase
	}
}

//Processing input de l'acteur et de tous ses componenets
void Actor::processInput(const Uint8* keyState)
{
	if (state == Actor::ActorState::Active)//Si l'acteur est actif
	{
		for (auto component : components)//On va process les inputs de tous ses componenets
		{
			component->processInput(keyState);
		}
		actorInput(keyState);

	}
}

void Actor::actorInput(const Uint8* keystate)
{
}
