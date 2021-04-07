#include "GuiButton.h"
#include "App.h"
#include "Audio.h"

GuiButton::GuiButton(uint32 id, SDL_Rect bounds, const char* text, Menu* listener, Font* font) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;
	this->state = GuiControlState::NORMAL;
	this->observer = listener;
	this->font = font;

	//Load Fx
	clickFx = app->audio->LoadFx("Assets/Audio/Fx/button_click.wav");
	focusedFx = app->audio->LoadFx("Assets/Audio/Fx/button_focused.wav");
	isPlayable = true;
}

GuiButton::~GuiButton()
{
	app->audio->UnLoadFx(clickFx);
	app->audio->UnLoadFx(focusedFx);
}

bool GuiButton::Update(Input* input, float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		int mouseX, mouseY;
		input->GetMousePosition(mouseX, mouseY);

		// Check collision between mouse and button bounds
		if ((mouseX > bounds.x) && (mouseX < (bounds.x + bounds.w)) && 
			(mouseY > bounds.y) && (mouseY < (bounds.y + bounds.h)))
		{
			state = GuiControlState::FOCUSED;

			if (isPlayable == true)
			{
				app->audio->PlayFx(focusedFx);
				isPlayable = false;
			}

			if (input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT || input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
			{
				state = GuiControlState::PRESSED;
				if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
				{
					app->audio->PlayFx(clickFx);
				}
			}

			// If mouse button pressed -> Generate event!
			if (input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
			{
				if (NotifyObserver() == false) return false;
			}
		}
		else
		{
			state = GuiControlState::NORMAL;
			isPlayable = true;
		}
	}

	return true;
}

bool GuiButton::Draw(Render* render, bool showColliders, int size, SDL_Color color)
{
	// Draw the right button depending on state
	switch (state)
	{
	case GuiControlState::DISABLED:
		render->DrawTexture(texture, bounds.x, bounds.y, &section);
		//render->DrawCenterText(font, text.GetString(), bounds, size, 5, color);

		if (showColliders) render->DrawRectangle(bounds, 150, 150, 150, 150);
		else render->DrawRectangle(bounds, 150, 150, 150, 150);
		break;
	case GuiControlState::NORMAL:
		render->DrawTexture(texture, bounds.x, bounds.y, &section);
		//render->DrawCenterText(font, text.GetString(), bounds, size, 5, color);

		if (showColliders) render->DrawRectangle(bounds, 255, 0, 0, 150);
		break;
	case GuiControlState::FOCUSED:
		render->DrawTexture(texture, bounds.x, bounds.y, &section);
		//render->DrawCenterText(font, text.GetString(), bounds, size, 5, color);
		
		if (showColliders) render->DrawRectangle(bounds, 255, 255, 0, 150);
		else render->DrawRectangle(bounds, 255, 255, 0, 150);
		break;
	case GuiControlState::PRESSED:
		render->DrawTexture(texture, bounds.x, bounds.y, &section);
		//render->DrawCenterText(font, text.GetString(), bounds, size, 5, color);

		if (showColliders) render->DrawRectangle(bounds, 0, 255, 255, 150);
		else render->DrawRectangle(bounds, 0, 255, 255, 150);
		break;
	case GuiControlState::SELECTED:
		render->DrawTexture(texture, bounds.x, bounds.y, &section);
		//render->DrawCenterText(font, text.GetString(), bounds, size, 5, color);

		if (showColliders) render->DrawRectangle(bounds, 0, 255, 0, 150);
		else render->DrawRectangle(bounds, 0, 255, 0, 150);
		break;
	default:
		break;
	}

	return false;
}