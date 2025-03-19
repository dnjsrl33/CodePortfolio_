// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Settings/AlienSettingSlotMainWidget.h"
#include "UI/Components/Text/AlienTextBlock.h"
#include "Components/VerticalBox.h"
#include "UI/Widget/Settings/AlienSettingSlotWidget.h"


void UAlienSettingSlotMainWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UAlienSettingSlotMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	m_SettingSlotBox->ClearChildren();
}

void UAlienSettingSlotMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UAlienSettingSlotMainWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
}

void UAlienSettingSlotMainWidget::SetTitleText(FString TitleText)
{
	if (IsValid(m_SettingSlotTitleText))
	{
		m_SettingSlotTitleText->SetStringID(TitleText);
		m_SettingSlotTitleText->Update();
	}
}


void UAlienSettingSlotMainWidget::AddSlot(const TObjectPtr<UAlienSettingSlotWidget>& InSlot)
{
	if (!IsValid(m_SettingSlotBox))
	{
		return;
	}

	OptionList.Add(InSlot);
	m_SettingSlotBox->AddChild(InSlot);
}
