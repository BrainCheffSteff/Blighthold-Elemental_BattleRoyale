// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelTestCharacter.h"
#include "Private/VoxelProceduralMeshComponent.h"


// Sets default values
AVoxelTestCharacter::AVoxelTestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVoxelTestCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoxelTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVoxelTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//To move on voxel terrain without tp due to voxel invoker
void AVoxelTestCharacter::SetBase(UPrimitiveComponent* NewBase, const FName BoneName /*= NAME_None*/, bool bNotifyActor /*= true*/)
{
	if (!Cast<UVoxelProceduralMeshComponent>(NewBase))
	{
		Super::SetBase(NewBase, BoneName, bNotifyActor);
	}
}