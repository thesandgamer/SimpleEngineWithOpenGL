#include "Game.h"
#include "Timer.h"
#include "SpriteComponent.h"
#include "AnimSpriteComponent.h"
#include "BackgroundSpriteComponent.h"
#include "MeshComponent.h"

bool Game::initialize()
{
	bool isWindowInit = window.initialize();
	bool isRendererInit = renderer.initialize(window);

	int windowWidth = window.getWidth();
	int windowHeight = window.getHeight();


	return isWindowInit && isRendererInit; // Return bool && bool && bool ...to detect error
}

void Game::load()
{
	
	//Load Textures _______________________________________________
	Assets::loadTexture(renderer, "Res\\Textures\\Default.png", "Default");
	Assets::loadTexture(renderer, "Res\\Textures\\Cube.png", "Cube");
	Assets::loadTexture(renderer, "Res\\Textures\\HealthBar.png", "HealthBar");
	Assets::loadTexture(renderer, "Res\\Textures\\Plane.png", "Plane");
	Assets::loadTexture(renderer, "Res\\Textures\\Radar.png", "Radar");
	Assets::loadTexture(renderer, "Res\\Textures\\Sphere.png", "Sphere");

	//_____________________________________________________________________

	//Load Shader _______________________________________________
	Assets::loadShader("Res\\Shaders\\Sprite.vert", "Res\\Shaders\\Sprite.frag", "", "", "", "Sprite");
	Assets::loadShader("Res\\Shaders\\BasicMesh.vert", "Res\\Shaders\\BasicMesh.frag", "", "", "", "BasicMesh");
	//_____________________________________________________________________

	//Load Meshes______________________________________________________
	Assets::loadMesh("Res\\Meshes\\Cube.gpmesh", "Mesh_Cube");
	Assets::loadMesh("Res\\Meshes\\Plane.gpmesh", "Mesh_Plane");
	Assets::loadMesh("Res\\Meshes\\Sphere.gpmesh", "Mesh_Sphere");
	//________________________________________________________________________________

	//Animated Sprite_________________________________

	//_________________________________________

	Actor* ui = new Actor();
	ui->setPosition(Vector3(-350.0f, -350.0f, 0.0f));
	SpriteComponent* sc = new SpriteComponent(ui, Assets::getTexture("HealthBar"));

	camera = new Camera();

	Actor* a = new Actor();
	a->setPosition(Vector3(200.0f, 105.0f, 0.0f));
	a->setScale(100.0f);
	Quaternion q(Vector3::unitY, -Maths::piOver2);
	q = Quaternion::concatenate(q, Quaternion(Vector3::unitZ, Maths::pi + Maths::pi / 4.0f));
	a->setRotation(q);
	MeshComponent* mc = new MeshComponent(a);
	mc->setMesh(Assets::getMesh("Mesh_Cube"));

	Actor* b = new Actor();
	b->setPosition(Vector3(200.0f, -75.0f, 0.0f));
	b->setScale(3.0f);
	MeshComponent* mcb = new MeshComponent(b);
	mcb->setMesh(Assets::getMesh("Mesh_Sphere"));
	

}

//Check des InputsS
void Game::processInput()
{
	// SDL Event
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		}
	}
	// Keyboard state
	const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
	// Escape: quit game
	if (keyboardState[SDL_SCANCODE_ESCAPE])
	{
		isRunning = false;
	}

	//Check tous les actors et process leurs inputs
	isUpdatingActors = true;
	for (auto actor : actors)
	{
		actor->processInput(keyboardState);
	}
	isUpdatingActors = false;


}

void Game::update(float dt)
{
	//Update les acteurs
	isUpdatingActors = true;
	for (auto actor : actors)
	{
		actor->update(dt);
	}
	isUpdatingActors = false;

	//Fait que les actors en attente ne le soit plus
	for (auto pendingActor : pendingActors)
	{
		pendingActor->computeWorldTransform();
		actors.emplace_back(pendingActor);
	}
	pendingActors.clear();


	//Ajoute les acteur mort dans un liste d'acteur morts
	vector<Actor*> deadActors;
	for (auto actor : actors)
	{
		if (actor->getState() == Actor::ActorState::Dead)
		{
			deadActors.emplace_back(actor);//Place les acteurs mort � la fin de la liste
		}
	}
	//Delete tous les acteurs morts
	for (auto deadActor : deadActors)
	{
		delete deadActor;
	}


}

//Dessin 
void Game::render()
{
	renderer.beginDraw();
	renderer.draw();
	renderer.endDraw();
}

//Loop du jeu
void Game::loop()
{
	Timer timer;//Cr�er l'objet de class timer
	float dt = 0;//Setup un delta time � 0
	while (isRunning)
	{
		float dt = timer.computeDeltaTime() / 1000.0f;//Calcul de delta time du timer
		processInput();
		update(dt);
		render();
		timer.delayTime();
	}
}

//Quand on va unload le jeu
void Game::unload()
{
	//Delete actors tant qu'il en reste stoqu� dans le game
	while (!actors.empty())
	{
		delete actors.back();
	}
	//Ressources
	Assets::clear();

}

//Quand on ferme le jeu
void Game::close()
{
	renderer.close();
	window.close();
	SDL_Quit();
}


//Ajoute un acteur au jeu
void Game::addActor(Actor* actor)
{
	if (isUpdatingActors)//Si on est en train d'updater les acteurs on ajoute l'acteur � la liste des acteurs en attente
	{
		pendingActors.emplace_back(actor);
	}
	else//Sinon on l'ajoute � la liste des acteurs
	{
		actors.emplace_back(actor);
	}
}

//Enl�ve un acteur
void Game::removeActor(Actor* actor)
{
	auto iter = std::find(begin(pendingActors), end(pendingActors), actor);
	if (iter != end(pendingActors))//Si l'acteur demand� est trouv� dasn la liste des acteurs en attente
	{
		std::iter_swap(iter, end(pendingActors) - 1);//Remplace l'objet actuel par le dernier
		pendingActors.pop_back(); //on l'enl�ve de la liste le dernier objet
	}
	iter = std::find(begin(actors), end(actors), actor);
	if (iter != end(actors))//Si l'acteur demand� est trouv� dans la liste des acteurs
	{
		std::iter_swap(iter, end(actors) - 1);
		actors.pop_back();
	}
}


//______Game Specific__________________________
