// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SDKOpenWorldMapWidget.h"

#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/RichTextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/SDKAssetManager.h"

#include "Character/SDKHUD.h"
#include "Character/SDKMyInfo.h"
#include "Character/SDKCharacter.h"
#include "Character/SDKOpenWorldController.h"
#include "Vehicle/SDKPlayerVehicle.h"

#include "Share/SDKEnum.h"
#include "Share/SDKData.h"
#include "Share/SDKDataTable.h"
#include "Manager/SDKTableManager.h"
#include "Engine/SDKGameInstance.h"
#include "Engine/SDKBlueprintFunctionLibrary.h"
#include "Engine/SDKGameDelegate.h"

#include "UI/SDKButtonParam.h"
#include "UI/SDKWidgetParam.h"
#include "UI/SDKUIOpenWorldMapWidget.h"


USDKOpenWorldMapWidget::USDKOpenWorldMapWidget()
{
	IconZoomScaleValue = 0.5f;
	PinchZoomLerpValue = .0f;
	PanSpeed = 0.8f;
}

void USDKOpenWorldMapWidget::CreateUIProcess()
{
	Super::CreateUIProcess();

	InitMovePoints();
}

void USDKOpenWorldMapWidget::OpenUIProcess()
{
	Super::OpenUIProcess();
}

void USDKOpenWorldMapWidget::CloseUIProcess()
{
	Super::CloseUIProcess();
}

//FReply USDKOpenWorldMapWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	Super::NativeOnMouseMove(InGeometry, InMouseEvent);
//
//	return FReply::Handled();
//}
//
//FReply USDKOpenWorldMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
//
//	FVector2D ScPosition = InMouseEvent.GetScreenSpacePosition();
//
//	IsDraggingMap = InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bPinching == false;
//
//	UE_LOG(LogTemp, Log, TEXT("NativeOnMouseButtonDown Point Index [%d] : [%.2f, %.2f]"), InMouseEvent.GetPointerIndex(), ScPosition.X, ScPosition.Y);
//	UE_LOG(LogTemp, Log, TEXT("Dragging Is %d"), IsDraggingMap ? 1 : 0);
//	return IsDraggingMap || bPinching ? FReply::Handled() : FReply::Unhandled();
//}
//
//FReply USDKOpenWorldMapWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
//
//	IsDraggingMap = !(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bPinching == false);
//
//	UE_LOG(LogTemp, Log, TEXT("NativeOnMouseButtonUp Point Index [%d] : [%.2f, %.2f]"), InMouseEvent.GetPointerIndex(), InMouseEvent.GetScreenSpacePosition().X, InMouseEvent.GetScreenSpacePosition().Y);
//	return IsDraggingMap || bPinching ? FReply::Unhandled() : FReply::Handled();
//}
//
//FReply USDKOpenWorldMapWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
//{
//	Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
//
//	if (TouchStartInfo.Num() < 2 && TouchStartInfo.Contains(InGestureEvent.GetPointerIndex()) == false)
//	{
//		FVector2D ScPosition = InGestureEvent.GetScreenSpacePosition();
//		TouchStartInfo.Emplace(InGestureEvent.GetPointerIndex(), ScPosition);
//	}
//
//	return FReply::Unhandled();
//}
//
//FReply USDKOpenWorldMapWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
//{
//	Super::NativeOnTouchMoved(InGeometry, InGestureEvent);
//
//	MyGeometry = InGeometry;
//	MouseEvent = InGestureEvent;
//
//	if (IsDraggingMap)
//	{
//		FVector2D Vec2D = Overlay_Full->RenderTransform.Translation + (InGestureEvent.GetCursorDelta() * PanSpeed * Overlay_Full->RenderTransform.Scale.X);
//		Overlay_Full->SetRenderTranslation(ClampSides(MyGeometry, Vec2D));
//	}
//
//	USDKGameDelegate::Get()->OnUpdateOpenWorldMap.Broadcast();
//
//	UE_LOG(LogTemp, Log, TEXT("Touch MovedIndex [%d] : [%.2f, %.2f]"), InGestureEvent.GetPointerIndex(), InGestureEvent.GetScreenSpacePosition().X, InGestureEvent.GetScreenSpacePosition().Y);
//
//	return FReply::Unhandled();
//}
//
//FReply USDKOpenWorldMapWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
//{
//	Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
//
//	if (TouchStartInfo.Contains(InGestureEvent.GetPointerIndex()) == true)
//	{
//		TouchStartInfo.Remove(InGestureEvent.GetPointerIndex());
//		FVector2D ScPosition = InGestureEvent.GetScreenSpacePosition();
//		UE_LOG(LogTemp, Log, TEXT("Touch EndedIndex [%d] : [%.2f, %.2f]"), InGestureEvent.GetPointerIndex(), ScPosition.X, ScPosition.Y);
//	}
//
//	return FReply::Unhandled();
//}

void USDKOpenWorldMapWidget::OnClickedButtonPotal(USDKWidgetParam* param)
{
	ASDKHUD* SDKHUD = Cast<ASDKHUD>(GetOwningPlayer()->GetHUD());
	if (IsValid(SDKHUD) == true)
	{
		if (IsValid(param) == false)
		{
			SDKHUD->CloseUI(EUI::UI_OpenWorldMap);
			return;
		}

		USDKWidgetParamWorldIcon* ParamIcon = Cast<USDKWidgetParamWorldIcon>(param);
		if (IsValid(ParamIcon) == true)
		{
			if (SDKHUD->IsOpenUI(EUI::UI_OpenWorldMap) == true)
			{
				USDKUIOpenWorldMapWidget* OpenWorldMapWidget = Cast<USDKUIOpenWorldMapWidget>(SDKHUD->GetUI(EUI::UI_OpenWorldMap));
				if (IsValid(OpenWorldMapWidget))
				{
					OpenWorldMapWidget->OpenMenuTeb(ParamIcon->GetPos(), ParamIcon->GetTableID());
				}
			}
		}
	}
}

void USDKOpenWorldMapWidget::EndTeleportPotalPoint()
{
	GetWorld()->GetTimerManager().ClearTimer(PortalHandle);

	auto MyCharacter = Cast<ASDKCharacter>(GetOwningPlayer()->GetPawn());
	if (IsValid(MyCharacter) == true)
	{
		ASDKOpenWorldController* SDKOpenWorldController = GetOwningPlayer<ASDKOpenWorldController>();
		if (IsValid(SDKOpenWorldController) == true)
		{
			SDKOpenWorldController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 0.5f, FLinearColor::Black);
			SDKOpenWorldController->ClientToggleWidgetBySequence(true);
		}

		MyCharacter->ServerSetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void USDKOpenWorldMapWidget::SelectGroupIconVisible(bool isVisible, EIconGroup iconGroup)
{
	if (mapIconWidgetList.Contains(iconGroup) == true)
	{
		for (auto Iter = mapIconWidgetList[iconGroup].CreateIterator(); Iter; ++Iter)
		{
			for (auto IterWidget : Iter->Value)
			{
				SetWidgetVisibility(IterWidget, isVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
			}

		}
	}
}

void USDKOpenWorldMapWidget::UpdateZoomIn(float ZoomValue)
{
	CurrentZoomValue = ZoomValue;

	//Overlay_Full->SetRenderScale(FVector2D::UnitVector * ZoomValue);
	//Overlay_Full->SetRenderTranslation(ClampSides(MyGeometry, WheelClampValue));

	UpdateIconScale();
}

FVector2D USDKOpenWorldMapWidget::ClampSides(FGeometry Geometry, FVector2D ClampTo)
{
	//FVector2D Vec2D = (GetDesiredSize() * Overlay_Full->RenderTransform.Scale - Geometry.GetLocalSize()) * 0.5f;
	FVector2D Vec2D;
	Vec2D.X = FMath::Abs((GetDesiredSize().X * RenderTransform.Scale.X * Overlay_Full->RenderTransform.Scale.X) - ViewportX);
	Vec2D.Y = FMath::Abs((GetDesiredSize().Y * RenderTransform.Scale.Y * Overlay_Full->RenderTransform.Scale.Y));

	Vec2D.X *= 2.0f;
	Vec2D.Y = (Vec2D.Y * 2.0f) - (ViewportY * 2.0f);

	return FVector2D(FMath::Clamp(ClampTo.X, -Vec2D.X, Vec2D.X), FMath::Clamp(ClampTo.Y, -Vec2D.Y, Vec2D.Y));

}

FWidgetTransform USDKOpenWorldMapWidget::GetMapRenderTransform()
{
	if (IsValid(Overlay_Full) == true)
	{
		return Overlay_Full->RenderTransform;
	}

	return FWidgetTransform();
}

void USDKOpenWorldMapWidget::UpdateIconScale()
{
	// 아이콘 타입 별 스케일 조정.
	FVector2D RootScale = Overlay_Full->RenderTransform.Scale;
	for (auto IterGroup = mapIconWidgetList.CreateIterator(); IterGroup; ++IterGroup)
	{
		for (auto IterType = IterGroup.Value().CreateIterator(); IterType; ++IterType)
		{
			switch (IterType->Key)
			{
			case EIconType::String:
				for (auto IterWidget : IterType->Value)
				{
					IterWidget->SetRenderScale(FMath::Clamp(FVector2D::UnitVector / (RootScale * IconZoomScaleValue * 0.5f), FVector2D::UnitVector, FVector2D::UnitVector * 8.0f));
				}
				break;
			case EIconType::GoldClash:
			case EIconType::RPG:
			case EIconType::Portal:
				for (auto IterWidget : IterType->Value)
				{
					IterWidget->SetRenderScale(FMath::Clamp(FVector2D::UnitVector / (RootScale * IconZoomScaleValue), FVector2D::UnitVector * 0.1f, FVector2D::UnitVector * 2.5f));
				}
				break;
			default:

				for (auto IterWidget : IterType->Value)
				{
					IterWidget->SetRenderScale(FMath::Clamp((FVector2D::UnitVector * 0.8f) / (RootScale * IconZoomScaleValue), FVector2D::UnitVector * 0.1f, FVector2D::UnitVector * 2.5f));
				}
				break;
			}

		}
	}
}

void USDKOpenWorldMapWidget::SetOpenFromWorldMap(float InMinZoom, float InMaxZoom, float ZoomStep, int32 ViewX, int32 ViewY)
{
	ViewportX = ViewX;
	ViewportY = ViewY;
	ZoomStepValue = ZoomStep;
	MinZoomValue = InMinZoom;
	MaxZoomValue = InMaxZoom;

	UpdateZoomIn(InMinZoom);
}

void USDKOpenWorldMapWidget::ResetCloseFromWorldMap()
{
	Overlay_Full->SetRenderTranslation(FVector2D::ZeroVector);
}

void USDKOpenWorldMapWidget::OpenInitCenterFromPlayerCharacter(FVector PlayerPos)
{
	FVector2D IconLoc = FVector2D(PlayerPos.Y, -PlayerPos.X) * MapRatio;

	Overlay_Full->SetRenderTranslation(ClampSides(GetCachedGeometry(), -IconLoc * CurrentZoomValue));
}

void USDKOpenWorldMapWidget::MiniMapIconIgnoreInteraction()
{
	for(const auto& IterGroup : mapIconWidgetList)
	{
		for(const auto& IterType : IterGroup.Value)
		{
			for(const auto& IterArr : IterType.Value)
			{
				if (IsValid(IterArr) && IsValid(IterArr->GetWidgetFromName(FName("Button"))))
				{
					USDKButtonParam* pButton = Cast<USDKButtonParam>(IterArr->GetWidgetFromName(FName("Button")));
					if (IsValid(pButton) && pButton->GetVisibility() != ESlateVisibility::Collapsed)
					{
						SetWidgetVisibility(pButton, ESlateVisibility::SelfHitTestInvisible);
					}
				}
			}
		}
	}

	if (PortalWidgetList.Num() > 0)
	{
		for (const auto& IterPortal : PortalWidgetList)
		{
			if (IsValid(IterPortal) && IsValid(IterPortal->GetWidgetFromName(FName("Button"))))
			{
				USDKButtonParam* pButton = Cast<USDKButtonParam>(IterPortal->GetWidgetFromName(FName("Button")));
				SetWidgetVisibility(pButton, ESlateVisibility::SelfHitTestInvisible);
			}
		}
	}
}

void USDKOpenWorldMapWidget::InitMovePoints()
{
	if (IsValid(OverlayPortal) == true)
	{
		OverlayPortal->ClearChildren();
	}

	if (IsValid(OverlayPortal) == true)
	{
		Overlay_String->ClearChildren();
	}

	auto MapTable = USDKTableManager::Get()->FindTableRow<FS_Map>(ETableType::tb_Map, UGameplayStatics::GetCurrentLevelName(GetWorld()));
	if (MapTable != nullptr)
	{
		OpenWorldSize = FVector2D(MapTable->MapSize);
	}

	ImageSize = ImageSquare->Brush.GetImageSize() * 0.5f;
	MapRatio = ImageSize / OpenWorldSize;
	auto mapTableData = USDKTableManager::Get()->GetRowMap(ETableType::tb_WorldMap);
	if (mapTableData.Num() > 0)
	{
		auto WidgetTable = USDKTableManager::Get()->FindTableWidgetBluePrint(EWidgetBluePrint::OpenWorldMapIcon);
		if (WidgetTable == nullptr)
		{
			return;
		}

		TSubclassOf<USDKUserWidget> WidgetClass(*USDKAssetManager::Get().LoadSynchronous(WidgetTable->WidgetBluePrintPath));
		if (IsValid(WidgetClass) == false)
		{
			return;
		}

		for (auto itMap : mapTableData)
		{
			if (itMap.Key.ToString().Find(TEXT("Version")) > INDEX_NONE)
			{
				continue;
			}

			auto TableData = reinterpret_cast<FS_WorldMap*>(itMap.Value);
			if (TableData == nullptr)
			{
				continue;
			}

			auto IconWidget = CreateWidget<USDKUserWidget>(GetOwningPlayer(), WidgetClass);
			if (IsValid(IconWidget) == false)
			{
				continue;
			}


			auto ImageIcon = Cast<UImage>(IconWidget->GetWidgetFromName(FName("Icon")));
			auto RichTextName = Cast<URichTextBlock>(IconWidget->GetWidgetFromName(FName("RichTextBlock_49")));
			auto ButtonPortal = Cast<USDKButtonParam>(IconWidget->GetWidgetFromName(FName("Button")));
			FString TableID = itMap.Key.ToString();

			if (IsValid(ImageIcon))
			{
				SetImageTexturePath(ImageIcon, TableData->IconPath.ToString());
			}
			
			if (IsValid(RichTextName))
			{
				SetRichTextBlockTableText(RichTextName, TableData->WorldMapPopTitle);
			}

			if (IsValid(ButtonPortal))
			{
				SetWidgetVisibility(ButtonPortal, ESlateVisibility::Visible);
			}

			if (IsValid(OverlayPortal))
			{
				auto pOverlaySlot = OverlayPortal->AddChildToOverlay(IconWidget);
				if (IsValid(pOverlaySlot))
				{
					pOverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
					pOverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
				}
			}


 			FVector2D IconLoc = FVector2D(TableData->TargetLocation.X, TableData->TargetLocation.Y) * MapRatio;
 			IconWidget->SetRenderTranslation(IconLoc);

			if (IsValid(ButtonPortal))
			{
				if (IsValid(ButtonPortal->GetClickedParam()) == false)
				{
					auto NewParamIcon = NewObject<USDKWidgetParamWorldIcon>(GetWorld(), USDKWidgetParamWorldIcon::StaticClass());
					if (IsValid(NewParamIcon))
					{
 						NewParamIcon->SetPos(TableData->TargetLocation);
						NewParamIcon->SetTableID(TableID);
						ButtonPortal->SetClickedParam(NewParamIcon);
					}
				}
				else
				{
					auto ParamIcon = Cast<USDKWidgetParamWorldIcon>(ButtonPortal->GetClickedParam());
					if (IsValid(ParamIcon))
					{
						ParamIcon->SetPos(TableData->TargetLocation);
						ParamIcon->SetTableID(TableID);
					}
				}

				if (ButtonPortal->OnClickedParam.Contains(this, FName("OnClickedButtonPotal")) == false)
				{
					ButtonPortal->OnClickedParam.AddDynamic(this, &USDKOpenWorldMapWidget::OnClickedButtonPotal);
				}
			}
		}
	}
}