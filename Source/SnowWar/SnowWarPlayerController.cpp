// Copyright Epic Games, Inc. All Rights Reserved.

#include "SnowWarPlayerController.h"
#include "GameFramework/Pawn.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "SnowWarCharacter.h"
#include "SWProjectile.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ASnowWarPlayerController::ASnowWarPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedTarget = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ASnowWarPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// 임시 (추후 로그인 시 처리)
	Team = ESWTeamType::Team1;
}

void ASnowWarPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(AttackTargetAction, ETriggerEvent::Started, this, &ASnowWarPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(AttackTargetAction, ETriggerEvent::Triggered, this, &ASnowWarPlayerController::OnAttackTargetTriggered);
		EnhancedInputComponent->BindAction(AttackTargetAction, ETriggerEvent::Completed, this, &ASnowWarPlayerController::OnAttackTargetReleased);
		EnhancedInputComponent->BindAction(AttackTargetAction, ETriggerEvent::Canceled, this, &ASnowWarPlayerController::OnAttackTargetReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASnowWarPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void ASnowWarPlayerController::OnAttackTargetTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;

	// If we hit a surface, cache the location
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
		CachedTarget = Hit.Location;
	
	if (APawn* ControlledPawn = GetPawn())
	{
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(ControlledPawn->GetActorLocation(), CachedTarget);
		ControlledPawn->SetActorRotation(TargetRotation);
	}
}

void ASnowWarPlayerController::OnAttackTargetReleased()
{
	if (FollowTime >= LongPressThreshold)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedTarget, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);

	FireProjectile(FollowTime);

	FollowTime = 0.f;
}

void ASnowWarPlayerController::FireProjectile(float InPressTime)
{
	// Try and fire a projectile
	if (ProjectileClass == nullptr)
		return;

	UWorld* const World = GetWorld();
	if (World == nullptr)
		return;

	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn == nullptr)
		return;

	FRotator ProjectileSpawnRotation = UKismetMathLibrary::FindLookAtRotation(ControlledPawn->GetActorLocation(), CachedTarget);
	// Z축을 기준으로만 회전되도록 고정 (Yaw만 바뀜)
	ProjectileSpawnRotation.Pitch = 0.f;
	ProjectileSpawnRotation.Roll = 0.f;

	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector ProjectileSpawnLocation = ControlledPawn->GetActorLocation() + ProjectileSpawnRotation.RotateVector(MuzzleOffset);

	FTransform SpawnTransform(ProjectileSpawnRotation, ProjectileSpawnLocation);
	if (ASWProjectile* SnowBall = World->SpawnActorDeferred<ASWProjectile>(ProjectileClass, SpawnTransform, this, ControlledPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
	{
		SnowBall->SetProjectileInfo(InPressTime);
		SnowBall->FinishSpawning(SpawnTransform);
	}

	// Try and play the sound if specified
	if (FireSound != nullptr)
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, ControlledPawn->GetActorLocation());
}