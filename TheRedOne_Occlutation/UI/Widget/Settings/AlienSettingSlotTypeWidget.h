// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/AlienUserWidget.h"
#include "Common/AlienEnumDefine.h"
#include "AlienSettingSlotTypeWidget.generated.h"

class UWidgetSwitcher;
class UAlienTextBlock;
class UCheckBox;
class UComboBox;
class UImage;
class USlider;
class UButton;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FSettingSliderValueChangedFunc, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FSettingCheckStateChangedFunc, bool);
DECLARE_MULTICAST_DELEGATE(FSettingButtonClickedFunc);


UCLASS()
class ALIEN_API UAlienSettingSlotTypeWidget : public UAlienUserWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;

	/////////////////////////// 바인드 관련
	
	UFUNCTION()
	void OnSliderValueChanged(float InValue);
	UFUNCTION()
	void OnClickedListLeftButton();
	UFUNCTION()
	void OnClickedListRightButton();
	UFUNCTION()
	void OnCheckedOn(bool bIsChecked);
	UFUNCTION()
	void OnCheckedOff(bool bIsChecked);



	void BindValueChagnedFunc(const FSettingSliderValueChangedFunc::FDelegate& ChangedFunc);
	bool UnBindValueChagnedFunc(const FDelegateHandle& DelegateHandle);
	FDelegateHandle BindListLeftClickedFunc(const FSettingButtonClickedFunc::FDelegate& ClickedFunc);
	bool UnBindListLeftClickedFunc(const FDelegateHandle& DelegateHandle);
	FDelegateHandle BindListRightClickedFunc(const FSettingButtonClickedFunc::FDelegate& ClickedFunc);
	bool UnBindListRightClickedFunc(const FDelegateHandle& DelegateHandle);
	FDelegateHandle BindOnClickedFunc(const FSettingCheckStateChangedFunc::FDelegate& ClickedFunc);
	bool UnBindOnClickedFunc(const FDelegateHandle& DelegateHandle);
	FDelegateHandle BindOffClickedFunc(const FSettingCheckStateChangedFunc::FDelegate& ClickedFunc);
	bool UnBindOffClickedFunc(const FDelegateHandle& DelegateHandle);

	///////////////////////////

	void SetSlotType(ESettingSlotType Type);

	void SetSliderMinMaxValue(float InMinValue, float InMaxValue);
	void SetSliderValue(float InValue);
	void SetSliderText(const FString& InStr);
	void SetSliderTableText(const FString& InStr);

	void SetListText(const FString& InStr);
	void SetListTableText(const FString& InStr);

	void SetListButtonIsEnabled(bool bEnabled);

	void SetLeftOnText(const FString& InStr);
	void SetLeftOnTableText(const FString& InStr);

	void SetRightOffText(const FString& InStr);
	void SetRightOffTableText(const FString& InStr);

	void SetOnOffState(bool bOn);
private:
	UPROPERTY(transient)
	TObjectPtr<UWidgetSwitcher> m_TypeSwitcher;

/////// switch 0
	UPROPERTY(transient)
	TObjectPtr<USlider> m_Slider;
	
	UPROPERTY(transient)
	TObjectPtr<UAlienTextBlock> m_SliderValueText;


/////// switch 1
	UPROPERTY(transient)
	TObjectPtr<UButton> m_LeftButton;
	
	UPROPERTY(transient)
	TObjectPtr<UAlienTextBlock> m_ValueTitleText;
	
	UPROPERTY(transient)
	TObjectPtr<UButton> m_RightButton;

/////// switch 2
	UPROPERTY(transient)
	TObjectPtr<UCheckBox> m_LeftCheckBox;

	UPROPERTY(transient)
	TObjectPtr<UAlienTextBlock> m_LeftCheckBoxText;

	UPROPERTY(transient)
	TObjectPtr<UCheckBox> m_RightCheckBox;

	UPROPERTY(transient)
	TObjectPtr<UAlienTextBlock> m_RightCheckBoxText;


	FSettingSliderValueChangedFunc SliderValueChangedFunc;
	FSettingButtonClickedFunc ListLeftButtonClickedFunc;
	FSettingButtonClickedFunc ListRightButtonClickedFunc;
	FSettingCheckStateChangedFunc OnButtonCheckedFunc;
	FSettingCheckStateChangedFunc OffButtonCheckedFunc;

};
