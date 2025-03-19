// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Components/Button/AlienButtonBase.h"
#include "AlienSettingCategoryWidget.generated.h"

class UImage;
class UAlienButtonBase;
class UAlienTextBlock;
class UAlienButtonBase;
class UButton;

/**
 * 
 */
UCLASS()
class ALIEN_API UAlienSettingCategoryWidget : public UAlienButtonBase
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;

	void SetDisable();
	void SetTitleTableText(FString InStr);
	void SetImageTexture2D(const FString& TexturePath);
private:
	UPROPERTY(transient)
	TObjectPtr<UButton> m_Button;

	UPROPERTY(transient)
	TObjectPtr<UImage> m_IconImage;

	UPROPERTY(transient)
	TObjectPtr<UAlienTextBlock> m_CategoryNameText;
};
