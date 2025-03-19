// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/AlienGameUserSettings.h"
#include "Engine/Engine.h"
#include "../Common/AlienGamePlayStatics.h"

UAlienGameUserSettings::UAlienGameUserSettings()
{
	MouseSensitive = 1.0f;
	MouseZoomSensitive = 1.0f;
	MasterVolume = 1.0f;
	BgmVolume = 1.0f;
	SfxVolume = 1.0f;
	UiVolume = 1.0f;
	VoiceVolume = 1.0f;
	MicVolume = 1.0f;
	Gamma = 2.2f;
	AchievementAlert = true;
}

UAlienGameUserSettings* UAlienGameUserSettings::Get()
{
	return GEngine ? CastChecked<UAlienGameUserSettings>(GEngine->GetGameUserSettings()) : nullptr;
}

void UAlienGameUserSettings::BeginDestroy()
{
	Super::BeginDestroy();
}

void UAlienGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);

	BroadcastMouseSensitiveDeleagte();
}

void UAlienGameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	ApplyGamma();
}

void UAlienGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	MouseSensitive = 1.0f;
	MouseZoomSensitive = 1.0f;
	MasterVolume = 1.0f;
	BgmVolume = 1.0f;
	SfxVolume = 1.0f;
	UiVolume = 1.0f;
	VoiceVolume = 1.0f;
	MicVolume = 1.0f;
	Gamma = 2.2f;
	AchievementAlert = true;
	Language = 0;
	UAlienGamePlayStatics::SetMasterVolumes(GetWorld(), MasterVolume);
	UAlienGamePlayStatics::SetBGMVolumes(GetWorld(), BgmVolume);
	UAlienGamePlayStatics::SetSFXVolumes(GetWorld(), SfxVolume);
	UAlienGamePlayStatics::SetVoiceVolumes(GetWorld(), VoiceVolume);
	BroadcastMouseSensitiveDeleagte();
	BroadcastMouseZoomSensitiveDeleagte();
	ApplyGamma();
}

void UAlienGameUserSettings::ConfirmVideoMode()
{
	Super::ConfirmVideoMode();
}

float UAlienGameUserSettings::GetEffectiveFrameRateLimit()
{
	return Super::GetEffectiveFrameRateLimit();
}

void UAlienGameUserSettings::ResetToCurrentSettings()
{
	Super::ResetToCurrentSettings();
}

void UAlienGameUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();

	BroadcastMouseSensitiveDeleagte();
	BroadcastMouseZoomSensitiveDeleagte();
}

int32 UAlienGameUserSettings::GetOverallScalabilityLevel() const
{
	int32 Result = Super::GetOverallScalabilityLevel();

	return Result;
}

void UAlienGameUserSettings::SetOverallScalabilityLevel(int32 Value)
{
	Super::SetOverallScalabilityLevel(Value);
}

void UAlienGameUserSettings::SetGamma(float InValue)
{
	Gamma = InValue;
	ApplyGamma();
}

void UAlienGameUserSettings::ApplyGamma()
{
	if (GEngine)
	{
		GEngine->DisplayGamma = Gamma;
	}
}

void UAlienGameUserSettings::BroadcastMouseSensitiveDeleagte()
{
	if (UpdateSettingMouseSensitiveDelegate.IsBound())
	{
		UpdateSettingMouseSensitiveDelegate.Broadcast(MouseSensitive);
	}
}

void UAlienGameUserSettings::BroadcastMouseZoomSensitiveDeleagte()
{
	if (UpdateSettingMouseZoomSensitiveDelegate.IsBound())
	{
		UpdateSettingMouseZoomSensitiveDelegate.Broadcast(MouseZoomSensitive);
	}
}
