#include "RendererOGL.h"
#include "GL\glew.h" 
#include "Rectangle.h"
#include "Log.h"
#include "SpriteComponent.h"
#include "Actor.h"
#include "Assets.h"
#include "MeshComponent.h"

#include <SDL_image.h>
#include "Assets.h"

RendererOGL::RendererOGL() :
	window(nullptr),
	context(nullptr),
	spriteVertexArray(nullptr),
	spriteViewProj(Matrix4::createSimpleViewProj(WINDOW_WIDTH, WINDOW_HEIGHT)),
	view(Matrix4::createLookAt(Vector3::zero, Vector3::unitX, Vector3::unitZ)),
	projection(Matrix4::createPerspectiveFOV(Maths::toRadians(70.0f),WINDOW_WIDTH, WINDOW_HEIGHT, 25.0f, 10000.0f))
{
}

RendererOGL::~RendererOGL()
{
}

RendererOGL::RendererOGL(const RendererOGL* sprite): window(nullptr), spriteVertexArray(nullptr),context(nullptr),shader(nullptr)
{
}

bool RendererOGL::initialize(Window& windowP)
{
	window = &windowP;

	//Set openGL attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Request a color buffer with 8-bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	context = SDL_GL_CreateContext(windowP.getSDLWindow());
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		Log::error(LogCategory::Video, "Failed to initialize GLEW");
		return false;
	}

	//On some platformes GLEW will meit a bening eror code, so on le clear
	glGetError();

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		Log::error(LogCategory::Video, "Unable to initialize SDL_image");
		return false;
	}
	/*
	shader = &Assets::getShader("Basic");
	shader = &Assets::getShader("Transform");
	shader = &Assets::getShader("Sprite");
	*/

	spriteVertexArray = new VertexArray(spriteVertices,4, indices, 6);

	return true;

}

void RendererOGL::beginDraw()
{
	glClearColor(0.45f, 0.45f, 1.0f, 1.0f);
	/*
	//Clear le color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	//Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Active le shader et le vertex array
	Assets::getShader("Sprite").use();
	Assets::getShader("Sprite").setMatrix4("uViewProj", spriteViewProj);
	spriteVertexArray->setActive();*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererOGL::draw()
{
	drawMesh();
	//drawSprites();
}

void RendererOGL::endDraw()
{
	SDL_GL_SwapWindow(window->getSDLWindow());
}

void RendererOGL::addSprite(SpriteComponent* sprite)
{
	//Ajoute le sprite en fonction de son draw order
	int spriteDrawOrder = sprite->getDrawOrder();
	auto iter = begin(sprites);
	for (; iter != end(sprites); ++iter)
	{
		if (spriteDrawOrder < (*iter)->getDrawOrder()) break;
	}
	sprites.insert(iter, sprite);

}

void RendererOGL::removeSprite(SpriteComponent* sprite)
{
	auto iter = std::find(begin(sprites), end(sprites), sprite);
	sprites.erase(iter);
}

void RendererOGL::drawSprite(const Actor& actor, const Texture& tex, Rectangle scrRect, Vector2 origin, Flip flip) const
{
	Matrix4 scaleMat = Matrix4::createScale((float)tex.getWidth(), (float)tex.getWidth(), 1.0f);
	Matrix4 world = scaleMat * actor.getWorldTransform();
	//Matrix4 pixelTranslation = Matrix4::createTranslation(Vector3(-WINDOW_WIDTH / 2 - origin.x, -WINDOW_HEIGHT / 2 - origin.y, 0.0f)); //Screen pixel coordinates
	Assets::getShader("Sprite").setMatrix4("uWorldTransform", world /** pixelTranslation*/);
	tex.setActive();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void RendererOGL::close()
{
	SDL_GL_DeleteContext(context);
	delete spriteVertexArray;
}

void RendererOGL::addMesh(MeshComponent* mesh)
{
	meshes.emplace_back(mesh);
}

void RendererOGL::removeMesh(MeshComponent* mesh)
{
	auto iter = std::find(begin(meshes), end(meshes), mesh);
	meshes.erase(iter);
}

void RendererOGL::setViewMatrix(const Matrix4& viewP)
{
	view = viewP;
}

void RendererOGL::drawSprites()
{
	glDisable(GL_DEPTH_TEST);
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// Active shader and vertex array
	Shader& spriteShader = Assets::getShader("Sprite");
	spriteShader.use();
	spriteShader.setMatrix4("uViewProj", spriteViewProj);
	spriteVertexArray->setActive();
	
	for (auto sprite : sprites)
	{
		sprite->draw(*this);
	}
}

void RendererOGL::drawMesh()
{
	//Enable depth buffering /disable alpha blend
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	Assets::getShader("BasicMesh").setMatrix4("uViewProj", view * projection);
	for (auto mc : meshes)
	{
		mc->draw(Assets::getShader("BasicMesh"));
	}
}

