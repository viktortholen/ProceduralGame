// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenerator.h"

// Sets default values
AMapGenerator::AMapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


}

// Called when the game starts or when spawned
void AMapGenerator::BeginPlay()
{
	Super::BeginPlay();

	GenerateMap(sizeX/2, sizeY/2, 0, GetActorLocation(), GetActorRotation(), Tile::Direction::NO_DIRECTION, false, 0);
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Yellow, FString::Printf(TEXT("Number of Rooms: %d"), roomList.Num()));
	}
	for (auto r : roomList) {
		if (r) {
			FLinearColor color = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f));
			r->setColor(color);
			//int rc = r->printRoomConnections();
			
		}
	}
}
int AMapGenerator::getMapIndex1D(const int x, const int y, const int z) const{
	//return sizeX * x + y;
	return x + sizeX * (y + sizeZ * z);
}
void AMapGenerator::GenerateMap(int x, int y, int z, const FVector location, const FRotator rotation, const Tile::Direction d, bool stairs, int depth) {
	//GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AMapGenerator::TimerEnd, 3.f, false);

	
	if (depth >= recursiveDepth) {
		return;
	}
	Room* room = new Room();
	//Random bools
	bool traverseEast = FMath::RandRange(0, 0) == 0 && d != Tile::Direction::WEST && (x + 1) < sizeX || depth == 0;
	bool traverseWest = FMath::RandRange(0, 0) == 0 && d != Tile::Direction::EAST && (x - 1) > 0 || depth == 0;
	bool traverseNorth = FMath::RandRange(0, 0) == 0&& d != Tile::Direction::SOUTH  && (y + 1) < sizeY || depth == 0;
	bool traverseSouth = FMath::RandRange(0, 0) == 0&& d != Tile::Direction::NORTH  && (y - 1) > 0 || depth == 0;
	
	bool wallEast = FMath::RandRange(0, 1) == 0;
	bool wallWest = FMath::RandRange(0, 1) == 0;
	bool wallNorth = FMath::RandRange(0, 1) == 0;
	bool wallSouth = FMath::RandRange(0, 1) == 0;

	//bool closeBehindEast = FMath::RandRange(0, 1) == 0;
	//bool closeBehindWest = FMath::RandRange(0, 1) == 0;
	//bool closeBehindNorth = FMath::RandRange(0, 1) == 0;
	//bool closeBehindSouth = FMath::RandRange(0, 1) == 0;




	/*
	- Different tile size (DONE)
		* spawn 2x2 floor tiles
		* add all tiles to the room and vice versa
		* add walls around, not inbetween, only use the outer wall part of the alg.
		* traverse from random tile (or all?)
		* need to check if tile is empty before creating new tile
		* only traverse from border tiles
		* changes to the alg.: loop through all tiles to spawn -> only spawn wall at borders
		* scale tiles by AxB size 
		* traverse from other side of big tile
		* other directions than up-right
	- Multiple floors
		CHECK* Change tilemap to be 3D with height too
		CHECK* At random, check if stairs is possible above/below, make stairs and increase/decrease height, close room
		CHECK* Stairs in direction of tile before, whole tile is a stairs
		CHECK* Next tile MUST be in same direction as stairs to continue
		CHECK* Mark tiles above/below stairs as occupied tiles but no floor
	- Secret/Shortcuts doors
		* After map is generated, pick a number of rooms and get some tiles with a wall attached.
		* Check the tiles room and its connection to neighboring room 
		* If not connected: Add secret door
	- Roofs, Lights, Windows, Start, End, 3rd-person movement
		* Option toggle
	*/
	TArray<Tile*> tilesToSpawn;
	int patchSize = 1;
	int patchExitOffsetX = 0, patchExitOffsetY = 0;
	//Floor and Roof
	FVector roofLocation = location;
	roofLocation.Z += tileHeight - 50;
	Tile* baseTile = new Tile();
	baseTile->setIndex(getMapIndex1D(x, y, z), 0, 0);
	if (!stairs) {
		AActor* baseFloor = GetWorld()->SpawnActor<AActor>(floorClass, location, rotation);
		baseTile->addFloor(baseFloor);
		if (FMath::RandRange(0, 3) == 0) {
			patchSize = FMath::RandRange(2, 5);
			if(generateRoof)
				AActor* baseRoof = GetWorld()->SpawnActor<AActor>(roofClass, roofLocation, rotation);
		}
	}
	tilesToSpawn.Add(baseTile);

	bool xRand = FMath::RandBool();
	bool yRand = FMath::RandBool();
	bool exitDir = FMath::RandBool();
	if (!stairs && patchSize > 1 && checkTilePatchEmpty(x, y, z, patchSize, xRand, yRand)) {
		for (int i = 0; i < patchSize; i++)
		{
			for (int j = 0; j < patchSize; j++)
			{
				int ii = xRand ? -i : i;
				int jj = yRand ? -j : j;
				
				if (!(ii == 0 && jj == 0)) {
					FVector tileLocation = location;
					Tile* tile = new Tile();
					tileLocation.X += ii * tileSize;
					tileLocation.Y += jj * tileSize;
					AActor* floor = GetWorld()->SpawnActor<AActor>(floorClass, tileLocation, rotation);
					if (generateRoof) {
						roofLocation.X = tileLocation.X;
						roofLocation.Y = tileLocation.Y;
						AActor* roof = GetWorld()->SpawnActor<AActor>(roofClass, roofLocation, rotation);
					}

					tile->addFloor(floor);
					tile->setIndex(getMapIndex1D(x + ii, y + jj, z), ii, jj);
					tilesToSpawn.Add(tile);
					
					if (exitDir && jj == (patchSize - 1) * FMath::Sign(jj) && ii == FMath::DivideAndRoundDown((patchSize - 1) * FMath::Sign(ii), 2)) {
						patchExitOffsetX = ii;
						patchExitOffsetY = jj;
					}
					else if (!exitDir && ii == (patchSize - 1) * FMath::Sign(ii) && jj == FMath::DivideAndRoundDown((patchSize - 1) * FMath::Sign(jj), 2)) {
						patchExitOffsetX = ii;
						patchExitOffsetY = jj;
					}
				}
			}
		}
	}
	

	for (auto& t : tilesToSpawn) {
		room->addTile(t);
		tileMap.Add(t->getIndex(), t);
	}
	//Walls
	FRotator sideRotation;
	FVector wallLocation;

	for (int tileIndex = 0; tileIndex < tilesToSpawn.Num(); tileIndex++)
	{
		FVector checkedTileLocation = location;
		int offsetX = tilesToSpawn[tileIndex]->getOffsetX();
		int offsetY = tilesToSpawn[tileIndex]->getOffsetY();
		checkedTileLocation.X += offsetX * tileSize;
		checkedTileLocation.Y += offsetY * tileSize;

		//EAST, WEST, NORTH, SOUTH
		for (int i = Tile::Direction::EAST; i != Tile::Direction::NO_DIRECTION; i++)
		{
			int stepX = offsetX, stepY = offsetY;
			sideRotation = rotation;
			wallLocation = checkedTileLocation;
			switch (i) {
			case Tile::Direction::EAST: {
				wallLocation.X += tileSize / 2;
				sideRotation.Add(0, 90.0f, 0);
				stepX += 1;
				break;
			}
			case Tile::Direction::WEST: {
				wallLocation.X -= tileSize / 2;
				sideRotation.Add(0, 90.0f, 0);
				stepX += -1;
				break;
			}
			case Tile::Direction::NORTH: {
				wallLocation.Y += tileSize / 2;
				stepY += 1;
				break;
			}
			case Tile::Direction::SOUTH: {
				wallLocation.Y -= tileSize / 2;
				stepY += -1;
				break;
			}
			}

			if (!tileMap.Contains(getMapIndex1D(x + stepX, y + stepY, z))) {
				//Add wall when no adjacent tile exists
				AActor* east = GetWorld()->SpawnActor<AActor>(wallClass, wallLocation, sideRotation);
				tilesToSpawn[tileIndex]->addWall(east, Tile::WallType::WALL, static_cast<Tile::Direction>(i));
			}
			else {
				Tile* foundTile = *tileMap.Find(getMapIndex1D(x + stepX, y + stepY, z));
				if (foundTile) {
					Room* otherRoom = foundTile->getRoom();
					if (tilesToSpawn.Num() == 1) {
						foundTile->deleteWall(Tile::getOppositeDirection(static_cast<Tile::Direction>(i)));
						if (d != Tile::getOppositeDirection(static_cast<Tile::Direction>(i))) {
							//Add walls at all directions except behind the tile
							AActor* south = GetWorld()->SpawnActor<AActor>(wallClass, wallLocation, sideRotation);
							tilesToSpawn[tileIndex]->addWall(south, Tile::WallType::WALL, static_cast<Tile::Direction>(i));
						}
						else if (FMath::RandRange(0, 3) == 0) {
							//Close room behind and add a door to it
							AActor* south = GetWorld()->SpawnActor<AActor>(wallDoorClass, wallLocation, sideRotation);
							tilesToSpawn[tileIndex]->addWall(south, Tile::WallType::DOOR, static_cast<Tile::Direction>(i));
							room->addRoomConnection(otherRoom);
						}
						else {
							//Wall is removed without replacement
							room->merge(otherRoom);
							roomList.Remove(otherRoom);
						}
					}
					else if (tilesToSpawn.Num() > 1 && tileIndex == 0) {
						if (d == Tile::getOppositeDirection(static_cast<Tile::Direction>(i))) {
							foundTile->deleteWall(Tile::getOppositeDirection(static_cast<Tile::Direction>(i)));

							//Close room behind and add a door to it
							AActor* south = GetWorld()->SpawnActor<AActor>(wallDoorClass, wallLocation, sideRotation);
							tilesToSpawn[tileIndex]->addWall(south, Tile::WallType::DOOR, static_cast<Tile::Direction>(i));
							room->addRoomConnection(otherRoom);
							
						}
					}
					
				}
			}
		}
		tileMap[tilesToSpawn[tileIndex]->getIndex()] = tilesToSpawn[tileIndex];
	}

	//Stairs
	bool stairUp = traverseVertically && !stairs && FMath::RandRange(0, 5) == 0 && patchSize == 1 && !tileMap.Contains(getMapIndex1D(x, y, z + 1)) && depth != 0;
	if (stairUp) {
		FRotator stairRotation = rotation;
		switch (d) {
			case Tile::Direction::EAST: {
				break;
			}
			case Tile::Direction::WEST: {
				stairRotation.Add(0, 180.0f, 0);
				break;
			}
			case Tile::Direction::NORTH: {
				stairRotation.Add(0, 90.0f, 0);
				break;
			}
			case Tile::Direction::SOUTH: {
				stairRotation.Add(0, -90.0f, 0);
				break;
			}
		}
		AActor* stairs = GetWorld()->SpawnActor<AActor>(stairClass, location, stairRotation);
		tileMap[getMapIndex1D(x, y, z)]->addStairs(stairs);
		
	}
	else{
		if (generateRoof)
			AActor* baseRoof = GetWorld()->SpawnActor<AActor>(roofClass, roofLocation, rotation);
	}

	roomList.Add(room);
#define TRAVERSE
#ifdef TRAVERSE
	float timeDelay = 1.0f;
	//If patch(more than one tile to spawn), then traverse from the other side of it
	x += patchExitOffsetX;
	y += patchExitOffsetY;
	FVector offsetPatchLocation = location;
	offsetPatchLocation.X += patchExitOffsetX * tileSize;
	offsetPatchLocation.Y += patchExitOffsetY * tileSize;

	//TODO: Reason for shape of map forming along line -> traverse in random order to avoid this
	//Traverse in 4 directions randomly
	if (stairUp) {
		FVector newLocation = location;
		newLocation.Z += tileHeight;
		GenerateMap(x, y, z + 1, newLocation, rotation, d, true, depth - 1);
		
	}
	else if (stairs) {
		traverse(d, x, y, z, location, rotation, depth);
	}
	else {
		if (traverseEast) {
			traverse(Tile::Direction::EAST, x, y, z, offsetPatchLocation, rotation, depth);
		}
		if (traverseNorth) {
			traverse(Tile::Direction::NORTH, x, y, z, offsetPatchLocation, rotation, depth);
		}
		if (traverseWest) {
			traverse(Tile::Direction::WEST, x, y, z, offsetPatchLocation, rotation, depth);
		}
		if (traverseSouth) {
			traverse(Tile::Direction::SOUTH, x, y, z, offsetPatchLocation, rotation, depth);
		}
	}
	
#endif
	
}
void AMapGenerator::traverse(Tile::Direction dir, int x, int y, int z, FVector location, FRotator rotation, int depth){
	
	switch (dir) {
		case Tile::Direction::EAST: {
			FVector newLocation = location;
			if (!tileMap.Contains(getMapIndex1D(x + 1, y, z))) {
				newLocation.X += tileSize;
				GenerateMap(x + 1, y, z, newLocation, rotation, Tile::Direction::EAST, false, depth + 1);
			}
			break;
		}
		case Tile::Direction::WEST: {
			//West
			FVector newLocation = location;
			if (!tileMap.Contains(getMapIndex1D(x - 1, y, z))) {
				newLocation.X -= tileSize;
				GenerateMap(x - 1, y, z, newLocation, rotation, Tile::Direction::WEST, false, depth + 1);
			}
			break;
		}
		case Tile::Direction::NORTH: {
			//North
			FVector newLocation = location;
			if (!tileMap.Contains(getMapIndex1D(x, y + 1, z)) ) {
				newLocation.Y += tileSize;
				GenerateMap(x, y + 1, z, newLocation, rotation, Tile::Direction::NORTH, false, depth + 1);
			}
			break;
		}
		case Tile::Direction::SOUTH: {
			//South
			FVector newLocation = location;
			if (!tileMap.Contains(getMapIndex1D(x, y - 1, z))) {
				newLocation.Y -= tileSize;
				GenerateMap(x, y - 1, z, newLocation, rotation, Tile::Direction::SOUTH, false, depth + 1);
			}
			break;
		}
	}

}
void AMapGenerator::TimerEnd() {
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Yellow, FString::Printf(TEXT("Delayed print")));
	}
}
bool AMapGenerator::checkTilePatchEmpty(const int x, const int y, const int z, const int patchSize, bool xRand, bool yRand) {
	for (int i = 0; i < patchSize; i++)
	{
		for (int j = 0; j < patchSize; j++)
		{
			int ii = xRand ? -i : i;
			int jj = yRand ? -j : j;
			if (!(i == 0 && j == 0) && tileMap.Contains(getMapIndex1D(x + ii, y + jj, z))) {
				return false;
			}
		}
	}
	return true;
}
