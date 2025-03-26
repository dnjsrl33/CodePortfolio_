// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SDKUserWidget.h"
#include "Share/SDKStructUI.h"
#include "SDKPopupGuideWidget.generated.h"

/**
 * 
 */

 class UTextBlock;
 class UImage;
 class USizeBox;
 class UVerticalBox;
 class UHorizontalBox;
 class UButton;
 class UWidgetSwitcher;
 class UBinkMediaPlayer;
 class UWidgetSwitcher;

 class USDKButtonWidget;

 class USDKGuideMainMenuTapWidget;
 class USDKGuideSubMenuTapWidget;
 class USDKPageIconWidget;


UCLASS()
class ARENA_API USDKPopupGuideWidget : public USDKUserWidget
{
	GENERATED_BODY()
public:
	virtual void CreateUIProcess() override;
	virtual void OpenUIProcess() override;
	virtual void CloseUIProcess() override;

	void SetGuideUI(const FString& Title, const TArray<FName>& Desc, const TArray<TSoftObjectPtr<UTexture2D>>& ImagePath);
	void SetGuideUI(int32 GuideTableID, bool bShowGuideMenu = false);

	UFUNCTION(BlueprintCallable)
	void SetAllGuideMenu();

	UFUNCTION(BlueprintCallable)
	UButton* GetCloseButton();

	UFUNCTION()
	void OnClickedMainMenu(USDKWidgetParam* Param);
	UFUNCTION()
	void OnClickedSubMenu(USDKWidgetParam* Param);

	UFUNCTION()
	void OnClickedClose();

	UFUNCTION()
	void OnClickedPrevGuidePage();

	UFUNCTION()
	void OnClickedNextGuidePage();

	UFUNCTION()
	void OnClickedGuideWarp();

	UFUNCTION()
	void OnKeyInputCloseAllGuide();

	void SetActivePageIcon(int32 Index);

	void SetVisibilityBoxGuideMenu(bool bVisible);

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextTitle;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextSubTitle;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextDesc;

	UPROPERTY(meta = (BindWidget))
	UImage* ImageContent;

	UPROPERTY(meta = (BindWidget))
	USDKUserWidget* CloseButtonWidget;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Page;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonPrevContent;	
	
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonNextContent;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* BoxPageIconList;

	UPROPERTY(meta = (BindWidget))
	USizeBox* BoxGuideMenu;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* BoxGuideMenuList;

	UPROPERTY(meta = (BindWidget))
	USDKButtonWidget* ButtonWarp;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* SwitcherEmpty;

	UPROPERTY()
	UWidgetAnimation* OpenAnim;

	UPROPERTY()
	UBinkMediaPlayer* BinkMediaPlayer;
	
	UPROPERTY()
	TArray<USDKPageIconWidget*> PageIconList;

	UPROPERTY()
	TMap<FString, USDKGuideMainMenuTapWidget*> MainMenuTapList;

	UPROPERTY()
	TMap<FString, USDKGuideSubMenuTapWidget*> SubMenuTapList;


	TMap<FString, TArray<FString>> MapMenu;

	UPROPERTY()
	FBPCancel CloseButton;

	FString CurrentGuideID;
	int32 CurrentGuideIndexMax;
	int32 CurrentGuideIndex;

	FDelegateHandle InputCloseUIHandle;

};
