// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SDKUIOpenWorldMapWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Components/WidgetSwitcher.h"

#include "Kismet/GameplayStatics.h"

#include "UI/SDKOpenWorldMapMenuWidget.h"
#include "UI/SDKOpenWorldMapWidget.h"
#include "UI/SDKCheckBoxParam.h"
#include "UI/SDKMapBookMarkIconWidget.h"
#include "UI/SDKTutorialGuideWidget.h"
#include "UI/SDKTopBarWidget.h"
#include "UI/SDKButtonWidget.h"

#include "Share/SDKData.h"
#include "Share/SDKEnum.h"
#include "Share/SDKHelper.h"
#include "Engine/SDKAssetManager.h"
#include "Engine/SDKGameInstance.h"
#include "Engine/SDKGameDelegate.h"
#include "Manager/SDKTableManager.h"
#include "Manager/SDKTutorialGuideManager.h"
#include "Manager/SDKLobbyServerManager.h"

#include "Character/SDKMyInfo.h"
#include "Character/SDKPlayerCharacter.h"
#include "Character/SDKPlayerController.h"
#include "Character/SDKOpenWorldController.h"
#include "Character/SDKHUD.h"
#include "Engine/TargetPoint.h"

USDKUIOpenWorldMapWidget::USDKUIOpenWorldMapWidget()
{
	MapTextureSize = FVector2D(2048.f, 2048.f);
}

void USDKUIOpenWorldMapWidget::OpenUIProcess()
{
	Super::OpenUIProcess();

	CheckPlayerCharacter();
	SetVisibleTopBarButtonBack(false);

	// CachedWidget, CachedGeometry는 한프레임 뒤 캐싱.
	GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &USDKUIOpenWorldMapWidget::InitWorldMap, 0.05f, false);
}

void USDKUIOpenWorldMapWidget::CreateUIProcess()
{
	Super::CreateUIProcess();

	if (OpenAnim == nullptr)
	{
		OpenAnim = GetWidgetAnimation(TEXT("Menu"));
	}

	auto MapTable = USDKTableManager::Get()->FindTableRow<FS_Map>(ETableType::tb_Map, UGameplayStatics::GetCurrentLevelName(GetWorld()));
	if (MapTable != nullptr)
	{
		OpenWorldSize = FVector2D(MapTable->MapSize);
	}

	// PlayerIcon 설정
	if (IsValid(BPPlayerIcon) == true)
	{
		CharacterIcon = Cast<UImage>(BPPlayerIcon->GetWidgetFromName(TEXT("ImageCharacterIcon")));
		CameraViewIcon = Cast<UImage>(BPPlayerIcon->GetWidgetFromName(TEXT("ImageCameraViewIcon")));
	}

	OpenWorldMapHandle = USDKGameDelegate::Get()->OnUpdateOpenWorldMap.AddUFunction(this, TEXT("UpdatePlayerCharacterPositionUI"));

	TArray<AActor*> MapCenterActor;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ATargetPoint::StaticClass(), TEXT("MapCenter"), MapCenterActor);

	if (MapCenterActor.Num() > 0)
	{
		FVector Gap = MapCenterActor.Last()->GetActorLocation();
		WorldZeroGap.X = Gap.X;
		WorldZeroGap.Y = Gap.Y;
	}
}

void USDKUIOpenWorldMapWidget::CloseUIProcess()
{
	Super::CloseUIProcess();

	SetWidgetVisibility(AdventureMap_Menu, ESlateVisibility::Collapsed);
	SetVisibleTopBarButtonBack(true);
	SetChangeTopBarExitFunc(true);

	if (MyPlayerCharacter.IsValid())
	{
		MyPlayerCharacter->ServerInputModeChange(true);
	}

	if (OpenWorldMapHandle.IsValid())
	{
		USDKGameDelegate::Get()->OnUpdateOpenWorldMap.Remove(OpenWorldMapHandle);
	}
	
}

void USDKUIOpenWorldMapWidget::CheckPlayerCharacter()
{
	if (MyPlayerCharacter.IsValid() == false)
	{
		auto SDKOwningCharacter = GetOwningPlayerPawn<ASDKPlayerCharacter>();

		if (IsValid(SDKOwningCharacter) == true)
		{
			MyPlayerCharacter = SDKOwningCharacter;
			MyPlayerCharacter->ServerInputModeChange(false);
		}
	}
}

// 플레이어 캐릭터의 현재위치에 따라 미니맵 갱신
void USDKUIOpenWorldMapWidget::UpdatePlayerCharacterPositionUI()
{
	if (MyPlayerCharacter.IsValid() == true)
	{
		FWidgetTransform MapRenderLoc = AdventureMap->GetMapRenderTransform();
		FVector Loc = MyPlayerCharacter.Get()->GetActorLocation();
		FVector2D MapSize = FVector2D::UnitVector * AdventureMap->GetDesiredSize() * AdventureMap->RenderTransform.Scale * MapRenderLoc.Scale * 0.5f;
		MapScaleRatio = MapSize / OpenWorldSize;
		FVector2D IconLoc = FVector2D(Loc.X, Loc.Y) * MapScaleRatio;
		BPPlayerIcon->SetRenderTranslation(IconLoc);

		if (IsValid(CharacterIcon) && IsValid(CameraViewIcon))
		{
			CharacterIcon->SetRenderTransformAngle(MyPlayerCharacter->GetActorRotation().Yaw);

			ASDKPlayerController* SDKPlayerController = MyPlayerCharacter->GetController<ASDKPlayerController>();
			if (IsValid(SDKPlayerController) == true)
			{
				CameraViewIcon->SetRenderTransformAngle(SDKPlayerController->GetControlRotation().Yaw);
			}
		}
	}
	else
	{
		CheckPlayerCharacter();
	}
}


// 월드맵 초기 설정, CreateUIProcess() 타이머에 의해 호출.
void USDKUIOpenWorldMapWidget::InitWorldMap()
{
	GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);

	if (!GetCachedWidget().IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &USDKUIOpenWorldMapWidget::InitWorldMap, 0.05f, false);
		return;
	}

	const FGeometry& MyGeometry = GetCachedGeometry();

	AdventureMapSize = MapTextureSize * AdventureMap->RenderTransform.Scale;
	int32 SizeX = MyGeometry.Size.X;
	int32 SizeY = MyGeometry.Size.Y;

	float SizeRatio = (float)SizeX / (float)SizeY;
	MinZoomValue = FMath::Max(SizeX / AdventureMapSize.X, SizeY / AdventureMapSize.Y);

	int32 MaxZoomCount = 15;
	MaxZoomValue = 6.0f;
	ZoomStepValue = (MaxZoomValue - MinZoomValue) / MaxZoomCount;

	AdventureMap->SetOpenFromWorldMap(MinZoomValue, MaxZoomValue, ZoomStepValue, SizeX, SizeY);
	//AdventureMap->OpenInitCenterFromPlayerCharacter(MyPlayerCharacter.IsValid() ? MyPlayerCharacter.Get()->GetActorLocation() : FVector::OneVector);
	UpdatePlayerCharacterPositionUI();
}
