// Copyright Epic Games, Inc. All Rights Reserved.

#include "TaskHUD.h"
#include "Engine/AssetManager.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "CanvasItem.h"
#include "Kismet/GameplayStatics.h"

ATaskHUD::ATaskHUD()
{

}


void ATaskHUD::BeginPlay()
{
	Super::BeginPlay();
	if (!CrosshairTexture.ToString().IsEmpty())
	{
		CrosshairTex = CrosshairTexture.LoadSynchronous();
	}
}

void ATaskHUD::DrawHUD()
{
	Super::DrawHUD();

	if (CrosshairTex)
	{
		// Draw a simple crosshair

		// find center of the Canvas
		const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

		// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
		const FVector2D CrosshairDrawPosition((Center.X - 5.0f), (Center.Y + 10.0f));

		// draw the crosshair
		FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
	}
}
