#pragma once
#include "InputDefinition.h"
#include <string>


class Input
{
public:
	Input();
	~Input();

	bool Initialize();

	void Update();

	/// Tells if the keyboard `key` was released just now.
	bool GetKeyDown(int key);

	/// Tells if the keyboard `key` was pressed just now.
	bool GetKeyUp(int key);

	bool GetKey(KeyCode key);


	bool GetMouseButton(MouseButton button);

	bool GetMouseButtonDown(MouseButton button);

	bool GetMouseButtonUp(MouseButton button);

	int GetMouseDeltaX();
	int GetMouseDeltaY();


/*
	/// Helper to tell if any Shift is being hold.
	bool shift();

	/// Helper to tell if any Ctrl is being hold.
	bool ctrl();

	/// Helper to tell if any Alt is being hold.
	bool alt();
*/

	/// Tells if the user asked the game to quit.
	bool QuitRequested();

	/// Returns the current mouse X position.
	int GetMouseX();

	/// Returns the current mouse Y position.
	int GetMouseY();

	/// Locks the `InputManager`, preventing it to actually
	/// refresh input and making all methods returning `false`
	/// (except for `quitRequested()`.
	///
	/// @note For an explanation and tutorial,
	///       see the header of this class.
	///
	void Lock();

	/// Unlocks the `InputManager`, allowing it to respond
	/// normally to input.
	///
	void Unlock();


	// Those deal with printable keys
	// (keys on the ASCII table that can be shown onscreen - not
	// control-characters)

	/// Tells if the *key* can be printed onscreen
	/// (not a control key).
	/// If a key is visible according to the ASCII table.
	static bool IsPrintable(SDL_Keycode key);

	bool IsPrintableKeyDown();
	std::string GetCurPrintableKey();

	// Removing copy-constructor and assignment operator.
	Input(Input const&) {};
	void operator=(Input const&) {};

private:

	/// Saves SDL internal keyboard state.
	const uint8_t* keyboard;

	/// Saves SDL internal mouse state.
	uint32_t mouse;

	int lastMosePosX;
	int lastMosePosY;

	/// Current mouse X position.
	int mouseX;

	/// Current mouse Y position.
	int mouseY;

	/// Saves which keys are currently down.
	bool keyDown[KEYBOARD_SIZE];

	/// Saves which keys are currently up.
	bool keyUp[KEYBOARD_SIZE];

	/// Saves which mouse buttons are currently down.
	bool mouseDown[MOUSE_MAX];

	/// Saves which mouse buttons are currently up.
	bool mouseUp[MOUSE_MAX];

	/// Tells if we must quit the game.
	bool will_quit;

	/// If the user pressed a printable key, this is where
	/// it'll be stored.
	int curPrintableKey;

	/// Tells if the input manager is currently locked.
	bool isLocked;
};