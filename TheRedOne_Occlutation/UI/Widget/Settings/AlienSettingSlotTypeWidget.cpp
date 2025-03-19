// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Settings/AlienSettingSlotTypeWidget.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "UI/Components/Text/AlienTextBlock.h"

void UAlienSettingSlotTypeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UAlienSettingSlotTypeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(m_Slider))
	{
		m_Slider->OnValueChanged.AddDynamic(this, &UAlienSettingSlotTypeWidget::OnSliderValueChanged);

	}
	if (IsValid(m_LeftButton))
	{
		m_LeftButton->OnClicked.AddDynamic(this, &UAlienSettingSlotTypeWidget::OnClickedListLeftButton);

	}
	if (IsValid(m_RightButton))
	{
		m_RightButton->OnClicked.AddDynamic(this, &UAlienSettingSlotTypeWidget::OnClickedListRightButton);

	}
	if (IsValid(m_LeftCheckBox))
	{
		m_LeftCheckBox->OnCheckStateChanged.AddDynamic(this, &UAlienSettingSlotTypeWidget::OnCheckedOn);

	}
	if (IsValid(m_RightCheckBox))
	{
		m_RightCheckBox->OnCheckStateChanged.AddDynamic(this, &UAlienSettingSlotTypeWidget::OnCheckedOff);
	}

}

void UAlienSettingSlotTypeWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UAlienSettingSlotTypeWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
}

void UAlienSettingSlotTypeWidget::OnSliderValueChanged(float InValue)
{
	if (SliderValueChangedFunc.IsBound())
	{
		SliderValueChangedFunc.Broadcast(InValue);
	}

	m_SliderValueText->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), InValue)));
}

void UAlienSettingSlotTypeWidget::OnClickedListLeftButton()
{
	if (ListLeftButtonClickedFunc.IsBound())
	{
		ListLeftButtonClickedFunc.Broadcast();
	}
}

void UAlienSettingSlotTypeWidget::OnClickedListRightButton()
{
	if (ListRightButtonClickedFunc.IsBound())
	{
		ListRightButtonClickedFunc.Broadcast();
	}
}

void UAlienSettingSlotTypeWidget::OnCheckedOn(bool bIsChecked)
{
	if (OnButtonCheckedFunc.IsBound())
	{
		OnButtonCheckedFunc.Broadcast(bIsChecked);
	}

	SetOnOffState(bIsChecked);
}

void UAlienSettingSlotTypeWidget::OnCheckedOff(bool bIsChecked)
{
	if (OffButtonCheckedFunc.IsBound())
	{
		OffButtonCheckedFunc.Broadcast(bIsChecked);
	}

	SetOnOffState(!bIsChecked);
}

void UAlienSettingSlotTypeWidget::BindValueChagnedFunc(const FSettingSliderValueChangedFunc::FDelegate& ChangedFunc)
{
	SliderValueChangedFunc.Add(ChangedFunc);
}


bool UAlienSettingSlotTypeWidget::UnBindValueChagnedFunc(const FDelegateHandle& DelegateHandle)
{
	return ListLeftButtonClickedFunc.Remove(DelegateHandle);
}

FDelegateHandle UAlienSettingSlotTypeWidget::BindListLeftClickedFunc(const FSettingButtonClickedFunc::FDelegate& ClickedFunc)
{
	return ListLeftButtonClickedFunc.Add(ClickedFunc);
}

bool UAlienSettingSlotTypeWidget::UnBindListLeftClickedFunc(const FDelegateHandle& DelegateHandle)
{
	return ListLeftButtonClickedFunc.Remove(DelegateHandle);
}


FDelegateHandle UAlienSettingSlotTypeWidget::BindOffClickedFunc(const FSettingCheckStateChangedFunc::FDelegate& ClickedFunc)
{
	return OffButtonCheckedFunc.Add(ClickedFunc);
}

bool UAlienSettingSlotTypeWidget::UnBindOffClickedFunc(const FDelegateHandle& DelegateHandle)
{
	return OffButtonCheckedFunc.Remove(DelegateHandle);
}

void UAlienSettingSlotTypeWidget::SetSlotType(ESettingSlotType Type)
{
	SetWidgetSwitcherActiveWidgetIndex(m_TypeSwitcher, (int32)Type);
}

void UAlienSettingSlotTypeWidget::SetSliderMinMaxValue(float InMinValue, float InMaxValue)
{
	if (IsValid(m_Slider))
	{
		m_Slider->SetMinValue(InMinValue);
		m_Slider->SetMaxValue(InMaxValue);
	}
}

void UAlienSettingSlotTypeWidget::SetSliderValue(float InValue)
{
	float ClampValue = FMath::Clamp(InValue, m_Slider->GetMinValue(), m_Slider->GetMaxValue());

	if (IsValid(m_Slider))
	{
		m_Slider->SetValue(ClampValue);
	}

	SetTextBlockText(m_SliderValueText, FText::FromString(FString::Printf(TEXT("%.2f"), ClampValue)));

}

void UAlienSettingSlotTypeWidget::SetSliderText(const FString& InStr)
{
	SetTextBlockText(m_SliderValueText, FText::FromString(InStr));
}

void UAlienSettingSlotTypeWidget::SetSliderTableText(const FString& InStr)
{
	SetTextBlockTableText(m_ValueTitleText, InStr);
}

void UAlienSettingSlotTypeWidget::SetListText(const FString& InStr)
{
	SetTextBlockText(m_ValueTitleText, FText::FromString(InStr));
}

void UAlienSettingSlotTypeWidget::SetListTableText(const FString& InStr)
{
	SetTextBlockTableText(m_ValueTitleText, InStr);
}

void UAlienSettingSlotTypeWidget::SetListButtonIsEnabled(bool bEnabled)
{
	SetButtonIsEnabled(m_LeftButton, bEnabled);
	SetButtonIsEnabled(m_RightButton, bEnabled);
}

void UAlienSettingSlotTypeWidget::SetLeftOnText(const FString& InStr)
{
	SetTextBlockText(m_LeftCheckBoxText, FText::FromString(InStr));
}

void UAlienSettingSlotTypeWidget::SetLeftOnTableText(const FString& InStr)
{
	SetTextBlockTableText(m_LeftCheckBoxText, InStr);
}

void UAlienSettingSlotTypeWidget::SetRightOffText(const FString& InStr)
{
	SetTextBlockText(m_RightCheckBoxText, FText::FromString(InStr));
}

void UAlienSettingSlotTypeWidget::SetRightOffTableText(const FString& InStr)
{
	SetTextBlockTableText(m_RightCheckBoxText, InStr);
}

void UAlienSettingSlotTypeWidget::SetOnOffState(bool bOn)
{
	if (IsValid(m_LeftCheckBox) && IsValid(m_RightCheckBox))
	{
		m_LeftCheckBox->SetCheckedState(bOn ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
		m_RightCheckBox->SetCheckedState(!bOn ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}
}

FDelegateHandle UAlienSettingSlotTypeWidget::BindOnClickedFunc(const FSettingCheckStateChangedFunc::FDelegate& ClickedFunc)
{
	return OnButtonCheckedFunc.Add(ClickedFunc);
}

bool UAlienSettingSlotTypeWidget::UnBindOnClickedFunc(const FDelegateHandle& DelegateHandle)
{
	return OnButtonCheckedFunc.Remove(DelegateHandle);
}

FDelegateHandle UAlienSettingSlotTypeWidget::BindListRightClickedFunc(const FSettingButtonClickedFunc::FDelegate& ClickedFunc)
{
	return ListRightButtonClickedFunc.Add(ClickedFunc);
}

bool UAlienSettingSlotTypeWidget::UnBindListRightClickedFunc(const FDelegateHandle& DelegateHandle)
{
	return ListRightButtonClickedFunc.Remove(DelegateHandle);
}
