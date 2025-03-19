// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Page/AlienPageBase.h"
#include "AlienSettingSlotWidget.h"
#include "AlienSettingSlotTypeWidget.h"
#include "../../AudioMixer/Public/AudioMixerBlueprintLibrary.h"
#include "AlienPageGameSettingWidget.generated.h"



enum class ELanguageType : uint8;

USTRUCT()
struct FAlienGameSettingValue
{
	GENERATED_BODY()

	FAlienGameSettingValue()
	{
		MouseSensitive = 1.0f;
		MouseZoomSensitive = 1.0f;
		WindowMode = EWindowMode::Fullscreen;
		Resolution = FIntPoint(1920, 1080);
		Scalability = 2;
		MasterVolumes = 1.0f;
		BgmVolumes = 1.0f;
		SfxVolumes = 1.0f;
		VoiceVolumes = 1.0f;
		MicVolumes = 1.0f;
		UiVolumes = 1.0f;
		Language = ELanguageType::Kr;
		bAchievementAlert = true;
		bVSync = false;
		Gamma = 2.2f;
		ResolutionScale = .5f;
		FrameRate = .0f;
		AudioQuality = 0;
		OutputDeviceIndex = 0;
		InputDeviceIndex = 0;
		AudioQuality = Scalability;
		ViewDistanceQuality = Scalability;
		ShadowQuality = Scalability;
		GlobalIlluminationQuality = Scalability;
		ReflectionQuality = Scalability;
		AntiAliasingQuality = Scalability;
		TextureQuality = Scalability;
		VisualEffectQuality = Scalability;
		PostProcessingQuality = Scalability;
		FoliageQuality = Scalability;
		ShadingQuality = Scalability;
	}

	bool operator==(const FAlienGameSettingValue& InValue) const
	{
		return MouseSensitive == InValue.MouseSensitive 
			&& MouseZoomSensitive == InValue.MouseZoomSensitive
			&& WindowMode == InValue.WindowMode
			&& Resolution == InValue.Resolution
			&& Scalability == InValue.Scalability
			&& MasterVolumes == InValue.MasterVolumes
			&& BgmVolumes == InValue.BgmVolumes
			&& SfxVolumes == InValue.SfxVolumes
			&& VoiceVolumes == InValue.VoiceVolumes
			&& MicVolumes == InValue.MicVolumes
			&& UiVolumes == InValue.UiVolumes
			&& Language == InValue.Language
			&& bAchievementAlert == InValue.bAchievementAlert
			&& Gamma == InValue.Gamma
			&& bVSync == InValue.bVSync
			&& ResolutionScale == InValue.ResolutionScale
			&& FrameRate == InValue.FrameRate
			&& OutputDeviceIndex == InValue.OutputDeviceIndex
			&& InputDeviceIndex == InValue.InputDeviceIndex
			&& AudioQuality == InValue.AudioQuality
			&& ViewDistanceQuality == InValue.ViewDistanceQuality
			&& ShadowQuality == InValue.ShadowQuality
			&& GlobalIlluminationQuality == InValue.GlobalIlluminationQuality
			&& ReflectionQuality == InValue.ReflectionQuality
			&& AntiAliasingQuality == InValue.AntiAliasingQuality
			&& TextureQuality == InValue.TextureQuality
			&& VisualEffectQuality == InValue.VisualEffectQuality
			&& PostProcessingQuality == InValue.PostProcessingQuality
			&& FoliageQuality == InValue.FoliageQuality
			&& ShadingQuality == InValue.ShadingQuality;
	}

	float MouseSensitive;
	float MouseZoomSensitive;
	int32 WindowMode;
	FIntPoint Resolution;
	int32 Scalability;
	float MasterVolumes;
	float BgmVolumes;
	float SfxVolumes;
	float VoiceVolumes;
	float MicVolumes;
	float UiVolumes;
	ELanguageType Language;
	bool bAchievementAlert;
	float Gamma;

	bool bVSync;
	float ResolutionScale;
	float FrameRate;
	int32 OutputDeviceIndex;
	int32 InputDeviceIndex;
	int32 AudioQuality;
	int32 ViewDistanceQuality;
	int32 ShadowQuality;
	int32 GlobalIlluminationQuality;
	int32 ReflectionQuality;
	int32 AntiAliasingQuality;
	int32 TextureQuality;
	int32 VisualEffectQuality;
	int32 PostProcessingQuality;
	int32 FoliageQuality;
	int32 ShadingQuality;
};


class UWidgetSwitcher;
class UAlienSettingCategoryWidget;
class UVerticalBox;
class UAlienTextBlock;
class UAlienRichTextBlock;
class UPageUIInfoBase;
class UAlienLobbyTabMenuButton;
class UAlienSettingSlotMainWidget;
class UAlienSettingSlotWidget;
class UAlienLobbyBottomMenuWidget;
class UAlienLobbyBottomMenuButton;

/**
 * 
 */
UCLASS()
class ALIEN_API UAlienPageGameSettingWidget : public UAlienPageBase
{
	GENERATED_BODY()

public:
	UAlienPageGameSettingWidget();

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void Open(class UPageUIInfoBase* PageUIInfo) override;
	virtual void Close() override;

	void CreateSlotMainWidget(const FString& Title, TObjectPtr<UAlienSettingSlotMainWidget>& OutWidget);
	void CreateSlotSliderOptionWidget(const FString& TitleText, const FString& SubText, ESettingType Type, float InMin, float InMax, float InInitValue, const TObjectPtr<UAlienSettingSlotMainWidget>& ParentWidget, TObjectPtr<UAlienSettingSlotWidget>& OutWidget, const FSettingSliderValueChangedFunc::FDelegate& EnterSliderFunc);
	void CreateSlotListOptionWidget(const FString& TitleText, const FString& SubText, ESettingType Type, const TObjectPtr<UAlienSettingSlotMainWidget>& ParentWidget, TObjectPtr<UAlienSettingSlotWidget>& OutWidget, const FSettingButtonClickedFunc::FDelegate& LeftButtonFunc, const FSettingButtonClickedFunc::FDelegate& RightButtonFunc);
	void CreateSlotToggleOptionWidget(const FString& TitleText, const FString& SubOnText, const FString& SubOffText, ESettingType Type, bool InbValue, const TObjectPtr<UAlienSettingSlotMainWidget>& ParentWidget, TObjectPtr<UAlienSettingSlotWidget>& OutWidget, const FSettingCheckStateChangedFunc::FDelegate& ClickedOnFunc, const FSettingCheckStateChangedFunc::FDelegate& ClickedOffFunc);

	UFUNCTION()
	void LoadSettings();

	UFUNCTION()
	void ApplySettings();

	UFUNCTION()
	void RevertSettings();

	UFUNCTION()
	void DefaultSettings();

	UFUNCTION()
	void OnClickedSaveChangeCancel();

	UFUNCTION()
	void UpdateUI();

	void OptionChangeTracking();
	bool IsOptionChange();
	void SetResolutionList(EWindowMode::Type InWindowMode);
	void UpdateResolutionOptionText(int32 InResIndex);

	// �ػ� ���� �˾��� ���� ����, ���� �ػ� ���� Ȥ�� �����ǵ�����.
	UFUNCTION()
	void OnPopupResolutionRevertOK();
	UFUNCTION()
	void OnPopupResolutionConfirmOK();

	void SwitchCategoryOrContents(bool bShowCategory);

	void InitGameplaySettings();
	void InitVideoSettings();
	void InitAudioSettings();

	UFUNCTION()
	void OnClickedGamePlaySetting();
	UFUNCTION()
	void OnClickedVideoSetting();
	UFUNCTION()
	void OnClickedAudioSetting();
	UFUNCTION()
	void OnClickedMouseAndKeyboardSetting();

	UFUNCTION()
	void OnClickedBackButton();
	UFUNCTION()
	void OnClickedApplySettingButton();
	UFUNCTION()
	void OnClickedResetSettingButton();

	UFUNCTION()
	void OnClickedRevertSettingButton();


	// �ɼǿ� ���콺�� �ö󰡰ų� ������ ��, ���� ǥ�� ���ε�
	UFUNCTION()
	void OnMouseEnterDescFunc(ESettingType SettingType);
	UFUNCTION()
	void OnMouseLeaveDescFunc();


	// ���콺 ���� ���� ���� ���ε�
	UFUNCTION()
	void OnMouseSensitiveChanged(float InSensitive);

	// ���콺 �� ���� ���� ���� ���ε�
	UFUNCTION()
	void OnMouseZoomSensitiveChanged(float InSensitive);

	// ������ ��� ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonScreenMode();

	UFUNCTION()
	void OnClickedRightButtonScreenMode();


	// �ػ� ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonResolution();

	UFUNCTION()
	void OnClickedRightButtonResolution();

	// �ػ� ������
	UFUNCTION()
	void OnSliderChangedResolutionScale(float InValue);

	// ����
	UFUNCTION()
	void OnSliderChangedGamma(float InValue);

	// �����ӷ���Ʈ
	UFUNCTION()
	void OnSliderChangedFrameRate(float InValue);

	// ��������
	UFUNCTION()
	void OnCheckedOnVSync(bool IsChecked);

	UFUNCTION()
	void OnCheckedOffVSync(bool IsChecked);

	// ��Į�����Ƽ ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonScalability();

	UFUNCTION()
	void OnClickedRightButtonScalability();

	// �þ�ǰ�� ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonViewDistance();

	UFUNCTION()
	void OnClickedRightButtonViewDistance();

	// �׸��� ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonShadow();

	UFUNCTION()
	void OnClickedRightButtonShadow();

	// ���� ���� ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonGlobalIllumination();

	UFUNCTION()
	void OnClickedRightButtonGlobalIllumination();

	// �ݻ� ǰ�� ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonReflection();

	UFUNCTION()
	void OnClickedRightButtonReflection();

	// AntiAliasing ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonAntiAliasing();

	UFUNCTION()
	void OnClickedRightButtonAntiAliasing();

	// �ؽ��� ǰ�� ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonTexture();

	UFUNCTION()
	void OnClickedRightButtonTexture();

	// VisualEffect ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonVisualEffect();

	UFUNCTION()
	void OnClickedRightButtonVisualEffect();

	// PostProcessing ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonPostProcessing();

	UFUNCTION()
	void OnClickedRightButtonPostProcessing();

	// Foliage ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonFoliage();

	UFUNCTION()
	void OnClickedRightButtonFoliage();

	// ���̵� ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonShading();

	UFUNCTION()
	void OnClickedRightButtonShading();


	// ��� ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonLanguage();
	UFUNCTION()
	void OnClickedRightButtonLanguage();

	// ���� �˸�
	UFUNCTION()
	void OnCheckedOnAchievementAlert(bool IsChecked);

	UFUNCTION()
	void OnCheckedOffAchievementAlert(bool IsChecked);

	// ����� �����ġ ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonAudioOutput();

	UFUNCTION()
	void OnClickedRightButtonAudioOutput();

	// ����� �Է���ġ ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonAudioInput();

	UFUNCTION()
	void OnClickedRightButtonAudioInput();
	// ������ ���� �����̴� ���ε�
	UFUNCTION()
	void OnMasterVolumeChanged(float InVolume);

	UFUNCTION()
	void OnBGMVolumeChanged(float InVolume);

	UFUNCTION()
	void OnSFXVolumeChanged(float InVolume);

	UFUNCTION()
	void OnVoiceVolumeChanged(float InVolume);
	UFUNCTION()
	void OnMICVolumeChanged(float InVolume);
	UFUNCTION()
	void OnUIVolumeChanged(float InVolume);

	// ���� ǰ�� ���� ��ư ���ε�
	UFUNCTION()
	void OnClickedLeftButtonAudioQuality();
	UFUNCTION()
	void OnClickedRightButtonAudioQuality();

	UFUNCTION()
	void OnClickedEscKeyButton();

public:
	// Overall ���� �� ���.
	void ChangeOverAllScalability();

	// Scalability ���� �ɼ� ���� �� üũ�� �� ���.
	bool CheckScalabilityOption();

	void UpdateListOptionSlotWidget(const FString& TitleString, UAlienSettingSlotTypeWidget* SlotWidget, int32 Value);
	void UpdateScalabilityListOptionSlotWidget(UAlienSettingSlotTypeWidget* SlotWidget, int32 Value);

	UFUNCTION()
	void SwapAudioOutputResult(const FSwapAudioOutputResult& SwapResult); 
private:
	UPROPERTY(transient)
	TObjectPtr<UAlienTextBlock> m_TitleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString TitleTextStr;

	UPROPERTY(transient)
	TObjectPtr<UWidgetSwitcher> m_SettingSwitcher;

	UPROPERTY(Transient)
	TObjectPtr<UAlienLobbyTabMenuButton> m_GameplayButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString GameplayButtonStr;

	UPROPERTY(Transient)
	TObjectPtr<UAlienLobbyTabMenuButton> m_VideoButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString VideoButtonStr;

	UPROPERTY(Transient)
	TObjectPtr<UAlienLobbyTabMenuButton> m_AudioButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString AudioButtonStr;

	UPROPERTY(Transient)
	TObjectPtr<UAlienLobbyTabMenuButton> m_InputButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString InputButtonStr;

	UPROPERTY(transient)
	TObjectPtr<UAlienSettingCategoryWidget> m_GameplaySettingCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString GameplaySettingCategoryStr;

	UPROPERTY(transient)
	TObjectPtr<UAlienSettingCategoryWidget> m_VideoCategory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString VideoCategoryStr;

	UPROPERTY(transient)
	TObjectPtr<UAlienSettingCategoryWidget> m_AudioCategory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString AudioCategoryStr;
	UPROPERTY(transient)
	TObjectPtr<UAlienSettingCategoryWidget> m_MouseCategory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString MouseCategoryStr;


	///////switch2

	UPROPERTY(transient)
	TObjectPtr<UVerticalBox> m_SlotBox;

	UPROPERTY(transient)
	TObjectPtr<UAlienRichTextBlock> m_SettingInfoText;

	UPROPERTY(transient)
	TObjectPtr<UAlienLobbyBottomMenuWidget> m_BottomMenu;

	UPROPERTY()
	TObjectPtr<UAlienLobbyBottomMenuButton> BackButton;	
	UPROPERTY()
	TObjectPtr<UAlienLobbyBottomMenuButton> ApplySettingButton;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString ApplySettingButtonStr;
	UPROPERTY()
	TObjectPtr<UAlienLobbyBottomMenuButton> RevertSettingButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString RevertSettingButtonStr;
	UPROPERTY()
	TObjectPtr<UAlienLobbyBottomMenuButton> ResetSettingButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="String", meta=(AllowPrivateAccess = "true"))
	FString ResetSettingButtonStr;
	
	UPROPERTY()
	TObjectPtr<UAlienLobbyBottomMenuButton> EscMenuButton;




	/////////////////
	// ������ ����.

	UPROPERTY()
	TArray<TObjectPtr<UAlienSettingSlotMainWidget>> GameplaySlotArr;

	UPROPERTY()
	TArray<TObjectPtr<UAlienSettingSlotMainWidget>> VideoSlotArr;

	UPROPERTY()
	TArray<TObjectPtr<UAlienSettingSlotMainWidget>> AudioSlotArr;



	// ���콺 ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotMainWidget> MouseSensitiveSlot;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> MouseSensitiveSlotOption;

	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> MouseZoomSensitiveSlotOption;

	//// ��ũ�� �ɼ� ����

	UPROPERTY()
	TObjectPtr<UAlienSettingSlotMainWidget> DisplayMainSlot;

	// ���÷��� ���
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> ModeSlotOption;
	// ���÷��� �ػ�
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> ResolutionSlotOption;
	// �ػ� ������ �ػ�
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> ResolutionScaleSlotOption;

	// ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> GammaSlotOption;

	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> FrameRateSlotOption;

	// ��������
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> VSyncSlotOption;

	// �׷��Ƚ� ��Į������Ƽ
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotMainWidget> ScalabilitySlot;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> ScalabilitySlotOption;

	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> ViewDistanceSlotOption;

	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> ShadowSlotOption;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> GlobalIlluminationSlotOption;

	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> ReflectionSlotOption;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> AntiAliasingSlotOption;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> TextureSlotOption;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> VisualEffectSlotOption;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> PostprocessingSlotOption;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> FoliageSlotOption; 
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> ShadingSlotOption;

	// ��� ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotMainWidget> LanguageSlot;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> LanguageSlotOption;

	// �����˸� ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotMainWidget> AchievementAlertSlot;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> AchievementAlertSlotOption;

	// ����� ��� ��ġ
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotMainWidget> AudioOutputSlot;

	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> AudioOutputSlotOption;

	// ����� �Է� ��ġ
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotMainWidget> AudioInputSlot;

	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> AudioInputSlotOption;

	// ���̽� ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> VoiceVolumeSlotOption;

	// ����ũ ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> MicVolumeSlotOption;

	// ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotMainWidget> VolumeSlot;
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> AudioQualitySlotOption;

	// ������ ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> MasterVolumeSlotOption;
	// ����� ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> BGMVolumeSlotOption;
	// ȿ���� ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> SFXVolumeSlotOption;
	// UI ����
	UPROPERTY()
	TObjectPtr<UAlienSettingSlotWidget> UIVolumeSlotOption;


	// EOS Voice Plugin SubSystem
	UPROPERTY()
	TWeakObjectPtr<class UEOSVoiceSubsystem> VoiceSubSystem;

	UPROPERTY()
	TArray<struct FEOSVoiceChatDeviceInfo> InputDeviceInfos;
	UPROPERTY()
	TArray<struct FEOSVoiceChatDeviceInfo> OutputDeviceInfos;

	// ���� ����
	FAlienGameSettingValue OldSettingValue;
	FAlienGameSettingValue ChangedSettingValue;
	FAlienGameSettingValue LastSettingValue;
	TArray<FIntPoint> ResolutionList;
	int32 ResolutionListIndex;
	TArray<FString> LocalizedCulturesList;

	bool bCloseReady;

	FOnCompletedDeviceSwap OnCompletedDeviceSwap;
};
