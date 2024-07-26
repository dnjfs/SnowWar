// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWEnum.h"
#include "GameFramework/Actor.h"
#include "SWProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class ASnowWarPlayerController;
class UNiagaraSystem;

UCLASS()
class SNOWWAR_API ASWProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASWProjectile();

	void SetProjectileInfo(float InPressTime);
	void SetSnowBallSpeed(float InPressTime);

protected:
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MinPressTime = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MaxPressTime = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float BaseDamage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* SnowEffect;

	UPROPERTY(Transient)
	ESWTeamType Team = ESWTeamType::None;

	UPROPERTY(Transient)
	float DamageMultiple = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float MinSpeed = 500.f;
};
