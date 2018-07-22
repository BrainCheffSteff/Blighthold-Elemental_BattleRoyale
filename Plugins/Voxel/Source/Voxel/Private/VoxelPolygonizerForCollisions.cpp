// Copyright 2017 Phyronnaz

#include "VoxelPolygonizerForCollisions.h"
#include "Transvoxel.h"
#include "VoxelData.h"
#include "VoxelMaterial.h"
#include <deque>

DECLARE_CYCLE_STAT(TEXT("VoxelPolygonizerForCollisions ~ Main Iter"), STAT_MAIN_ITER_FC, STATGROUP_Voxel);
DECLARE_CYCLE_STAT(TEXT("VoxelPolygonizerForCollisions ~ CreateSection"), STAT_CREATE_SECTION_FC, STATGROUP_Voxel);
DECLARE_CYCLE_STAT(TEXT("VoxelPolygonizerForCollisions ~ Cache"), STAT_CACHE_FC, STATGROUP_Voxel);

FVoxelPolygonizerForCollisions::FVoxelPolygonizerForCollisions(FVoxelData* Data, const FIntVector& ChunkPosition, bool bEnableRender)
	: Data(Data)
	, ChunkPosition(ChunkPosition)
	, bEnableRender(bEnableRender)
{
}

void FVoxelPolygonizerForCollisions::CreateSection(FVoxelProcMeshSection& OutSection)
{
	// Create forward lists
	std::deque<FVector> Vertices;
	std::deque<int32> Triangles;
	int VerticesSize = 0;
	int TrianglesSize = 0;

	Data->BeginGet();

	{
		SCOPE_CYCLE_COUNTER(STAT_CACHE_FC);

		FIntVector Size(CHUNKSIZE_FC + 1, CHUNKSIZE_FC + 1, CHUNKSIZE_FC + 1);
		Data->GetValuesAndMaterials(CachedValues, nullptr, ChunkPosition, FIntVector::ZeroValue, 1, Size, Size);


		// Cache signs
		for (int CubeX = 0; CubeX < 6; CubeX++)
		{
			for (int CubeY = 0; CubeY < 6; CubeY++)
			{
				for (int CubeZ = 0; CubeZ < 6; CubeZ++)
				{
					uint64& CurrentCube = CachedSigns[CubeX + 6 * CubeY + 6 * 6 * CubeZ];
					CurrentCube = 0;
					for (int LocalX = 0; LocalX < 4; LocalX++)
					{
						for (int LocalY = 0; LocalY < 4; LocalY++)
						{
							for (int LocalZ = 0; LocalZ < 4; LocalZ++)
							{
								const int X = 3 * CubeX + LocalX;
								const int Y = 3 * CubeY + LocalY;
								const int Z = 3 * CubeZ + LocalZ;

								const uint64 ONE = 1;
								uint64 CurrentBit = ONE << (LocalX + 4 * LocalY + 4 * 4 * LocalZ);

								float CurrentValue = GetValue(X, Y, Z);

								bool Sign = CurrentValue > 0;
								CurrentCube = CurrentCube | (CurrentBit * Sign);
							}
						}
					}
				}
			}
		}
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_MAIN_ITER_FC);
		// Iterate over cubes
		for (int CubeX = 0; CubeX < 6; CubeX++)
		{
			for (int CubeY = 0; CubeY < 6; CubeY++)
			{
				for (int CubeZ = 0; CubeZ < 6; CubeZ++)
				{
					uint64 CurrentCube = CachedSigns[CubeX + 6 * CubeY + 6 * 6 * CubeZ];
					if (CurrentCube == 0 || CurrentCube == /*MAXUINT64*/ ((uint64)~((uint64)0)))
					{
						continue;
					}
					for (int LocalX = 0; LocalX < 3; LocalX++)
					{
						for (int LocalY = 0; LocalY < 3; LocalY++)
						{
							for (int LocalZ = 0; LocalZ < 3; LocalZ++)
							{
								const uint64 ONE = 1;
								unsigned long CaseCode =
									(static_cast<bool>((CurrentCube & (ONE << ((LocalX + 0) + 4 * (LocalY + 0) + 4 * 4 * (LocalZ + 0)))) != 0) << 0)
									| (static_cast<bool>((CurrentCube & (ONE << ((LocalX + 1) + 4 * (LocalY + 0) + 4 * 4 * (LocalZ + 0)))) != 0) << 1)
									| (static_cast<bool>((CurrentCube & (ONE << ((LocalX + 0) + 4 * (LocalY + 1) + 4 * 4 * (LocalZ + 0)))) != 0) << 2)
									| (static_cast<bool>((CurrentCube & (ONE << ((LocalX + 1) + 4 * (LocalY + 1) + 4 * 4 * (LocalZ + 0)))) != 0) << 3)
									| (static_cast<bool>((CurrentCube & (ONE << ((LocalX + 0) + 4 * (LocalY + 0) + 4 * 4 * (LocalZ + 1)))) != 0) << 4)
									| (static_cast<bool>((CurrentCube & (ONE << ((LocalX + 1) + 4 * (LocalY + 0) + 4 * 4 * (LocalZ + 1)))) != 0) << 5)
									| (static_cast<bool>((CurrentCube & (ONE << ((LocalX + 0) + 4 * (LocalY + 1) + 4 * 4 * (LocalZ + 1)))) != 0) << 6)
									| (static_cast<bool>((CurrentCube & (ONE << ((LocalX + 1) + 4 * (LocalY + 1) + 4 * 4 * (LocalZ + 1)))) != 0) << 7);

								if (CaseCode != 0 && CaseCode != 511)
								{
									// Cell has a nontrivial triangulation

									const int X = 3 * CubeX + LocalX;
									const int Y = 3 * CubeY + LocalY;
									const int Z = 3 * CubeZ + LocalZ;


									float CornerValues[8] = {
										GetValue(X + 0, Y + 0, Z + 0),
										GetValue(X + 1, Y + 0, Z + 0),
										GetValue(X + 0, Y + 1, Z + 0),
										GetValue(X + 1, Y + 1, Z + 0),
										GetValue(X + 0, Y + 0, Z + 1),
										GetValue(X + 1, Y + 0, Z + 1),
										GetValue(X + 0, Y + 1, Z + 1),
										GetValue(X + 1, Y + 1, Z + 1)
									};

									const FIntVector CornerPositions[8] = {
										FIntVector(X + 0, Y + 0, Z + 0),
										FIntVector(X + 1, Y + 0, Z + 0),
										FIntVector(X + 0, Y + 1, Z + 0),
										FIntVector(X + 1, Y + 1, Z + 0),
										FIntVector(X + 0, Y + 0, Z + 1),
										FIntVector(X + 1, Y + 0, Z + 1),
										FIntVector(X + 0, Y + 1, Z + 1),
										FIntVector(X + 1, Y + 1, Z + 1)
									};

									short ValidityMask = (X != 0) + 2 * (Y != 0) + 4 * (Z != 0);

									check(0 <= CaseCode && CaseCode < 256);
									unsigned char CellClass = Transvoxel::regularCellClass[CaseCode];
									const unsigned short* VertexData = Transvoxel::regularVertexData[CaseCode];
									check(0 <= CellClass && CellClass < 16);
									Transvoxel::RegularCellData CellData = Transvoxel::regularCellData[CellClass];

									// Indices of the vertices used in this cube
									TArray<int> VertexIndices;
									VertexIndices.SetNumUninitialized(CellData.GetVertexCount());

									for (int i = 0; i < CellData.GetVertexCount(); i++)
									{
										int VertexIndex;
										const unsigned short EdgeCode = VertexData[i];

										// A: low point / B: high point
										const unsigned short IndexVerticeA = (EdgeCode >> 4) & 0x0F;
										const unsigned short IndexVerticeB = EdgeCode & 0x0F;

										check(0 <= IndexVerticeA && IndexVerticeA < 8);
										check(0 <= IndexVerticeB && IndexVerticeB < 8);

										const FIntVector PositionA = CornerPositions[IndexVerticeA];
										const FIntVector PositionB = CornerPositions[IndexVerticeB];

										// Index of vertex on a generic cube (0, 1, 2 or 3 in the transvoxel paper, but it's always != 0 so we substract 1 to have 0, 1, or 2)
										const short EdgeIndex = ((EdgeCode >> 8) & 0x0F) - 1;
										check(0 <= EdgeIndex && EdgeIndex < 3);

										// Direction to go to use an already created vertex: 
										// first bit:  x is different
										// second bit: y is different
										// third bit:  z is different
										// fourth bit: vertex isn't cached
										const short CacheDirection = EdgeCode >> 12;

										if ((ValidityMask & CacheDirection) != CacheDirection)
										{
											// If we are on one the lower edges of the chunk, or precedent color is not the same as current one
											const float ValueAtA = CornerValues[IndexVerticeA];
											const float ValueAtB = CornerValues[IndexVerticeB];

											check(ValueAtA - ValueAtB != 0);
											const float t = ValueAtB / (ValueAtB - ValueAtA);

											FVector Q = t * static_cast<FVector>(PositionA) + (1 - t) * static_cast<FVector>(PositionB);


											VertexIndex = VerticesSize;

											Vertices.push_front(Q);
											VerticesSize++;

											// If own vertex, save it
											if (CacheDirection & 0x08)
											{
												SaveVertex(X, Y, Z, EdgeIndex, VertexIndex);
											}
										}
										else
										{
											VertexIndex = LoadVertex(X, Y, Z, CacheDirection, EdgeIndex);
										}

										VertexIndices[i] = VertexIndex;
									}

									// Add triangles
									// 3 vertex per triangle
									int n = 3 * CellData.GetTriangleCount();
									for (int i = 0; i < n; i++)
									{
										Triangles.push_front(VertexIndices[CellData.vertexIndex[i]]);
									}
									TrianglesSize += n;
								}
							}
						}
					}
				}
			}
		}
	}
	Data->EndGet();


	if (VerticesSize < 3)
	{
		// Early exit
		OutSection.Reset();
		return;
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_CREATE_SECTION_FC);
		// Create section
		OutSection.Reset();
		OutSection.bEnableCollision = true;
		OutSection.bSectionVisible = bEnableRender;
		OutSection.SectionLocalBox.Min = -FVector::OneVector;
		OutSection.SectionLocalBox.Max = FVector(CHUNKSIZE_FC, CHUNKSIZE_FC, CHUNKSIZE_FC);
		OutSection.SectionLocalBox.IsValid = true;

		OutSection.ProcVertexBuffer.SetNum(VerticesSize);
		OutSection.ProcIndexBuffer.SetNumUninitialized(TrianglesSize);

		int i = 0;
		for (auto Vertice : Vertices)
		{
			OutSection.ProcVertexBuffer[VerticesSize - 1 - i].Position = Vertice;
			i++;
		}

		i = 0;
		for (auto it = Triangles.begin(); it != Triangles.end(); )
		{
			const int A = *it;
			++it;
			const int B = *it;
			++it;
			const int C = *it;
			++it;

			OutSection.ProcIndexBuffer[i + 2] = A;
			OutSection.ProcIndexBuffer[i + 1] = B;
			OutSection.ProcIndexBuffer[i] = C;

			i += 3;
		}

	}

	if (OutSection.ProcVertexBuffer.Num() < 3 || OutSection.ProcIndexBuffer.Num() == 0)
	{
		// Else physics thread crash
		OutSection.Reset();
	}
}

float FVoxelPolygonizerForCollisions::GetValue(int X, int Y, int Z)
{
	check(0 <= X && X < CHUNKSIZE_FC + 1);
	check(0 <= Y && Y < CHUNKSIZE_FC + 1);
	check(0 <= Z && Z < CHUNKSIZE_FC + 1);

	return CachedValues[X + (CHUNKSIZE_FC + 1) * Y + (CHUNKSIZE_FC + 1) * (CHUNKSIZE_FC + 1) * Z];
}

void FVoxelPolygonizerForCollisions::SaveVertex(int X, int Y, int Z, short EdgeIndex, int Index)
{
	check(0 <= X && X < CHUNKSIZE_FC);
	check(0 <= Y && Y < CHUNKSIZE_FC);
	check(0 <= Z && Z < CHUNKSIZE_FC);
	check(0 <= EdgeIndex && EdgeIndex < 3);

	Cache[X][Y][Z][EdgeIndex] = Index;
}

int FVoxelPolygonizerForCollisions::LoadVertex(int X, int Y, int Z, short Direction, short EdgeIndex)
{
	bool XIsDifferent = static_cast<bool>((Direction & 0x01) != 0);
	bool YIsDifferent = static_cast<bool>((Direction & 0x02) != 0);
	bool ZIsDifferent = static_cast<bool>((Direction & 0x04) != 0);

	check(0 <= X - XIsDifferent && X - XIsDifferent < CHUNKSIZE_FC);
	check(0 <= Y - YIsDifferent && Y - YIsDifferent < CHUNKSIZE_FC);
	check(0 <= Z - ZIsDifferent && Z - ZIsDifferent < CHUNKSIZE_FC);
	check(0 <= EdgeIndex && EdgeIndex < 3);


	check(Cache[X - XIsDifferent][Y - YIsDifferent][Z - ZIsDifferent][EdgeIndex] >= 0);
	return Cache[X - XIsDifferent][Y - YIsDifferent][Z - ZIsDifferent][EdgeIndex];
}
