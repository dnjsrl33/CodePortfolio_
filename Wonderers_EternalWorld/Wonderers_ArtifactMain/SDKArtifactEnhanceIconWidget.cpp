// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SDKArtifactEnhanceIconWidget.h"
#include "UI/SDKUIArtifactEnhanceWidget.h"
#include "UI/SDKPopupArtifactInfoWidget.h"

#include "Character/SDKHUD.h"
#include "Character/SDKMyInfo.h"

#include "Engine/SDKGameInstance.h"
#include "Manager/SDKTableManager.h"

#include "Share/SDKHelper.h"

#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetSwitcher.h"

#include "Animation/WidgetAnimation.h"


void USDKArtifactEnhanceIconWidget::CreateUIProcess()
{
	BindImages();

	RateUp = GetWidgetAnimation(TEXT("ArtifactRateUp"));

	if (Button_Artifact->OnClicked.Contains(this, TEXT("OnSelectedArtifactIcon")) == false)
	{
		Button_Artifact->OnClicked.AddDynamic(this, &USDKArtifactEnhanceIconWidget::OnSelectedArtifactIcon);
	}
}

void USDKArtifactEnhanceIconWidget::CloseUIProcess()
{
	Super::CloseUIProcess();
}

// 아이콘 선택 시 호출
void USDKArtifactEnhanceIconWidget::OnSelectedArtifactIcon()
{
	ASDKHUD* SDKHUD = Cast<ASDKHUD>(GetOwningPlayer()->GetHUD());
	if (IsValid(SDKHUD) == true)
	{
		if (SDKHUD->IsOpenUI(EUI::UI_ArtifactEnhance))
		{
			USDKUIArtifactEnhanceWidget* SDKMainWidget = Cast<USDKUIArtifactEnhanceWidget>(SDKHUD->GetUI(EUI::UI_ArtifactEnhance));
			if (IsValid(SDKMainWidget) == true)
			{
				SDKMainWidget->SetCurrentSelectedArtifact(ArtifactParam);
			}
		}
	}
}

// 아이콘 위젯 생성 시 호출출
void USDKArtifactEnhanceIconWidget::NativeOnListItemObjectSet(UObject* ListItemData)
{
	if (IsValid(ListItemData) == false || IsValid(Cast<USDKWidgetParamArtifactEnhance>(ListItemData)) == false)
	{
		UE_LOG(LogWidget, Log, TEXT("ListItemObject is Null"));
		return;
	}

	USDKWidgetParamArtifactEnhance* Param = Cast<USDKWidgetParamArtifactEnhance>(ListItemData);
	ArtifactParam = Param;
	SetArtifactIcon(Param);

	if (Button_Artifact->OnClicked.Contains(this, TEXT("OnSelectedArtifactIcon")) == false)
	{
		Button_Artifact->OnClicked.AddDynamic(this, &USDKArtifactEnhanceIconWidget::OnSelectedArtifactIcon);
	}
}

void USDKArtifactEnhanceIconWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	SetSelectItemInfo(bIsSelected);
}

void USDKArtifactEnhanceIconWidget::SetArtifactIcon(UObject* ObjectData)
{
	BindImages();

	USDKWidgetParamArtifactEnhance* _ArtifactData = Cast<USDKWidgetParamArtifactEnhance>(ObjectData);

	if (IsValid(_ArtifactData))
	{
		ArtifactParam = _ArtifactData;
		ArtifactData = _ArtifactData->GetArtifactData();
	}

	// 이미지 설정 및 각성 별 설정
	FS_Artifact* ArtifactTable = USDKTableManager::Get()->FindTableRow<FS_Artifact>(ETableType::tb_Artifact, FString::FromInt(_ArtifactData->GetArtifactID()));
	if (ArtifactTable != nullptr)
	{
		SetVisibilityInfoType(_ArtifactData->GetDeckSetting());
		SetImageTexturePath(Image_Artifact, ArtifactTable->IconPath.ToString());
		SetWidgetVisibility(WidgetSwitcher_InfoOrDeck, ESlateVisibility::SelfHitTestInvisible);

		FS_ArtifactUpgrade* UpgradeTable = USDKTableManager::Get()->FindTableRow<FS_ArtifactUpgrade>(ETableType::tb_ArtifactUpgrade, FString::FromInt((int32)ArtifactTable->Grade));
		if (UpgradeTable != nullptr)
		{
			int32 EnhanceIndex = _ArtifactData->GetArtifactEnhance();

			SetWidgetVisibility(BorderGrade, ESlateVisibility::SelfHitTestInvisible);
			SetBorderGradeTexture(BorderGrade, ArtifactTable->Grade);

			SetTextBlockText(TextArtifactCost, NumberToText(ArtifactTable->Price));
			SetWidgetVisibility(TextEnhance, ArtifactData.Enhance > 0 ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
			SetTextBlockText(TextEnhance, CountToText(_ArtifactData->GetArtifactEnhance(), false));

			// 중복 갯수에 따른 PrograssBar 설정
			if (ArtifactTable->MaxEnhance == _ArtifactData->GetArtifactEnhance())
			{
				// 최대 각성 일때.
				ArtifactStackPrograss->WidgetStyle.FillImage.TintColor = DesignFormatToLinearColor(EDesignFormat::QuestBoard_Complete);
				SetWidgetVisibility(RateUpIcon, ESlateVisibility::Collapsed);

				//애니메이션 설정
				StopWidgetAnimation(RateUp);


				SetWidgetSwitcherActiveWidgetIndex(WidgetSwitcher_PrograssText, 2);
				SetTextBlockTableText(TextStackPercent_Max, TEXT("UIText_Artifact_MaxLevel"));
				ArtifactStackPrograss->SetPercent(1.0f);
			}
			else
			{

				int32 ReqStackCount = UpgradeTable->ReqArtifactCount[EnhanceIndex];

				if (ReqStackCount <= ArtifactData.Stack)
				{
					// 각성 가능한 상태.
					SetWidgetVisibility(RateUpIcon, ESlateVisibility::Visible);
					ArtifactStackPrograss->WidgetStyle.FillImage.TintColor = DesignFormatToLinearColor(EDesignFormat::QuestBoard_Complete);

					SetWidgetSwitcherActiveWidgetIndex(WidgetSwitcher_PrograssText, 1);
					SetTextBlockText(TextStackPercent_Full, CountToText(ArtifactData.Stack, ReqStackCount));
					//애니메이션 설정
					PlayWidgetAnimationLoop(RateUp);

				}
				else
				{
					// 각성 불가능
					ArtifactStackPrograss->WidgetStyle.FillImage.TintColor = DesignFormatToLinearColor(EDesignFormat::QuestBoard_Progress);

					SetWidgetSwitcherActiveWidgetIndex(WidgetSwitcher_PrograssText, 0);
					SetTextBlockText(TextStackPercent, CountToText(ArtifactData.Stack, ReqStackCount));

					//애니메이션 설정
					SetWidgetVisibility(RateUpIcon, ESlateVisibility::Collapsed);
					StopWidgetAnimation(RateUp);
				}

				ArtifactStackPrograss->SetPercent(FMath::Clamp((float)ArtifactData.Stack / (float)ReqStackCount, 0.0f, 1.0f));

				SetWidgetVisibility(ArtifactStackOverlay, ESlateVisibility::SelfHitTestInvisible);
			}
		}

		FS_ItemSet* SetTable = USDKTableManager::Get()->FindTableRow<FS_ItemSet>(ETableType::tb_ItemSet, FString::FromInt(ArtifactTable->ArtifactSet[0]));
		if (SetTable != nullptr)
		{
			SetImageTexturePath(SetIconImage, SetTable->IconPath.ToString());
			SetIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}


		USDKGameInstance* SDKGameInstance = GetGameInstance<USDKGameInstance>();
		if (SDKGameInstance != nullptr)
		{
			SetWidgetVisibility(StatusNew, ESlateVisibility::Collapsed);
			SetWidgetVisibility(StatusUpgrade, ESlateVisibility::Collapsed);

			if (_ArtifactData->GetIsCollect())
			{
				WidgetSwitcher_Enabled->SetActiveWidgetIndex(0);
				SetWidgetVisibility(WidgetSwitcher_InfoOrDeck, ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				//가지고 있지 않음
				WidgetSwitcher_Enabled->SetActiveWidgetIndex(1);
				SetWidgetVisibility(TextEnhance, ESlateVisibility::Collapsed);
				SetWidgetVisibility(WidgetSwitcher_InfoOrDeck, ESlateVisibility::Collapsed);
			}
		}
	}
}

void USDKArtifactEnhanceIconWidget::SetArtifactIcon(FArtifactData _ArtifactData)
{
	BindImages();

	//if (_ArtifactData.Stack == 0)
	//{
	//	// 프리셋에 비어있는 슬롯.
	//	SetEmptyIcon();
	//}
	//else
	{
		// 설정되어있는 슬롯.
		SetVisibility(ESlateVisibility::Visible);
		SetWidgetVisibility(Image_Artifact, ESlateVisibility::Visible);
		SetWidgetVisibility(ArtifactStackOverlay, ESlateVisibility::Collapsed);
		WidgetSwitcher_Enabled->SetActiveWidgetIndex(0);
		SetWidgetVisibility(WidgetSwitcher_InfoOrDeck, ESlateVisibility::SelfHitTestInvisible);
		ArtifactData = _ArtifactData;
		
		// 각성 레벨
		int32 EnhanceIndex = ArtifactData.Enhance;

		FS_Artifact* ArtifactTable = USDKTableManager::Get()->FindTableRow<FS_Artifact>(ETableType::tb_Artifact, FString::FromInt(_ArtifactData.ArtifactID));
		if (ArtifactTable != nullptr)
		{
			if (IsValid(ArtifactParam) == false)
			{
				ArtifactParam = NewObject<USDKWidgetParamArtifactEnhance>(this, USDKWidgetParamArtifactEnhance::StaticClass());
			}

			ArtifactParam->SetArtifactData(_ArtifactData);
			ArtifactParam->SetArtifactSetIDs(ArtifactTable->ArtifactSet);

			SetImageTexturePath(Image_Artifact, ArtifactTable->IconPath.ToString());

			SetWidgetVisibility(TextEnhance, ArtifactData.Enhance > 0 ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
			SetTextBlockText(TextEnhance, CountToText(ArtifactData.Enhance, false));
			// Global Define에서 각 각성등급 상승에 대한 재료의 배열 갯수로 맥스를 파악
			FS_ArtifactUpgrade* UpgradeTable = USDKTableManager::Get()->FindTableRow<FS_ArtifactUpgrade>(ETableType::tb_ArtifactUpgrade, FString::FromInt((int32)ArtifactTable->Grade));
			if (UpgradeTable != nullptr)
			{
				SetWidgetVisibility(BorderGrade, ESlateVisibility::SelfHitTestInvisible);
				SetBorderGradeTexture(BorderGrade, ArtifactTable->Grade);

				// 중복 갯수에 따른 PrograssBar 설정
				if (ArtifactData.Enhance == ArtifactTable->MaxEnhance)
				{
					// 최대 각성 일때.
					ArtifactStackPrograss->WidgetStyle.FillImage.TintColor = DesignFormatToLinearColor(EDesignFormat::QuestBoard_Complete);
					ArtifactStackPrograss->SetPercent(1.0f);

					SetTextBlockTableText(TextStackPercent, TEXT("UIText_Artifact_MaxLevel")); // 최대 각성
				}
				else
				{
					int32 ReqStackCount = UpgradeTable->ReqArtifactCount[EnhanceIndex];

					ArtifactStackPrograss->SetFillColorAndOpacity(DesignFormatToLinearColor(EDesignFormat::QuestBoard_Progress));

					if (ReqStackCount <= ArtifactData.Stack)
					{
						// 각성 가능한 상태.
						//SetWidgetVisibility(RateUpIcon, ESlateVisibility::Visible);

						////애니메이션 설정
						//PlayWidgetAnimationLoop(RateUp);
						ArtifactStackPrograss->WidgetStyle.FillImage.TintColor = DesignFormatToLinearColor(EDesignFormat::QuestBoard_Complete);
					}
					else
					{
						// 각성 불가능
						ArtifactStackPrograss->WidgetStyle.FillImage.TintColor = DesignFormatToLinearColor(EDesignFormat::QuestBoard_Progress);
						//애니메이션 설정
						//SetWidgetVisibility(RateUpIcon, ESlateVisibility::Collapsed);
						//StopWidgetAnimation(RateUp);
					}

					SetWidgetVisibility(RateUpIcon, ESlateVisibility::Collapsed);

					//애니메이션 설정
					StopWidgetAnimation(RateUp);

					ArtifactStackPrograss->SetPercent(FMath::Clamp((float)ArtifactData.Stack / (float)ReqStackCount, 0.0f, 1.0f));
					SetTextBlockText(TextStackPercent, CountToText(ArtifactData.Stack, ReqStackCount));
				}

			}



			FS_ItemSet* SetTable = USDKTableManager::Get()->FindTableRow<FS_ItemSet>(ETableType::tb_ItemSet, FString::FromInt(ArtifactTable->ArtifactSet[0]));
			if (SetTable != nullptr)
			{
				SetImageTexturePath(SetIconImage, SetTable->IconPath.ToString());
				SetIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}


			StatusNew->SetVisibility(ESlateVisibility::Collapsed);
			StatusUpgrade->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
}

void USDKArtifactEnhanceIconWidget::InitArtifacticon()
{
	BindImages();

	if (Button_Artifact->OnClicked.Contains(this, TEXT("OnSelectedArtifactIcon")) == false)
	{
		Button_Artifact->OnClicked.AddDynamic(this, &USDKArtifactEnhanceIconWidget::OnSelectedArtifactIcon);
	}
}

void USDKArtifactEnhanceIconWidget::SetSelectItemInfo(bool bSelect)
{
	if (bSelect)
	{
		ASDKHUD* SDKHUD = Cast<ASDKHUD>(GetOwningPlayer()->GetHUD());
		if (IsValid(SDKHUD) == true)
		{
			if (SDKHUD->IsOpenUI(EUI::UI_ArtifactEnhance))
			{
				USDKUIArtifactEnhanceWidget* SDKMainWidget = Cast<USDKUIArtifactEnhanceWidget>(SDKHUD->GetUI(EUI::UI_ArtifactEnhance));
				if (IsValid(SDKMainWidget) == true)
				{
					SDKMainWidget->ClearListItemOverlay(ArtifactParam);
					SDKMainWidget->ClearSelectSetList();

					//USDKPopupArtifactInfoWidget* InfoWidget = SDKMainWidget->GetInfoWidget();
					//if (IsValid(InfoWidget) == true)
					//{
					//	InfoWidget->SetArtifactParamInfo(ArtifactParam);
					//}
				}
			}
		}
	}

	SetOverlaySelect(bSelect);
}

void USDKArtifactEnhanceIconWidget::SetOverlaySelect(bool bVisible)
{
	SetWidgetVisibility(OverlaySelect, bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void USDKArtifactEnhanceIconWidget::BindImages()
{
	if(IsValid(RateUp) == false)
	{
		RateUp = GetWidgetAnimation(TEXT("ArtifactRateUp"));
	}

	SetWidgetVisibility(SetIconImage, ESlateVisibility::Collapsed);
	SetWidgetVisibility(RateUpIcon,ESlateVisibility::Collapsed);
}

void USDKArtifactEnhanceIconWidget::SetEmptyIcon()
{
	WidgetSwitcher_Enabled->SetActiveWidgetIndex(0);

	SetVisibility(ESlateVisibility::HitTestInvisible);
	SetWidgetVisibility(BorderGrade, ESlateVisibility::Collapsed);
	SetWidgetVisibility(Image_Artifact, ESlateVisibility::Collapsed);
	SetWidgetVisibility(StatusNew, ESlateVisibility::Collapsed);
	SetWidgetVisibility(StatusUpgrade, ESlateVisibility::Collapsed);
	SetWidgetVisibility(ArtifactStackOverlay, ESlateVisibility::Collapsed);
	SetWidgetVisibility(TextEnhance, ESlateVisibility::Collapsed);
	SetWidgetVisibility(WidgetSwitcher_InfoOrDeck, ESlateVisibility::Collapsed);

	SetOverlaySelect(false);
}

void USDKArtifactEnhanceIconWidget::SetVisibilityInfoType(bool bShowDeck)
{
	SetWidgetSwitcherActiveWidgetIndex(WidgetSwitcher_InfoOrDeck, bShowDeck ? 1 : 0);
	SetVisibilityEquipDeck(ArtifactParam->GetDeckEquip());

}

void USDKArtifactEnhanceIconWidget::SetVisibilityEquipDeck(bool bVisible)
{
	SetWidgetVisibility(EquipDeck, bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}
