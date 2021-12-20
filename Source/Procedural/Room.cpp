// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"

Room::Room(Tile* t)
{
	addTile(t);
}

Room::~Room()
{
}
void Room::addTile(Tile* tile) {
	tile->setRoom(this);
	tileList.Add(tile);
}
void Room::merge(Room* roomToMerge) {
	for (auto& t : roomToMerge->getTiles())
	{
		t->setRoom(this);
		tileList.Add(t);
	}
	for (auto& t : roomToMerge->getConnectedRooms())
	{
		connectedRooms.Add(t);
	}
}