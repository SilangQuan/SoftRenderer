
#include "Input.h"

Input::Input() :
	keyboard(nullptr),
	mouse(0),
	mouseX(0),
	mouseY(0),
	will_quit(false),
	isLocked(false)
{ }

Input::~Input()
{

}

bool Input::Initialize()
{
	return true;
}

int Input::GetMouseDeltaX()
{
	return mouseX - lastMosePosX;
}

int Input::GetMouseDeltaY()
{
	return mouseY - lastMosePosY;
}

void Input::Update()
{
	int i;
	for (i = 0; i < KEYBOARD_SIZE; i++)
	{
		this->keyDown[i] = false;
		this->keyUp[i] = false;
	}
	for (i = 0; i < MOUSE_MAX; i++)
	{
		this->mouseDown[i] = false;
		this->mouseUp[i] = false;
	}
	curPrintableKey = 0;

	lastMosePosX = mouseX;
	lastMosePosY = mouseY;

	// Get key events from the OS
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			this->will_quit = true;
			break;

			// SDL2's new way of handling input
		case SDL_TEXTINPUT:
			// WHAT
			break;

		case SDL_KEYDOWN:
		{
			this->keyboard = SDL_GetKeyboardState(nullptr);

			int index = event.key.keysym.scancode;

			this->keyDown[index] = true;

			if (Input::IsPrintable(event.key.keysym.sym))
				this->curPrintableKey = event.key.keysym.sym;
		}
		break;

		case SDL_KEYUP:
		{
			this->keyboard = SDL_GetKeyboardState(nullptr);

			int index = event.key.keysym.scancode;
			this->keyUp[index] = true;
		}
		break;

		case SDL_MOUSEMOTION:

			this->mouseX = event.motion.x ;
			this->mouseY = event.motion.y ;
			break;

		case SDL_MOUSEBUTTONDOWN:
			this->mouse = SDL_GetMouseState(&(this->mouseX),
				&(this->mouseY));

			if (event.button.button == SDL_BUTTON_LEFT)
				this->mouseDown[MOUSE_LEFT] = true;

			else if (event.button.button == SDL_BUTTON_RIGHT)
				this->mouseDown[MOUSE_RIGHT] = true;
			break;

		case SDL_MOUSEBUTTONUP:
			this->mouse = SDL_GetMouseState(&(this->mouseX),
				&(this->mouseY));

			if (event.button.button == SDL_BUTTON_LEFT)
				this->mouseUp[MOUSE_LEFT] = true;

			else if (event.button.button == SDL_BUTTON_RIGHT)
				this->mouseUp[MOUSE_RIGHT] = true;
			break;

			// Brand new SDL2 event.
		case SDL_MOUSEWHEEL:
			// event.x; // Ammount scrolled horizontally
			// // If negative, scrolled to the right
			// // If positive, scrolled to the left

			// event.y; // Ammount scrolled vertically
			// // If negative, scrolled down
			// // If positive, scrolled up
			break;

		default:
			break;
		}
	}
}
bool Input::GetKeyDown(int key)
{
	if (this->isLocked) return false;

	if (key < 0 || key >= KEYBOARD_SIZE)
		return false;

	return (this->keyDown[key]);
}
bool Input::GetKeyUp(int key)
{
	if (this->isLocked) return false;

	if (key < 0 || key >= KEYBOARD_SIZE)
		return false;

	return (this->keyUp[key]);
}
bool Input::GetKey(KeyCode key)
{
	if (this->isLocked) return false;

	if (!(this->keyboard))
		return false;

	int sdl_key = static_cast<int>(key);

	if (this->keyboard[sdl_key])
		return true;

	return false;
}

bool Input::GetMouseButtonDown(MouseButton button)
{
	if (this->isLocked) return false;

	if (button == MOUSE_MAX)
		return false;

	return this->mouseDown[button];
}
bool Input::GetMouseButtonUp(MouseButton button)
{
	if (this->isLocked) return false;

	if (button == MOUSE_MAX)
		return false;

	return this->mouseUp[button];
}
bool Input::GetMouseButton(MouseButton button)
{
	if (this->isLocked) return false;

	switch (button)
	{
	case MOUSE_LEFT:
		if (this->mouse & SDL_BUTTON(1))
			return true;
		break;

	case MOUSE_RIGHT:
		if (this->mouse & SDL_BUTTON(3))
			return true;
		break;

	default:
		break;
	}

	return false;
}
int Input::GetMouseX()
{
	return this->mouseX;
}
int Input::GetMouseY()
{
	return this->mouseY;
}
bool Input::QuitRequested()
{
	return (this->will_quit);
}

bool Input::IsPrintable(SDL_Keycode key)
{
	return ((key > SDLK_SPACE) && (key < SDLK_z));
}

bool Input::IsPrintableKeyDown()
{
	return (Input::IsPrintable(this->curPrintableKey));
}

std::string Input::GetCurPrintableKey()
{
	char c = (char)(this->curPrintableKey);
	const char* a = &c;
	return (std::string(a));
}
void Input::Lock()
{
	this->isLocked = true;
}
void Input::Unlock()
{
	this->isLocked = false;
}