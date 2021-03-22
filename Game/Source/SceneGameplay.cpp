#include "App.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"

#include "Player.h"
#include "SceneGameplay.h"
#include "CharacterManager.h"

#include "Audio.h"

#include "Log.h"

SceneGameplay::SceneGameplay()
{
	name.Create("scenegameplay");

	player = new Player(PlayerType::HUNTER);

	showColliders = false;

	bg = nullptr;
}

bool SceneGameplay::Load()
{
	LOG("Loading Scene Gameplay");
	bool ret = true;

	player->Load();

	// Startup
	state = GameplayMenuState::NONE;

	// Instantiate character swap manager
	charManager = new CharacterManager(player, this);
	charManager->Load();

	return ret;
}

bool SceneGameplay::Update(float dt)
{
	bool ret = true;

	player->Update(dt);

	if (app->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
	{
		state = GameplayMenuState::CHARACTER_SWAP;
	}

	switch (state)
	{
	case GameplayMenuState::CHARACTER_SWAP:
		charManager->Update(dt);
		break;
	}

	return ret;
}

void SceneGameplay::Draw()
{
	player->Draw(showColliders);

	if (state == GameplayMenuState::CHARACTER_SWAP)
	{
		charManager->Draw(showColliders);
	}
}

bool SceneGameplay::UnLoad()
{
	LOG("Unloading Scene Gameplay");
	bool ret = true;

	RELEASE(player);

	return ret;
}

bool SceneGameplay::LoadState(pugi::xml_node& load)
{

	return true;
}

bool SceneGameplay::SaveState(pugi::xml_node& save) const
{

	return true;
}

void SceneGameplay::ChangeState(GameplayMenuState type)
{
	state = type;
}

void SceneGameplay::SetPlayer(Player* pl)
{
	player = pl;
}