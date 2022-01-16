// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Tile.h"
#include "CoreMinimal.h"


class PROCEDURAL_API Room
{
public:
	Room(Tile* t);

	Room() {};

	~Room();

	void addTile(Tile* tile);
	void addRoomConnection(Room* roomToConnect) {
		connectedRooms.AddUnique(roomToConnect);
	}
	TArray<Room*> getConnectedRooms() {
		return connectedRooms;
	}

	void merge(Room* roomToMerge);

	TArray<Tile*> getTiles() { return tileList; }

	bool roomsAreConnected(Room* roomToCheck) {
		if (roomToCheck == this) {
			return true;
		}
		return connectedRooms.Contains(roomToCheck);
	}
	void setColor(FLinearColor c) {
		for (auto& t : tileList) {
			t->setFloorColor(c);
		}
	}
	int printRoomConnections() {
		return connectedRooms.Num();
	}
private:
	TArray<Tile*> tileList;
	TArray<Room*> connectedRooms;
};
