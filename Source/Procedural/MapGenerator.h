// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Tile.h"
#include "Room.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapGenerator.generated.h"
UCLASS()
class PROCEDURAL_API AMapGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	void TimerEnd();
	// Sets default values for this actor's properties
	AMapGenerator();
	void generateMap(int x, int y, int z, const FVector location, const FRotator rotation, const Tile::Direction d, bool stairs, int depth);
	void generateSecretDoors();
	int getMapIndex1D(const int x, const int y, const int z) const;
	bool checkTilePatchEmpty(const int x, const int y, const int z, const int patchSize, bool xRand, bool yRand);
	void traverse(Tile::Direction dir, int x, int y, int z, FVector location, FRotator rotation, int depth);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
		int32 sizeX = 20;
	UPROPERTY(EditAnywhere)
		int32 sizeY = 20;
	UPROPERTY(EditAnywhere)
		int32 sizeZ = 20;
	UPROPERTY(EditAnywhere)
		int32 tileSize = 1000;
	UPROPERTY(EditAnywhere)
		int32 tileHeight = 500;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> floorClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> roofClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> wallClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> wallDoorClass;		
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> secretDoorClass;	
	UPROPERTY(EditAnywhere)
		int maxNumberOfSecretDoors = 3;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> stairClass;
	UPROPERTY(EditAnywhere)
		int recursiveDepth = 10;
	UPROPERTY(EditAnywhere)
		bool generateRoof = true;	
	UPROPERTY(EditAnywhere)
		bool traverseVertically = true;


	TMap<int, Tile*> tileMap;
	TArray<Room*> roomList;
public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;


	/*
	DONE: 1. 
	Create vector and store Room Objects in
	- Create a room for each tile created
	- Add the room reference to the tile
	- Merge rooms when deleting walls and not replacing them with doors
		* Each room has references to all its tiles
		* Transfer all tile references to the new room
		* Set the room reference of the tiles to the new room
		* Delete the other rooms which are now empty
	- Check when creating a door if that tile already has a door connected to the room of the tile, 
		then spawn a wall instead of a door.
	DONE: 2. Take boundaries into account, required for large depth.
	DONE: 3. Make it possible to spawn differently sized rooms like 4x4 and 8x8 tiles
	4. Multiple floors and stairs

	*/
	
};
