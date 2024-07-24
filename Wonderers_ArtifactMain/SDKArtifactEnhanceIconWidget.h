// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SDKUserWidget.h"
#include "Share/SDKDataStruct.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "SDKArtifactEnhanceIconWidget.generated.h"

class UImage;
class UBorder;
class UButton;
class UOverlay;
class USizeBox;
class UTextBlock;
class UProgressBar;
class UHorizontalBox;
class UWidgetAnimation;

class USDKWidgetParamArtifactEnhance;

/**
 * 
 */
UCLASS()
class ARENA_API USDKArtifactEnhanceIconWidget : public USDKUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void CreateUIProcess();
	virtual void CloseUIProcess();

	UFUNCTION()
	void OnSelectedArtifactIcon();
	
	/* List View 가상 함수 아이템 추가 시 데이터로 세팅해야 함*/
	virtual void NativeOnListItemObjectSet(UObject* ListItemData) override;

	/* List View 가상 함수, 선택 시 이벤트 */
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

public:
	// 유물 제작 및 강화에서 사용
	void SetArtifactIcon(UObject* ObjectData);
	
	// 유물 관리에서 사용
	void SetArtifactIcon(FArtifactData _ArtifactData);
	void InitArtifacticon();

	FArtifactData GetArtifactData() { return ArtifactData; }
	bool GetbHaveArtifact() {return bHaveArtifact; }
	
	void SetSelectItemInfo(bool bSelect);
	void SetOverlaySelect(bool bVisible);

	void BindImages();

	void SetEmptyIcon();

	void SetVisibilityInfoType(bool bShowDeck);

	void SetVisibilityEquipDeck(bool bVisible);


private:
	UPROPERTY(meta =(BindWidget))
	UWidgetSwitcher* WidgetSwitcher_Enabled;

	UPROPERTY(meta = (BindWidget))
	UOverlay* OverlaySelect;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Artifact;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Artifact;

	UPROPERTY(meta = (BindWidget))
	UImage* StatusUpgrade;

	UPROPERTY(meta = (BindWidget))
	UImage* StatusNew;

	UPROPERTY(meta = (BindWidget))
	UBorder* BorderGrade;

	UPROPERTY(meta = (BindWidget))
	USDKUserWidget* EquipDeck;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ArtifactTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextEnhance;

	UPROPERTY(meta = (BindWidget))
	UImage* SetIconImage;

	UPROPERTY(meta = (BindWidget))
	UOverlay* ArtifactStackOverlay;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ArtifactStackPrograss;

	UPROPERTY(meta = (BindWidget))
	USizeBox* RateUpIcon;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher_InfoOrDeck;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextArtifactCost;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher_PrograssText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextStackPercent;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextStackPercent_Full;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextStackPercent_Max;

	UPROPERTY()
	UWidgetAnimation* RateUp;
private:

	UPROPERTY()
	USDKWidgetParamArtifactEnhance* ArtifactParam;
	FArtifactData ArtifactData;
	bool bHaveArtifact;
};
