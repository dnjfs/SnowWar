// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWHPWidget.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class SNOWWAR_API USWHPWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHPBar(int32 InLife, int32 InMaxLife);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> HPBar;
};
