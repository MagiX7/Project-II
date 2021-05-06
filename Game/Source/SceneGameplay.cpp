#include "App.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"

#include "SceneGameplay.h"
#include "SceneBattle.h"

#include "EntityManager.h"
#include "Hunter.h"
#include "Wizard.h"
#include "Thief.h"
#include "Warrior.h"

#include "Golem.h"
#include "Bat.h"
#include "Skull.h"

#include "UltraPotion.h"
#include "Potion.h"

#include "KnightHelmet.h"

#include "Npc.h"
#include "Map.h"
#include "DialogueManager.h"

#include "CharacterManager.h"
#include "PauseMenu.h"

#include "Inventory.h"

#include "Log.h"

#define NPC_RADIUS 10

SceneGameplay::SceneGameplay()
{
	name.Create("scenegameplay");

	entityManager = new EntityManager();

	iPoint position = { 260,290 };

	pugi::xml_document animations;
	pugi::xml_node anims;
	pugi::xml_parse_result result = animations.load_file("animations.xml");

	if (result == NULL) 
		LOG("Could not load xml file: %s. pugi error: %s", "animations.xml", result.description());
	else 
		anims = animations.child("animations");

	Player* player = nullptr;
	player = new Hunter(position, anims);
	currentPlayer = player;
	playerList.push_back(player);
	position = { 18,90 };
	player = new Wizard(position, anims);
	playerList.push_back(player);

	position = { 12,45 };
	player = new Thief(position, anims);
	playerList.push_back(player);

	position = { 13,56 };
	player = new Warrior(position, anims);
	playerList.push_back(player);

	tmpPosPlayer = { 0,0 };

	Enemy* en;
	en = new Golem({ 865, 365 }, anims);
	en->SetCurrentState(EnemyState::ROAMING);
	enemyList.push_back(en);

	en = new Skull({ 260, 868 }, anims);
	en->SetCurrentState(EnemyState::ROAMING);
	enemyList.push_back(en);

	en = new Bat({ 1087, 850 }, anims);
	en->SetCurrentState(EnemyState::ROAMING);
	enemyList.push_back(en);

	Npc* generalNpc = nullptr;
	/*position = { 500,500 };
	generalNpc=(Npc*)entityManager->CreateEntity(EntityType::TABERN, position,anims);*/

	position = { 200,250 };
	generalNpc = (Npc*)entityManager->CreateEntity(EntityType::TOWN, position, anims, 0);

	position = { 700,1060 };
	generalNpc = (Npc*)entityManager->CreateEntity(EntityType::RAY, position, anims, 3);
	//generalNpc->NpcMove(false);

	position = { 700,810 };
	generalNpc = (Npc*)entityManager->CreateEntity(EntityType::TABERN, position, anims, 2);

	position = { 500,350 };
	generalNpc = (Npc*)entityManager->CreateEntity(EntityType::NPC_WIZARD, position, anims, 1);

	SDL_Texture* atlas = app->tex->Load("Assets/Textures/Items/items_atlas.png");
	inventory = new Inventory(playerList, atlas);

	Item *item = new UltraPotion(iPoint(200,250), atlas);
	items.push_back(item);
	
	item = new UltraPotion(iPoint(300,350), atlas);
	items.push_back(item);

	item = new Potion(iPoint(250, 350), atlas);
	items.push_back(item);

	helmet = new KnightHelmet({ 270, 350, 32, 32 }, iPoint(270, 350),atlas);

	//Create Enemies

	/*Enemy* skull = nullptr;
	position = { 300,100 };
	skull = (Enemy*)entityManager->CreateEntity(EntityType::SKULL, position);

	Enemy* golem = nullptr;
	position = { 300,200 };
	golem = (Enemy*)entityManager->CreateEntity(EntityType::GOLEM, position);

	Enemy* bat = nullptr;
	position = { 300,300 };
	bat = (Enemy*)entityManager->CreateEntity(EntityType::BAT, position);*/

	/*player = new Player(PlayerType::HUNTER);
	npc = new Npc(EntityType::NPC);*/

	


	pause = new PauseMenu(this);

	font = new Font("Assets/Font/font3.xml", app->tex);

	showColliders = false;
	transition = false;
	fadeOut = false;
	alpha = 0.0f;
}

bool SceneGameplay::Load()
{
	LOG("Loading Scene Gameplay");
	bool ret = true;


	entityManager->Load();

	eastl::list<Player*>::iterator it = playerList.begin();
	for (; it != playerList.end(); ++it)
		(*it)->Load();

	// Startup
	menuState = GameplayMenuState::NONE;
	gameState = GameplayState::NONE;

	// Instantiate character swap manager
	charManager = new CharacterManager(this, currentPlayer->playerType, font);
	charManager->Load(font);

	// Start music
	app->audio->PlayMusic("Assets/Audio/Music/village_theme_1.ogg", 0);

	map = new Map();
	isTown = true;
	map->Load("town_map.tmx", app->tex);

	dialogueManager = new DialogueManager();
	dialogueManager->Start();

	pause->Load(font);

	inventory->Load(font);

	helmet->Load();

	eastl::list<Item*>::iterator item = items.begin();
	for (; item != items.end(); ++item)
	{
		(*item)->Load();
	}

	sceneBattle = nullptr;

	SDL_ShowCursor(SDL_ENABLE);

	return ret;
}

bool SceneGameplay::Update(float dt)
{
	bool ret = true;

	switch (gameState)
	{
	case GameplayState::NONE:
		gameState = GameplayState::ROAMING;
		break;
	case GameplayState::ROAMING:
		switch (menuState)
		{
		case GameplayMenuState::NONE:
			if(SDL_ShowCursor(-1) == SDL_DISABLE)
				SDL_ShowCursor(SDL_ENABLE);

			if (dialogueManager->isDialogueActive == false)
			{
				map->Update(dt);
				HandleInput(dt);
				SDL_Rect tmpBounds = currentPlayer->bounds;
				currentPlayer->Update(dt);
				if (isTown)
				{
					eastl::list<Enemy*>::iterator enemies = enemyList.begin();
					for (; enemies != enemyList.end(); ++enemies)
					{
						(*enemies)->Update(dt);
						if (!showColliders && CheckCollision((*enemies)->bounds, currentPlayer->bounds))
						{
							GenerateBattle();
							tmp = (*enemies);
							break;
						}
					}
					//if (ultraPotion != nullptr) ultraPotion->Update(dt);
				}
				if (showColliders == false && CollisionMapEntity(currentPlayer->bounds,currentPlayer->type) == true) 
					currentPlayer->bounds = tmpBounds;

				CameraFollow(app->render);
				/*npc->Update(dt);*/
				entityManager->Update(dt);
				entityManager->CheckEntityColision(this);
				CheckDialogue();

				eastl::list<Item*>::iterator it = items.begin();
				for (; it != items.end(); ++it)
				{
					(*it)->Update(dt);
					if (CheckCollision(currentPlayer->bounds, (*it)->bounds))
					{
						inventory->AddItem(*it);
						items.erase(it);
						//items.erase(it);
						//RELEASE((*it));
					}
				}

				if (CheckCollision(currentPlayer->bounds, helmet->bounds))
				{
					inventory->AddArmor(helmet);
				}
			}
			else
			{
				dialogueManager->Update(dt);
			}
			break;
		case GameplayMenuState::CHARACTER_SWAP:
			charManager->Update(dt);
			break;

		case GameplayMenuState::INVENTORY:
			inventory->Update(dt);
			HandleInput(dt);
			break;

		case GameplayMenuState::PAUSE:
			ret = pause->Update(dt);
			break;
		}
		break;
	case GameplayState::BATTLE:
		if (sceneBattle->Update(dt) == false)
		{
			transition = true;
			fadeOut = true;
		}
	break;
	}

	if (transition) Fading(dt);

	return ret;
}

void SceneGameplay::Draw()
{
	eastl::list<Enemy*>::iterator enemies = enemyList.begin();
	switch (gameState)
	{
	case GameplayState::ROAMING:
		map->Draw(showColliders);
		entityManager->Draw(showColliders);
		currentPlayer->Draw(showColliders);

		if (isTown)
		{
			for (; enemies != enemyList.end(); ++enemies)
			{
				(*enemies)->Draw(showColliders);
			}

			//if (ultraPotion != nullptr) ultraPotion->Draw(showColliders);

			eastl::list<Item*>::iterator it = items.begin();
			for(; it != items.end(); ++it)
			{
				(*it)->Draw(showColliders);
			}

			helmet->Draw(showColliders);
		}

		if (dialogueManager->isDialogueActive)
		{
			app->render->DrawRectangle({ 0, 0,1280, 720 }, 0, 0, 0, 150, true, false);
			dialogueManager->Draw();
		}

		switch (menuState)
		{
		case GameplayMenuState::CHARACTER_SWAP:
			app->render->DrawRectangle({ 0, 0, 1280, 720 }, 0, 0, 0, 150, true, false);
			charManager->Draw(font, showColliders);
			break;

		case GameplayMenuState::PAUSE:
			pause->Draw(font, showColliders);
			break;

		case GameplayMenuState::INVENTORY:
			inventory->Draw(font, showColliders);
			break;
		}
		//if (menuState == GameplayMenuState::CHARACTER_SWAP)
		//{
		//	app->render->DrawRectangle({ -app->render->camera.x, -app->render->camera.y, 1280, 720 }, 0, 0, 0, 150);
		//	charManager->Draw(font, showColliders);
		//}

		//if (menuState == GameplayMenuState::PAUSE)
		//{
		//	//app->render->DrawRectangle({ -app->render->camera.x, -app->render->camera.y, 1280, 720 }, 0, 0, 0, 150);
		//	pause->Draw(font, showColliders);
		//}
		break;
	case GameplayState::BATTLE:
		sceneBattle->Draw(showColliders);
		break;
	}

	if (transition) 
		app->render->DrawRectangle({ 0, 0, 1280, 720 }, 0, 0, 0, 255 * alpha, true, false);
}

bool SceneGameplay::UnLoad()
{
	LOG("Unloading Scene Gameplay");
	bool ret = true;

	entityManager->UnLoad();
	//entityManager->DeleteAllEntities();
	RELEASE(entityManager);

	map->CleanUp();
	RELEASE(map);
	
	eastl::list<Player*>::iterator it = playerList.begin();
	for (; it != playerList.end(); ++it)
	{
		(*it)->UnLoad();
		RELEASE((*it));
		playerList.erase(it);
	}
	playerList.clear();

	charManager->UnLoad();
	RELEASE(charManager);

	pause->UnLoad();
	RELEASE(pause);

	inventory->UnLoad();
	RELEASE(inventory);
	
	dialogueManager->UnLoad();
	RELEASE(dialogueManager);

	font->UnLoad(app->tex);
	RELEASE(font);

	return ret;
}

void SceneGameplay::CharacterSwap(PlayerType player)
{
	SDL_Rect tmpBounds = currentPlayer->bounds;
	if (player != currentPlayer->playerType)
	{
		eastl::list<Player*>::iterator it = playerList.begin();
		for (; it != playerList.end(); ++it)
		{
			if ((*it)->playerType == player)
			{
				currentPlayer = (*it);
				break;
			}
		}
	}
	currentPlayer->bounds = tmpBounds;
}

bool SceneGameplay::CheckDialogue()
{
	bool ret = false;
	eastl::list<Entity*>::iterator it = entityManager->entities.begin();
	while (it != entityManager->entities.end())
	{
		if ((*it) != nullptr) {
			ret = (*it)->CheckCollision(currentPlayer);
			if (ret) break;
		}
		++it;
	}

	if (ret)
	{
		dialogueManager->current = dialogueManager->LoadDialogue((*it)->GetDialogeId());
		dialogueManager->isDialogueActive = true;
		dialogueManager->printText = true;
		(*it)->SetDrawPtext(false);
		(*it)->SetTalkStart(true);
	}
	return ret;
}

bool SceneGameplay::LoadState(pugi::xml_node& load)
{
	pugi::xml_node toLoadEntities = load.child("entities");
	pugi::xml_document animations;
	pugi::xml_node anims;
	pugi::xml_parse_result result = animations.load_file("animations.xml");
	anims = animations.child("animations");

	entityManager->LoadState(&toLoadEntities, &anims);

	eastl::list<Player*>::iterator item = playerList.begin();
	for (item; item != playerList.end(); ++item)
	{
		(*item)->UnLoad();
		RELEASE((*item));
		playerList.erase(item);
	}

	int playerAmount = toLoadEntities.child("players").attribute("amount").as_int();
	pugi::xml_node NodePlayer = toLoadEntities.child("players");
	pugi::xml_node NodePlayerAuxiliar = NodePlayer.child("player");
	bool iscurrent = false;
	for (int i = 0; i < playerAmount; ++i) 
	{
		iscurrent = false;

		SString string;
		SString string1;
		string = NodePlayerAuxiliar.child("playerType").attribute("type").as_string();
		string1 = NodePlayerAuxiliar.child("isCurrent").attribute("current").as_string();
		if (string1 == "true") iscurrent = true;

		EntityType plType;
		Player* player = nullptr;
		if (string == "HUNTER") 
			player = new Hunter({ NodePlayerAuxiliar.child("bounds").attribute("X").as_int(), NodePlayerAuxiliar.child("bounds").attribute("Y").as_int() }, anims);
		else if (string == "WIZARD") 
			player = new Wizard({ NodePlayerAuxiliar.child("bounds").attribute("X").as_int(), NodePlayerAuxiliar.child("bounds").attribute("Y").as_int() }, anims);
		else if (string == "WARRIOR") 
			player = new Warrior({ NodePlayerAuxiliar.child("bounds").attribute("X").as_int(), NodePlayerAuxiliar.child("bounds").attribute("Y").as_int() }, anims);
		else if (string == "THIEF") 
			player = new Thief({ NodePlayerAuxiliar.child("bounds").attribute("X").as_int(), NodePlayerAuxiliar.child("bounds").attribute("Y").as_int() }, anims);
		else 
			plType = EntityType::UNKNOWN;
		
		if (iscurrent == true) currentPlayer = player;
		playerList.push_back(player);
		player->Load();
		NodePlayerAuxiliar = NodePlayerAuxiliar.next_sibling();
	}
	return true;
}

bool SceneGameplay::SaveState(pugi::xml_node& save) const
{
	if (isTown == false) return true;

	save.append_attribute("map_name").set_value(map->name.GetString());

	pugi::xml_node toSaveEntites = save.append_child("entities");
	entityManager->SaveState(&toSaveEntites);
	/*const pugi::char_t* name = map->name.GetString();
	pugi::xml_node toSaveScene = toSaveEntites.append_child("scene");
	toSaveScene.append_attribute("mapName").set_value(name);
	toSaveScene.append_attribute("sceneType").set_value("sceneGameplay");*/

	pugi::xml_node nodePlayers = toSaveEntites.append_child("players");
	pugi::xml_node nodePlayersAuxiliar;
	int playerAmount = 0;
	nodePlayers.append_attribute("amount").set_value(playerList.size());
	nodePlayersAuxiliar = nodePlayers.append_child("player");
	eastl::list<Player*>::iterator aux;
	aux = playerList.begin().mpNode;
	for (aux; aux != playerList.end(); ++aux)
	{
		if ((*aux) == currentPlayer)
		{
			nodePlayersAuxiliar.append_child("isCurrent").append_attribute("current").set_value("true");
		}
		else
		{
			nodePlayersAuxiliar.append_child("isCurrent").append_attribute("current").set_value("false");
		}
		(*aux)->SaveState(nodePlayersAuxiliar);
		nodePlayersAuxiliar = nodePlayers.append_child("player");
	}

	return true;
}

void SceneGameplay::ChangeState(GameplayMenuState type)
{
	menuState = type;
}

void SceneGameplay::HandleInput(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN || app->input->pad->GetButton(SDL_CONTROLLER_BUTTON_X) == KEY_DOWN)
	{
		// Instantiate and load scene battle
		GenerateBattle();
	}

	if (app->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN || app->input->pad->GetButton(SDL_CONTROLLER_BUTTON_Y) == KEY_DOWN) menuState = GameplayMenuState::CHARACTER_SWAP;
	
	if (menuState != GameplayMenuState::INVENTORY && app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->pad->GetButton(SDL_CONTROLLER_BUTTON_START) == KEY_DOWN) menuState = GameplayMenuState::PAUSE;

	if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		if (menuState == GameplayMenuState::INVENTORY)
		{
			menuState = GameplayMenuState::NONE;
		}
		else
		{
			menuState = GameplayMenuState::INVENTORY;
			inventory->GetEquipment(currentPlayer);
		}
	}
}

bool SceneGameplay::CollisionMapEntity(SDL_Rect rect, EntityType type)
{
	iPoint pos = map->WorldToMap(rect.x, rect.y);

	if (rect.x + rect.w > map->data.width * map->data.tileWidth) return true;
	else if (rect.x < 0) return true;
	else if (rect.y > map->data.tileWidth * map->data.height) return true;
	else if (rect.y < 0) return true;

	int x = pos.x + (rect.w / map->data.tileWidth);
	int y = pos.y + (rect.h / map->data.tileHeight);

	if (rect.w < map->data.tileWidth) x = pos.x + 1;
	if (rect.h < map->data.tileHeight) y = pos.y + 1;

	bool exit = false;

	// Only check adyacent tiles
	for (int j = pos.y; j <= y; j++)
	{
		for (int i = pos.x; i <= x; i++)
		{
			if (i >= 0 && j >= 0)
			{
				if (((*map->data.layers.end().prev())->Get(i, j) == 769) && CheckCollision(map->GetTilemapRec(i, j), rect) ||
					((*map->data.layers.end().prev())->Get(i, j) == 1537) && CheckCollision(map->GetTilemapRec(i, j), rect) ||
					((*map->data.layers.end().prev())->Get(i, j) == 1) && CheckCollision(map->GetTilemapRec(i, j), rect) ||
					((*map->data.layers.end().prev())->Get(i, j) == 3073) && CheckCollision(map->GetTilemapRec(i, j), rect))
				{
					return true;
				}
				if (type != EntityType::TOWN && type != EntityType::TABERN && type != EntityType::NPC_WIZARD)
				{
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 771) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();

						iPoint position = { 625,480 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("house1.tmx", app->tex);

						exit = true;
						break;
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 771) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 97,520 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 772) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 625,480 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("house2.tmx", app->tex);

						exit = true;
						break;
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 772) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 70,765 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 773) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 625,480 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("house3.tmx", app->tex);

						exit = true;
						break;
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 773) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 380,1120 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 774) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 625,430 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("cave_house.tmx", app->tex);

						exit = true;
						break;
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 6) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 97,1100 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 775) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position;

						position = { 655,430 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("pub.tmx", app->tex);
						
						pugi::xml_document animations;
						pugi::xml_node anims;
						pugi::xml_parse_result result = animations.load_file("animations.xml");

						if (result == NULL)
							LOG("Could not load xml file: %s. pugi error: %s", CONFIG_FILENAME, result.description());
						else
							anims = animations.child("animations");
						Npc* generalNpc = nullptr;
						position = { 670,280 };
						generalNpc = (Npc*)entityManager->CreateEntity(EntityType::TOWN, position, anims, 4);
						generalNpc->NpcMove(false);

						position = { 670,360 };
						generalNpc = (Npc*)entityManager->CreateEntity(EntityType::NPC_WIZARD, position, anims, 1);

						exit = true;
						break;						
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 7) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 770,710 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 776) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 630,450 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("adventurer_house.tmx", app->tex);

						pugi::xml_document animations;
						pugi::xml_node anims;
						pugi::xml_parse_result result = animations.load_file("animations.xml");
						if (result == NULL)
							LOG("Could not load xml file: %s. pugi error: %s", CONFIG_FILENAME, result.description());
						else
							anims = animations.child("animations");
						Npc* generalNpc = nullptr;
						position = { 630,450 };
						generalNpc = (Npc*)entityManager->CreateEntity(EntityType::TABERN, position, anims, 4);

						position = { 630,420 };
						generalNpc = (Npc*)entityManager->CreateEntity(EntityType::NPC_WIZARD, position, anims, 1);

						exit = true;
						break;
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 1544) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 385,610 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 777) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 750,300 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("adventurer_house.tmx", app->tex);

						pugi::xml_document animations;
						pugi::xml_node anims;
						pugi::xml_parse_result result = animations.load_file("animations.xml");
						if (result == NULL)
							LOG("Could not load xml file: %s. pugi error: %s", CONFIG_FILENAME, result.description());
						else
							anims = animations.child("animations");
						Npc* generalNpc = nullptr;
						position = { 630,450 };
						generalNpc = (Npc*)entityManager->CreateEntity(EntityType::TABERN, position, anims, 4);

						position = { 630,420 };
						generalNpc = (Npc*)entityManager->CreateEntity(EntityType::NPC_WIZARD, position, anims, 1);

						exit = true;
						break;

					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 1545) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 448,465 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 778) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 650,480 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("inn.tmx", app->tex);

						exit = true;
						break;
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 1546) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 380,136 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 779) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 615,480 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						pugi::xml_document animations;
						pugi::xml_node anims;
						pugi::xml_parse_result result = animations.load_file("animations.xml");
						if (result == NULL)
							LOG("Could not load xml file: %s. pugi error: %s", CONFIG_FILENAME, result.description());
						else
							anims = animations.child("animations");
						Npc* generalNpc = nullptr;
						position = { 615,460 };
						generalNpc = (Npc*)entityManager->CreateEntity(EntityType::TOWN, position, anims, 5);

						position = { 600,450 };
						generalNpc = (Npc*)entityManager->CreateEntity(EntityType::NPC_WIZARD, position, anims, 6);
						map->Load("library.tmx", app->tex);

						exit = true;
						break;
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 1547) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 160,136 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 780) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 615,480 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("shop.tmx", app->tex);

						exit = true;
						break;
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 12) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 898,152 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 782) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 615,480 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("shop2.tmx", app->tex);

						exit = true;
						break;
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 14) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 900,1250 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					if (isTown && ((*map->data.layers.end().prev())->Get(i, j) == 783) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 615,480 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("red_house.tmx", app->tex);

						exit = true;
						break;
					}
					else if (((*map->data.layers.end().prev())->Get(i, j) == 1551) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 1350,390 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}
					/*if (((*map->data.layers.end().prev())->Get(i, j) == 784) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 705,950 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("graveyard_path.tmx", app->tex);

						exit = true;
						break;
					}*/
					/*if (((*map->data.layers.end().prev())->Get(i, j) == 785) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = false;
						entityManager->SetAllNpcInactive();
						iPoint position = { 620,480 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("bath_house.tmx", app->tex);

						exit = true;
						break;
					}*/
					/*if (((*map->data.layers.end().prev())->Get(i, j) == 3089) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						iPoint position = { 320,380 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("graveyard_path.tmx", app->tex);

						exit = true;
						break;
					}*/
					/*if (((*map->data.layers.end().prev())->Get(i, j) == 784) && CheckCollision(map->GetTilemapRec(i, j), rect))
					{
						isTown = true;
						entityManager->DeleteAllNpcActive();
						entityManager->SetAllNpcActive();
						iPoint position = { 705,30 };
						currentPlayer->bounds.x = position.x;
						currentPlayer->bounds.y = position.y;
						map->CleanUp();
						map->Load("town_map.tmx", app->tex);

						exit = true;
						break;
					}*/
				}
			}
		}
		if (exit) break;
	}

	return false;
}

bool SceneGameplay::CheckCollision(SDL_Rect rec1, SDL_Rect rec2)
{
	if ((rec1.x < (rec2.x + rec2.w) && (rec1.x + rec1.w) > rec2.x) &&
		(rec1.y < (rec2.y + rec2.h) && (rec1.y + rec1.h) > rec2.y)) return true;

	else return false;
}

void SceneGameplay::GenerateBattle()
{
	tmpPosPlayer = iPoint(currentPlayer->bounds.x, currentPlayer->bounds.y);
	transition = true;
	fadeOut = true;
}

void SceneGameplay::CameraFollow(Render* render)
{
	int midPlayerPosX = (currentPlayer->bounds.w / 2) + currentPlayer->bounds.x;
	int midPlayerPosY = (currentPlayer->bounds.h / 2) + currentPlayer->bounds.y;
	if (midPlayerPosX > -render->camera.x + (render->camera.w / 2) || midPlayerPosX < -render->camera.x + (render->camera.w / 2))
	{
		render->camera.x = -(midPlayerPosX - (render->camera.w / 2));
	}

	if (midPlayerPosY > -render->camera.y + (render->camera.h / 2) || midPlayerPosY < -render->camera.y + (render->camera.h / 2))
	{
		render->camera.y = -(midPlayerPosY - (render->camera.h / 2));
	}

	int mapWidth = map->data.width * map->data.tileWidth;
	int mapHeight = map->data.height * map->data.tileHeight;
	if (-render->camera.x + render->camera.w >= mapWidth) render->camera.x = -(mapWidth - render->camera.w);
	if (-render->camera.x <= 0) render->camera.x = 0;

	if (-render->camera.y + render->camera.h >= mapHeight) render->camera.y = -(mapHeight - render->camera.h);
	if (-render->camera.y <= 0) render->camera.y = 0;
}

void SceneGameplay::Fading(float dt)
{
	if (fadeOut)
	{
		alpha += 1.0f * dt;
		if (alpha >= 1.01f)
		{
			if (sceneBattle == nullptr)
			{
				sceneBattle = new SceneBattle(playerList, 3, this);
				sceneBattle->Load();
				gameState = GameplayState::BATTLE;
			}
			else
			{
				sceneBattle->UnLoad();
				RELEASE(sceneBattle);
				gameState = GameplayState::ROAMING;
				eastl::list<Enemy*>::iterator en = enemyList.begin();
				for (; en != enemyList.end(); ++en)
				{
					if (tmp != nullptr && (*en) == tmp)
					{
						(*en)->UnLoad();
						RELEASE((*en));
						enemyList.erase(en);
						break;
					}
				}
				eastl::list<Player*>::iterator pl = playerList.begin();
				for (; pl != playerList.end(); ++pl)
				{
					(*pl)->GetHealed(4000);
				}
				currentPlayer->bounds.x = tmpPosPlayer.x;
				currentPlayer->bounds.y = tmpPosPlayer.y;
				app->audio->PlayMusic("Assets/Audio/Music/village_theme_1.ogg", 0);
			}
			fadeOut = false;
			alpha = 1.0f;
		}
	}
	else
	{
		alpha -= 2.0f * dt;
		if (alpha < -0.1f)
		{
			transition = false;
			alpha = 0.0f;
		}
	}
}