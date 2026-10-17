// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOverlay.h"
#include "CharacterInfo.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/BattleField/Character/PlayerCharacter.h"

void UPlayerOverlay::CreateUIMat(UTextureRenderTarget2D* RT)
{
	Player = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (!Player)
	{
		return;
	}

	BPC = Cast<ABattleFieldPlayerController>(Player->GetController());
	if (!BPC)
	{
		return;
	}

	UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(MiniMapBaseMaterial, this);
	DynMat->SetTextureParameterValue(TEXT("MiniMapTex"), RT);

	MiniMap->SetBrushFromMaterial(DynMat);

	Geo = MiniMap->GetCachedGeometry();
	FVector2D MiniMapSize = Geo.GetLocalSize();
	Radius = MiniMapSize.X * 0.5f - 1;
	Ratio = MapSize / MiniMapSize.X;
	Center = MiniMapSize * 0.5f;
}

FReply UPlayerOverlay::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FVector2D LocalPos = Geo.AbsoluteToLocal(
	InMouseEvent.GetScreenSpacePosition()	);

	float Dist = FVector2D::Distance(LocalPos, Center);

	if (Dist > Radius)
	{
		return FReply::Handled().CaptureMouse(TakeWidget());
	}

	MapPos = FVector2D{ (MapSize / 2 - LocalPos.Y * Ratio), ((MapSize / 2) * -1 + LocalPos.X * Ratio) };

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		IsLMB_HoldingMap = true;
		BPC->Map_LMB_Pressed(MapPos);
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		IsRMB_HoldingMap = true;
		BPC->Map_RMB_Pressed(MapPos);
	}

	return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply UPlayerOverlay::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

FReply UPlayerOverlay::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FVector2D LocalPos = Geo.AbsoluteToLocal(
		InMouseEvent.GetScreenSpacePosition()
	);

	float Dist = FVector2D::Distance(LocalPos, Center);

	bool bInside = Dist <= Radius;
	bool bMouseDown = InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton);

	if (!bInside)
	{
		if (IsLMB_HoldingMap)
		{
			IsLMB_HoldingMap = false;
			IsLMB_MovingMap = false;
		}

		return FReply::Unhandled();
	}

	if (bInside && bMouseDown && !IsLMB_HoldingMap)
	{
		IsLMB_HoldingMap = true;
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		IsLMB_MovingMap = true;
	}

	MapPos = FVector2D{ (MapSize / 2 - LocalPos.Y * Ratio), ((MapSize / 2) * -1 + LocalPos.X * Ratio) };

	if (IsLMB_HoldingMap)
	{
		BPC->Map_LMB_Pressed(MapPos);
	}

	return FReply::Handled();
}

FReply UPlayerOverlay::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		IsLMB_HoldingMap = false;
		IsLMB_MovingMap = false;
		BPC->Map_LMB_Released(MapPos);
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		FVector2D LocalPos = Geo.AbsoluteToLocal(
			InMouseEvent.GetScreenSpacePosition());

		float Dist = FVector2D::Distance(LocalPos, Center);

		if (Dist > Radius)
		{
			ForceLMBRelease();
		}
	}

	return FReply::Handled().ReleaseMouseCapture();
}

void UPlayerOverlay::ForceLMBRelease()
{
	IsLMB_HoldingMap = false;
	IsLMB_MovingMap = false;
	if (BPC)
	{
		BPC->Map_LMB_Released(MapPos);
	}
}