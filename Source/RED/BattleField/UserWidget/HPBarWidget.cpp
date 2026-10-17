// Fill out your copyright notice in the Description page of Project Settings.


#include "HPBarWidget.h"

#include "Components/ProgressBar.h"

void UHPBarWidget::UpdateBar(float _CurHP, float _ShieldAmount, float _MaxHP)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(_CurHP / _MaxHP);
	}

	if (ShieldBar)
	{
		ShieldBar->SetPercent(_ShieldAmount / _MaxHP);
	}
}
