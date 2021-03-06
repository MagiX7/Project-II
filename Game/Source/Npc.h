#pragma once

#include "Entity.h"
#include "Font.h"
#include "Point.h"

#define FIND_RADIOUS 10

enum class NpcType
{
	NONE = 0,
	TABERN,
	WIZARD,
	TOWN,
	MERCHANT
};

class Npc : public Entity
{
public:
	Npc(EntityType type, iPoint position, int id);
	virtual ~Npc();

	bool Load() override;
	bool Update(float dt) override;
	void Draw(bool showColliders) override;
	bool UnLoad() override;
	bool SaveState(pugi::xml_node& node);

	bool CheckCollision(Player* player) override;

	inline int const GetDialogeId() const override { return dialogeId; }

	void SetDrawPtext(bool DrawPtext) override;
	void SetTalkStart(bool TalkStart) override;

	void SetInactive() override;
	void SetActive() override;
	inline EntityState GetState() override { return state; }
	
	void NpcMove(bool move);

	EntityState GetNewState();

	void OnCollision();

public:
	EntityState state;

	SDL_Rect lastPosition;
	bool npcMove;
	float stateTimer;
	int stateMaxTimer;
	Animation idleDown;
	Animation idleLeft;
	Animation idleRight;
	Animation idleUp;
	Animation walkDown;
	Animation walkLeft;
	Animation walkRight;
	Animation walkUp;
	NpcType npcType;

	int dialogeId;
	bool talkStart;
	bool drawPtext;
	Font* font;

	SDL_Texture* talkTex;
};