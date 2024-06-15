// Fill out your copyright notice in the Description page of Project Settings.


#include "SWHPWidget.h"
#include "Components/ProgressBar.h"


void USWHPWidget::UpdateHPBar(int32 InLife, int32 InMaxLife)
{
	const float LifeRatio = InLife / static_cast<float>(InMaxLife);

	HPBar->SetPercent(LifeRatio);
}