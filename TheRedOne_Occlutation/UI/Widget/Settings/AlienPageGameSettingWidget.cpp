// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Settings/AlienPageGameSettingWidget.h"

#include "UI/Widget/Settings/AlienSettingCategoryWidget.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Widget/Lobby/AlienLobbyTabMenuButton.h"

#include "Settings/AlienGameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Manager/AlienUIManager.h"
#include "Manager/AlienTableManager.h"
#include "Core/GameInstance/AlienGameInstanceBase.h"
#include "Core/AssetManager/AlienAssetManager.h"
#include "UI/Widget/Settings/AlienSettingSlotMainWidget.h"
#include "UI/Widget/Settings/AlienSettingSlotWidget.h"
#include "UI/Widget/Settings/AlienSettingSlotTypeWidget.h"

#include "Components/VerticalBox.h"

#include "UI/Components/Text/AlienRichTextBlock.h"
#include "UI/Components/Text/AlienTextBlock.h"
#include "Common/AlienGamePlayStatics.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "UI/Widget/AlienLobbyBottomMenuWidget.h"
#include "UI/Widget/Lobby/AlienLobbyBottomMenuButton.h"
#include "Core/PlayerController/AlienOnlinePlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Core/GameMode/AlienOnlineGameMode.h"
#include "Core/GameMode/AlienOfflineGameMode.h"
#include "Common/AlienEnumDefine.h"
#include "Core/PlayerController/AlienOfflinePlayerController.h"
#include "../Private/EOSVoiceSubsystem.h"

UAlienPageGameSettingWidget::UAlienPageGameSettingWidget()
	:ResolutionListIndex(0)
{

}

void UAlienPageGameSettingWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	TitleTextStr = TEXT("UI_Setting_Title");
	GameplayButtonStr = TEXT("UI_Setting_GamePlay");
	VideoButtonStr = TEXT("");
	AudioButtonStr = TEXT("UI_Setting_Sound_Audio");
	InputButtonStr = TEXT("");
	GameplaySettingCategoryStr = TEXT("UI_Setting_GamePlay");
	VideoCategoryStr = TEXT("UI_Setting_Screen");
	AudioCategoryStr = TEXT("UI_Setting_Sound_Audio");
	MouseCategoryStr = TEXT("");

	ApplySettingButtonStr = TEXT("UI_Setting_Apply");
	RevertSettingButtonStr = TEXT("UI_Setting_Revert");
	ResetSettingButtonStr = TEXT("UI_Setting_Reset");
}

void UAlienPageGameSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	AAlienOnlinePlayerController* PlayerController = Cast<AAlienOnlinePlayerController>(GetWorld()->GetFirstPlayerController());

	VoiceSubSystem = UEOSVoiceSubsystem::Get(this);
	if (IsValid(PlayerController))
	{
		PlayerController->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, this, EMouseLockMode::DoNotLock);
		
	}
	else
	{
		AAlienOfflinePlayerController* OfflineController = Cast<AAlienOfflinePlayerController>(GetWorld()->GetFirstPlayerController());

		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(OfflineController, this, EMouseLockMode::DoNotLock);
	}
	SetKeyboardFocus();
	SetTextBlockTableText(m_TitleText, TitleTextStr);

	if (IsValid(m_GameplaySettingCategory))
	{
		m_GameplaySettingCategory->SetTitleTableText(GameplaySettingCategoryStr);
		m_GameplaySettingCategory->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedGamePlaySetting));
	}
	if (IsValid(m_GameplayButton))
	{
		m_VideoButton->SetSelectSceneIndex(1);
		m_GameplayButton->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedGamePlaySetting));
	}
	if (IsValid(m_VideoCategory))
	{
		//m_VideoCategory->SetDisable();
		m_VideoCategory->SetTitleTableText(VideoCategoryStr);
		m_VideoCategory->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedVideoSetting));
	}
	if (IsValid(m_VideoButton))
	{
		m_VideoButton->SetSelectSceneIndex(1);
		m_VideoButton->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedVideoSetting));
	}
	if (IsValid(m_AudioCategory))
	{
		m_AudioCategory->SetTitleTableText(AudioCategoryStr);
		//m_AudioCategory->SetDisable();
		m_AudioCategory->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedAudioSetting));

	}
	if (IsValid(m_AudioButton))
	{
		m_AudioButton->SetSelectSceneIndex(1);
		m_AudioButton->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedAudioSetting));

	}
	if (IsValid(m_MouseCategory))
	{
		m_MouseCategory->SetTitleTableText(TEXT(""));
		SetWidgetVisibility(m_MouseCategory, ESlateVisibility::Collapsed);
		m_MouseCategory->SetDisable();
		m_MouseCategory->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedMouseAndKeyboardSetting));
	}

	if (IsValid(m_InputButton))
	{
		m_InputButton->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedMouseAndKeyboardSetting));
	}

	BackButton = m_BottomMenu->GetFirstButton();
	if (IsValid(BackButton))
	{
		BackButton->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedBackButton));
	}

	ApplySettingButton = m_BottomMenu->GetSecondButton();
	if (IsValid(ApplySettingButton))
	{
		SetWidgetVisibility(ApplySettingButton, ESlateVisibility::Collapsed);
		ApplySettingButton->SetTableText(ApplySettingButtonStr);
		ApplySettingButton->SetIsEnabled(false);
		ApplySettingButton->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedApplySettingButton));
	}

	ResetSettingButton = m_BottomMenu->GetThirdButton();
	if (IsValid(ResetSettingButton))
	{
		SetWidgetVisibility(ResetSettingButton, ESlateVisibility::Collapsed);
		ResetSettingButton->SetIsEnabled(false);
		ResetSettingButton->SetTableText(ResetSettingButtonStr);
		ResetSettingButton->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedResetSettingButton));
	}

	RevertSettingButton = m_BottomMenu->GetFourthButton();
	if (IsValid(RevertSettingButton))
	{
		SetWidgetVisibility(RevertSettingButton, ESlateVisibility::Collapsed);
		RevertSettingButton->SetTableText(RevertSettingButtonStr);
		RevertSettingButton->SetIsEnabled(false);
		RevertSettingButton->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRevertSettingButton));
	}

	if (IsValid(m_BottomMenu))
	{
		EscMenuButton = m_BottomMenu->GetFifthButton();
		if (IsValid(EscMenuButton))
		{
			EscMenuButton->SetVisibility(ESlateVisibility::Collapsed);
			EscMenuButton->BindClickedFunc(FButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedEscKeyButton));
		}
	}

	if (IsValid(m_SlotBox))
	{
		m_SlotBox->ClearChildren();
	}
	

	OnCompletedDeviceSwap.BindUFunction(this, TEXT("SwapAudioOutputResult"));
	LoadSettings();
	InitGameplaySettings();
	InitVideoSettings();
	InitAudioSettings();
}

void UAlienPageGameSettingWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UAlienPageGameSettingWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
}

FReply UAlienPageGameSettingWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Escape)
	{
		OnClickedBackButton();

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void UAlienPageGameSettingWidget::Open(class UPageUIInfoBase* PageUIInfo)
{
	Super::Open(PageUIInfo);
}

void UAlienPageGameSettingWidget::Close()
{
	Super::Close();

	AAlienOnlinePlayerController* PlayerController = Cast<AAlienOnlinePlayerController>(GetWorld()->GetFirstPlayerController());

	if (IsValid(PlayerController))
	{
		PlayerController->SetShowMouseCursor(false);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
	}
	else
	{
		AAlienOfflinePlayerController* OfflineController = Cast<AAlienOfflinePlayerController>(GetWorld()->GetFirstPlayerController());

		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(OfflineController, this, EMouseLockMode::DoNotLock);

		if (UPageUILobbyInfo* PageUILobbyInfo = NewObject<UPageUILobbyInfo>())
		{
			UAlienGamePlayStatics::OpenPageUI(this, PageUILobbyInfo);
		}
	}

	if (IsOptionChange())
	{
		RevertSettings();
	}
}
void UAlienPageGameSettingWidget::CreateSlotMainWidget(const FString& Title, TObjectPtr<UAlienSettingSlotMainWidget>& OutWidget)
{
	// create BaseWiget
	const FAlienTableRow_UI* const UITable = UAlienGameInstanceBase::GetManager<UAlienTableManager>()->FindUI("UAlienSettingSlotMainWidget");
	UClass* const LoadClass = (UITable) ? UAlienAssetManager::Get().LoadResource<UClass>(UITable->WidgetPath.ToSoftObjectPath()) : nullptr;

	if (!LoadClass)
	{
		ALIEN_LOG(Warning, TEXT("UIManager::CreateWidget - Null Fail: UAlienSettingSlotMainWidget"));
		return;
	}

	/// 마우스 감도 위젯 설정
	UAlienSettingSlotMainWidget* MainSlotWidget = CreateWidget<UAlienSettingSlotMainWidget>(GetWorld(), LoadClass);
	if (!IsValid(MainSlotWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::CreateSlotMainWidget MainSlotWidget"));
		return;
	}
	OutWidget = MainSlotWidget;

	m_SlotBox->AddChild(OutWidget);
	MainSlotWidget->SetTitleText(Title);
}

void UAlienPageGameSettingWidget::CreateSlotSliderOptionWidget(const FString& TitleText, const FString& SubText, ESettingType Type, float InMin, float InMax, float InInitValue, const TObjectPtr<UAlienSettingSlotMainWidget>& ParentWidget, TObjectPtr<UAlienSettingSlotWidget>& OutWidget, const FSettingSliderValueChangedFunc::FDelegate& EnterSliderFunc)
{
	const FAlienTableRow_UI* const UITableSlot = UAlienGameInstanceBase::GetManager<UAlienTableManager>()->FindUI(TEXT("UAlienSettingSlotWidget"));
	UClass* const LoadClassSlot = (UITableSlot) ? UAlienAssetManager::Get().LoadResource<UClass>(UITableSlot->WidgetPath.ToSoftObjectPath()) : nullptr;
	if (!LoadClassSlot)
	{
		ALIEN_LOG(Warning, TEXT("UIManager::CreateWidget - Null Fail: CreateSlotOptionWidget"));
		return;
	}

	if (!IsValid(ParentWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::CreateSlotSliderOptionWidget ParentWidget"));
		return;
	}

	/// 옵션 위젯 추가 및 설정
	UAlienSettingSlotWidget* SlotWidget = CreateWidget<UAlienSettingSlotWidget>(GetWorld(), LoadClassSlot);
	if (!IsValid(SlotWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::CreateSlotSliderOptionWidget SlotWidget"));
		return;
	}

	OutWidget = SlotWidget;
	
	ParentWidget->AddSlot(OutWidget);

	SlotWidget->SetTitleTableText(TitleText);
	SlotWidget->SetSettingType(Type);
	SlotWidget->BindMouseEnterFunc(FSettingMouseEnterDelegate::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnMouseEnterDescFunc));
	SlotWidget->BindMouseLeaveFunc(FSettingMouseLeaveDelegate::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnMouseLeaveDescFunc));

	UAlienSettingSlotTypeWidget* SlotTypeWidget = SlotWidget->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::CreateSlotSliderOptionWidget SlotTypeWidget"));
		return;
	}

	SlotTypeWidget->SetSliderTableText(SubText);
	SlotTypeWidget->SetSlotType(ESettingSlotType::Slider);
	SlotTypeWidget->SetSliderMinMaxValue(InMin, InMax);
	SlotTypeWidget->SetSliderValue(InInitValue);
	SlotTypeWidget->BindValueChagnedFunc(EnterSliderFunc);

}

void UAlienPageGameSettingWidget::CreateSlotListOptionWidget(const FString& TitleText, const FString& SubText, ESettingType Type, const TObjectPtr<UAlienSettingSlotMainWidget>& ParentWidget, TObjectPtr<UAlienSettingSlotWidget>& OutWidget, const FSettingButtonClickedFunc::FDelegate& LeftButtonFunc, const FSettingButtonClickedFunc::FDelegate& RightButtonFunc)
{
	const FAlienTableRow_UI* const UITableSlot = UAlienGameInstanceBase::GetManager<UAlienTableManager>()->FindUI(TEXT("UAlienSettingSlotWidget"));
	UClass* const LoadClassSlot = (UITableSlot) ? UAlienAssetManager::Get().LoadResource<UClass>(UITableSlot->WidgetPath.ToSoftObjectPath()) : nullptr;
	if (!LoadClassSlot)
	{
		ALIEN_LOG(Warning, TEXT("UIManager::CreateWidget - Null Fail: CreateSlotListOptionWidget"));
		return;
	}

	if (!IsValid(ParentWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::CreateSlotListOptionWidget ParentWidget"));
		return;
	}

	/// 옵션 위젯 추가 및 설정
	UAlienSettingSlotWidget* SlotWidget = CreateWidget<UAlienSettingSlotWidget>(GetWorld(), LoadClassSlot);
	if (!IsValid(SlotWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::CreateSlotListOptionWidget SlotWidget"));
		return;
	}

	OutWidget = SlotWidget;

	ParentWidget->AddSlot(OutWidget);

	FString InTitleTextStr = UAlienGamePlayStatics::GetTableString(this, TitleText);
	if (InTitleTextStr.Contains(TEXT("StringID")))
	{
		SlotWidget->SetTitleText(TitleText);
	}
	else
	{
		SlotWidget->SetTitleText(InTitleTextStr);
	}

	SlotWidget->SetSettingType(Type);
	SlotWidget->BindMouseEnterFunc(FSettingMouseEnterDelegate::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnMouseEnterDescFunc));
	SlotWidget->BindMouseLeaveFunc(FSettingMouseLeaveDelegate::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnMouseLeaveDescFunc));

	UAlienSettingSlotTypeWidget* SlotTypeWidget = SlotWidget->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::CreateSlotListOptionWidget SlotTypeWidget"));
		return;
	}

	FString SubTextStr = UAlienGamePlayStatics::GetTableString(this, SubText);
	if (SubTextStr.Contains(TEXT("StringID")))
	{
		SlotTypeWidget->SetListText(SubText);
	}
	else
	{
		SlotTypeWidget->SetListText(SubTextStr);
	}

	SlotTypeWidget->SetSlotType(ESettingSlotType::List);
	SlotTypeWidget->BindListLeftClickedFunc(LeftButtonFunc);
	SlotTypeWidget->BindListRightClickedFunc(RightButtonFunc);

}

void UAlienPageGameSettingWidget::CreateSlotToggleOptionWidget(const FString& TitleText, const FString& SubOnText, const FString& SubOffText, ESettingType Type, bool InbValue, const TObjectPtr<UAlienSettingSlotMainWidget>& ParentWidget, TObjectPtr<UAlienSettingSlotWidget>& OutWidget, const FSettingCheckStateChangedFunc::FDelegate& ClickedOnFunc, const FSettingCheckStateChangedFunc::FDelegate& ClickedOffFunc)
{
	const FAlienTableRow_UI* const UITableSlot = UAlienGameInstanceBase::GetManager<UAlienTableManager>()->FindUI(TEXT("UAlienSettingSlotWidget"));
	UClass* const LoadClassSlot = (UITableSlot) ? UAlienAssetManager::Get().LoadResource<UClass>(UITableSlot->WidgetPath.ToSoftObjectPath()) : nullptr;
	if (!LoadClassSlot)
	{
		ALIEN_LOG(Warning, TEXT("UIManager::CreateWidget - Null Fail: CreateSlotListOptionWidget"));
		return;
	}

	if (!IsValid(ParentWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::CreateSlotListOptionWidget ParentWidget"));
		return;
	}

	/// 옵션 위젯 추가 및 설정
	UAlienSettingSlotWidget* SlotWidget = CreateWidget<UAlienSettingSlotWidget>(GetWorld(), LoadClassSlot);
	if (!IsValid(SlotWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::CreateSlotListOptionWidget SlotWidget"));
		return;
	}

	OutWidget = SlotWidget;

	ParentWidget->AddSlot(OutWidget);

	FString InTitleTextStr = UAlienGamePlayStatics::GetTableString(this, TitleText);
	if (InTitleTextStr.Contains(TEXT("StringID")))
	{
		SlotWidget->SetTitleText(TitleText);
	}
	else
	{
		SlotWidget->SetTitleText(InTitleTextStr);
	}

	SlotWidget->SetSettingType(Type);
	SlotWidget->BindMouseEnterFunc(FSettingMouseEnterDelegate::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnMouseEnterDescFunc));
	SlotWidget->BindMouseLeaveFunc(FSettingMouseLeaveDelegate::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnMouseLeaveDescFunc));

	UAlienSettingSlotTypeWidget* SlotTypeWidget = SlotWidget->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::CreateSlotListOptionWidget SlotTypeWidget"));
		return;
	}

	FString SubTextStr = UAlienGamePlayStatics::GetTableString(this, SubOnText);
	if (SubTextStr.Contains(TEXT("StringID")))
	{
		SlotTypeWidget->SetLeftOnText(SubOnText);
	}
	else
	{
		SlotTypeWidget->SetLeftOnText(SubTextStr);
	}

	SubTextStr = UAlienGamePlayStatics::GetTableString(this, SubOffText);

	if (SubTextStr.Contains(TEXT("StringID")))
	{
		SlotTypeWidget->SetRightOffText(SubOffText);
	}
	else
	{
		SlotTypeWidget->SetRightOffText(SubTextStr);
	}

	SlotTypeWidget->SetSlotType(ESettingSlotType::OnOff);
	SlotTypeWidget->SetOnOffState(InbValue);
	SlotTypeWidget->BindOnClickedFunc(ClickedOnFunc);
	SlotTypeWidget->BindOffClickedFunc(ClickedOffFunc);
	
}

void UAlienPageGameSettingWidget::LoadSettings()
{
	UAlienGameUserSettings* Settings = UAlienGameUserSettings::Get();

	if (IsValid(Settings))
	{
		// 마우스 감도
		OldSettingValue.MouseSensitive = Settings->GetMouseSensitive();

			// 마우스 감도
		OldSettingValue.MouseZoomSensitive = Settings->GetMouseZoomSensitive();

		// 윈도우 모드
		OldSettingValue.WindowMode = Settings->GetFullscreenMode();

		// 해상도
		OldSettingValue.Resolution = Settings->GetScreenResolution();
		SetResolutionList((EWindowMode::Type)OldSettingValue.WindowMode);
		
		// 해상도 스케일
		OldSettingValue.ResolutionScale = Settings->GetResolutionScaleNormalized();

		// 수직동기화
		OldSettingValue.bVSync = Settings->IsVSyncEnabled();
		
		// 프레임 레이트
		OldSettingValue.FrameRate = Settings->GetFrameRateLimit();

		// 감마
		OldSettingValue.Gamma = Settings->GetGamma();

		// 스칼라빌리티
		OldSettingValue.Scalability = Settings->GetOverallScalabilityLevel();

		// 시야 품질
		OldSettingValue.ViewDistanceQuality = Settings->GetViewDistanceQuality();
		
		// 그림자 품질
		OldSettingValue.ShadowQuality = Settings->GetShadowQuality();
		
		// 조명 품질
		OldSettingValue.GlobalIlluminationQuality = Settings->GetGlobalIlluminationQuality();

		// 반사 품질
		OldSettingValue.ReflectionQuality = Settings->GetReflectionQuality();

		// AntiAliasing 품질
		OldSettingValue.AntiAliasingQuality = Settings->GetAntiAliasingQuality();

		// 텍스쳐 품질
		OldSettingValue.TextureQuality = Settings->GetTextureQuality();

		// 효과 품질
		OldSettingValue.VisualEffectQuality = Settings->GetVisualEffectQuality();

		// 후처리 품질
		OldSettingValue.PostProcessingQuality = Settings->GetPostProcessingQuality();

		// 식생 품질
		OldSettingValue.FoliageQuality = Settings->GetFoliageQuality();

		// 셰이딩 품질
		OldSettingValue.ShadingQuality = Settings->GetShadingQuality();


		// 언어
		// CBT에서만 영어로 표현되도록 해야 함.
		OldSettingValue.Language = ELanguageType::Kr;//(ELanguageType)Settings->GetLanguage();
		LocalizedCulturesList = UKismetInternationalizationLibrary::GetLocalizedCultures(ELocalizationLoadFlags::Game);

		if (LocalizedCulturesList.Contains(TEXT("ko")))
		{
			LocalizedCulturesList.Remove(TEXT("ko"));
		}

		OldSettingValue.AudioQuality = Settings->GetAudioQualityLevel();
		

		// 오디오 장치 정보
		if (VoiceSubSystem->IsValidLowLevel())
		{
			InputDeviceInfos = VoiceSubSystem->GetAvailableInputDeviceInfos();
			OutputDeviceInfos = VoiceSubSystem->GetAvailableOutputDeviceInfos();

			auto InputDeviceInfo = VoiceSubSystem->GetInputDeviceInfo();
			for (int32 i = 0; i < InputDeviceInfos.Num(); i++)
			{
				if (InputDeviceInfos[i].Id == InputDeviceInfo.Id)
				{
					ChangedSettingValue.InputDeviceIndex = OldSettingValue.InputDeviceIndex = i;
					break;
				}
			}

			auto OutputDeviceInfo = VoiceSubSystem->GetOutputDeviceInfo();
			for (int32 i = 0; i < OutputDeviceInfos.Num(); i++)
			{
				if (OutputDeviceInfos[i].Id == OutputDeviceInfo.Id)
				{
					ChangedSettingValue.OutputDeviceIndex = OldSettingValue.OutputDeviceIndex = i;
					break;
				}
			}
		}

		// 볼륨
		OldSettingValue.MasterVolumes = Settings->GetMasterVolume();
		OldSettingValue.BgmVolumes = Settings->GetBGMVolume();
		OldSettingValue.SfxVolumes = Settings->GetSFXVolume();
		OldSettingValue.VoiceVolumes = Settings->GetVoiceVolume();
		OldSettingValue.MicVolumes = Settings->GetMicVolume();
		OldSettingValue.UiVolumes = Settings->GetUIVolume();

		OldSettingValue.bAchievementAlert = Settings->GetAchievementAlert();

		LastSettingValue = ChangedSettingValue = OldSettingValue;

	}
	
}

void UAlienPageGameSettingWidget::ApplySettings()
{
	UAlienGameUserSettings* Settings = UAlienGameUserSettings::Get();

	bool ResolutionChanged = false;

	if (OldSettingValue.WindowMode != ChangedSettingValue.WindowMode)
	{
		Settings->SetFullscreenMode((EWindowMode::Type)ChangedSettingValue.WindowMode);
		ResolutionChanged = true;
	}

	if (OldSettingValue.Resolution != ChangedSettingValue.Resolution)
	{
		Settings->SetScreenResolution(ChangedSettingValue.Resolution);
		LastSettingValue = OldSettingValue;
		ResolutionChanged = true;
	}

	if (OldSettingValue.ResolutionScale != ChangedSettingValue.ResolutionScale)
	{
		Settings->SetResolutionScaleNormalized(ChangedSettingValue.ResolutionScale);
	}

	if (OldSettingValue.bVSync != ChangedSettingValue.bVSync)
	{
		Settings->SetVSyncEnabled(ChangedSettingValue.bVSync);
	}

	if (OldSettingValue.FrameRate != ChangedSettingValue.FrameRate)
	{
		Settings->SetFrameRateLimit(ChangedSettingValue.FrameRate);
	}

	if (OldSettingValue.Gamma != ChangedSettingValue.Gamma)
	{
		Settings->SetGamma(ChangedSettingValue.Gamma);
	}

	if (OldSettingValue.OutputDeviceIndex != ChangedSettingValue.OutputDeviceIndex)
	{
		if (OutputDeviceInfos.IsValidIndex(ChangedSettingValue.OutputDeviceIndex))
		{
			UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(this, OutputDeviceInfos[ChangedSettingValue.OutputDeviceIndex].Id, OnCompletedDeviceSwap);
			VoiceSubSystem->SetOutputDeviceId(OutputDeviceInfos[ChangedSettingValue.OutputDeviceIndex].Id);
		}
	}

	if (OldSettingValue.InputDeviceIndex != ChangedSettingValue.InputDeviceIndex)
	{
		if (InputDeviceInfos.IsValidIndex(ChangedSettingValue.InputDeviceIndex))
		{
			VoiceSubSystem->SetInputDeviceId(InputDeviceInfos[ChangedSettingValue.InputDeviceIndex].Id);
		}
	}

	if (OldSettingValue.AudioQuality != ChangedSettingValue.AudioQuality)
	{
		Settings->SetAudioQualityLevel(ChangedSettingValue.AudioQuality);
	}

	if (OldSettingValue.MasterVolumes != ChangedSettingValue.MasterVolumes)
	{
		Settings->SetMasterVolume(ChangedSettingValue.MasterVolumes);

		UAlienGamePlayStatics::SetMasterVolumes(this, ChangedSettingValue.MasterVolumes);
	}

	if (OldSettingValue.BgmVolumes != ChangedSettingValue.BgmVolumes)
	{
		Settings->SetBGMVolume(ChangedSettingValue.BgmVolumes);

		UAlienGamePlayStatics::SetBGMVolumes(this, ChangedSettingValue.BgmVolumes);
	}

	if (OldSettingValue.SfxVolumes != ChangedSettingValue.SfxVolumes)
	{
		Settings->SetSFXVolume(ChangedSettingValue.SfxVolumes);

		UAlienGamePlayStatics::SetSFXVolumes(this, ChangedSettingValue.SfxVolumes);
	}

	if (OldSettingValue.UiVolumes != ChangedSettingValue.UiVolumes)
	{
		Settings->SetUIVolume(ChangedSettingValue.UiVolumes);

		UAlienGamePlayStatics::SetUIVolumes(this, ChangedSettingValue.UiVolumes);
	}

	if (OldSettingValue.VoiceVolumes != ChangedSettingValue.VoiceVolumes)
	{
		
		Settings->SetVoiceVolume(ChangedSettingValue.VoiceVolumes);

		VoiceSubSystem->SetAudioOutputVolume(ChangedSettingValue.VoiceVolumes);
	}

	if (OldSettingValue.MicVolumes != ChangedSettingValue.MicVolumes)
	{

		Settings->SetMicVolume(ChangedSettingValue.MicVolumes);

		VoiceSubSystem->SetAudioInputVolume(ChangedSettingValue.MicVolumes);
	}

	if (OldSettingValue.MouseSensitive != ChangedSettingValue.MouseSensitive)
	{
		Settings->SetMouseSensitive(ChangedSettingValue.MouseSensitive);
	}

		if (OldSettingValue.MouseZoomSensitive != ChangedSettingValue.MouseZoomSensitive)
	{
		Settings->SetMouseZoomSensitive(ChangedSettingValue.MouseZoomSensitive);
	}

	if (OldSettingValue.Scalability != ChangedSettingValue.Scalability)
	{
		Settings->SetOverallScalabilityLevel(ChangedSettingValue.Scalability);
	}

	if (OldSettingValue.ViewDistanceQuality != ChangedSettingValue.ViewDistanceQuality)
	{
		Settings->SetViewDistanceQuality(ChangedSettingValue.ViewDistanceQuality);
	}

	if (OldSettingValue.ShadowQuality != ChangedSettingValue.ShadowQuality)
	{
		Settings->SetShadowQuality(ChangedSettingValue.ShadowQuality);
	}

	if (OldSettingValue.GlobalIlluminationQuality != ChangedSettingValue.GlobalIlluminationQuality)
	{
		Settings->SetGlobalIlluminationQuality(ChangedSettingValue.GlobalIlluminationQuality);
	}

	if (OldSettingValue.ReflectionQuality != ChangedSettingValue.ReflectionQuality)
	{
		Settings->SetReflectionQuality(ChangedSettingValue.ReflectionQuality);
	}

	if (OldSettingValue.AntiAliasingQuality != ChangedSettingValue.AntiAliasingQuality)
	{
		Settings->SetAntiAliasingQuality(ChangedSettingValue.AntiAliasingQuality);
	}

	if (OldSettingValue.TextureQuality != ChangedSettingValue.TextureQuality)
	{
		Settings->SetTextureQuality(ChangedSettingValue.TextureQuality);
	}

	if (OldSettingValue.VisualEffectQuality != ChangedSettingValue.VisualEffectQuality)
	{
		Settings->SetVisualEffectQuality(ChangedSettingValue.VisualEffectQuality);
	}

	if (OldSettingValue.PostProcessingQuality != ChangedSettingValue.PostProcessingQuality)
	{
		Settings->SetPostProcessingQuality(ChangedSettingValue.PostProcessingQuality);
	}
	if (OldSettingValue.FoliageQuality != ChangedSettingValue.FoliageQuality)
	{
		Settings->SetFoliageQuality(ChangedSettingValue.FoliageQuality);
	}

	if (OldSettingValue.ShadingQuality != ChangedSettingValue.ShadingQuality)
	{
		Settings->SetShadingQuality(ChangedSettingValue.ShadingQuality);
	}

	if (OldSettingValue.Language != ChangedSettingValue.Language)
	{
		Settings->SetLanguage((int32)ChangedSettingValue.Language);
		UKismetInternationalizationLibrary::SetCurrentCulture(LocalizedCulturesList[(int32)ChangedSettingValue.Language]);
	}

	if (OldSettingValue.bAchievementAlert != ChangedSettingValue.bAchievementAlert)
	{
		Settings->SetAchievementAlert(ChangedSettingValue.bAchievementAlert);
	}
	

	Settings->ApplySettings(false);
	OldSettingValue = ChangedSettingValue;


	if (ResolutionChanged)
	{
		if (UPopupUIOKCancelTimerInfo* PopupUIOkCancelTimerInfo = NewObject<UPopupUIOKCancelTimerInfo>())
		{
			PopupUIOkCancelTimerInfo->Title_Str = UAlienGamePlayStatics::GetTableString(this, TEXT("UI_Setting_Screen_Basic_ScreenResolution"));
			PopupUIOkCancelTimerInfo->Desc_Str = UAlienGamePlayStatics::GetTableString(this, TEXT("UI_Setting_Message_ScreenResolution"));
			PopupUIOkCancelTimerInfo->TimerDesc_Str = UAlienGamePlayStatics::GetTableString(this, TEXT("UI_Setting_Message_ScreenResolution_Timer"));
			PopupUIOkCancelTimerInfo->OK_Str = UAlienGamePlayStatics::GetTableString(this, TEXT("UI_Setting_Button_Bool_1"));
			PopupUIOkCancelTimerInfo->Cancel_Str = UAlienGamePlayStatics::GetTableString(this, TEXT("UI_Setting_Button_Bool_2"));
			PopupUIOkCancelTimerInfo->TimerValue = 15.0f;
			PopupUIOkCancelTimerInfo->Delegate_OK.BindUFunction(this, TEXT("OnPopupResolutionConfirmOK"));
			PopupUIOkCancelTimerInfo->Delegate_Cancel.BindUFunction(this, TEXT("OnPopupResolutionRevertOK"));
			UAlienGamePlayStatics::OpenPopupUI(this, PopupUIOkCancelTimerInfo);
		}
	}

	OptionChangeTracking();

	if (bCloseReady)
	{
		OnClickedBackButton();
	}
}

void UAlienPageGameSettingWidget::RevertSettings()
{

	if (OldSettingValue.WindowMode != ChangedSettingValue.WindowMode)
	{
		UAlienSettingSlotTypeWidget* SlotType = ModeSlotOption->GetSlotTypeWidget();

		if (IsValid(SlotType))
		{
			SlotType->SetListTableText(FString::Printf(TEXT("UI_Setting_Screen_Basic_ScrrenMode_%d"), OldSettingValue.WindowMode + 1));
		}
	}

	if (OldSettingValue.Resolution != ChangedSettingValue.Resolution)
	{
		UAlienSettingSlotTypeWidget* ResolutionSlotTypeWidget = ResolutionSlotOption->GetSlotTypeWidget();

		if (!IsValid(ResolutionSlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings ResolutionSlotTypeWidget"));
			return;
		}
		ResolutionList.Find(OldSettingValue.Resolution, ResolutionListIndex);
		FString ResolutionStr = ResolutionList[ResolutionListIndex].ToString();
		ResolutionSlotTypeWidget->SetListText(ResolutionStr);
	}

	if (OldSettingValue.ResolutionScale != ChangedSettingValue.ResolutionScale)
	{
		UAlienSettingSlotTypeWidget* SlotTypeWidget = ResolutionScaleSlotOption->GetSlotTypeWidget();

		if (!IsValid(SlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings ResolutionScaleSlotOption"));
			return;
		}

		SlotTypeWidget->SetSliderValue(OldSettingValue.ResolutionScale);
	}

	if (OldSettingValue.Gamma != ChangedSettingValue.Gamma)
	{
		UAlienSettingSlotTypeWidget* GammaSlotTypeWidget = GammaSlotOption->GetSlotTypeWidget();

		if (!IsValid(GammaSlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings GammaSlotTypeWidget"));
			return;
		}

		GammaSlotTypeWidget->SetSliderValue(OldSettingValue.Gamma);
	}

	if (OldSettingValue.FrameRate != ChangedSettingValue.FrameRate)
	{
		UAlienSettingSlotTypeWidget* FrameRateTypeWidget = FrameRateSlotOption->GetSlotTypeWidget();

		if (!IsValid(FrameRateTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings GammaSlotTypeWidget"));
			return;
		}

		FrameRateTypeWidget->SetSliderValue(OldSettingValue.FrameRate);
	}

	if (OldSettingValue.bVSync != ChangedSettingValue.bVSync)
	{
		UAlienSettingSlotTypeWidget* VSyncTypeWidget = VSyncSlotOption->GetSlotTypeWidget();

		if (!IsValid(VSyncTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings VSyncTypeWidget"));
			return;
		}

		VSyncTypeWidget->SetOnOffState(OldSettingValue.bVSync);
	}

	if (OldSettingValue.OutputDeviceIndex != ChangedSettingValue.OutputDeviceIndex)
	{
		UAlienSettingSlotTypeWidget* OutputSlotTypeWidget = AudioOutputSlotOption->GetSlotTypeWidget();

		if (IsValid(OutputSlotTypeWidget))
		{
			if (OutputDeviceInfos.IsValidIndex(OldSettingValue.OutputDeviceIndex))
			{
				OutputSlotTypeWidget->SetListText(OutputDeviceInfos[OldSettingValue.OutputDeviceIndex].DisplayName);
			}
		}
	}

	if (OldSettingValue.InputDeviceIndex != ChangedSettingValue.InputDeviceIndex)
	{
		UAlienSettingSlotTypeWidget* InputSlotTypeWidget = AudioInputSlotOption->GetSlotTypeWidget();

		if (IsValid(InputSlotTypeWidget))
		{
			if (InputDeviceInfos.IsValidIndex(OldSettingValue.InputDeviceIndex))
			{
				InputSlotTypeWidget->SetListText(InputDeviceInfos[OldSettingValue.InputDeviceIndex].DisplayName);
			}
		}
	}

	if (OldSettingValue.AudioQuality != ChangedSettingValue.AudioQuality)
	{
		UpdateListOptionSlotWidget(UAlienGamePlayStatics::EnumToString(TEXT("EAudioQuality"), OldSettingValue.AudioQuality), AudioQualitySlotOption->GetSlotTypeWidget(), OldSettingValue.AudioQuality);
	}

	if (OldSettingValue.MasterVolumes != ChangedSettingValue.MasterVolumes)
	{
		UAlienSettingSlotTypeWidget* SlotTypeWidget = MasterVolumeSlotOption->GetSlotTypeWidget();

		if (!IsValid(SlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
			return;
		}
		SlotTypeWidget->SetSliderValue(OldSettingValue.MasterVolumes);

		UAlienGamePlayStatics::SetMasterVolumes(this, OldSettingValue.MasterVolumes);
	}

	if (OldSettingValue.BgmVolumes != ChangedSettingValue.BgmVolumes)
	{
		UAlienSettingSlotTypeWidget* SlotTypeWidget = BGMVolumeSlotOption->GetSlotTypeWidget();

		if (!IsValid(SlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
			return;
		}
		SlotTypeWidget->SetSliderValue(OldSettingValue.BgmVolumes);

		UAlienGamePlayStatics::SetBGMVolumes(this, OldSettingValue.BgmVolumes);
	}

	if (OldSettingValue.SfxVolumes != ChangedSettingValue.SfxVolumes)
	{
		UAlienSettingSlotTypeWidget* SlotTypeWidget = SFXVolumeSlotOption->GetSlotTypeWidget();

		if (!IsValid(SlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
			return;
		}
		SlotTypeWidget->SetSliderValue(OldSettingValue.SfxVolumes);

		UAlienGamePlayStatics::SetSFXVolumes(this, OldSettingValue.SfxVolumes);
	}

	if (OldSettingValue.VoiceVolumes != ChangedSettingValue.VoiceVolumes)
	{
		UAlienSettingSlotTypeWidget* SlotTypeWidget = VoiceVolumeSlotOption->GetSlotTypeWidget();

		if (!IsValid(SlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
			return;
		}
		SlotTypeWidget->SetSliderValue(OldSettingValue.VoiceVolumes);

		VoiceSubSystem->SetAudioOutputVolume(OldSettingValue.VoiceVolumes);
	}

	if (OldSettingValue.MicVolumes != ChangedSettingValue.MicVolumes)
	{
		UAlienSettingSlotTypeWidget* SlotTypeWidget = MicVolumeSlotOption->GetSlotTypeWidget();

		if (!IsValid(SlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
			return;
		}
		SlotTypeWidget->SetSliderValue(OldSettingValue.MicVolumes);

		VoiceSubSystem->SetAudioInputVolume(OldSettingValue.MicVolumes);
	}

	if (OldSettingValue.MouseSensitive != ChangedSettingValue.MouseSensitive)
	{
		UAlienSettingSlotTypeWidget* SlotTypeWidget = MouseSensitiveSlotOption->GetSlotTypeWidget();

		if (!IsValid(SlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
			return;
		}
		SlotTypeWidget->SetSliderValue(OldSettingValue.MouseSensitive);

		UAlienGameUserSettings::Get()->BroadcastMouseSensitiveDeleagte();
	}

	if (OldSettingValue.MouseZoomSensitive != ChangedSettingValue.MouseZoomSensitive)
	{
		UAlienSettingSlotTypeWidget* SlotTypeWidget = MouseZoomSensitiveSlotOption->GetSlotTypeWidget();

		if (!IsValid(SlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
			return;
		}
		SlotTypeWidget->SetSliderValue(OldSettingValue.MouseZoomSensitive);

		UAlienGameUserSettings::Get()->BroadcastMouseZoomSensitiveDeleagte();
	}

	if (OldSettingValue.bAchievementAlert != ChangedSettingValue.bAchievementAlert)
	{
		UAlienSettingSlotTypeWidget* SlotTypeWidget = AchievementAlertSlotOption->GetSlotTypeWidget();

		if (!IsValid(SlotTypeWidget))
		{
			ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
			return;
		}
		SlotTypeWidget->SetOnOffState(OldSettingValue.bAchievementAlert);
	}


	// Scalability 옵션들========================================
	if (OldSettingValue.Scalability != ChangedSettingValue.Scalability)
	{
		UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), OldSettingValue.Scalability + 1);
	}

	if (OldSettingValue.ViewDistanceQuality != ChangedSettingValue.ViewDistanceQuality)
	{
		UpdateScalabilityListOptionSlotWidget(ViewDistanceSlotOption->GetSlotTypeWidget(), OldSettingValue.ViewDistanceQuality + 1);
	}

	if (OldSettingValue.ShadowQuality != ChangedSettingValue.ShadowQuality)
	{
		UpdateScalabilityListOptionSlotWidget(ShadowSlotOption->GetSlotTypeWidget(), OldSettingValue.ShadowQuality + 1);
	}

	if (OldSettingValue.GlobalIlluminationQuality != ChangedSettingValue.GlobalIlluminationQuality)
	{
		UpdateScalabilityListOptionSlotWidget(GlobalIlluminationSlotOption->GetSlotTypeWidget(), OldSettingValue.GlobalIlluminationQuality + 1);
	}

	if (OldSettingValue.ReflectionQuality != ChangedSettingValue.ReflectionQuality)
	{
		UpdateScalabilityListOptionSlotWidget(ReflectionSlotOption->GetSlotTypeWidget(), OldSettingValue.ReflectionQuality + 1);
	}

	if (OldSettingValue.AntiAliasingQuality != ChangedSettingValue.AntiAliasingQuality)
	{
		UpdateScalabilityListOptionSlotWidget(AntiAliasingSlotOption->GetSlotTypeWidget(), OldSettingValue.AntiAliasingQuality + 1);
	}

	if (OldSettingValue.TextureQuality != ChangedSettingValue.TextureQuality)
	{
		UpdateScalabilityListOptionSlotWidget(TextureSlotOption->GetSlotTypeWidget(), OldSettingValue.TextureQuality + 1);
	}

	if (OldSettingValue.VisualEffectQuality != ChangedSettingValue.VisualEffectQuality)
	{
		UpdateScalabilityListOptionSlotWidget(VisualEffectSlotOption->GetSlotTypeWidget(), OldSettingValue.VisualEffectQuality + 1);
	}

	if (OldSettingValue.PostProcessingQuality != ChangedSettingValue.PostProcessingQuality)
	{
		UpdateScalabilityListOptionSlotWidget(PostprocessingSlotOption->GetSlotTypeWidget(), OldSettingValue.PostProcessingQuality + 1);
	}

	if (OldSettingValue.FoliageQuality != ChangedSettingValue.FoliageQuality)
	{
		UpdateScalabilityListOptionSlotWidget(FoliageSlotOption->GetSlotTypeWidget(), OldSettingValue.FoliageQuality + 1);
	}

	if (OldSettingValue.ShadingQuality != ChangedSettingValue.ShadingQuality)
	{
		UpdateScalabilityListOptionSlotWidget(FoliageSlotOption->GetSlotTypeWidget(), OldSettingValue.ShadingQuality + 1);
	}
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);
	// end ...Scalability 옵션들========================================


	ChangedSettingValue = OldSettingValue;
}

void UAlienPageGameSettingWidget::DefaultSettings()
{
	ChangedSettingValue = FAlienGameSettingValue();
	ApplySettings();
	UpdateUI();
}

void UAlienPageGameSettingWidget::OnClickedSaveChangeCancel()
{
	if (bCloseReady)
	{
		CloseUI();
	}
}

void UAlienPageGameSettingWidget::UpdateUI()
{
	UAlienSettingSlotTypeWidget* SlotType = ModeSlotOption->GetSlotTypeWidget();

	if (IsValid(SlotType))
	{
		SlotType->SetListTableText(FString::Printf(TEXT("UI_Setting_Screen_Basic_ScrrenMode_%d"), OldSettingValue.WindowMode + 1));
	}

	UAlienSettingSlotTypeWidget* ResolutionSlotTypeWidget = ResolutionSlotOption->GetSlotTypeWidget();

	if (!IsValid(ResolutionSlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings ResolutionSlotTypeWidget"));
		return;
	}

	FString ResolutionStr = OldSettingValue.Resolution.ToString();
	ResolutionSlotTypeWidget->SetListText(ResolutionStr);

	UAlienSettingSlotTypeWidget* GammaSlotTypeWidget = GammaSlotOption->GetSlotTypeWidget();

	if (!IsValid(GammaSlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings GammaSlotTypeWidget"));
		return;
	}

	GammaSlotTypeWidget->SetSliderValue(OldSettingValue.Gamma);

	UAlienSettingSlotTypeWidget* VolumeSlotTypeWidget = MasterVolumeSlotOption->GetSlotTypeWidget();

	if (!IsValid(VolumeSlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
		return;
	}
	VolumeSlotTypeWidget->SetSliderValue(OldSettingValue.MasterVolumes);

	UAlienGamePlayStatics::SetMasterVolumes(this, OldSettingValue.MasterVolumes);

	UAlienSettingSlotTypeWidget* MouseSlotTypeWidget = MouseSensitiveSlotOption->GetSlotTypeWidget();

	if (!IsValid(MouseSlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
		return;
	}
	MouseSlotTypeWidget->SetSliderValue(OldSettingValue.MouseSensitive);

	UAlienGameUserSettings::Get()->BroadcastMouseSensitiveDeleagte();

	UAlienSettingSlotTypeWidget* MouseZoomSlotTypeWidget = MouseZoomSensitiveSlotOption->GetSlotTypeWidget();

	if (!IsValid(MouseZoomSlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::RevertSettings SlotTypeWidget"));
		return;
	}
	MouseZoomSlotTypeWidget->SetSliderValue(OldSettingValue.MouseZoomSensitive);

	UAlienGameUserSettings::Get()->BroadcastMouseZoomSensitiveDeleagte();

	UAlienSettingSlotTypeWidget* ScalabilitySlotTypeWidget = ScalabilitySlotOption->GetSlotTypeWidget();

	if (!IsValid(ScalabilitySlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnClickedLeftButtonScalability ScalabilitySlotTypeWidget"));
		return;
	}
	FString ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.Scalability + 1);
	ScalabilitySlotTypeWidget->SetListTableText(ScalabilityTitleStr);
}

void UAlienPageGameSettingWidget::OptionChangeTracking()
{
	bool bChanged = IsOptionChange();
	if (IsValid(ApplySettingButton))
	{
		ApplySettingButton->SetIsEnabled(bChanged);
	}

	if (IsValid(RevertSettingButton))
	{
		RevertSettingButton->SetIsEnabled(bChanged);
	}

	if (IsValid(ResetSettingButton))
	{
		ResetSettingButton->SetIsEnabled(bChanged);
	}
}	

bool UAlienPageGameSettingWidget::IsOptionChange()
{
	return OldSettingValue != ChangedSettingValue;
}

void UAlienPageGameSettingWidget::SetResolutionList(EWindowMode::Type InWindowMode)
{
	ResolutionList.Empty();

	if (InWindowMode == EWindowMode::Fullscreen)
	{
		bool bResolutionOk = UKismetSystemLibrary::GetSupportedFullscreenResolutions(ResolutionList);
		if (bResolutionOk)
		{
			if (!ResolutionList.Find(UAlienGameUserSettings::Get()->GetLastConfirmedScreenResolution(), ResolutionListIndex))
			{

				ResolutionList.Add(UAlienGameUserSettings::Get()->GetLastConfirmedScreenResolution());
				ResolutionListIndex = FMath::Clamp(ResolutionList.Num() - 1, 0, ResolutionList.Num() - 1);
			}

		}
		else
		{
			ResolutionList.Add(UAlienGameUserSettings::Get()->GetLastConfirmedScreenResolution());
			ResolutionListIndex = 0;
		}
	}
	else if (InWindowMode == EWindowMode::Windowed)
	{
		bool bResolutionOk = UKismetSystemLibrary::GetConvenientWindowedResolutions(ResolutionList);
		if (bResolutionOk)
		{
			if (!ResolutionList.Find(UAlienGameUserSettings::Get()->GetLastConfirmedScreenResolution(), ResolutionListIndex))
			{

				ResolutionList.Add(UAlienGameUserSettings::Get()->GetLastConfirmedScreenResolution());
				ResolutionListIndex = FMath::Clamp(ResolutionList.Num() - 1, 0, ResolutionList.Num() - 1);
			}

		}
		else
		{
			ResolutionList.Add(UAlienGameUserSettings::Get()->GetLastConfirmedScreenResolution());
			ResolutionListIndex = 0;
		}
	}
	else
	{
		ResolutionList.Add(UAlienGameUserSettings::Get()->GetDesktopResolution());
		ResolutionListIndex = 0;
	}

	UpdateResolutionOptionText(ResolutionListIndex);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("======Environment Resolution List "));
	for (auto Iter : ResolutionList)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Iter.ToString());
	}

}

void UAlienPageGameSettingWidget::UpdateResolutionOptionText(int32 InResIndex)
{
	if (!IsValid(ResolutionSlotOption))
	{
		return;
	}

	FString ResolutionStr = TEXT("");

	if (ResolutionList.IsValidIndex(InResIndex))
	{
		ChangedSettingValue.Resolution = ResolutionList[InResIndex];

		ResolutionStr = ResolutionList[InResIndex].ToString();

		UAlienSettingSlotTypeWidget* ResolutionSlotTypeWidget = ResolutionSlotOption->GetSlotTypeWidget();

		if (IsValid(ResolutionSlotTypeWidget))
		{
			ResolutionSlotTypeWidget->SetListText(ResolutionStr);
		}
	}
}

void UAlienPageGameSettingWidget::OnPopupResolutionRevertOK()
{
	UAlienGameUserSettings* Settings = UAlienGameUserSettings::Get();

	OldSettingValue.Resolution = ChangedSettingValue.Resolution = LastSettingValue.Resolution;
	OldSettingValue.WindowMode = ChangedSettingValue.WindowMode = LastSettingValue.WindowMode;
	Settings->SetScreenResolution(ChangedSettingValue.Resolution);
	Settings->SetFullscreenMode((EWindowMode::Type)ChangedSettingValue.WindowMode);
	Settings->ApplySettings(false);

	UAlienSettingSlotTypeWidget* ModeSlotTypeWidget = ModeSlotOption->GetSlotTypeWidget();

	if (!IsValid(ModeSlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::InitGameplaySettings SlotTypeWidget"));
		return;
	}
	ModeSlotTypeWidget->SetListTableText(FString::Printf(TEXT("UI_Setting_Screen_Basic_ScrrenMode_%d"), OldSettingValue.WindowMode + 1));



	UAlienSettingSlotTypeWidget* ResolutionSlotTypeWidget = ResolutionSlotOption->GetSlotTypeWidget();

	if (!IsValid(ResolutionSlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::ResetSettings ResolutionSlotTypeWidget"));
		return;
	}
	ResolutionList.Find(OldSettingValue.Resolution, ResolutionListIndex);
	FString ResolutionStr = ResolutionList[ResolutionListIndex].ToString();
	ResolutionSlotTypeWidget->SetListText(ResolutionStr);

	OptionChangeTracking();

}

void UAlienPageGameSettingWidget::OnPopupResolutionConfirmOK()
{
	UAlienGameUserSettings* Settings = UAlienGameUserSettings::Get();
	Settings->ConfirmVideoMode();
	LastSettingValue = OldSettingValue;
}

void UAlienPageGameSettingWidget::SwitchCategoryOrContents(bool bShowCategory)
{
	bShowCategory ? SetWidgetSwitcherActiveWidgetIndex(m_SettingSwitcher, 1) : SetWidgetSwitcherActiveWidgetIndex(m_SettingSwitcher, 0);
}

void UAlienPageGameSettingWidget::InitGameplaySettings()
{
	/// 마우스 감도 위젯 설정
	CreateSlotMainWidget(TEXT("UI_Setting_GamePlay_Mouse"), MouseSensitiveSlot);
	if (!IsValid(MouseSensitiveSlot))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::InitGameplaySettings MouseSensitiveSlot"));
		return;
	}
	GameplaySlotArr.Add(MouseSensitiveSlot);

	/// 마우스 감도 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("UI_Setting_GamePlay_Mouse_MouseSensi"),
		TEXT("UI_Setting_GamePlay_Mouse_MouseSensi"),
		ESettingType::MouseSensitivity,
		0.1f,
		5.0f,
		OldSettingValue.MouseSensitive,
		MouseSensitiveSlot,
		MouseSensitiveSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnMouseSensitiveChanged));


	/// 마우스 줌 감도 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("UI_Setting_GamePlay_Mouse_AimSensi"),
		TEXT("UI_Setting_GamePlay_Mouse_AimSensi"),
		ESettingType::MouseZoomSensitivity,
		0.1f,
		5.0f,
		OldSettingValue.MouseZoomSensitive,
		MouseSensitiveSlot,
		MouseZoomSensitiveSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnMouseZoomSensitiveChanged));

	// 언어 설정 위젯
	CreateSlotMainWidget(TEXT("UI_Setting_GamePlay_Basic_Language"), LanguageSlot);
	if (!IsValid(LanguageSlot))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::InitGameplaySettings LanguageSlot"));
		return;
	}

	GameplaySlotArr.Add(LanguageSlot);


	/// 언어 설정 옵션 위젯 추가 및 설정
	FString LanguageStr = TEXT("");
	if (LocalizedCulturesList.Num() > 0)
	{
		const FCulturePtr CulturePtr = FInternationalization::Get().GetCulture(LocalizedCulturesList[(int32)OldSettingValue.Language]);
		LanguageStr = CulturePtr->GetEnglishName();
	}
	else
	{
		LanguageStr = TEXT("Empty Cultures");
	}

	CreateSlotListOptionWidget(TEXT("UI_Setting_GamePlay_Basic_Language"),
		LanguageStr,
		ESettingType::Language,
		LanguageSlot,
		LanguageSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonLanguage),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonLanguage));

	if (!IsValid(LanguageSlotOption))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::InitGameplaySettings LanguageSlotOption"));
		return;
	}

	UAlienSettingSlotTypeWidget* LanguageSlotTypeWidget = LanguageSlotOption->GetSlotTypeWidget();

	if (IsValid(LanguageSlotTypeWidget))
	{
		LanguageSlotTypeWidget->SetListButtonIsEnabled(LocalizedCulturesList.Num() > 0 ? true : false);
	}

	// 업적 알림 설정 위젯
	CreateSlotMainWidget(TEXT("UI_Setting_GamePlay_Basic_Achievement"), AchievementAlertSlot);
	if (!IsValid(AchievementAlertSlot))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::InitGameplaySettings AchievementAlertSlot"));
		return;
	}

	GameplaySlotArr.Add(AchievementAlertSlot);


	// 업적 알림 옵션 위젯 추가 및 설정
	CreateSlotToggleOptionWidget(
		TEXT("UI_Setting_GamePlay_Basic_Achievement"),
		TEXT("*ON"),
		TEXT("*OFF"),
		ESettingType::AchievementAlert,
		OldSettingValue.bAchievementAlert,
		AchievementAlertSlot,
		AchievementAlertSlotOption,
		FSettingCheckStateChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnCheckedOnAchievementAlert),
		FSettingCheckStateChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnCheckedOffAchievementAlert));

}

void UAlienPageGameSettingWidget::InitVideoSettings()
{
	// create BaseWiget
	const FAlienTableRow_UI* const UITable = UAlienGameInstanceBase::GetManager<UAlienTableManager>()->FindUI("UAlienSettingSlotMainWidget");
	UClass* const LoadClass = (UITable) ? UAlienAssetManager::Get().LoadResource<UClass>(UITable->WidgetPath.ToSoftObjectPath()) : nullptr;

	const FAlienTableRow_UI* const UITableSlot = UAlienGameInstanceBase::GetManager<UAlienTableManager>()->FindUI("UAlienSettingSlotWidget");
	UClass* const LoadClassSlot = (UITableSlot) ? UAlienAssetManager::Get().LoadResource<UClass>(UITableSlot->WidgetPath.ToSoftObjectPath()) : nullptr;
	if (!LoadClass && !LoadClassSlot)
	{
		ALIEN_LOG(Warning, TEXT("UIManager::CreateWidget - Null Fail: UAlienSettingSlotMainWidget"));
		return;
	}

	/// 스크린 메인 옵션 위젯 생성
	CreateSlotMainWidget(TEXT("UI_Setting_Screen"), DisplayMainSlot);
	if (!IsValid(DisplayMainSlot))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::InitVideoSettings DisplayMainSlot"));
		return;
	}
	VideoSlotArr.Add(DisplayMainSlot);

	/// 윈도우 모드 설정 옵션 위젯 추가 및 설정
	FString ScreenModeStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_ScrrenMode_%d"), OldSettingValue.WindowMode + 1);

	CreateSlotListOptionWidget(
		TEXT("UI_Setting_Screen_Basic_ScrrenMode"),
		ScreenModeStr,
		ESettingType::ScreenMode,
		DisplayMainSlot,
		ModeSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonScreenMode),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonScreenMode));

	/// 해상도 설정 옵션 위젯 추가 및 설정
	FString ResolutionStr = TEXT("");
	if (ResolutionList.IsValidIndex(ResolutionListIndex))
	{
		ResolutionStr = ResolutionList[ResolutionListIndex].ToString();
	}

	CreateSlotListOptionWidget(
		TEXT("UI_Setting_Screen_Basic_ScreenResolution"),
		ResolutionStr,
		ESettingType::DisplayResolution,
		DisplayMainSlot,
		ResolutionSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonResolution),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonResolution));

	UAlienSettingSlotTypeWidget* ResolutionSlotTypeWidget = ResolutionSlotOption->GetSlotTypeWidget();
	if (!IsValid(ResolutionSlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::InitVideoSettings ResolutionSlotTypeWidget"));
		return;
	}
	ResolutionSlotTypeWidget->SetListButtonIsEnabled(OldSettingValue.WindowMode != EWindowMode::WindowedFullscreen);

	/// 해상도 스케일 설정 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("*ResolutionScale"),
		TEXT("*ResolutionScale"),
		ESettingType::ResolutionScale,
		0.0f,
		1.0f,
		OldSettingValue.ResolutionScale,
		DisplayMainSlot,
		ResolutionScaleSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnSliderChangedResolutionScale));
	
	// 비활성 처리.
	SetWidgetVisibility(ResolutionScaleSlotOption, ESlateVisibility::Collapsed);


	/// 감마 설정 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("UI_Setting_Screen_Basic_Brightness"),
		TEXT("UI_Setting_Screen_Basic_Brightness"),
		ESettingType::Gamma,
		1.0f,
		10.0f,
		OldSettingValue.Gamma,
		DisplayMainSlot,
		GammaSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnSliderChangedGamma));

	/// 프레임 제한 설정 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("UI_Setting_Screen_Basic_MaxFPS"),
		TEXT("UI_Setting_Screen_Basic_MaxFPS"),
		ESettingType::FrameRate,
		0.0f,
		540.0f,
		OldSettingValue.FrameRate,
		DisplayMainSlot,
		FrameRateSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnSliderChangedFrameRate));

	// 수직 동기 옵션 위젯 추가 및 설정
	CreateSlotToggleOptionWidget(
		TEXT("UI_Setting_Screen_Basic_V-Sync"),
		TEXT("*ON"),
		TEXT("*OFF"),
		ESettingType::VSync,
		OldSettingValue.bVSync,
		DisplayMainSlot,
		VSyncSlotOption,
		FSettingCheckStateChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnCheckedOnVSync),
		FSettingCheckStateChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnCheckedOffVSync));
		

	// 스칼라빌리티 설정 위젯
	CreateSlotMainWidget(TEXT("UI_Setting_Screen_Basic_Graphics"), ScalabilitySlot);
	if (!IsValid(ScalabilitySlot))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::InitVideoSettings ScalabilitySlot"));
		return;
	}
	VideoSlotArr.Add(ScalabilitySlot);


	/// 스칼라빌리티 옵션 위젯 추가 및 설정
	FString ScalabilityTitleStr = TEXT("");
	if (OldSettingValue.Scalability == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.Scalability + 1);
	}
	CreateSlotListOptionWidget(
		TEXT("UI_Setting_Screen_Basic_Graphics"),
		ScalabilityTitleStr,
		ESettingType::Scalability,
		ScalabilitySlot,
		ScalabilitySlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonScalability),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonScalability));

	/// 시야품질 옵션 위젯 추가 및 설정
	ScalabilityTitleStr = TEXT("");
	if (OldSettingValue.ViewDistanceQuality == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.ViewDistanceQuality + 1);
	}

	CreateSlotListOptionWidget(
		TEXT("ViewDistance"),
		ScalabilityTitleStr,
		ESettingType::ViewDistance,
		ScalabilitySlot,
		ViewDistanceSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonViewDistance),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonViewDistance));

	/// 그림자 옵션 위젯 추가 및 설정
	ScalabilityTitleStr = TEXT("");
	if (OldSettingValue.ShadowQuality == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.ShadowQuality + 1);
	}
	CreateSlotListOptionWidget(
		TEXT("Shadow"),
		ScalabilityTitleStr,
		ESettingType::Shadow,
		ScalabilitySlot,
		ShadowSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonShadow),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonShadow));

	/// 전역조명 옵션 위젯 추가 및 설정
	ScalabilityTitleStr = TEXT("");
	if (OldSettingValue.GlobalIlluminationQuality == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.GlobalIlluminationQuality + 1);
	}
	CreateSlotListOptionWidget(
		TEXT("GlobalIllumination"),
		ScalabilityTitleStr,
		ESettingType::GlobalIllumination,
		ScalabilitySlot,
		GlobalIlluminationSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonGlobalIllumination),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonGlobalIllumination));

	/// 반사 옵션 위젯 추가 및 설정
	ScalabilityTitleStr = TEXT("");
	if (OldSettingValue.ViewDistanceQuality == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.ViewDistanceQuality + 1);
	}

	CreateSlotListOptionWidget(
		TEXT("Reflection"),
		ScalabilityTitleStr,
		ESettingType::Refletion,
		ScalabilitySlot,
		ReflectionSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonReflection),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonReflection));

	/// AntiAliasing 옵션 위젯 추가 및 설정
	if (OldSettingValue.AntiAliasingQuality == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.AntiAliasingQuality + 1);
	}
	CreateSlotListOptionWidget(
		TEXT("AntiAliasing"),
		ScalabilityTitleStr,
		ESettingType::AntiAliasing,
		ScalabilitySlot,
		AntiAliasingSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonAntiAliasing),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonAntiAliasing));

	/// 텍스쳐 품질 옵션 위젯 추가 및 설정
	if (OldSettingValue.TextureQuality == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.TextureQuality + 1);
	}
	CreateSlotListOptionWidget(
		TEXT("Texture"),
		ScalabilityTitleStr,
		ESettingType::Texture,
		ScalabilitySlot,
		TextureSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonTexture),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonTexture));

	/// VisualEffect 옵션 위젯 추가 및 설정
	if (OldSettingValue.VisualEffectQuality == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.VisualEffectQuality + 1);
	}
	CreateSlotListOptionWidget(
		TEXT("VisualEffect"),
		ScalabilityTitleStr,
		ESettingType::VisualEffect,
		ScalabilitySlot,
		VisualEffectSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonVisualEffect),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonVisualEffect));

	/// Postprocessing 옵션 위젯 추가 및 설정
	if (OldSettingValue.PostProcessingQuality == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.PostProcessingQuality + 1);
	}
	CreateSlotListOptionWidget(
		TEXT("PostProcessing"),
		ScalabilityTitleStr,
		ESettingType::Postprocessing,
		ScalabilitySlot,
		PostprocessingSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonPostProcessing),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonPostProcessing));

	/// Foliage 옵션 위젯 추가 및 설정
	if (OldSettingValue.FoliageQuality == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.FoliageQuality + 1);
	}
	CreateSlotListOptionWidget(
		TEXT("Foliage"),
		ScalabilityTitleStr,
		ESettingType::Foliage,
		ScalabilitySlot,
		FoliageSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonFoliage),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonFoliage));
	
	/// Shading 옵션 위젯 추가 및 설정
	if (OldSettingValue.ShadingQuality == -1)
	{
		ScalabilityTitleStr = TEXT("UI_Setting_Value_Custom");
	}
	else
	{
		ScalabilityTitleStr = FString::Printf(TEXT("UI_Setting_Value_%d"), OldSettingValue.ShadingQuality + 1);
	}
	CreateSlotListOptionWidget(
		TEXT("Shading"),
		ScalabilityTitleStr,
		ESettingType::Shading,
		ScalabilitySlot,
		ShadingSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonShading),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonShading));
}

void UAlienPageGameSettingWidget::InitAudioSettings()
{
	// create BaseWiget
	const FAlienTableRow_UI* const UITable = UAlienGameInstanceBase::GetManager<UAlienTableManager>()->FindUI("UAlienSettingSlotMainWidget");
	UClass* const LoadClass = (UITable) ? UAlienAssetManager::Get().LoadResource<UClass>(UITable->WidgetPath.ToSoftObjectPath()) : nullptr;

	const FAlienTableRow_UI* const UITableSlot = UAlienGameInstanceBase::GetManager<UAlienTableManager>()->FindUI("UAlienSettingSlotWidget");
	UClass* const LoadClassSlot = (UITableSlot) ? UAlienAssetManager::Get().LoadResource<UClass>(UITableSlot->WidgetPath.ToSoftObjectPath()) : nullptr;
	if (!LoadClass && !LoadClassSlot)
	{
		ALIEN_LOG(Warning, TEXT("UIManager::CreateWidget - Null Fail: UAlienSettingSlotMainWidget"));
		return;
	}

	//출력 장치 설정
	CreateSlotMainWidget(TEXT("UI_Setting_Sound_Audio_AudioOutput"), AudioOutputSlot);
	if (IsValid(AudioOutputSlot))
	{
		AudioSlotArr.Add(AudioOutputSlot);
	}
	FString OutputStr = TEXT("");
	if (OutputDeviceInfos.IsValidIndex(OldSettingValue.OutputDeviceIndex))
	{
		OutputStr = OutputDeviceInfos[OldSettingValue.OutputDeviceIndex].DisplayName;
	}

	/// 출력장치 옵션 위젯 추가 및 설정
	CreateSlotListOptionWidget(
		TEXT("UI_Setting_Sound_Audio_AudioOutput"),
		OutputStr,
		ESettingType::AudioOutput,
		AudioOutputSlot,
		AudioOutputSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonAudioOutput),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonAudioOutput));
	
	//입력 장치 설정
	CreateSlotMainWidget(TEXT("UI_Setting_Sound_Audio_AudioInput"), AudioInputSlot);
	if (IsValid(AudioInputSlot))
	{
		AudioSlotArr.Add(AudioInputSlot);
	}
	

	FString InputStr = TEXT("");
	if (InputDeviceInfos.IsValidIndex(OldSettingValue.InputDeviceIndex))
	{
		InputStr = InputDeviceInfos[OldSettingValue.InputDeviceIndex].DisplayName;
	}

	/// 입력장치 옵션 위젯 추가 및 설정
	CreateSlotListOptionWidget(
		TEXT("UI_Setting_Sound_Audio_AudioInput"),
		InputStr,
		ESettingType::AudioInput,
		AudioInputSlot,
		AudioInputSlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonAudioInput),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonAudioInput));
	/// 마이크 볼륨 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("UI_Setting_Sound_Audio_MicVol"),
		TEXT("UI_Setting_Sound_Audio_MicVol"),
		ESettingType::VoiceVolume,
		.0f,
		1.0f,
		OldSettingValue.VoiceVolumes,
		AudioInputSlot,
		MicVolumeSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnMICVolumeChanged));



	//볼륨 설정
	CreateSlotMainWidget(TEXT("UI_Setting_Sound"), VolumeSlot);
	if (IsValid(VolumeSlot))
	{
		AudioSlotArr.Add(VolumeSlot);
	}


	/// 마스터 볼륨 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("UI_Setting_Sound_Basic_MasterVol"), 
		TEXT("UI_Setting_Sound_Basic_MasterVol"),
		ESettingType::MasterVolume,
		.0f,
		1.0f,
		OldSettingValue.MasterVolumes,
		VolumeSlot,
		MasterVolumeSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnMasterVolumeChanged));

	/// 배경음 볼륨 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("UI_Setting_Sound_Basic_BGMVol"),
		TEXT("UI_Setting_Sound_Basic_BGMVol"),
		ESettingType::BGMVolume,
		.0f,
		1.0f,
		OldSettingValue.BgmVolumes,
		VolumeSlot,
		BGMVolumeSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnBGMVolumeChanged));

	/// 효과음 볼륨 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("UI_Setting_Sound_Basic_SFXVol"),
		TEXT("UI_Setting_Sound_Basic_SFXVol"),
		ESettingType::SFXVolume,
		.0f,
		1.0f,
		OldSettingValue.SfxVolumes,
		VolumeSlot,
		SFXVolumeSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnSFXVolumeChanged));

	/// 보이스 볼륨 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("UI_Setting_Sound_Audio_VoiceVol"),
		TEXT("UI_Setting_Sound_Audio_VoiceVol"),
		ESettingType::VoiceVolume,
		.0f,
		1.0f,
		OldSettingValue.VoiceVolumes,
		VolumeSlot,
		VoiceVolumeSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnVoiceVolumeChanged));

	/// UI 볼륨 옵션 위젯 추가 및 설정
	CreateSlotSliderOptionWidget(
		TEXT("UI_Setting_Sound_Basic_UIVol"),
		TEXT("UI_Setting_Sound_Basic_UIVol"),
		ESettingType::UIVolume,
		.0f,
		1.0f,
		OldSettingValue.UiVolumes,
		VolumeSlot,
		UIVolumeSlotOption,
		FSettingSliderValueChangedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnUIVolumeChanged));


	/// 음향 품질 옵션 위젯 추가 및 설정
	CreateSlotListOptionWidget(
		TEXT("UI_Setting_Sound_Audio_AudioQuality"),
		UAlienGamePlayStatics::EnumToString(TEXT("EAudioQuality"), OldSettingValue.AudioQuality),
		ESettingType::AudioQuality,
		VolumeSlot,
		AudioQualitySlotOption,
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedLeftButtonAudioQuality),
		FSettingButtonClickedFunc::FDelegate::CreateUObject(this, &UAlienPageGameSettingWidget::OnClickedRightButtonAudioQuality));
}

void UAlienPageGameSettingWidget::OnClickedGamePlaySetting()
{
	ApplySettingButton->SetVisibility(ESlateVisibility::Visible);
	ResetSettingButton->SetVisibility(ESlateVisibility::Visible);
	RevertSettingButton->SetVisibility(ESlateVisibility::Visible);
	EscMenuButton->SetVisibility(ESlateVisibility::Collapsed);
	SwitchCategoryOrContents(true);

	if (IsValid(m_GameplayButton))
	{
		m_GameplayButton->SetSelectSceneIndex(0);
	}

	if (IsValid(m_VideoButton))
	{
		m_VideoButton->SetSelectSceneIndex(1);
	}

	if (IsValid(m_AudioButton))
	{

		m_AudioButton->SetSelectSceneIndex(1);
	}

	for (auto Iter : GameplaySlotArr)
	{
		SetWidgetVisibility(Iter, ESlateVisibility::Visible);
	}

	for (auto Iter : VideoSlotArr)
	{
		SetWidgetVisibility(Iter, ESlateVisibility::Collapsed);
	}

	for (auto Iter : AudioSlotArr)
	{
		SetWidgetVisibility(Iter, ESlateVisibility::Collapsed);
	}
}

void UAlienPageGameSettingWidget::OnClickedVideoSetting()
{
	ApplySettingButton->SetVisibility(ESlateVisibility::Visible);
	ResetSettingButton->SetVisibility(ESlateVisibility::Visible);
	RevertSettingButton->SetVisibility(ESlateVisibility::Visible);
	EscMenuButton->SetVisibility(ESlateVisibility::Collapsed);
	SwitchCategoryOrContents(true);

	if (IsValid(m_GameplayButton))
	{
		m_GameplayButton->SetSelectSceneIndex(1);
	}

	if (IsValid(m_VideoButton))
	{
		m_VideoButton->SetSelectSceneIndex(0);
	}

	if (IsValid(m_AudioButton))
	{

		m_AudioButton->SetSelectSceneIndex(1);
	}

	for (auto Iter : GameplaySlotArr)
	{
		SetWidgetVisibility(Iter, ESlateVisibility::Collapsed);
	}

	for (auto Iter : VideoSlotArr)
	{
		SetWidgetVisibility(Iter, ESlateVisibility::Visible);
	}

	for (auto Iter : AudioSlotArr)
	{
		SetWidgetVisibility(Iter, ESlateVisibility::Collapsed);
	}
}

void UAlienPageGameSettingWidget::OnClickedAudioSetting()
{
	ApplySettingButton->SetVisibility(ESlateVisibility::Visible);
	ResetSettingButton->SetVisibility(ESlateVisibility::Visible);
	RevertSettingButton->SetVisibility(ESlateVisibility::Visible);
	EscMenuButton->SetVisibility(ESlateVisibility::Collapsed);
	SwitchCategoryOrContents(true);

	if (IsValid(m_GameplayButton))
	{
		m_GameplayButton->SetSelectSceneIndex(1);
	}

	if (IsValid(m_VideoButton))
	{
		m_VideoButton->SetSelectSceneIndex(1);
	}

	if (IsValid(m_AudioButton))
	{
		m_AudioButton->SetSelectSceneIndex(0);
	}

	for (auto Iter : GameplaySlotArr)
	{
		SetWidgetVisibility(Iter, ESlateVisibility::Collapsed);
	}

	for (auto Iter : VideoSlotArr)
	{
		SetWidgetVisibility(Iter, ESlateVisibility::Collapsed);
	}

	for (auto Iter : AudioSlotArr)
	{
		SetWidgetVisibility(Iter, ESlateVisibility::Visible);
	}
}

void UAlienPageGameSettingWidget::OnClickedMouseAndKeyboardSetting()
{
	FString str = UAlienGamePlayStatics::GetTableString(this, TEXT("SysMessage_Development"));
	UAlienGamePlayStatics::OpenSystemMessageUI(this, str, 2.0f);
}

void UAlienPageGameSettingWidget::OnClickedBackButton()
{
	if (IsOptionChange())
	{
		bCloseReady = true;
		OnClickedApplySettingButton();
	}
	else
	{
		AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());

		AAlienOnlineGameMode* OnlineGameMode = Cast<AAlienOnlineGameMode>(GameMode);
		AAlienOfflineGameMode* OffLineGameMode = Cast<AAlienOfflineGameMode>(GameMode);

		// 현재는 게임모드가 두개라 둘중 하나임.
		{
			CloseUI();
		}
	}
}

void UAlienPageGameSettingWidget::OnClickedApplySettingButton()
{
	UPopupUIOKCancelInfo* PopupUIOkCancelInfo = NewObject<UPopupUIOKCancelInfo>();
	if (!UAlienGamePlayStatics::IsOpenedPopupUI(this, PopupUIOkCancelInfo->GetUIName()))
	{
		PopupUIOkCancelInfo->Title_Str = UAlienGamePlayStatics::GetTableString(this, TEXT("UI_Setting_Title"));
		PopupUIOkCancelInfo->Desc_Str = UAlienGamePlayStatics::GetTableString(this, TEXT("UI_Setting_Message_Save")); 
		PopupUIOkCancelInfo->OK_Str = UAlienGamePlayStatics::GetTableString(this, TEXT("UI_Setting_Button_Bool_1")); 
		PopupUIOkCancelInfo->Cancel_Str = UAlienGamePlayStatics::GetTableString(this, TEXT("UI_Setting_Button_Bool_2"));
		PopupUIOkCancelInfo->Delegate_OK.BindUFunction(this, TEXT("ApplySettings"));
		PopupUIOkCancelInfo->Delegate_Cancel.BindUFunction(this, TEXT("OnClickedSaveChangeCancel"));
		UAlienGamePlayStatics::OpenPopupUI(this, PopupUIOkCancelInfo);
	}
}

void UAlienPageGameSettingWidget::OnClickedResetSettingButton()
{
	DefaultSettings();
}

void UAlienPageGameSettingWidget::OnClickedRevertSettingButton()
{
	RevertSettings();
}

void UAlienPageGameSettingWidget::OnMouseEnterDescFunc(ESettingType SettingType)
{
	FString SettingDescStr = TEXT("");

	if (SettingType == ESettingType::MouseSensitivity)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_GamePlay_Mouse_MouseSensi_Desc"));
	}
	else if (SettingType == ESettingType::MouseZoomSensitivity)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_GamePlay_Mouse_AimSensi_Desc"));
	}
	else if (SettingType == ESettingType::ScreenMode)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_ScrrenMode_Desc"));
	}
	else if (SettingType == ESettingType::DisplayResolution)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_ScreenResolution_Desc"));
	}
	else if (SettingType == ESettingType::MasterVolume)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Sound_Basic_MasterVol_Desc"));
	}
	else if (SettingType == ESettingType::BGMVolume)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Sound_Basic_MasterVol_Desc"));
	}
	else if (SettingType == ESettingType::SFXVolume)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Sound_Basic_SFXVol_Desc"));
	}	
	else if (SettingType == ESettingType::VoiceVolume)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Sound_Basic_BGMVol_Desc"));
	}
	else if (SettingType == ESettingType::UIVolume)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Sound_Basic_UIVol_Desc"));
	}
	else if (SettingType == ESettingType::MicVolume)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Sound_Audio_MicVol_Desc"));
	}
	else if (SettingType == ESettingType::Language)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_GamePlay_Basic_Language_Desc"));
	}
	else if (SettingType == ESettingType::Scalability)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_Graphics_Desc"));
	}	
	else if (SettingType == ESettingType::Gamma)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_Brightness"));
	}
	else if (SettingType == ESettingType::FrameRate)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_MaxFPS_Desc"));
	}
	else if (SettingType == ESettingType::VSync)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_V-Sync_Desc"));
	}
	else if (SettingType == ESettingType::ViewDistance)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Detailed_ViewDistance_Desc"));
	}
	else if (SettingType == ESettingType::Shadow)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Detailed_ShadowQuality_Desc"));
	}
	else if (SettingType == ESettingType::GlobalIllumination)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_Brightness"));
	}
	else if (SettingType == ESettingType::Refletion)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_Brightness"));
	}
	else if (SettingType == ESettingType::AntiAliasing)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Detailed_AntiAliasing_Desc"));
	}
	else if (SettingType == ESettingType::Texture)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Detailed_TextureQuality_Desc"));
	}
	else if (SettingType == ESettingType::VisualEffect)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Detailed_EffectQuality_Desc"));
	}
	else if (SettingType == ESettingType::Postprocessing)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_Brightness"));
	}
	else if (SettingType == ESettingType::Foliage)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_Brightness"));
	}
	else if (SettingType == ESettingType::Shading)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_Brightness"));
	}
	else if (SettingType == ESettingType::AudioQuality)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_Screen_Basic_Brightness"));
	}
	else if (SettingType == ESettingType::AchievementAlert)
	{
		SettingDescStr = FString::Printf(TEXT("UI_Setting_GamePlay_Basic_Achievement_Desc"));
	}

	SetRichTextBlockTableText(m_SettingInfoText, SettingDescStr);
}

void UAlienPageGameSettingWidget::OnMouseLeaveDescFunc()
{

	SetRichTextBlockText(m_SettingInfoText, FText::FromString(TEXT("")));
}

void UAlienPageGameSettingWidget::OnMouseSensitiveChanged(float InSensitive)
{
	ChangedSettingValue.MouseSensitive = InSensitive;

	UAlienSettingSlotTypeWidget* SlotTypeWidget = MouseSensitiveSlotOption->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnMouseSensitiveChanged SlotTypeWidget"));
		return;
	}

	SlotTypeWidget->SetSliderValue(InSensitive);

	UAlienGameUserSettings::Get()->BroadcastMouseSensitiveDeleagte();

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnMouseZoomSensitiveChanged(float InSensitive)
{
	ChangedSettingValue.MouseZoomSensitive = InSensitive;

	UAlienSettingSlotTypeWidget* SlotTypeWidget = MouseZoomSensitiveSlotOption->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnMouseZoomSensitiveChanged SlotTypeWidget"));
		return;
	}

	SlotTypeWidget->SetSliderValue(InSensitive);

	UAlienGameUserSettings::Get()->BroadcastMouseZoomSensitiveDeleagte();

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonScreenMode()
{
	ChangedSettingValue.WindowMode = FMath::Clamp(ChangedSettingValue.WindowMode - 1, (int32)EWindowMode::Fullscreen, (int32)EWindowMode::Windowed);
	UAlienSettingSlotTypeWidget* SlotType = ModeSlotOption->GetSlotTypeWidget();

	if (IsValid(SlotType))
	{
		SlotType->SetListTableText(FString::Printf(TEXT("UI_Setting_Screen_Basic_ScrrenMode_%d"), ChangedSettingValue.WindowMode+1));
	}

	SetResolutionList((EWindowMode::Type)ChangedSettingValue.WindowMode);

	// 해상도 조절 버튼 활성/비활성 처리
	UAlienSettingSlotTypeWidget* ResolutionSlotTypeWidget = ResolutionSlotOption->GetSlotTypeWidget();
	if (IsValid(ResolutionSlotTypeWidget))
	{
		ResolutionSlotTypeWidget->SetListButtonIsEnabled(ChangedSettingValue.WindowMode != EWindowMode::WindowedFullscreen);
	}

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonScreenMode()
{
	ChangedSettingValue.WindowMode = FMath::Clamp(ChangedSettingValue.WindowMode + 1, (int32)EWindowMode::Fullscreen, (int32)EWindowMode::Windowed);

	UAlienSettingSlotTypeWidget* SlotType = ModeSlotOption->GetSlotTypeWidget();

	if (IsValid(SlotType))
	{
		SlotType->SetListTableText(FString::Printf(TEXT("UI_Setting_Screen_Basic_ScrrenMode_%d"), ChangedSettingValue.WindowMode + 1));
	}

	SetResolutionList((EWindowMode::Type)ChangedSettingValue.WindowMode);

	// 해상도 조절 버튼 활성/비활성 처리
	UAlienSettingSlotTypeWidget* ResolutionSlotTypeWidget = ResolutionSlotOption->GetSlotTypeWidget();
	if (IsValid(ResolutionSlotTypeWidget))
	{
		ResolutionSlotTypeWidget->SetListButtonIsEnabled(ChangedSettingValue.WindowMode != EWindowMode::WindowedFullscreen);
	}

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonResolution()
{
	ResolutionListIndex = FMath::Clamp(ResolutionListIndex - 1, 0, ResolutionList.Num() - 1);

	UpdateResolutionOptionText(ResolutionListIndex);

	
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonResolution()
{
	ResolutionListIndex = FMath::Clamp(ResolutionListIndex + 1, 0, ResolutionList.Num() - 1);

	FString ResolutionStr = TEXT("");
	UpdateResolutionOptionText(ResolutionListIndex);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnSliderChangedResolutionScale(float InValue)
{
	ChangedSettingValue.ResolutionScale = InValue;
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnSliderChangedGamma(float InValue)
{
	ChangedSettingValue.Gamma = InValue;
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnSliderChangedFrameRate(float InValue)
{
	ChangedSettingValue.FrameRate = InValue;
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnCheckedOnVSync(bool IsChecked)
{
	ChangedSettingValue.bVSync = IsChecked;

	//UAlienSettingSlotTypeWidget* TypeWidget = VSyncSlotOption->GetSlotTypeWidget();
	//if (IsValid(TypeWidget))
	//{
	//	TypeWidget->Set
	//}
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnCheckedOffVSync(bool IsChecked)
{
	ChangedSettingValue.bVSync = !IsChecked;
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonScalability()
{
	ChangedSettingValue.Scalability = FMath::Clamp(ChangedSettingValue.Scalability - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* ScalabilitySlotTypeWidget = ScalabilitySlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotTypeWidget, ChangedSettingValue.Scalability + 1);

	ChangeOverAllScalability();

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonScalability()
{
	ChangedSettingValue.Scalability = FMath::Clamp(ChangedSettingValue.Scalability + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* ScalabilitySlotTypeWidget = ScalabilitySlotOption->GetSlotTypeWidget();
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotTypeWidget, ChangedSettingValue.Scalability + 1);

	ChangeOverAllScalability();

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonViewDistance()
{
	ChangedSettingValue.ViewDistanceQuality = FMath::Clamp(ChangedSettingValue.ViewDistanceQuality - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = ViewDistanceSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.ViewDistanceQuality + 1);
	
	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonViewDistance()
{
	ChangedSettingValue.ViewDistanceQuality = FMath::Clamp(ChangedSettingValue.ViewDistanceQuality + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = ViewDistanceSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.ViewDistanceQuality + 1);
	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);


	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonShadow()
{
	ChangedSettingValue.ShadowQuality = FMath::Clamp(ChangedSettingValue.ShadowQuality - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = ShadowSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.ShadowQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonShadow()
{
	ChangedSettingValue.ShadowQuality = FMath::Clamp(ChangedSettingValue.ShadowQuality + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = ShadowSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.ShadowQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonGlobalIllumination()
{
	ChangedSettingValue.GlobalIlluminationQuality = FMath::Clamp(ChangedSettingValue.GlobalIlluminationQuality - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = GlobalIlluminationSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.GlobalIlluminationQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonGlobalIllumination()
{
	ChangedSettingValue.GlobalIlluminationQuality = FMath::Clamp(ChangedSettingValue.GlobalIlluminationQuality + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = GlobalIlluminationSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.GlobalIlluminationQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();

}

void UAlienPageGameSettingWidget::OnClickedLeftButtonReflection()
{
	ChangedSettingValue.ReflectionQuality = FMath::Clamp(ChangedSettingValue.ReflectionQuality - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = ReflectionSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.ReflectionQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonReflection()
{
	ChangedSettingValue.ReflectionQuality = FMath::Clamp(ChangedSettingValue.ReflectionQuality + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = ReflectionSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.ReflectionQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonAntiAliasing()
{
	ChangedSettingValue.AntiAliasingQuality = FMath::Clamp(ChangedSettingValue.AntiAliasingQuality - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = AntiAliasingSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.AntiAliasingQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonAntiAliasing()
{
	ChangedSettingValue.AntiAliasingQuality = FMath::Clamp(ChangedSettingValue.AntiAliasingQuality + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = AntiAliasingSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.AntiAliasingQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonTexture()
{
	ChangedSettingValue.TextureQuality = FMath::Clamp(ChangedSettingValue.TextureQuality - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = TextureSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.TextureQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();

}

void UAlienPageGameSettingWidget::OnClickedRightButtonTexture()
{
	ChangedSettingValue.TextureQuality = FMath::Clamp(ChangedSettingValue.TextureQuality + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = TextureSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.TextureQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonVisualEffect()
{
	ChangedSettingValue.VisualEffectQuality = FMath::Clamp(ChangedSettingValue.VisualEffectQuality - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = VisualEffectSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.VisualEffectQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonVisualEffect()
{
	ChangedSettingValue.VisualEffectQuality = FMath::Clamp(ChangedSettingValue.VisualEffectQuality + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = VisualEffectSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.VisualEffectQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonPostProcessing()
{
	ChangedSettingValue.PostProcessingQuality = FMath::Clamp(ChangedSettingValue.PostProcessingQuality - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = PostprocessingSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.PostProcessingQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonPostProcessing()
{
	ChangedSettingValue.PostProcessingQuality = FMath::Clamp(ChangedSettingValue.PostProcessingQuality + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = PostprocessingSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.PostProcessingQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonFoliage()
{
	ChangedSettingValue.FoliageQuality= FMath::Clamp(ChangedSettingValue.FoliageQuality - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = FoliageSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.FoliageQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonFoliage()
{
	ChangedSettingValue.FoliageQuality = FMath::Clamp(ChangedSettingValue.FoliageQuality + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = FoliageSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.FoliageQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonShading()
{
	ChangedSettingValue.ShadingQuality = FMath::Clamp(ChangedSettingValue.ShadingQuality - 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = ShadingSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.ShadingQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonShading()
{
	ChangedSettingValue.ShadingQuality = FMath::Clamp(ChangedSettingValue.ShadingQuality + 1, (int32)ESettingScalability::Low, (int32)ESettingScalability::Cinematic);
	UAlienSettingSlotTypeWidget* SlotTypeWidget = ShadingSlotOption->GetSlotTypeWidget();

	UpdateScalabilityListOptionSlotWidget(SlotTypeWidget, ChangedSettingValue.ShadingQuality + 1);

	// OverAllScalability조절
	UpdateScalabilityListOptionSlotWidget(ScalabilitySlotOption->GetSlotTypeWidget(), CheckScalabilityOption() ? ChangedSettingValue.Scalability + 1 : -1);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonLanguage()
{
	ChangedSettingValue.Language = (ELanguageType)FMath::Clamp((int32)ChangedSettingValue.Language - 1, 0, LocalizedCulturesList.Num() - 1);

	UAlienSettingSlotTypeWidget* LanguageSlotTypeWidget = LanguageSlotOption->GetSlotTypeWidget();

	if (!IsValid(LanguageSlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnClickedLeftButtonLanguage LanguageSlotTypeWidget"));
		return;
	}
	const FCulturePtr CulturePtr = FInternationalization::Get().GetCulture(LocalizedCulturesList[(int32)ChangedSettingValue.Language]);

	LanguageSlotTypeWidget->SetListText(CulturePtr->GetEnglishName());

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonLanguage()
{
	ChangedSettingValue.Language = (ELanguageType)FMath::Clamp((int32)ChangedSettingValue.Language + 1, 0, LocalizedCulturesList.Num() - 1);

	UAlienSettingSlotTypeWidget* LanguageSlotTypeWidget = LanguageSlotOption->GetSlotTypeWidget();

	if (!IsValid(LanguageSlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnClickedLeftButtonLanguage LanguageSlotTypeWidget"));
		return;
	}
	const FCulturePtr CulturePtr = FInternationalization::Get().GetCulture(LocalizedCulturesList[(int32)ChangedSettingValue.Language]);

	LanguageSlotTypeWidget->SetListText(CulturePtr->GetEnglishName());

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnCheckedOnAchievementAlert(bool IsChecked)
{
	ChangedSettingValue.bAchievementAlert = IsChecked;
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnCheckedOffAchievementAlert(bool IsChecked)
{
	ChangedSettingValue.bAchievementAlert = !IsChecked;
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonAudioOutput()
{
	ChangedSettingValue.OutputDeviceIndex =  FMath::Clamp(ChangedSettingValue.OutputDeviceIndex - 1, 0, OutputDeviceInfos.Num() - 1);
	
	UAlienSettingSlotTypeWidget* OutputSlotTypeWidget = AudioOutputSlotOption->GetSlotTypeWidget();

	if (IsValid(OutputSlotTypeWidget))
	{
		if (OutputDeviceInfos.IsValidIndex(ChangedSettingValue.OutputDeviceIndex))
		{
			OutputSlotTypeWidget->SetListText(OutputDeviceInfos[ChangedSettingValue.OutputDeviceIndex].DisplayName);
		}
	}
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonAudioOutput()
{
	ChangedSettingValue.OutputDeviceIndex = FMath::Clamp(ChangedSettingValue.OutputDeviceIndex + 1, 0, OutputDeviceInfos.Num() - 1);

	UAlienSettingSlotTypeWidget* OutputSlotTypeWidget = AudioOutputSlotOption->GetSlotTypeWidget();

	if (IsValid(OutputSlotTypeWidget))
	{
		if (OutputDeviceInfos.IsValidIndex(ChangedSettingValue.OutputDeviceIndex))
		{
			OutputSlotTypeWidget->SetListText(OutputDeviceInfos[ChangedSettingValue.OutputDeviceIndex].DisplayName);
		}
	}
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonAudioInput()
{
	ChangedSettingValue.InputDeviceIndex = FMath::Clamp(ChangedSettingValue.InputDeviceIndex - 1, 0, InputDeviceInfos.Num() - 1);

	UAlienSettingSlotTypeWidget* InputSlotTypeWidget = AudioInputSlotOption->GetSlotTypeWidget();

	if (IsValid(InputSlotTypeWidget))
	{
		if (InputDeviceInfos.IsValidIndex(ChangedSettingValue.InputDeviceIndex))
		{
			InputSlotTypeWidget->SetListText(InputDeviceInfos[ChangedSettingValue.InputDeviceIndex].DisplayName);
		}
	}
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonAudioInput()
{
	ChangedSettingValue.InputDeviceIndex = FMath::Clamp(ChangedSettingValue.InputDeviceIndex + 1, 0, InputDeviceInfos.Num() - 1);

	UAlienSettingSlotTypeWidget* InputSlotTypeWidget = AudioInputSlotOption->GetSlotTypeWidget();

	if (IsValid(InputSlotTypeWidget))
	{
		if (InputDeviceInfos.IsValidIndex(ChangedSettingValue.InputDeviceIndex))
		{
			InputSlotTypeWidget->SetListText(InputDeviceInfos[ChangedSettingValue.InputDeviceIndex].DisplayName);
		}
	}
	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnMasterVolumeChanged(float InVolume)
{
	ChangedSettingValue.MasterVolumes = InVolume;

	UAlienSettingSlotTypeWidget* SlotTypeWidget = MasterVolumeSlotOption->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnMasterVolumeChanged SlotTypeWidget"));
		return;
	}

	SlotTypeWidget->SetSliderValue(ChangedSettingValue.MasterVolumes);

	UAlienGamePlayStatics::SetMasterVolumes(this, ChangedSettingValue.MasterVolumes);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnBGMVolumeChanged(float InVolume)
{
	ChangedSettingValue.BgmVolumes = InVolume;

	UAlienSettingSlotTypeWidget* SlotTypeWidget = BGMVolumeSlotOption->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnBGMVolumeChanged SlotTypeWidget"));
		return;
	}

	SlotTypeWidget->SetSliderValue(ChangedSettingValue.BgmVolumes);

	UAlienGamePlayStatics::SetBGMVolumes(this, ChangedSettingValue.BgmVolumes);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnSFXVolumeChanged(float InVolume)
{
	ChangedSettingValue.SfxVolumes = InVolume;

	UAlienSettingSlotTypeWidget* SlotTypeWidget = SFXVolumeSlotOption->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnSFXVolumeChanged SlotTypeWidget"));
		return;
	}

	SlotTypeWidget->SetSliderValue(ChangedSettingValue.SfxVolumes);

	UAlienGamePlayStatics::SetSFXVolumes(this, ChangedSettingValue.SfxVolumes);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnVoiceVolumeChanged(float InVolume)
{
	ChangedSettingValue.VoiceVolumes = InVolume;

	UAlienSettingSlotTypeWidget* SlotTypeWidget = VoiceVolumeSlotOption->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnVoiceVolumeChanged SlotTypeWidget"));
		return;
	}

	SlotTypeWidget->SetSliderValue(ChangedSettingValue.VoiceVolumes);
	VoiceSubSystem->SetAudioOutputVolume(ChangedSettingValue.VoiceVolumes);
	//UAlienGamePlayStatics::SetVoiceVolumes(this, ChangedSettingValue.VoiceVolumes);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnMICVolumeChanged(float InVolume)
{
	ChangedSettingValue.MicVolumes = InVolume;

	UAlienSettingSlotTypeWidget* SlotTypeWidget = MicVolumeSlotOption->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnMICVolumeChanged SlotTypeWidget"));
		return;
	}

	SlotTypeWidget->SetSliderValue(ChangedSettingValue.MicVolumes);
	VoiceSubSystem->SetAudioInputVolume(ChangedSettingValue.MicVolumes);
	//UAlienGamePlayStatics::SetVoiceVolumes(this, ChangedSettingValue.VoiceVolumes);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnUIVolumeChanged(float InVolume)
{
	ChangedSettingValue.UiVolumes = InVolume;

	UAlienSettingSlotTypeWidget* SlotTypeWidget = UIVolumeSlotOption->GetSlotTypeWidget();

	if (!IsValid(SlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnUIVolumeChanged SlotTypeWidget"));
		return;
	}

	SlotTypeWidget->SetSliderValue(ChangedSettingValue.UiVolumes);
	UAlienGamePlayStatics::SetUIVolumes(this, ChangedSettingValue.UiVolumes);

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedLeftButtonAudioQuality()
{
	ChangedSettingValue.AudioQuality = FMath::Clamp((int32)ChangedSettingValue.AudioQuality - 1, (int32)EAudioQuality::Low, (int32)EAudioQuality::High);

	UAlienSettingSlotTypeWidget* AudioQualitySlotTypeWidget = AudioQualitySlotOption->GetSlotTypeWidget();

	if (!IsValid(AudioQualitySlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnClickedLeftButtonAudioQuality AudioQualitySlotTypeWidget"));
		return;
	}

	AudioQualitySlotTypeWidget->SetListText(UAlienGamePlayStatics::EnumToString(TEXT("EAudioQuality"), ChangedSettingValue.AudioQuality));

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedRightButtonAudioQuality()
{
	ChangedSettingValue.AudioQuality = FMath::Clamp((int32)ChangedSettingValue.AudioQuality + 1, (int32)EAudioQuality::Low, (int32)EAudioQuality::High);

	UAlienSettingSlotTypeWidget* AudioQualitySlotTypeWidget = AudioQualitySlotOption->GetSlotTypeWidget();

	if (!IsValid(AudioQualitySlotTypeWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::OnClickedLeftButtonAudioQuality AudioQualitySlotTypeWidget"));
		return;
	}

	AudioQualitySlotTypeWidget->SetListText(UAlienGamePlayStatics::EnumToString(TEXT("EAudioQuality"), ChangedSettingValue.AudioQuality));

	OptionChangeTracking();
}

void UAlienPageGameSettingWidget::OnClickedEscKeyButton()
{
	UAlienGamePlayStatics::OpenSystemMenu(this);
}

void UAlienPageGameSettingWidget::ChangeOverAllScalability()
{
	ChangedSettingValue.ViewDistanceQuality = ChangedSettingValue.Scalability;
	ChangedSettingValue.ShadowQuality = ChangedSettingValue.Scalability;
	ChangedSettingValue.GlobalIlluminationQuality = ChangedSettingValue.Scalability;
	ChangedSettingValue.ReflectionQuality = ChangedSettingValue.Scalability;
	ChangedSettingValue.AntiAliasingQuality = ChangedSettingValue.Scalability;
	ChangedSettingValue.TextureQuality = ChangedSettingValue.Scalability;
	ChangedSettingValue.VisualEffectQuality = ChangedSettingValue.Scalability;
	ChangedSettingValue.PostProcessingQuality = ChangedSettingValue.Scalability;
	ChangedSettingValue.FoliageQuality = ChangedSettingValue.Scalability;
	ChangedSettingValue.ShadingQuality = ChangedSettingValue.Scalability;

	int32 Value = ChangedSettingValue.Scalability + 1;

	if (IsValid(ViewDistanceSlotOption))
	{
		UpdateScalabilityListOptionSlotWidget(ViewDistanceSlotOption->GetSlotTypeWidget(), Value);
	}
	if (IsValid(ShadowSlotOption))
	{
		UpdateScalabilityListOptionSlotWidget(ShadowSlotOption->GetSlotTypeWidget(), Value);
	}
	if (IsValid(GlobalIlluminationSlotOption))
	{
		UpdateScalabilityListOptionSlotWidget(GlobalIlluminationSlotOption->GetSlotTypeWidget(), Value);
	}
	if (IsValid(ReflectionSlotOption))
	{
		UpdateScalabilityListOptionSlotWidget(ReflectionSlotOption->GetSlotTypeWidget(), Value);
	}
	if (IsValid(AntiAliasingSlotOption))
	{
		UpdateScalabilityListOptionSlotWidget(AntiAliasingSlotOption->GetSlotTypeWidget(), Value);
	}
	if (IsValid(TextureSlotOption))
	{
		UpdateScalabilityListOptionSlotWidget(TextureSlotOption->GetSlotTypeWidget(), Value);
	}
	if (IsValid(VisualEffectSlotOption))
	{
		UpdateScalabilityListOptionSlotWidget(VisualEffectSlotOption->GetSlotTypeWidget(), Value);
	}
	if (IsValid(PostprocessingSlotOption))
	{
		UpdateScalabilityListOptionSlotWidget(PostprocessingSlotOption->GetSlotTypeWidget(), Value);
	}
	if (IsValid(FoliageSlotOption))
	{
		UpdateScalabilityListOptionSlotWidget(FoliageSlotOption->GetSlotTypeWidget(), Value);
	}
	if (IsValid(ShadingSlotOption))
	{
		UpdateScalabilityListOptionSlotWidget(ShadingSlotOption->GetSlotTypeWidget(), Value);
	}
}

bool UAlienPageGameSettingWidget::CheckScalabilityOption()
{
	if (ChangedSettingValue.Scalability == ChangedSettingValue.ViewDistanceQuality
		&& ChangedSettingValue.Scalability == ChangedSettingValue.ShadowQuality
		&& ChangedSettingValue.Scalability == ChangedSettingValue.GlobalIlluminationQuality
		&& ChangedSettingValue.Scalability == ChangedSettingValue.ReflectionQuality
		&& ChangedSettingValue.Scalability == ChangedSettingValue.AntiAliasingQuality
		&& ChangedSettingValue.Scalability == ChangedSettingValue.TextureQuality
		&& ChangedSettingValue.Scalability == ChangedSettingValue.VisualEffectQuality
		&& ChangedSettingValue.Scalability == ChangedSettingValue.PostProcessingQuality
		&& ChangedSettingValue.Scalability == ChangedSettingValue.FoliageQuality
		&& ChangedSettingValue.Scalability == ChangedSettingValue.ShadingQuality
		)
	{
		return true;
	}

	return false;
}

void UAlienPageGameSettingWidget::UpdateListOptionSlotWidget(const FString& TitleString, UAlienSettingSlotTypeWidget* SlotWidget, int32 Value)
{
	if (!IsValid(SlotWidget))
	{
		ALIEN_LOG(Warning, TEXT("Null Fail: UAlienPageGameSettingWidget::UpdateScalabilityListOptionSlotWidget SlotWidget"));
		return;
	}
	wchar_t FormatStr[256];

	wcscpy_s(FormatStr, TCHAR_TO_WCHAR(*TitleString));
	// 가변파라미터를 어떻게 적용 해줄 것..?
	FString TitleStr = FString::Printf(FormatStr, Value);

	SlotWidget->SetListTableText(TitleStr);
}

void UAlienPageGameSettingWidget::UpdateScalabilityListOptionSlotWidget(UAlienSettingSlotTypeWidget* SlotWidget, int32 Value)
{
	if (Value <= 0)
	{
		UpdateListOptionSlotWidget(TEXT("UI_Setting_Value_Custom"), SlotWidget, Value);
	}
	else
	{
		UpdateListOptionSlotWidget(TEXT("UI_Setting_Value_%d"), SlotWidget, Value);
	}
}

void UAlienPageGameSettingWidget::SwapAudioOutputResult(const FSwapAudioOutputResult& SwapResult)
{

}
