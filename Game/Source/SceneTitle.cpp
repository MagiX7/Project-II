#include "App.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "GuiManager.h"

#include "SceneTitle.h"

#include "Audio.h"

#include "Log.h"

SceneTitle::SceneTitle()
{
	bg = nullptr;

}

bool SceneTitle::Load()
{
	LOG("Loading Scene Title");
	bool ret = true;
	
	bg = app->tex->Load("Assets/Textures/Scenes/Title/game_title.png");

	return ret;
}

bool SceneTitle::Update(float dt)
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		TransitionToScene(SceneType::MENU);
	}

	return ret;
}

void SceneTitle::Draw()
{
	app->render->DrawTexture(bg, 378, 257, NULL);
}

bool SceneTitle::UnLoad()
{
	LOG("Unloading Scene Title");
	bool ret = true;

	app->tex->UnLoad(bg);

	return ret;
}

bool SceneTitle::OnGuiMouseClickEvent(GuiControl* control)
{
	switch (control->type)
	{
	case GuiControlType::BUTTON: break;
	case GuiControlType::CHECKBOX: break;
	case GuiControlType::SLIDER: break;
	}
	return true;
}