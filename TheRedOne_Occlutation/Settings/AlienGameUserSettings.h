// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "AlienGameUserSettings.generated.h"

class UAlienLocalPlayer;
class UObject;

/**
 * GameUserSettings, 참고 자료 Lyra.
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateSettingMouseSensitiveDelegate, float);



UCLASS()
class UAlienGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	UAlienGameUserSettings();

	static UAlienGameUserSettings* Get();
	// UObject interface
	virtual void BeginDestroy() override;
	// GameUserSettings interface
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	virtual void LoadSettings(bool bForceReload) override;
	virtual void SetToDefaults() override;
	virtual void ConfirmVideoMode() override;
	virtual float GetEffectiveFrameRateLimit() override;
	virtual void ResetToCurrentSettings() override;
	virtual void ApplyNonResolutionSettings() override;
	virtual int32 GetOverallScalabilityLevel() const override;
	virtual void SetOverallScalabilityLevel(int32 Value) override;


	float GetMouseSensitive() const { return MouseSensitive; }
	void SetMouseSensitive(float InValue) { MouseSensitive = InValue; }

	float GetMouseZoomSensitive() const { return MouseZoomSensitive; }
	void SetMouseZoomSensitive(float InValue) { MouseZoomSensitive = InValue; }

	float GetMasterVolume() const { return MasterVolume; }
	void SetMasterVolume(float InVolume) { MasterVolume = InVolume; }

	float GetBGMVolume() const { return BgmVolume; }
	void SetBGMVolume(float InVolume) { BgmVolume = InVolume; }

	float GetSFXVolume() const { return SfxVolume; }
	void SetSFXVolume(float InVolume) { SfxVolume = InVolume; }

	float GetVoiceVolume() const { return VoiceVolume; }
	void SetVoiceVolume(float InVolume) { VoiceVolume = InVolume; }

	float GetMicVolume() const { return VoiceVolume; }
	void SetMicVolume(float InVolume) { VoiceVolume = InVolume; }

	float GetUIVolume() const { return UiVolume; }
	void SetUIVolume(float InVolume) { UiVolume = InVolume; }

	int32 GetLanguage() const { return Language; }
	void SetLanguage(int32 InLanguage) { Language = InLanguage; }

	bool GetAchievementAlert() const { return AchievementAlert; }
	void SetAchievementAlert(bool bValue) { AchievementAlert = bValue; }

	float GetGamma() const {return Gamma; }
	void SetGamma(float InValue);

	void ApplyGamma();

	void BroadcastMouseSensitiveDeleagte();
	void BroadcastMouseZoomSensitiveDeleagte();
public:
	FUpdateSettingMouseSensitiveDelegate UpdateSettingMouseSensitiveDelegate;
	FUpdateSettingMouseSensitiveDelegate UpdateSettingMouseZoomSensitiveDelegate;

private:
	UPROPERTY(config)
	float MouseSensitive;
	UPROPERTY(config)
	float MouseZoomSensitive;
	UPROPERTY(config)
	float MasterVolume;
	UPROPERTY(config)
	float BgmVolume;
	UPROPERTY(config)
	float SfxVolume;
	UPROPERTY(config)
	float UiVolume;
	UPROPERTY(config)
	float VoiceVolume;
	UPROPERTY(config)
	float MicVolume;
	UPROPERTY(config)
	int32 Language;
	UPROPERTY(config)
	bool AchievementAlert;
	UPROPERTY(config)
	float Gamma;

};
