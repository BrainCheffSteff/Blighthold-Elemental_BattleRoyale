// Copyright 2017 Phyronnaz

#pragma once

#include "CoreMinimal.h"
#include "VoxelMaterial.h"
#include <deque>
#include "VoxelSave.generated.h"


struct FVoxelChunkSave
{
	uint64 Id;

	TArray<float, TFixedAllocator<16 * 16 * 16>> Values;

	TArray<FVoxelMaterial, TFixedAllocator<16 * 16 * 16>> Materials;

	FVoxelChunkSave();

	FVoxelChunkSave(uint64 Id, FIntVector Position, float Values[16 * 16 * 16], FVoxelMaterial Materials[16 * 16 * 16]);
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, FVoxelChunkSave& Save)
{
	Ar << Save.Id;
	Ar << Save.Values;
	Ar << Save.Materials;

	return Ar;
}

USTRUCT(BlueprintType, Category = Voxel)
struct VOXEL_API FVoxelWorldSave
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
		int Depth;

	UPROPERTY()
		TArray<uint8> Data;


	FVoxelWorldSave();

	void Init(int NewDepth, const std::deque<TSharedRef<FVoxelChunkSave>>& ChunksList);

	std::deque<FVoxelChunkSave> GetChunksList() const;
};


USTRUCT()
struct FVoxelValueDiff
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		uint64 Id;

	UPROPERTY(EditAnywhere)
		int Index;

	UPROPERTY(EditAnywhere)
		float Value;

	FVoxelValueDiff();

	FVoxelValueDiff(uint64 Id, int Index, float Value);
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, FVoxelValueDiff& ValueDiff)
{
	Ar << ValueDiff.Id;
	Ar << ValueDiff.Index;
	Ar << ValueDiff.Value;

	return Ar;
}

USTRUCT()
struct FVoxelMaterialDiff
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		uint64 Id;

	UPROPERTY(EditAnywhere)
		int Index;

	UPROPERTY(EditAnywhere)
		FVoxelMaterial Material;

	FVoxelMaterialDiff();

	FVoxelMaterialDiff(uint64 Id, int Index, FVoxelMaterial Material);
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, FVoxelMaterialDiff& MaterialDiff)
{
	Ar << MaterialDiff.Id;
	Ar << MaterialDiff.Index;
	Ar << MaterialDiff.Material;

	return Ar;
}