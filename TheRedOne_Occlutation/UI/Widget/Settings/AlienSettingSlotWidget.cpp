// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Settings/AlienSettingSlotWidget.h"

#include "UI/Components/Text/AlienTextBlock.h"
#include "UI/Widget/Settings/AlienSettingSlotTypeWidget.h"

void UAlienSettingSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UAlienSettingSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAlienSettingSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UAlienSettingSlotWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
}

void UAlienSettingSlotWidget::SetTitleTableText(FString TitleText)
{
	SetTextBlockTableText(m_OptionNameText, TitleText);
}

void UAlienSettingSlotWidget::SetTitleText(FString TitleText)
{
	SetTextBlockText(m_OptionNameText, FText::FromString(TitleText));
}

void UAlienSettingSlotWidget::SetSettingType(ESettingType Type)
{
	SettingType = Type;
}

void UAlienSettingSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	MouseEnterDeleagte.Broadcast(SettingType);
}

void UAlienSettingSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	MouseLeaveDelegate.Broadcast();
}

FDelegateHandle UAlienSettingSlotWidget::BindMouseEnterFunc(const FSettingMouseEnterDelegate::FDelegate& EnterFunc)
{
	return MouseEnterDeleagte.Add(EnterFunc);
}

bool UAlienSettingSlotWidget::UnBindMouseEnterFunc(const FDelegateHandle& DelegateHandle)
{
	return MouseEnterDeleagte.Remove(DelegateHandle);
}

FDelegateHandle UAlienSettingSlotWidget::BindMouseLeaveFunc(const FSettingMouseLeaveDelegate::FDelegate& LeaveFunc)
{
	return MouseLeaveDelegate.Add(LeaveFunc);
}

bool UAlienSettingSlotWidget::UnBindMouseLeaveFunc(const FDelegateHandle& DelegateHandle)
{
	return MouseLeaveDelegate.Remove(DelegateHandle);
}
