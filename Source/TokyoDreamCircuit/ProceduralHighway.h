// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralHighway.generated.h"

class USplineComponent;
class UProceduralMeshComponent;
class UMaterialInterface;

/** One point of the road cross-section. LateralFraction is normalised so the
 *  profile scales with RoadWidth: -0.5 is the left edge, +0.5 the right edge,
 *  0.0 the centreline. HeightOffset is in centimetres (negative = below). */
USTRUCT(BlueprintType)
struct FRoadProfilePoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Profile")
	float LateralFraction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Profile")
	float HeightOffset = 0.0f;
};

/**
 * AProceduralHighway
 *
 * Generates a drivable road as a procedural mesh swept along a USplineComponent,
 * with complex collision. Everything is adjustable in the details panel:
 * width, elevation, cross-slope crown, kerbs, and speed-derived banking.
 *
 * The mesh is rebuilt by pressing "Rebuild Road" in the details panel (or by
 * calling BuildDefaultCircuit / RebuildRoad from Blueprint). It is intentionally
 * tick-free: geometry costs nothing at runtime.
 */
UCLASS(BlueprintType, Blueprintable)
class TOKYODREAMCIRCUIT_API AProceduralHighway : public AActor
{
	GENERATED_BODY()

public:
	AProceduralHighway();

	/** Spline that defines the centreline of the circuit. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Highway")
	TObjectPtr<USplineComponent> Centerline;

	/** Generated road surface. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Highway")
	TObjectPtr<UProceduralMeshComponent> RoadMesh;

	// ---------------------------------------------------------------------
	// Shape
	// ---------------------------------------------------------------------

	/** Total width of the road surface, in centimetres. 1800 = 18 m. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Shape", meta = (ClampMin = "100.0", UIMax = "4000.0"))
	float RoadWidth = 1800.0f;

	/** Number of segments along the road. Higher = smoother curves, more triangles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Shape", meta = (ClampMin = "8", UIMax = "2000"))
	int32 NumSegments = 512;

	/** Extra subdivision between each pair of profile points (across the road). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Shape", meta = (ClampMin = "1", UIMax = "16"))
	int32 LateralSubdivisions = 4;

	/** How far the surface texture repeats along the road, in centimetres. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Shape", meta = (ClampMin = "50.0"))
	float UVTiling = 1200.0f;

	/** Cross-section, left edge (-0.5) to right edge (+0.5). Default has a crown
	 *  in the middle so water drains to the sides. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Shape")
	TArray<FRoadProfilePoint> RoadProfile;

	/** Flip the triangle winding if the road renders inside-out / unlit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Shape")
	bool bFlipWinding = false;

	/** Closed circuit (true) or a length of open road (false). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Shape")
	bool bClosedLoop = true;

	// ---------------------------------------------------------------------
	// Banking
	// ---------------------------------------------------------------------

	/** How strongly curvature rolls the road into the corner. Larger = more
	 *  aggressive banking. The roll is clamped by MaxBankAngle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Banking", meta = (ClampMin = "0.0"))
	float BankingScale = 4000.0f;

	/** Hard limit on bank angle, in degrees. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Banking", meta = (ClampMin = "0.0", UIMax = "30.0"))
	float MaxBankAngle = 8.0f;

	// ---------------------------------------------------------------------
	// Generated-base-circuit parameters (used by BuildDefaultCircuit)
	// ---------------------------------------------------------------------

	/** Rough radius of the generated circuit, in centimetres. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Circuit", meta = (ClampMin = "1000.0"))
	float CircuitRadius = 60000.0f;

	/** Number of control points in the generated circuit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Circuit", meta = (ClampMin = "8", UIMax = "256"))
	int32 CircuitPoints = 28;

	/** 0 = perfect circle, 1 = very twisty. Drives the harmonic sum. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Circuit", meta = (ClampMin = "0.0", UIMax = "1.0"))
	float Curvature = 0.28f;

	/** Vertical amplitude of the generated circuit, in centimetres. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Circuit", meta = (ClampMin = "0.0"))
	float ElevationAmplitude = 4000.0f;

	/** Randomisation seed for the generated circuit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Circuit")
	int32 CircuitSeed = 1337;

	// ---------------------------------------------------------------------
	// Material / collision
	// ---------------------------------------------------------------------

	/** Assign a wet-asphalt material here. Lane markings belong in the material,
	 *  driven by the U coordinate (0 = left edge, 1 = right edge). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Render")
	TObjectPtr<UMaterialInterface> RoadMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highway|Render")
	bool bCreateCollision = true;

	// ---------------------------------------------------------------------
	// API
	// ---------------------------------------------------------------------

	/** Regenerate the road mesh from the current spline and settings. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Highway")
	void RebuildRoad();

	/** Discard the spline and generate a fresh circuit from the Circuit settings. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Highway")
	void BuildDefaultCircuit();

	/** Handy for the chase camera and AI: total length of the centreline. */
	UFUNCTION(BlueprintPure, Category = "Highway")
	float GetTrackLength() const;

	/** Project a world location onto the centreline. Returns distance along the
	 *  spline (cm) and the signed lateral offset (cm, + = right of centreline). */
	UFUNCTION(BlueprintCallable, Category = "Highway")
	void ProjectOntoTrack(const FVector& WorldLocation, float& OutDistanceAlongTrack, float& OutLateralOffset) const;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
