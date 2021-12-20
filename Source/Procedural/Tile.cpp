// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

Tile::Tile()
{
	floor = nullptr;
	stairs = nullptr;

	east = new Wall();
	west = new Wall();
	north = new Wall();
	south = new Wall();
}

Tile::~Tile()
{
}
