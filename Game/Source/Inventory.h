#pragma once

#include "Menu.h"
#include "Item.h"

#include "GuiButton.h"

#include <iostream>

#define MAX_INVENTORY_SLOTS 32
#define MAX_EQUIPMENT_SLOTS 4

class Input;
class Player;
class SDL_Texture;

enum class SlotState
{
	NONE = 0,
	UNSELECTED,
	SELECTED,
	USE,
	DELETE
};

enum class InventoryState
{
	NONE = 0,
	EQUIPMENT,
	ITEMS,
	WEAPONS,
};

struct InventorySlot
{
	SDL_Rect bounds;
	Item* item;
	int itemsAmount;
	bool filled;
	int id;
	SlotState state;

	bool operator==(InventorySlot& i) const
	{
		return (i.bounds.x == bounds.x && i.bounds.y == bounds.y && i.bounds.w == bounds.w && i.bounds.h == bounds.h &&
			i.item->iType == item->iType && i.itemsAmount == itemsAmount && i.filled == filled);
	}
	bool operator!=(InventorySlot& i) const
	{
		return (i.bounds.x != bounds.x || i.bounds.y != bounds.y || i.bounds.w != bounds.w || i.bounds.h != bounds.h ||
			i.item->iType != item->iType || i.itemsAmount != itemsAmount || i.filled != filled);
	}
};

class Inventory : public Menu
{
public:
	Inventory(eastl::list<Player*> pls, SDL_Texture* atlas);
	virtual ~Inventory();

	bool Load(Font* font) override;
	bool Update(float dt) override;
	void Draw(Font* font, bool showColliders) override;
	bool UnLoad() override;
	bool OnGuiMouseClickEvent(GuiControl* control) override;
	void UpdatingButtons(Input* input) override;

	void AddItem(Item *it);

	void AddArmor(Item* ar);

	bool IsMouseInside(SDL_Rect r);

	void DisplayText(SDL_Rect bounds, bool showColliders);

	void DragItem(Item& item);

	void HandleItems();

	void HandleEquipment();

	void GetEquipment(Player* player);

private:
	Player* GetPlayer(PlayerType type);

public:
	eastl::list<Player*> players;

	GuiButton* btnEquipment; // Armors
	GuiButton* btnItems;     // Potions etc
	GuiButton* btnWeapons;   // Weapons

private:
	SDL_Texture* atlasTexture;
	SDL_Texture* playersTexture;

	InventoryState state;

	// Armor and weapons equiped slots
	InventorySlot equipment[MAX_EQUIPMENT_SLOTS];

	// There will be 32 slots for items
	InventorySlot slots[MAX_INVENTORY_SLOTS];
	int currentSlotId;
	InventorySlot* originSlot;

	// There will be 32 slots for Armor
	InventorySlot armorSlots[MAX_INVENTORY_SLOTS];
	int currentArmorSlotId;
	InventorySlot* originArmorSlot;

	Player* currentPlayer;

	// Display Text buttons to select with player uses the item
	GuiButton* btnHunter;
	GuiButton* btnWizard;
	GuiButton* btnThief;
	GuiButton* btnWarrior;

	GuiButton* btnUse;       // Use item
	GuiButton* btnDelete;	 // Delete an item.

	// Use this buttons for going through characters
	GuiButton* btnNext;
	GuiButton* btnPrev;


	SDL_Rect tmpBounds;

	bool grabbed;
	float toGrabCount;
	bool isTextDisplayed;
};