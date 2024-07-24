// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SDKUserWidget.h"
#include "SDKOpenWorldMapWidget.generated.h"

class UOverlay;

class USDKWidgetParam;
class UImage;

UCLASS()
class ARENA_API USDKOpenWorldMapWidget : public USDKUserWidget
{
	GENERATED_BODY()
	
public:
	USDKOpenWorldMapWidget();

	// Begin SDKUserWidget interface
	virtual void CreateUIProcess() override;
	virtual void OpenUIProcess() override;
	virtual void CloseUIProcess() override;

	//virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	//virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	//virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	//virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	// End SDKUserWidget interface

	TArray<USDKUserWidget*> GetIconWidgetList(EIconGroup InGroup, EIconType InType);

	UFUNCTION()
	void OnClickedButtonPotal(USDKWidgetParam* param);

	UFUNCTION()
	void EndTeleportPotalPoint();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInitMapData();

	UFUNCTION(BlueprintCallable)
	void SelectGroupIconVisible(bool isVisible, EIconGroup iconGroup);

	UFUNCTION(BlueprintCallable)
	void UpdateZoomIn(float ZoomValue);

	UFUNCTION(BlueprintCallable)
	FVector2D ClampSides(FGeometry Geometry, FVector2D ClampTo);

	FWidgetTransform GetMapRenderTransform();

	void UpdateIconScale();

	void SetOpenFromWorldMap(float MinZoom, float MaxZoom, float ZoomStep, int32 ViewX, int32 ViewY);
	void ResetCloseFromWorldMap();

	void OpenInitCenterFromPlayerCharacter(FVector PlayerPos);

	/* 미니맵의 InvalidationBox에서 상호작용 되지않도록 처리 */
	void MiniMapIconIgnoreInteraction();

protected:
	void InitMovePoints();

protected:
	UPROPERTY(meta = (BindWidget))
	UOverlay* OverlayPortal;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UOverlay* Overlay_Full;

	UPROPERTY(meta = (BindWidget))
	UOverlay* Overlay_String;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ImageSquare;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	bool bPinching;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TMap<int32, FVector2D> TouchStartInfo;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	bool IsDraggingMap;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float ZoomFactor;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float ZoomFactorAdd;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float PanSpeed;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	int32 CurrentZoomStep;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FGeometry MyGeometry;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FPointerEvent MouseEvent;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FVector2D WheelClampValue;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	int32 MinZoom;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	int32 MaxZoom;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float IconZoomScaleValue;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float PinchZoomLerpValue;
private:
	UPROPERTY()
	TArray<USDKUserWidget*> PortalWidgetList;

	FTimerHandle PortalHandle;

	TMap<EIconGroup, TMap<EIconType, TArray<USDKUserWidget*>>> mapIconWidgetList;

	int32 ViewportX;
	int32 ViewportY;

	float CurrentZoomValue;
	float ZoomStepValue;
	float MinZoomValue;
	float MaxZoomValue;

	FVector2D BeginPinchCenterVec;
	float BeginPinchSize;

	// 포탈 활성화 여부
	bool bEnablePortal;
	FVector2D OpenWorldSize;

	FVector2D ImageSize;
	FVector2D MapRatio;
};
