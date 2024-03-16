// Fill out your copyright notice in the Description page of Project Settings.


#include "SWProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ASWProjectile::ASWProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASWProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ASWProjectile::SetSnowBallSpeed(float InPressTime)
{
	if (InPressTime <= MinPressTime)
		InPressTime = MinPressTime;
	else if (InPressTime >= MaxPressTime)
		InPressTime = MaxPressTime;

	// 누르는 시간에 비례하여 눈덩이 속도 조절
	const float Multiple = InPressTime / MaxPressTime;
	ProjectileMovement->MaxSpeed = ProjectileMovement->MaxSpeed * Multiple;

	UE_LOG(LogTemp, Warning, TEXT("Time: %f / Speed: %f"), InPressTime, ProjectileMovement->MaxSpeed);
}

void ASWProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}