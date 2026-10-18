// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ChaseCameraComponent.generated.h"

class UCameraComponent;

/**
 * UChaseCameraComponent
 *
 * Spring-arm style chase camera. Attach it to the car body and it does the rest:
 * lags behind the car, pulls back with speed, accelerates its field of view and
 * adds a little high-speed shake. No SpringArm/blueprint wiring required.
 */
UCLASS(ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class TOKYODREAMCIRCUIT_API UChaseCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UChaseCameraComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chase Camera")
	TObjectPtr<UCameraComponent> Camera;

	/** Distance behind the car at a standstill, in centimetres. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "100.0"))
	float BaseDistance = 850.0f;

	/** Extra distance pulled back at top speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "0.0"))
	float SpeedDistanceGain = 320.0f;

	/** Height above the car, in centimetres. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "0.0"))
	float Height = 320.0f;

	/** FOV when stationary. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "20.0", UIMax = "120.0"))
	float BaseFOV = 55.0f;

	/** FOV at top speed. This is most of the "sense of speed". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "20.0", UIMax = "150.0"))
	float MaxFOV = 88.0f;

	/** How quickly the camera catches up to its target position. Lower = more lag. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "0.5"))
	float PositionInterpSpeed = 6.5f;

	/** How quickly the camera catches up to the car's facing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "0.5"))
	float RotationInterpSpeed = 4.5f;

	/** How far ahead of the car the camera looks, as a fraction of speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "0.0"))
	float LookAheadGain = 0.35f;

	/** Speed treated as "flat out" for the speed01 blend, in km/h. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "10.0"))
	float ReferenceTopSpeedKmh = 380.0f;

	/** Maximum camera shake offset at top speed, in centimetres. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "0.0"))
	float ShakeAmplitude = 1.4f;

	/** Height the camera aims at above the car's origin. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase Camera", meta = (ClampMin = "0.0"))
	float LookAtHeight = 90.0f;

private:
	/** Speed blended from 0..1, cached for other systems (FOV, effects). */
	float Speed01 = 0.0f;

public:
	/** 0 at rest, 1 at ReferenceTopSpeedKmh. Read this to drive motion blur,
	 *  chromatic aberration or engine audio. */
	UFUNCTION(BlueprintPure, Category = "Chase Camera")
	float GetSpeed01() const { return Speed01; }
};
