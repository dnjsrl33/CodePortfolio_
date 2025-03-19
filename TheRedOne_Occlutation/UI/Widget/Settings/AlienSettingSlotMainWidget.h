// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AlienUserWidget.h"
#include "AlienSettingSlotMainWidget.generated.h"

class UAlienTextBlock;
class UAlienSettingSlotWidget;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class ALIEN_API UAlienSettingSlotMainWidget : public UAlienUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;

	void SetTitleText(FString TitleText);

	void AddSlot(const TObjectPtr<UAlienSettingSlotWidget>& InSlot);
	
private:
	UPROPERTY(transient)
	TObjectPtr<UAlienTextBlock> m_SettingSlotTitleText;

	UPROPERTY(transient)
	TObjectPtr<UVerticalBox> m_SettingSlotBox;
	
	UPROPERTY()
	TArray<TObjectPtr<UAlienSettingSlotWidget>> OptionList;


};
