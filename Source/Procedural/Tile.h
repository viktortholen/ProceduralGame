// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//#include "Room.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/StaticMeshComponent.h"
/**
 * 
 */
class Room;
class PROCEDURAL_API Tile
{
public:
	enum WallType {
		WALL, DOOR, NO_TYPE
	};
	enum Direction {
		EAST, WEST, NORTH, SOUTH, NO_DIRECTION
	};
	static Direction getOppositeDirection(const Direction d) {
		switch (d) {
			case EAST: return WEST;
			case WEST: return EAST;
			case NORTH: return SOUTH;
			case SOUTH: return NORTH;
			default: return NO_DIRECTION;
		}
	}
	static Direction getPerpendicularDirection(const Direction d) {
		switch (d) {
		case EAST: return SOUTH;
		case WEST: return NORTH;
		case NORTH: return EAST;
		case SOUTH: return WEST;
		default: return NO_DIRECTION;
		}
	}
	struct Wall {
		Wall(AActor* a, WallType t)
			:actor{a}, type{t}{}
		Wall() { dummy = true; };
		AActor* actor;
		WallType type;
		bool dummy = false;
	};

	Tile();
	~Tile();
	void setRoom(Room* r) { room = r; }
	Room* getRoom() { return room; }
	//add and remove south/north/east/west door/wall
	void addWall(AActor* a, WallType type, Direction d) {
		if(d == Direction::WEST)
			west = new Wall(a, type);
		else if (d == Direction::EAST)
			east = new Wall(a, type);
		else if (d == Direction::NORTH)
			north = new Wall(a, type);
		else if (d == Direction::SOUTH)
			south = new Wall(a, type);
	}
	void addFloor(AActor* a) { floor = a; }
	void addStairs(AActor* a) { stairs = a; }
	bool hasStairs() { return IsValid(stairs); }
	void deleteWall(Direction d) {
		if (d == WEST && west && !west->dummy && west->actor && !west->actor->IsPendingKill()) {
			west->actor->Destroy();
			//west->actor = nullptr;
		}
		else if (d == EAST && east && !east->dummy && east->actor && !east->actor->IsPendingKill()) {
			east->actor->Destroy();
			//east->actor = nullptr;
		}
		else if (d == NORTH && west && !north->dummy && north->actor && !north->actor->IsPendingKill()) {
			north->actor->Destroy();
			//north->actor = nullptr;
		}
		else if (d == SOUTH && south && !south->dummy && south->actor && !south->actor->IsPendingKill()) {
			south->actor->Destroy();
			//south->actor = nullptr;
		}
	}
	void deleteFloor(){
		if (floor) {
			floor->Destroy(); 
			floor = nullptr;
		}
	}	
	bool tileHasDoor() {
		return false;
		return  (east && east->type == WallType::DOOR) ||
			(west && west->type == WallType::DOOR) ||
			(north && north->type == WallType::DOOR) ||
			(south && south->type == WallType::DOOR);
	}
	bool tileHasOpening() {
		return  !((east && east->actor ) &&
				(west && west->actor) &&
				(north && north->actor) &&
				(south && south->actor));
	}
	void setFloorColor(FLinearColor c) {
		if (floor) {
			auto mesh = floor->FindComponentByClass<UStaticMeshComponent>();
			auto material = mesh->GetMaterial(0);
			auto dynamicMaterial = UMaterialInstanceDynamic::Create(material, NULL);
			mesh->SetMaterial(0, dynamicMaterial);
			dynamicMaterial->SetVectorParameterValue(TEXT("Color"), c);

		}
	}
	void setIndex(int i, int _x, int _y) { 
		index = i;
		offsetX = _x;
		offsetY = _y;
	}
	int getIndex() { return index; }
	int getOffsetX() { return offsetX; }
	int getOffsetY() { return offsetY; }
protected:
	Room* room;
	//pointers to each object in the tile
	//Walls
	AActor* floor;
	Wall* east;
	Wall* west;
	Wall* north;
	Wall* south;
	AActor* stairs;

	int index;
	int offsetX;
	int offsetY;
};
