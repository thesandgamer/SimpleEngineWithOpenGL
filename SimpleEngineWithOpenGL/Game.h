#pragma once
#include "Window.h"
#include "RendererSDL.h"
#include "IRenderer.h"
#include "Vector2.h"
#include <vector>
#include "Actor.h"
#include "Assets.h"
#include "SpriteComponent.h"

#include "Camera.h"

using std::vector;

class Game
{
public:
	static Game& instance()
	{
		static Game inst;
		return inst;
	}

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;

private:
	Game() :isRunning(true), isUpdatingActors(false), camera(nullptr) {}


public:
	bool initialize();
	void load();
	void loop();
	void unload();
	void close();

	void addActor(Actor* actor);
	void removeActor(Actor* actor);

	RendererOGL& getRenderer() { return renderer; }
	//IRenderer::Type type() { return Type::SDL; }

	//Game Specific


private:
	void processInput();
	void update(float dt);
	void render();

	bool isRunning;
	Window window;
	RendererOGL renderer;

	bool isUpdatingActors;
	vector<Actor*> actors;
	vector<Actor*> pendingActors;

	Camera* camera;
	//Game Specific


};

