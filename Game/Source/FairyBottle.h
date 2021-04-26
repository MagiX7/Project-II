#pragma once

#include "Item.h"

class FairyBottle : public Item
{
public:
	FairyBottle(iPoint pos);
	virtual ~FairyBottle();

	bool Load() override;
	bool Update(float dt) override;
	void Draw(bool showColliders);
	bool UnLoad() override;

public:

};