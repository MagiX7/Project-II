#include "App.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"

#include "Player.h"

#define SPEED_X 200.0f
#define SPEED_Y 200.0f

int Player::gold = 500;

Player::Player(PlayerType t, EntityType type, iPoint position) : Entity(type), playerType(t)
{
	bounds.x= position.x;
	bounds.y = position.y;
	bounds.w = 26;
	bounds.h = 32;
	currentAbilitySelected = 0;
	helmet = nullptr;
	chest = nullptr;
	boots = nullptr;
}

Player::~Player()
{
}

bool Player::Load()
{
	bool ret = true;

	//bounds = { 0, 0, 16, 32 };

	return ret;
}
void Player::startPaarticles(iPoint pos,int particle) {
	if (particle == -1) {
		particlesBattle->setGeneratorState(particleNum, GeneratorState::STARTING);
		particlesBattle->setGeneratorPosition(particleNum, pos);
	}
	else {
		particlesBattle->setGeneratorState(particle, GeneratorState::STARTING);
		particlesBattle->setGeneratorPosition(particle, pos);
	}
	
	//particleNum
}
bool Player::Update(float dt)
{
	bool ret = true;

	HandleInput(dt);

	return ret;
}

void Player::Draw(bool showColliders)
{
	switch (playerType)
	{
	case PlayerType::HUNTER:
		if (showColliders) app->render->DrawRectangle(bounds, 255, 0, 0);
		break;
	case PlayerType::WIZARD:
		if (showColliders) app->render->DrawRectangle(bounds, 0, 255, 0);
		break;
	}
}

bool Player::SaveState(pugi::xml_node& node) 
{
	pugi::xml_node auxiliar1 = node.append_child("bounds");
	auxiliar1.append_attribute("X").set_value(bounds.x);
	auxiliar1.append_attribute("Y").set_value(bounds.y);
	auxiliar1.append_attribute("W").set_value(bounds.w);
	auxiliar1.append_attribute("H").set_value(bounds.h);

	pugi::xml_node equipment = node.append_child("Equipment");
	if (helmet != nullptr) equipment.append_attribute("helmet").set_value((int)helmet->armorType);
	if (chest != nullptr) equipment.append_attribute("chest").set_value((int)chest->armorType);
	if (boots != nullptr) equipment.append_attribute("boots").set_value((int)boots->armorType);
	return true;
}

void Player::Attack(Enemy* enemy)
{
}

void Player::Ability(Enemy* enemy, int currentAbility)
{
}


void Player::UseObject(Player* player, int currentObject)
{
}

void Player::GetDamage(int dmg)
{
}

void Player::GetHealed(int heal)
{
}

void Player::GetMana(int amount)
{
}

bool Player::CanUseAbility(int abilityNum)
{
	return true;
}

int GetAbilityManaCost(int abilityNum)
{
	return -1;
}

void Player::SetDefend(bool option)
{
}

void Player::HandleInput(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || app->input->pad->GetButton(SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_REPEAT || app->input->pad->l_y < -0.5)
	{
		bounds.y -= SPEED_Y * dt;
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || app->input->pad->GetButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_REPEAT || app->input->pad->l_y > 0.5)
	{
		bounds.y += SPEED_Y * dt;
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || app->input->pad->GetButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_REPEAT || app->input->pad->l_x < -0.5)
	{
		bounds.x -= SPEED_X * dt;
	}
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || app->input->pad->GetButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_REPEAT || app->input->pad->l_x > 0.5)
	{
		bounds.x += SPEED_X * dt;
	}
}