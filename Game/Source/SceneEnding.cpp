#include "App.h"

#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Font.h"
#include "Easings.h"

#include "SceneEnding.h"

#include "Log.h"

SceneEnding::SceneEnding(bool win)
{
	bg = nullptr;
	font = nullptr;
	hasWin = win;

	if (hasWin) app->audio->PlayMusic("Assets/Audio/Music/win_theme.ogg");
	else app->audio->PlayMusic("Assets/Audio/Music/gameover_theme.ogg");

	easingUp = new Easing();
	easingDown = new Easing();

	easingUp->easingsActivated = true;
	easingUp->currentIteration = 0;
	easingUp->totalIterations = 75;
	easingUp->initialPos = 300;
	easingUp->deltaPos = 50;

	easingDown->easingsActivated = false;
	easingDown->currentIteration = 0;
	easingDown->totalIterations = 75;
	easingDown->initialPos = 350;
	easingDown->deltaPos = -50;
	
	easingPosition = 300;

	showColliders = false;
}

bool SceneEnding::Load()
{
	LOG("Loading Scene Ending");
	bool ret = true;

	if (hasWin) bg = app->tex->Load("Assets/Textures/Scenes/battle_bg3.png");
	else bg = app->tex->Load("Assets/Textures/Scenes/battle_bg2.png");

	font = new Font("Assets/Font/font3.xml", app->tex);

	return ret;
}

bool SceneEnding::Update(float dt)
{
	bool ret = true;

	if (easingUp->easingsActivated)
	{
		easingPosition = easingUp->exponentialEaseInOut(easingUp->currentIteration, easingUp->initialPos, easingUp->deltaPos, easingUp->totalIterations);
		if (easingUp->currentIteration < easingUp->totalIterations)
		{
			easingUp->currentIteration++;
		}
		else
		{
			easingUp->currentIteration = 0;
			easingUp->easingsActivated = false;
			easingDown->easingsActivated = true;
		}
	}

	if (easingDown->easingsActivated)
	{
		easingPosition = easingDown->exponentialEaseInOut(easingDown->currentIteration, easingDown->initialPos, easingDown->deltaPos, easingDown->totalIterations);
		if (easingDown->currentIteration < easingDown->totalIterations)
		{
			easingDown->currentIteration++;
		}
		else
		{
			easingDown->currentIteration = 0;
			easingDown->easingsActivated = false;
			easingUp->easingsActivated = true;
		}
	}

	return ret;
}

void SceneEnding::Draw()
{
	app->render->DrawTexture(bg, 0, 0);

	SDL_Color color = { 255,255,255,255 };

	if (hasWin) app->render->DrawText(font, "YOU WIN", 350, easingPosition, 150, 10, color);
	else app->render->DrawText(font, "YOU LOSE", 350, easingPosition, 150, 10, color);
}

bool SceneEnding::UnLoad()
{
	LOG("Unloading Scene Ending");
	bool ret = true;

	app->tex->UnLoad(bg);
	font->UnLoad(app->tex);

	RELEASE(font);
	RELEASE(easingUp);
	RELEASE(easingDown);

	return ret;
}