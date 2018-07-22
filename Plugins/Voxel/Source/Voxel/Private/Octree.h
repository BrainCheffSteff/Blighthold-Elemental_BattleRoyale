// Copyright 2017 Phyronnaz

#pragma once
#include "CoreMinimal.h"

FORCEINLINE uint64 IntPow9(int Power)
{
	check(Power <= 19);
	uint64 Pow = 1;
	for (int i = 0; i < Power; i++)
	{
		Pow *= 9;
	}
	return Pow;
}

/**
 * Base Octree class
 */
class FOctree
{
public:
	/**
	 * Constructor
	 * @param	Position	Position (center) of this chunk
	 * @param	Depth		Distance to the highest resolution
	 */
	FOctree(FIntVector Position, uint8 Depth, uint64 Id);

	bool operator==(const FOctree& Other) const;

	bool operator<(const FOctree& Other) const;

	bool operator>(const FOctree& Other) const;

	// Center of the octree
	const FIntVector Position;

	// Distance to the highest resolution
	const uint8 Depth;

	// Id of the Octree (position in the octree)
	const uint64 Id;

	/**
	 * Get the width at this level
	 * @return	Width of this chunk
	 */
	FORCEINLINE int Size() const;

	FORCEINLINE FIntVector GetMinimalCornerPosition() const;
	FORCEINLINE FIntVector GetMaximalCornerPosition() const;

	/**
	 * Is Leaf?
	 * @return IsLeaf
	 */
	FORCEINLINE bool IsLeaf() const;

	/**
	 * Is GlobalPosition in this octree?
	 * @param	GlobalPosition	Position in voxel space
	 * @return	If IsInOctree
	 */
	FORCEINLINE bool IsInOctree(int X, int Y, int Z) const;

	/**
	 * Convert from chunk space to voxel space
	 * @param	LocalPosition	Position in chunk space
	 * @return	Position in voxel space
	 */
	FORCEINLINE void LocalToGlobal(int X, int Y, int Z, int& OutX, int& OutY, int& OutZ) const;

	/**
	 * Convert from voxel space to chunk space
	 * @param	GlobalPosition	Position in voxel space
	 * @return	Position in chunk space
	 */
	FORCEINLINE void GlobalToLocal(int X, int Y, int Z, int& OutX, int& OutY, int& OutZ) const;

	FORCEINLINE static uint64 GetTopIdFromDepth(int8 Depth);

protected:
	// Does this octree has child?
	bool bHasChilds;
};