#include "App.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"

#include "Town.h"

Town::Town(iPoint position, pugi::xml_node anim, int id) : Npc(EntityType::TOWN, position, id)
{
	/*bounds = { 0,0, 16,32 };
	type = EntityType::TOWN;*/
	texture = app->tex->Load("Textures/Npc/Town.png");

	state = EntityState::WALKING_RIGHT;
	pugi::xml_node player = anim.child("town").child("overworld");

	for (pugi::xml_node n = player.child("walk_front").child("pushback"); n; n = n.next_sibling("pushback"))
	{
		walkDown.PushBack({ n.attribute("x").as_int(), n.attribute("y").as_int(), n.attribute("w").as_int(), n.attribute("h").as_int() });
	}

	for (pugi::xml_node n = player.child("walk_left").child("pushback"); n; n = n.next_sibling("pushback"))
	{
		walkLeft.PushBack({ n.attribute("x").as_int(), n.attribute("y").as_int(), n.attribute("w").as_int(), n.attribute("h").as_int() });
	}

	for (pugi::xml_node n = player.child("walk_right").child("pushback"); n; n = n.next_sibling("pushback"))
	{
		walkRight.PushBack({ n.attribute("x").as_int(), n.attribute("y").as_int(), n.attribute("w").as_int(), n.attribute("h").as_int() });
	}

	for (pugi::xml_node n = player.child("walk_up").child("pushback"); n; n = n.next_sibling("pushback"))
	{
		walkUp.PushBack({ n.attribute("x").as_int(), n.attribute("y").as_int(), n.attribute("w").as_int(), n.attribute("h").as_int() });
	}

	idleDown.PushBack(walkDown.frames[0]);
	idleLeft.PushBack(walkLeft.frames[0]);
	idleRight.PushBack(walkRight.frames[0]);
	idleUp.PushBack(walkUp.frames[0]);

	currentAnim = &idleDown;
}

Town::~Town()
{

}

bool Town::Load()
{
	//scene->dialogueManager->current = scene->dialogueManager->LoadDialogue(0);
	return true;
}

bool Town::Update(float dt)
{
	if (state != EntityState::INACTIVE) Npc::Update(dt);

	return true;
}

void Town::Draw(bool showColliders)
{
	if (state != EntityState::INACTIVE)
	{
		if (bounds.x + bounds.w > (-app->render->camera.x) && bounds.x < (-app->render->camera.x) + app->render->camera.w && bounds.y + bounds.h >(-app->render->camera.y) && bounds.y < (-app->render->camera.y) + app->render->camera.h) 
		{
			Npc::Draw(showColliders);
			if (showColliders) app->render->DrawRectangle(bounds, 255, 0, 0);

			SDL_Rect textureRect = { 446, 11, 43,49 };
			app->render->DrawTexture(texture, bounds.x, bounds.y, &currentAnim->GetCurrentFrame());
		}
	}
}

bool Town::UnLoad()
{
	app->tex->UnLoad(texture);

	return true;
}

bool Town::SaveState(pugi::xml_node& node)
{
	Npc::SaveState(node);
	pugi::xml_node auxiliar1;
	pugi::xml_node auxiliar2;
	auxiliar1 = node.append_child("NpcType");
	auxiliar2 = node.append_child("Dialoge");
	auxiliar2.append_attribute("idDialoge").set_value(dialogeId);
	auxiliar1.append_attribute("type").set_value("TOWN");
	
	return true;
}