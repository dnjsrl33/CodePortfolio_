// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AlienUserWidget.h"
#include "AlienSettingSlotWidget.generated.h"

class UAlienTextBlock;
class UAlienSettingSlotTypeWidget;
/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FSettingMouseEnterDelegate, ESettingType);
DECLARE_MULTICAST_DELEGATE(FSettingMouseLeaveDelegate);

UCLASS()
class ALIEN_API UAlienSettingSlotWidget : public UAlienUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;

	void SetTitleTableText(FString TitleText);
	void SetTitleText(FString TitleText);
	void SetSettingType(ESettingType Type);

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;


	FDelegateHandle BindMouseEnterFunc(const FSettingMouseEnterDelegate::FDelegate& EnterFunc);
	bool UnBindMouseEnterFunc(const FDelegateHandle& DelegateHandle);

	FDelegateHandle BindMouseLeaveFunc(const FSettingMouseLeaveDelegate::FDelegate& LeaveFunc);
	bool UnBindMouseLeaveFunc(const FDelegateHandle& DelegateHandle);

	UAlienSettingSlotTypeWidget* GetSlotTypeWidget() const { return m_SlotType; }
private:
	UPROPERTY(Transient)
	TObjectPtr<UAlienTextBlock> m_OptionNameText;
	
	UPROPERTY(Transient)
	TObjectPtr<UAlienSettingSlotTypeWidget> m_SlotType;

	FSettingMouseEnterDelegate MouseEnterDeleagte;
	FSettingMouseLeaveDelegate MouseLeaveDelegate;

	ESettingType SettingType;
};
