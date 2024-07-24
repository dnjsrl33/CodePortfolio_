// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SDKUserWidget.h"
#include "SDKUIOpenWorldMapWidget.generated.h"


class USDKOpenWorldMapMenuWidget;
class USDKOpenWorldMapWidget;
class USDKCheckBoxParam;
class UComboBoxString;
class UVerticalBox;
class USDKMapBookMarkIconWidget;
class USDKWidgetParam;
class UOverlay;
class USlider;
class UButton;
class UWidgetSwitcher;

class ASDKPlayerCharacter;

USTRUCT(BlueprintType)
struct FMapComboBox
{
	GENERATED_USTRUCT_BODY()
public:

	void SetUserWidget(USDKUserWidget* Widget);
	void SetTradeInfo(FString TradeID);
	void UpdateTradeStatus(int32 MaxTime, int32 RemainTime);
private:
	UPROPERTY()
	USDKUserWidget* SDKUserWidget = nullptr;

	UPROPERTY()
	UTextBlock* TextTitle = nullptr;

	UPROPERTY()
	USDKCheckBoxParam* CheckBoxParam = nullptr;

	UPROPERTY()
	UComboBoxString* ComboBox = nullptr;
};


UCLASS()
class ARENA_API USDKUIOpenWorldMapWidget : public USDKUserWidget
{
	GENERATED_BODY()

public:
	USDKUIOpenWorldMapWidget();

	virtual void OpenUIProcess() override;
	virtual void CreateUIProcess() override;
	virtual void CloseUIProcess() override;
	void OpenMenuTeb(const FVector& MovePos, const FString& TableID);

	USDKOpenWorldMapMenuWidget* GetProductionWidget() const { return AdventureMap_Menu; }
	void CheckPlayerCharacter();

	void PlayReverseAnim();

	UFUNCTION()
	void OnClickedTopBarExitButton();

	UFUNCTION()
	void UpdatePlayerCharacterPositionUI();

	void OnUpdateZoomIn(float ZoomValue);

	void SetVisibleTopBarButtonBack(bool bVisible);
	void SetChangeTopBarExitFunc(bool bOrigin);
	void SetEnablePortal(bool bEnable);
protected:
	UFUNCTION()
	void InitWorldMap();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USDKOpenWorldMapWidget* AdventureMap;

	/* 오픈월드의 사이즈 (단위 : cm) **정사각형사이즈 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D OpenWorldSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bClickZoomInButton;

private:
	UPROPERTY(meta = (BindWidget))
	USDKOpenWorldMapMenuWidget* AdventureMap_Menu;

	UPROPERTY(meta = (BindWidget))
	USDKUserWidget* BPPlayerIcon;

	//플레이어 위치 아이콘
	UPROPERTY()
	UImage* CharacterIcon;

	//플레이어 방향 아이콘
	UPROPERTY()
	UImage* CameraViewIcon;

	// NPC 위치 아이콘
	UPROPERTY(meta = (BindWidget))
	UOverlay* Overlay_QuestSlot;
	UPROPERTY()
	TArray<UImage*> NpcIcons;

	UPROPERTY()
	UWidgetAnimation* OpenAnim;

	// 오픈월드 필드 사이즈와 위젯 맵의 축척값
	FVector2D MapScaleRatio;

	UPROPERTY()
	TWeakObjectPtr<ASDKPlayerCharacter> MyPlayerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector2D MapTextureSize;

	FVector2D AdventureMapSize;
	// 맵 중앙이 0,0이 아니라서 갭 저장
	FVector2D WorldZeroGap;
	float MinZoomValue;
	float MaxZoomValue;
	float ZoomStepValue;

	// 초기화 타이머 핸들
	FTimerHandle InitTimerHandle;
	FDelegateHandle OpenWorldMapHandle;
};