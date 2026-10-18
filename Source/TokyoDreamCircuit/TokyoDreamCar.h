// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TokyoDreamCar.generated.h"

class UStaticMeshComponent;
class UChaseCameraComponent;

/**
 * ATokyoDreamCar
 *
 * Physics-driven car prototype. A single rigid body with four contact points
 * and a simplified bicycle-model tyre solver: no Chaos Vehicle skeletal rig, so
 * it works with any static mesh out of the box.
 *
 * The tyre model has one job the arcade ones usually skip: it fades grip as the
 * surface gets wet, so the car understeers on a soaked road, and the rear steps
 * out under power. Wetness blends between dry grip and wet grip.
 *
 * ALL PHYSICS IS COMPUTED IN SI UNITS (metres, m/s, newtons, kg) and converted
 * to Unreal's centimetre units only at the moment forces are applied. This keeps
 * the tuning numbers readable against real-world references (a 1450 kg car, ~1.5
 * friction coefficient, ~4 N/(m/s)^2 of downforce, and so on).
 */
UCLASS(BlueprintType, Blueprintable)
class TOKYODREAMCIRCUIT_API ATokyoDreamCar : public APawn
{
	GENERATED_BODY()

public:
	ATokyoDreamCar();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// ---------------------------------------------------------------------
	// Components
	// ---------------------------------------------------------------------

	/** Simulates physics; this is the car body. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Car")
	TObjectPtr<UStaticMeshComponent> Body;

	/** Visual wheels, index order: 0 = front left, 1 = front right,
	 *  2 = rear left, 3 = rear right. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Car")
	TArray<TObjectPtr<UStaticMeshComponent>> Wheels;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Car")
	TObjectPtr<UChaseCameraComponent> ChaseCamera;

	// ---------------------------------------------------------------------
	// Mass and dimensions
	// ---------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Body", meta = (ClampMin = "100.0"))
	float MassKg = 1450.0f;

	/** Distance between front and rear axle, in centimetres. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Body", meta = (ClampMin = "100.0"))
	float WheelBase = 280.0f;

	/** Distance between left and right wheels, in centimetres. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Body", meta = (ClampMin = "50.0"))
	float TrackWidth = 162.0f;

	/** 0.5 = balanced. 0.45 = slightly rear-biased (typical sports car). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Body", meta = (ClampMin = "0.05", ClampMax = "0.95"))
	float FrontWeightBias = 0.46f;

	/** Radius of the wheels, in centimetres. Used for contact points and for
	 *  the visual wheel spin rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Body", meta = (ClampMin = "5.0"))
	float WheelRadius = 34.0f;

	// ---------------------------------------------------------------------
	// Engine / brakes
	// ---------------------------------------------------------------------

	/** Peak force delivered at the wheels, in newtons. 20000 N in a 1450 kg car
	 *  is roughly a 3.5 s 0-100 km/h. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Engine", meta = (ClampMin = "0.0"))
	float MaxEngineForce = 20000.0f;

	/** Speed at which the engine stops accelerating the car, in km/h. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Engine", meta = (ClampMin = "10.0"))
	float MaxSpeedKmh = 362.0f;

	/** Peak braking force total, in newtons. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Engine", meta = (ClampMin = "0.0"))
	float MaxBrakeForce = 26000.0f;

	/** 1.0 = reverse disabled (single direction). 0.0 = full reverse. Raising
	 *  this makes the wheels resist backwards motion on throttle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Engine", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ReverseThrottleResistance = 1.0f;

	/** Aerodynamic drag coefficient (0.5 * rho * Cd * A). 0.42 is a slippery
	 *  supercar at sea level. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Engine", meta = (ClampMin = "0.0"))
	float DragCoefficient = 0.42f;

	/** Rolling resistance, in newtons per (m/s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Engine", meta = (ClampMin = "0.0"))
	float RollingResistance = 12.0f;

	/** Downforce coefficient (0.5 * rho * Cl * A), in N per (m/s)^2. Higher =
	 *  more grip at speed and more stable high-speed cornering. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Engine", meta = (ClampMin = "0.0"))
	float DownforceCoefficient = 1.8f;

	// ---------------------------------------------------------------------
	// Steering
	// ---------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Steering", meta = (ClampMin = "1.0", ClampMax = "60.0"))
	float MaxSteerAngle = 32.0f;

	/** How much of the steering lock is removed at top speed (0 = none). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Steering", meta = (ClampMin = "0.0", ClampMax = "0.95"))
	float HighSpeedSteerReduction = 0.6f;

	/** Steering responsiveness. Higher = snappier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Steering", meta = (ClampMin = "0.5"))
	float SteerInterpSpeed = 7.0f;

	// ---------------------------------------------------------------------
	// Tyres / surface
	// ---------------------------------------------------------------------

	/** Cornering stiffness of the front axle, N per radian of slip. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Tyres", meta = (ClampMin = "1000.0"))
	float FrontCorneringStiffness = 90000.0f;

	/** Cornering stiffness of the rear axle. Rear > front = stable understeer. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Tyres", meta = (ClampMin = "1000.0"))
	float RearCorneringStiffness = 115000.0f;

	/** Peak friction coefficient on dry asphalt. ~1.5 for a road tyre, up to
	 *  ~1.8 for a semi-slick. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Tyres", meta = (ClampMin = "0.1"))
	float DryFrictionCoefficient = 1.5f;

	/** Same, on fully soaked asphalt. 0.55 is roughly half grip, which is what
	 *  a wet road really costs you. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Tyres", meta = (ClampMin = "0.05"))
	float WetFrictionCoefficient = 0.72f;

	/** 0 = bone dry, 1 = standing water. Blend between the two friction values.
	 *  Drive this from weather, a volumetric lookup, or the track material. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Car|Tyres", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Wetness = 0.0f;

	/** Rear grip multiplier while the handbrake is pulled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Tyres", meta = (ClampMin = "0.05", ClampMax = "1.0"))
	float HandbrakeGripScale = 0.45f;

	/** Grip lost while the throttle is pinned (power oversteer). 1.0 = none. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Tyres", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ThrottleGripLoss = 0.12f;

	/** Diagnostic switch. If the car steers the wrong way, flip this to -1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Tyres", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float LateralForceSign = 1.0f;

	/** Turns the whole physics model on or off. Off = let an external system
	 *  (Chaos Vehicles, replay, cutscene) drive the body instead. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Car|Debug")
	bool bPhysicsEnabled = true;

	// ---------------------------------------------------------------------
	// Read-only telemetry (bind these to the HUD / audio / post FX)
	// ---------------------------------------------------------------------

	/** Signed forward speed in km/h. Negative = reversing. */
	UFUNCTION(BlueprintPure, Category = "Car|Telemetry")
	float GetSpeedKmh() const;

	/** Total speed in km/h, regardless of direction. */
	UFUNCTION(BlueprintPure, Category = "Car|Telemetry")
	float GetAbsoluteSpeedKmh() const;

	/** Slip angle of the rear axle in degrees. Large values = drifting. */
	UFUNCTION(BlueprintPure, Category = "Car|Telemetry")
	float GetRearSlipAngleDeg() const { return FMath::RadiansToDegrees(RearSlipAngle); }

	UFUNCTION(BlueprintPure, Category = "Car|Telemetry")
	float GetSteerAngleDeg() const { return FMath::RadiansToDegrees(SteerAngle); }

	UFUNCTION(BlueprintPure, Category = "Car|Telemetry")
	bool IsDrifting() const;

	/** 0..1, for motion blur, engine note, chromatic aberration, FOV. */
	UFUNCTION(BlueprintPure, Category = "Car|Telemetry")
	float GetSpeed01() const;

	/** 0..1 brake pedal, for brake lights. */
	UFUNCTION(BlueprintPure, Category = "Car|Telemetry")
	float GetBrakeInput() const { return BrakeInput; }

	// Input handlers (also callable from Blueprint / AI).
	UFUNCTION(BlueprintCallable, Category = "Car|Input")
	void SetThrottleInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Car|Input")
	void SetSteeringInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Car|Input")
	void SetBrakeInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Car|Input")
	void SetHandbrakeInput(bool bActive);

protected:
	// Input axis/action handlers, bound in SetupPlayerInputComponent.
	void InputThrottle(float Value);
	void InputSteer(float Value);
	void InputBrake(float Value);
	void InputHandbrake(float Value);

private:
	/** SI helper. ForceN in newtons, applied as an impulse over DeltaTime. */
	void ApplyForceAtLocationSI(const FVector& ForceN, const FVector& WorldLocation, float DeltaTime);

	/** World-space position of one wheel's contact patch (local offsets are in cm). */
	FVector GetWheelContactLocation(int32 WheelIndex) const;

	/** Update the visual wheel meshes (spin + steer). */
	void UpdateWheelVisuals(float DeltaTime, float ForwardSpeedMs);

	// Inputs, all 0..1 unless noted.
	float ThrottleInput = 0.0f;
	float SteeringInput = 0.0f; // -1..1
	float BrakeInput = 0.0f;
	bool bHandbrake = false;

	// State.
	float SteerAngle = 0.0f;    // radians
	float RearSlipAngle = 0.0f; // radians
	float WheelSpin = 0.0f;     // accumulated visual rotation, radians
};
