// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SnowWarCharacter.generated.h"

class UInputAction;
class UWidgetComponent;
struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangedLife, int32 Life, int32 MaxLife);

UCLASS(Blueprintable)
class ASnowWarCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASnowWarCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Called for movement input
	void Move(const FInputActionValue& Value);
	
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Move Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> HPWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = Stat)
	int32 MaxLife = 100;

	UPROPERTY(Transient)
	int32 Life = 0;

	FOnChangedLife OnChangedLife;
};

