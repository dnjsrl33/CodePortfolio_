// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SDKUIArtifactEnhanceWidget.h"
#include "UI/SDKTileView.h"
#include "UI/SDKWidgetParam.h"
#include "UI/SDKPopupArtifactInfoWidget.h"
#include "UI/SDKArtifactEnhanceSetListWidget.h"
#include "UI/SDKArtifactEnhanceIconWidget.h"

#include "UI/SDKArtifactGradeInfoWidget.h"
#include "UI/SDKArtifactDeckListWidget.h"
#include "UI/SDKArtifactDeckSettingWidget.h"

#include "UI/SDKButtonWidget.h"
#include "UI/SDKButtonParam.h"
#include "UI/SDKArtifactFiliterWidget.h"
#include "UI/SDKTopBarWidget.h"
#include "UI/SDKYesNoPopupWidget.h"

#include "Character/SDKHUD.h"
#include "Character/SDKMyInfo.h"
#include "Character/SDKPlayerController.h"

#include "Share/SDKHelper.h"

#include "Engine/SDKGameInstance.h"

#include "Manager/SDKTableManager.h"
#include "Manager/SDKLobbyServerManager.h"

#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/GridSlot.h"
#include "Components/GridPanel.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

#include "Kismet/GameplayStatics.h"




USDKUIArtifactEnhanceWidget::USDKUIArtifactEnhanceWidget()
	: bUpgrade(false)
{
	ArtifactList.Empty();
	ArtifactSetList.Empty();
}

void USDKUIArtifactEnhanceWidget::SetID(const FString& ID)
{
	Super::SetID(ID);
}

void USDKUIArtifactEnhanceWidget::CreateUIProcess()
{
	Super::CreateUIProcess();

	InitWidget();

	SetTextBlockTableText(TextSetTitle, TEXT("UIText_Artifact_ActiveArtifact"));
	SetTextBlockTableText(TextSeasonInfoTitle, TEXT("UIText_Artifact_Season"));
	SetTextBlockTableText(TextFilterTitle, TEXT("UIText_Filter"));

	ArtifactList = USDKMyInfo::Get().GetArtifactData().GetArtifactList();

	CreateArtifactList();
}

void USDKUIArtifactEnhanceWidget::OpenUIProcess()
{
	Super::OpenUIProcess();

	LoadArtifactData();
}

void USDKUIArtifactEnhanceWidget::CloseUIProcess()
{
	Super::CloseUIProcess();

	ASDKHUD* SDKHUD = Cast<ASDKHUD>(GetOwningPlayer()->GetHUD());
	if (nullptr != SDKHUD)
	{
		if (SDKHUD->IsOpenUI(EUI::Popup_ArtifactEnhance))
		{
			SDKHUD->CloseUI(EUI::Popup_ArtifactEnhance);
		}

		if (SDKHUD->IsOpenUI(EUI::Popup_ArtifactInfo))
		{
			SDKHUD->CloseUI(EUI::Popup_ArtifactInfo);
		}

		if (SDKHUD->IsOpenUI(EUI::UI_ArtifactEnhance))
		{
			SDKHUD->CloseUI(EUI::UI_ArtifactEnhance);
		}
	}

	OnCloseCheckBackUI();
}

void USDKUIArtifactEnhanceWidget::OnClosePopupDeckModifyAlert()
{
	PopupOpen<USDKYesNoPopupWidget>()->InitWith<USDKYesNoPopupWidgetArg>([this](auto Arg)
	{
		Arg->TitleId = TEXT("PopupText_ArtifactDeck_CustomName");
		Arg->MessageId = TEXT("PopupText_ArtifactDeck_CustomDesc");
		Arg->YesId = TEXT("PopupText_ArtifactDeck_CustomCancel");
		Arg->NoId = TEXT("Button_Cancel");
		Arg->OnClickedYes.BindDynamic(this, &USDKUIArtifactEnhanceWidget::OnClickedReturnYesButtonDeckModify);
		Arg->OnClickedNo.BindDynamic(this, &USDKUIArtifactEnhanceWidget::OnClickedReturnNoButtonDeckModify);
	});
}

void USDKUIArtifactEnhanceWidget::OnClickedReturnYesButtonDeckModify()
{
	ASDKHUD* SDKHUD = Cast<ASDKHUD>(GetOwningPlayer()->GetHUD());
	if (!IsValid(SDKHUD))
	{
		return;
	}

	DeactiveDeckSettingState();
	SetWidgetSwitcherActiveWidgetIndex(WidgetSwitcher_GradeOrDeck, 0);

	// USDKYesNoPopupWidget 종료
	SDKHUD->CloseFrontPopupWidget();

	CloseWidget();
}

void USDKUIArtifactEnhanceWidget::OnClickedReturnNoButtonDeckModify()
{
}

FArtifactData USDKUIArtifactEnhanceWidget::FindArtifactData(int32 InArtifactID)
{
	for (int32 i = 0; i < ArtifactList.Num(); i++)
	{
		if (ArtifactList[i].ArtifactID == InArtifactID)
		{
			return ArtifactList[i];
		}
	}

	return FArtifactData(InArtifactID, 0, 0, 0);
}

// 전체 유물 목록 아이콘 생성
// TileView Widget을 활용한 아이콘 생성
void USDKUIArtifactEnhanceWidget::CreateArtifactList()
{
	TArray<int32> SeasonSet = USDKTableManager::Get()->GetSeasonArtifactSetIDByGameMode(EGameMode::GoldClash);

	auto pSetWidget = FSDKHelpers::LoadWidgetBluePrint(EWidgetBluePrint::ArtifactEnhanceSetList);
	if (nullptr == pSetWidget)
		return;

	TMap<int32, TArray<FString>> ArtifactMap = USDKTableManager::Get()->GetArtifactMap();
	if (ArtifactMap.Num() > 0)
	{
		// 전체 유물 리스트
		for(const auto& Iter : ArtifactMap)
		{
			for(const auto& ItID : Iter.Value)
			{
				int32 CurrArtifactID = FSDKHelpers::StringToInt32(ItID);

				FS_Artifact* ArtifactTable = USDKTableManager::Get()->FindTableRow<FS_Artifact>(ETableType::tb_Artifact, FString::FromInt(CurrArtifactID));
				if (ArtifactTable != nullptr)
				{
					if (ArtifactTable->Enable == false)
					{
						continue;
					}

					if (ArtifactTable->Category == EItemCategory::DarkArtifact)
					{
						continue;
					}

					if (MapArtifactGradeData.Contains(ArtifactTable->Grade))
					{
						MapArtifactGradeData[ArtifactTable->Grade].Y++;
					}
					else
					{
						MapArtifactGradeData.Emplace(ArtifactTable->Grade, FIntPoint(0, 1));
					}

					FArtifactData ArtifactData = FindArtifactData(CurrArtifactID);

					FS_ItemSet* ItemSetTable = USDKTableManager::Get()->FindTableRow<FS_ItemSet>(ETableType::tb_ItemSet, FString::FromInt(ArtifactTable->ArtifactSet[0]));

					USDKWidgetParamArtifactEnhance* SDKIconData = NewObject<USDKWidgetParamArtifactEnhance>(this, USDKWidgetParamArtifactEnhance::StaticClass());

					if (!IsValid(SDKIconData))
					{
						continue;
					}

					SDKIconData->SetArtifactData(ArtifactData);
					SDKIconData->SetArtifactSetIDs(ArtifactTable->ArtifactSet);
					ArtifactIconList.Emplace(ArtifactData.ArtifactID, SDKIconData);

					if (ArtifactSetList.Contains(ArtifactTable->ArtifactSet[0]))
					{
						/* 기존 세트 리스트가 있을 때*/
						if (ArtifactSetList[ArtifactTable->ArtifactSet[0]].Contains(ArtifactData) == false)
						{
							ArtifactSetList[ArtifactTable->ArtifactSet[0]].Emplace(ArtifactData);
							ArtifactSetIconList[ArtifactTable->ArtifactSet[0]]->AddArtifactListItem(SDKIconData);
						}
					}
					else
					{
						/* 기존 세트 리스트가 없을 때*/

						USDKArtifactEnhanceSetListWidget* SDKSetListWidget = CreateWidget<USDKArtifactEnhanceSetListWidget>(GetWorld(), pSetWidget);
						if (IsValid(SDKSetListWidget))
						{
							SDKSetListWidget->CreateUserWidget();
							SDKSetListWidget->SetArtifactSetID(ArtifactTable->ArtifactSet[0]);

							if (SeasonSet.Contains(ArtifactTable->ArtifactSet[0]))
							{
								SDKSetListWidget->ActiveSeasonTag();
							}

							ArtifactSetList.Emplace(ArtifactTable->ArtifactSet[0], TArray<FArtifactData>{ ArtifactData });
							ArtifactSetIconList.Emplace(ArtifactTable->ArtifactSet[0], SDKSetListWidget);
							ArtifactSetIconList[ArtifactTable->ArtifactSet[0]]->AddArtifactListItem(SDKIconData);
						}
					}
				}
			}
		}
	}

	for(int32 i = MAX_ARTIFACT_ENHANCE; i >= MIN_ARTIFACT_ENHANCE; i--)
	{
		USDKArtifactEnhanceSetListWidget* SDKSetListWidget = CreateWidget<USDKArtifactEnhanceSetListWidget>(GetWorld(), pSetWidget);
		if (IsValid(SDKSetListWidget))
		{
			SDKSetListWidget->CreateUserWidget();
			SDKSetListWidget->SetFilterEnhanceList(i);
			FilterEnhanceIconList.Emplace(i, SDKSetListWidget);
		}
	}

	for (int32 i = static_cast<int32>(EItemGrade::EItemGrade_MAX) - 1; i >= static_cast<int32>(EItemGrade::Normal); i--)
	{
		USDKArtifactEnhanceSetListWidget* SDKSetListWidget = CreateWidget<USDKArtifactEnhanceSetListWidget>(GetWorld(), pSetWidget);
		if (IsValid(SDKSetListWidget))
		{
			SDKSetListWidget->CreateUserWidget();
			SDKSetListWidget->SetFilterGradeList(static_cast<EItemGrade>(i));
			FilterGradeIconList.Emplace(static_cast<EItemGrade>(i), SDKSetListWidget);
		}
	}


}

// 유물 정렬
// 플레이어가 설정한 정렬기준에 따라 정렬.
void USDKUIArtifactEnhanceWidget::SortArtifactSetList()
{
	if (ArtifactSetListBox->GetChildrenCount() > 0)
	{
		ArtifactSetListBox->ClearChildren();
	}

	for (auto Iter = FilterEnhanceIconList.CreateIterator(); Iter; ++Iter)
	{
		Iter->Value->ClearSetIconList();
	}

	for (auto Iter = FilterGradeIconList.CreateIterator(); Iter; ++Iter)
	{
		Iter->Value->ClearSetIconList();
	}

	TArray<USDKArtifactEnhanceSetListWidget*> ActivateList;
	TArray<USDKArtifactEnhanceSetListWidget*> NotActivateList;

	ActivateList.Empty();
	NotActivateList.Empty();

	bool bSeason = ArtifactFilter->GetFilterSeason();

	TArray<int32> SeasonSet;
	TArray<FName> RowNames = USDKTableManager::Get()->GetRowNames(ETableType::tb_SeasonArtifact);

	for (auto TableName : RowNames)
	{
		FS_SeasonArtifact* SeasonArtifact = USDKTableManager::Get()->FindTableRow<FS_SeasonArtifact>(ETableType::tb_SeasonArtifact, TableName.ToString());

		if (SeasonArtifact)
		{
			if (SeasonArtifact->Enable && SeasonArtifact->GameMode == TEXT("GoldClash"))
			{
				SeasonSet = SeasonArtifact->SeasonSet;
				break;
			}
		}
	}

	for (auto It = ArtifactSetIconList.CreateConstIterator(); It; ++It)
	{
		if (!bSeason)
		{
			if (SeasonSet.Contains(It->Key))
			{
				continue;
			}
		}

		ActivateList.Add(It->Value);
	}
	
	if (ArtifactFilter->GetFilterSet())
	{
		// 세트 내 아이콘 정렬
		ActivateList.Sort([](USDKArtifactEnhanceSetListWidget& A, USDKArtifactEnhanceSetListWidget& B)
			{
				int32 CurrentSetCount_A = A.GetCurrentSetCount();
				int32 CurrentSetCount_B = B.GetCurrentSetCount();
				int32 SetCount_A = A.GetSetCount();
				int32 SetCount_B = B.GetSetCount();

				int32 GapA = CurrentSetCount_A - SetCount_A;
				int32 GapB = CurrentSetCount_B - SetCount_B;

				// 세트 완성에 가까운 기준
				if ((CurrentSetCount_A > 0 && CurrentSetCount_B > 0) || (CurrentSetCount_A == CurrentSetCount_B))
				{
					return A.GetArtifactSetID() < B.GetArtifactSetID();
				}
				else
				{
					return CurrentSetCount_A > CurrentSetCount_B;
				}

			});


		for (int32 i = 0; i < ActivateList.Num(); i++)
		{
			ArtifactSetListBox->AddChildToVerticalBox(ActivateList[i]);
			ActivateList[i]->SetVisibility(ESlateVisibility::Visible);

			ActivateList[i]->SortArtifactList(ArtifactFilter->GetEnhanceFilter(), ArtifactFilter->GetGradeFilter(), ArtifactFilter->GetFilterUnknown(), ArtifactFilter->GetFilterEnhanceAvaliable());
		}
	}
	else if (ArtifactFilter->GetFilterEnhance())
	{
		for (int32 i = 0; i < ActivateList.Num(); i++)
		{
			auto IconDataArr = ActivateList[i]->GetIconListData();
			for(auto Iter : IconDataArr)
			{
				FilterEnhanceIconList[Iter->GetArtifactEnhance()]->AddArtifactListItem(Iter);
			}
		}

		for(auto Iter = FilterEnhanceIconList.CreateIterator(); Iter; ++Iter)
		{
			ArtifactSetListBox->AddChildToVerticalBox(Iter->Value);
			SetWidgetVisibility(Iter->Value, ESlateVisibility::Visible);

			Iter->Value->SortArtifactList(ArtifactFilter->GetEnhanceFilter(), ArtifactFilter->GetGradeFilter(), ArtifactFilter->GetFilterUnknown(), ArtifactFilter->GetFilterEnhanceAvaliable());
		}
	}
	else if(ArtifactFilter->GetFilterGrade())
	{
		for (int32 i = 0; i < ActivateList.Num(); i++)
		{
			auto IconDataArr = ActivateList[i]->GetIconListData();
			for (auto Iter : IconDataArr)
			{
				FS_Artifact* ArtifactTable = USDKTableManager::Get()->FindTableRow<FS_Artifact>(ETableType::tb_Artifact, FString::FromInt(Iter->GetArtifactData().ArtifactID));
				if (ArtifactTable)
				{
					FilterGradeIconList[ArtifactTable->Grade]->AddArtifactListItem(Iter);
				}
			}

			for (auto Iter = FilterGradeIconList.CreateIterator(); Iter; ++Iter)
			{
				ArtifactSetListBox->AddChildToVerticalBox(Iter->Value);
				SetWidgetVisibility(Iter->Value, ESlateVisibility::Visible);

				Iter->Value->SortArtifactList(ArtifactFilter->GetEnhanceFilter(), ArtifactFilter->GetGradeFilter(), ArtifactFilter->GetFilterUnknown(), ArtifactFilter->GetFilterEnhanceAvaliable());
			}
		}
	}
	else
	{
		for (int32 i = 0; i < ActivateList.Num(); i++)
		{
			ArtifactSetListBox->AddChildToVerticalBox(ActivateList[i]);
			ActivateList[i]->SetVisibility(ESlateVisibility::Visible);

			ActivateList[i]->SortArtifactList(ArtifactFilter->GetEnhanceFilter(), ArtifactFilter->GetGradeFilter(), ArtifactFilter->GetFilterUnknown(), ArtifactFilter->GetFilterEnhanceAvaliable());
		}
	}

}


// 유물 정보 갱신
void USDKUIArtifactEnhanceWidget::LoadArtifactData()
{	
	for(auto Iter = MapArtifactGradeData.CreateIterator(); Iter; ++Iter)
	{
		Iter->Value.X = 0;
	}

	ArtifactList = USDKMyInfo::Get().GetArtifactData().GetArtifactList();
	for (auto It : ArtifactList)
	{
		FS_Artifact* ArtifactTable = USDKTableManager::Get()->FindTableRow<FS_Artifact>(ETableType::tb_Artifact, FString::FromInt(It.ArtifactID));
		if (ArtifactTable == nullptr)
		{
			continue;
		}

		// UI에서 처리해야할 유물 데이터 갱신 해줌.
		if (ArtifactIconList.Contains(It.ArtifactID))
		{
			ArtifactIconList[It.ArtifactID]->SetIsCollect(true);
			ArtifactIconList[It.ArtifactID]->SetArtifactData(It);

			if (MapArtifactGradeData.Contains(ArtifactTable->Grade))
			{
				MapArtifactGradeData[ArtifactTable->Grade].X++;
			}
		}

		// 유물 세트 정보 갱신
		if (ArtifactSetList.Contains(ArtifactTable->ArtifactSet[0]))
		{
			for (int32 i = 0; i < ArtifactSetList[ArtifactTable->ArtifactSet[0]].Num(); i++)
			{
				if (ArtifactSetList[ArtifactTable->ArtifactSet[0]][i] == It)
				{
					ArtifactSetList[ArtifactTable->ArtifactSet[0]][i] = It;
					break;
				}
			}
		}
	}

	SortArtifactSetList();

	ArtifactGradeInfo->SetArtifactInfoByGrade(MapArtifactGradeData);
}

// 유물 강화 성공 후의 UI정보 갱신
void USDKUIArtifactEnhanceWidget::UpgradeArtifact(FArtifactData ArtifactData)
{
	USDKGameInstance* SDKGameInstance = GetGameInstance<USDKGameInstance>();
	if (SDKGameInstance != nullptr)
	{
		// 유물 정보 갱신
		for (auto& It : ArtifactList)
		{
			if (It.ArtifactID == ArtifactData.ArtifactID)
			{
				bUpgrade = true;

				FS_Artifact* ArtifactTable = USDKTableManager::Get()->FindTableRow<FS_Artifact>(ETableType::tb_Artifact, FString::FromInt(It.GetTableID()));
				if (ArtifactTable != nullptr)
				{
					int32 PreviousID = It.ArtifactID;
					USDKWidgetParamArtifactEnhance* ArtifactParamData = nullptr;
					// 세트 뷰 설정
					// 기존 유물 ID를 강화된 유물 ID로 변경.
					ArtifactParamData = ArtifactSetIconList[ArtifactTable->ArtifactSet[0]]->GetIconData(PreviousID);

					It = ArtifactData;

					if (IsValid(ArtifactParamData))
					{
						USDKArtifactEnhanceIconWidget* Icon = ArtifactSetIconList[ArtifactTable->ArtifactSet[0]]->GetSetIconTileView()->GetEntryWidgetFromItem<USDKArtifactEnhanceIconWidget>(ArtifactParamData);
						if (IsValid(Icon))
						{
							ArtifactParamData->SetArtifactData(It);
							ArtifactParamData->SetIsCollect(true);
							Icon->SetArtifactIcon(ArtifactParamData);
						}
					}
				}

				if (IsValid(CurrentArtifactParam) == true)
				{
					CurrentArtifactParam->SetArtifactData(It);
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("ArtifactUpgrade CurrentArtifactParam Is Null."));
					for (auto& Iter : ArtifactIconList)
					{
						if (IsValid(Iter.Value) == true)
						{
							if (ArtifactData.ArtifactID == Iter.Value->GetArtifactData().ArtifactID)
							{
								Iter.Value->SetArtifactData(It);
								CurrentArtifactParam = Iter.Value;
							}
						}

					}
				}

				SDKGameInstance->MyInfoManager->GetArtifactData().SetArtifactList(ArtifactList);
				break;
			}
		}

	}

	if (GetDeckSetting())
	{
		for (auto& Iter : CurrentArtifactDeck.ArtifactList)
		{
			if (Iter == ArtifactData)
			{
				Iter = ArtifactData;
				ArtifactDeckSetting->SetDeckInfo(CurrentArtifactDeck);
				break;
			}
		}
	}


	LoadArtifactData();

}