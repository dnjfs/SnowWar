// Fill out your copyright notice in the Description page of Project Settings.


#include "SWProjectile.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

#include "SnowWarCharacter.h"
#include "SnowWarPlayerController.h"

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
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ASWProjectile::SetProjectileInfo(float InPressTime)
{
	Team = Cast<ASnowWarPlayerController>(GetOwner())->GetTeam();

	SetSnowBallSpeed(InPressTime);
}

void ASWProjectile::SetSnowBallSpeed(float InPressTime)
{
	InPressTime = FMath::Clamp(InPressTime, MinPressTime, MaxPressTime);

	DamageMultiple = InPressTime;

	// 누르는 시간에 비례하여 눈덩이 속도 조절
	const float Multiple = InPressTime / MaxPressTime;

	ProjectileMovement->InitialSpeed = (ProjectileMovement->MaxSpeed - MinSpeed) * Multiple + MinSpeed;

	UE_LOG(LogTemp, Warning, TEXT("Time: %f / Speed: %f"), InPressTime, ProjectileMovement->InitialSpeed);
}

void ASWProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FRotator Rotation = Hit.Normal.Rotation();
	Rotation.Pitch -= 90.f; // 회전값 보정
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SnowEffect, Hit.Location, Rotation);

	do
	{
		auto Character = Cast<ASnowWarCharacter>(OtherActor);
		if (Character == nullptr)
			break;

		ASnowWarPlayerController* PC = Cast<ASnowWarPlayerController>(GetOwner());
		if (PC == nullptr)
			break;

		ESWTeamType MyTeam = PC->GetTeam();
		ESWTeamType OtherTeam = ESWTeamType::None;
		if (auto OtherController = Cast<ASnowWarPlayerController>(Character->GetController()))
			OtherTeam = OtherController->GetTeam();

		if (MyTeam == OtherTeam)
			break;

		OtherActor->TakeDamage(BaseDamage * DamageMultiple, FDamageEvent(), PC, this);
	} while(false);

	Destroy();
}