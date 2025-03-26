// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SDKUIPetWidget.h"

#include "Components/Border.h"
#include "Components/GridSlot.h"
#include "Components/GridPanel.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Components/SceneCaptureComponent2D.h"

#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Character/SDKHUD.h"
#include "Character/SDKMyInfo.h"
#include "Character/SDKCharacter.h"
#include "Character/SDKPlayerController.h"

#include "Engine/SDKAssetManager.h"
#include "Engine/SDKGameInstance.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/SDKGameTask.h"

#include "Manager/SDKTableManager.h"
#include "Manager/SDKLobbyServerManager.h"
#include "Share/SDKData.h"
#include "Share/SDKHelper.h"

#include "Pet/SDKPet.h"

#include "UI/SDKButtonParam.h"
#include "UI/SDKWidgetParam.h"
#include "UI/SDKComboBoxWidget.h"
#include "UI/SDKCharacterStatusWidget.h"

#include "UI/SDKButtonWidget.h"
#include "UI/SDKPetIconWidget.h"
#include "UI/SDKSceneChangeWidget.h"
#include "UI/SDKPopupPetUpgradeWidget.h"
#include "UI/SDKPetInfoWidget.h"
#include "UI/SDKPetCombineListWidget.h"
#include "UI/SDKTopBarWidget.h"
#include "UI/SDKPetEggIncubatorWidget.h"
#include "UI/SDKUIGachaWidget.h"

#include "Subsystem/SDKWorldSubsystem.h"
#include "Animation/SkeletalMeshActor.h"
#include "GameFramework/Character.h"


USDKUIPetWidget::USDKUIPetWidget() : MaxColumnCount(5), MaxListCount(PET_COUNT_MAX), SDKPet(nullptr), PetSpawnTransform(FTransform()), PetViewCamera(nullptr), TouchPosition(FVector2D::ZeroVector), CurrentSortType(ESortOrderCategory::None)
{
	bCamMove = false;
	PetActorMove = false;
	mapPetIndex.Empty();
	PetIconList.Empty();
	CamLerpSpeed = 25.0f;
	PetLerpSpeed = 2.5f;
	PetRotateSpeed = 8.0f;

	for(int32 i = 0; i < 10; i++)
	{
		PetCombineDataList.Add(FPetCombineData());
	}
}

void USDKUIPetWidget::UpdatePetList()
{
	// 펫의 Table ID는 종류의 의미로써 이해해야함.
	// 각각의 펫이 고유한 UniqueID를 부여받기 때문.

	USDKGameInstance* SDKGameInstance = GetGameInstance<USDKGameInstance>();

	CPetDataList& PetDataList = USDKMyInfo::Get().GetPetDataList();

	if (PetDataList.GetAllPetNum() > 0)
	{
		SetWidgetSwitcherActiveWidgetIndex(SwitcherPetExist, 0);

		TArray<int64> PetUniqueIDs = WidgetIndex == 0 ? PetDataList.GetPetUniqueIDs() : PetDataList.GetPetCombineReadyUniqueID();

		if(WidgetIndex == 1)
		{
			int64 EquipedPetID = USDKMyInfo::Get().GetAccountData().GetPetID();
			if (EquipedPetID > 0)
			{
				PetUniqueIDs.Remove(EquipedPetID);
			}
		}

		SortPetList(PetUniqueIDs, CurrentSortType);
		SetTextBlockText(TextTotalCount, CountToText(FMath::Clamp(PetDataList.GetAllPetDataList().Num(), 0, MaxListCount), MaxListCount));

		if (PetIconList.Num() > 0)
		{
			for (int32 idx = 0; idx < MaxListCount; ++idx)
			{
				int64 UniqueID = PetUniqueIDs.IsValidIndex(idx) ? PetUniqueIDs[idx] : 0;
				if (PetIconList.IsValidIndex(idx) == false)
				{
					CreatePetBoxWidget(PetDataList.GetPetDataByUniqueID(UniqueID));
				}
				else
				{
					if (UniqueID > 0)
					{
						PetIconList[idx]->SetPetData(PetDataList.GetPetDataByUniqueID(UniqueID));

						int64 DataID = UniqueID;
						if (mapPetIndex.Contains(DataID) == true)
						{
							mapPetIndex[DataID] = idx;
						}
						else
						{
							mapPetIndex.Add(DataID, idx);
						}
					}
					else
					{
						PetIconList[idx]->SetEmptyData(true);
					}
				}
			}
		}
		else
		{
			UE_LOG(LogWidget, Log, TEXT("Not Work Create PetIcon"));
		}

		// 펫 자동 선택
		if (WidgetIndex == 0)
		{
			if ((SelectUniqueID <= 0) || (PetDataList.IsContainsPet(SelectUniqueID) == false))
			{
				int64 EquipedPetID = USDKMyInfo::Get().GetAccountData().GetPetID();
				if (EquipedPetID > 0)
				{
					CPetData PetData = PetDataList.GetPetDataByUniqueID(EquipedPetID);
					SetSelectPet(PetData.GetPetTableID(), PetData.GetPetUniqueID());
				}
				else
				{
					// 선택된 펫이 없거나, 유효하지 않은 펫 아이디 이거나, 갖고 있지 않은 펫인 경우
					CPetData PetData = PetDataList.GetAllPetDataList()[0];
					SetSelectPet(PetData.GetPetTableID(), PetData.GetPetUniqueID());
				}

			}
			else
			{
				SetSelectPet(SelectTableID, SelectUniqueID);
			}
		}
	}
	else
	{
		SetWidgetSwitcherActiveWidgetIndex(SwitcherPetExist, 1);
		SetVisibilityRotate(false);
	}
}

void USDKUIPetWidget::OnSelectSortOrder(ESortOrderCategory SelectOrderCategory)
{
	if (CurrentSortType != SelectOrderCategory)
	{
		CurrentSortType = SelectOrderCategory;

		UpdatePetList();
	}
}

// 펫 아이콘 생성.
// 펫 보유최대수 만큼 비어있는 목록 생성.
// 유저의 펫 정보와 테이블 정보를 로드하여 아이콘 설정.
void USDKUIPetWidget::CreatePetList()
{
	mapPetIndex.Empty();
	PetIconList.Empty();

	auto WidgetTable = USDKTableManager::Get()->FindTableWidgetBluePrint(EWidgetBluePrint::PetIcon);
	if (WidgetTable == nullptr || WidgetTable->WidgetBluePrintPath.IsNull())
	{
		return;
	}

	TSubclassOf<USDKUserWidget> WidgetClass(*USDKAssetManager::Get().LoadSynchronous(WidgetTable->WidgetBluePrintPath));
	if (WidgetClass == nullptr)
	{
		return;
	}

	auto GDTable = USDKTableManager::Get()->FindTableGlobalDefine(EGlobalDefine::PetDefaultCount);
	MaxListCount = GDTable != nullptr ? FSDKHelpers::StringToInt32(*GDTable->Value[0]) : PET_COUNT_MAX;

	for (int32 i = 0; i < MaxListCount; i++)
	{
		auto NewIconWidget = CreateWidget<USDKPetIconWidget>(GetOwningPlayer(), WidgetClass);
		if (NewIconWidget != nullptr)
		{
			int32 index = PetIconList.Num();
			PetIconList.Add(NewIconWidget);

			NewIconWidget->CreateUserWidget();
			NewIconWidget->SetEmptyData(true);

			if (NewIconWidget->GetButtonParamPet() != nullptr)
			{
				auto ButtonPet = NewIconWidget->GetButtonParamPet();
				if (ButtonPet->OnClickedParam.Contains(this, FName("OnClickedButtonSelectPet")) == false)
				{
					ButtonPet->OnClickedParam.AddDynamic(this, &USDKUIPetWidget::OnClickedButtonSelectPet);
				}
			}

			if (GridPanelList != nullptr)
			{
				auto pSlot = GridPanelList->AddChildToGrid(NewIconWidget, index / MaxColumnCount, index % MaxColumnCount);
				if (pSlot != nullptr)
				{
					pSlot->SetPadding(WidgetTable->Padding);
				}
			}
		}
	}
}

void USDKUIPetWidget::CreatePetBoxWidget(const CPetData& Data)
{
	auto WidgetTable = USDKTableManager::Get()->FindTableWidgetBluePrint(EWidgetBluePrint::PetIcon);
	if (WidgetTable == nullptr || WidgetTable->WidgetBluePrintPath.IsNull())
	{
		return;
	}

	TSubclassOf<USDKUserWidget> WidgetClass(*USDKAssetManager::Get().LoadSynchronous(WidgetTable->WidgetBluePrintPath));
	if (WidgetClass == nullptr)
	{
		return;
	}

	auto NewIconWidget = CreateWidget<USDKPetIconWidget>(GetOwningPlayer(), WidgetClass);
	if (NewIconWidget != nullptr)
	{
		int32 index = PetIconList.Num();
		PetIconList.Add(NewIconWidget);

		NewIconWidget->CreateUserWidget();
		NewIconWidget->SetPetData(Data);

		if (NewIconWidget->GetButtonParamPet() != nullptr)
		{
			auto ButtonPet = NewIconWidget->GetButtonParamPet();
			if (ButtonPet->OnClickedParam.Contains(this, FName("OnClickedButtonSelectPet")) == false)
			{
				ButtonPet->OnClickedParam.AddDynamic(this, &USDKUIPetWidget::OnClickedButtonSelectPet);
			}
		}

		if (GridPanelList != nullptr)
		{
			auto pSlot = GridPanelList->AddChildToGrid(NewIconWidget, index / MaxColumnCount, index % MaxColumnCount);
			if (pSlot != nullptr)
			{
				pSlot->SetPadding(WidgetTable->Padding);
			}
		}
	}
}

// 펫 리스트 정렬
void USDKUIPetWidget::SortPetList(TArray<int64>& Array, ESortOrderCategory SrotType)
{
	USDKGameInstance* SDKGameInstance = GetGameInstance<USDKGameInstance>();
	if (SDKGameInstance != nullptr)
	{
		int64 EquipID = USDKMyInfo::Get().GetAccountData().GetPetID();
		CPetDataList DataList = USDKMyInfo::Get().GetPetDataList();

		switch (SrotType)
		{
		case ESortOrderCategory::None:
		case ESortOrderCategory::ID:
		{
			Array.Sort([EquipID, DataList](const int64& A, const int64& B)
				{
					if (EquipID == A || EquipID == B)
					{
						return EquipID == A;
					}

					if (DataList.IsContainsPetByUniqueID(A) && DataList.IsContainsPetByUniqueID(B))
					{
						return A < B;
					}
					else if (DataList.IsContainsPetByUniqueID(A) && DataList.IsContainsPetByUniqueID(B) == false)
					{
						return true;
					}
					else if (DataList.IsContainsPetByUniqueID(A) == false && DataList.IsContainsPetByUniqueID(B))
					{
						return false;
					}

					return A < B;
				});
		}
		break;
		case ESortOrderCategory::Name:
		{
			TMap<FString, TArray<int64>> mapPetName, mapTempIDs;
			for (auto& itID : Array)
			{
				auto PetTable = USDKTableManager::Get()->FindTableRow<FS_Pet>(ETableType::tb_Pet, FString::FromInt(DataList.GetPetTableIDByUniqueID(itID)));
				if (PetTable != nullptr)
				{
					FString PetName = FSDKHelpers::GetTableText(TEXT("Common"), PetTable->Name).ToString();
					if (mapPetName.Contains(PetName) == false)
					{
						TArray<int64> UniqueIDs;
						UniqueIDs.Add(itID);
						mapPetName.Emplace(PetName, UniqueIDs);
					}
					else
					{
						mapPetName[PetName].Add(itID);
					}

				}
			}

			mapTempIDs = mapPetName;
			mapPetName.KeySort([EquipID, mapTempIDs, DataList](const FString& A, const FString& B)
				{
					if (mapTempIDs[A].Contains(EquipID) || mapTempIDs[B].Contains(EquipID))
					{
						return mapTempIDs[A].Contains(EquipID);
					}

					auto ValueA = mapTempIDs[A];
					auto ValueB = mapTempIDs[B];

					auto UTF32A = TCHAR_TO_UTF32(*A);
					auto UTF32B = TCHAR_TO_UTF32(*B);

					if (DataList.IsContainsPetByUniqueID(ValueA[0]) && DataList.IsContainsPetByUniqueID(ValueB[0]))
					{
						return *UTF32A < *UTF32B;
					}
					else if (DataList.IsContainsPetByUniqueID(ValueA[0]) && DataList.IsContainsPetByUniqueID(ValueB[0]) == false)
					{
						return true;
					}
					else if (DataList.IsContainsPetByUniqueID(ValueA[0]) == false && DataList.IsContainsPetByUniqueID(ValueB[0]))
					{
						return false;
					}

					return *UTF32A < *UTF32B;
				});

			TArray<int64> SortArray;
			for (auto itArr = mapPetName.CreateIterator(); itArr; ++itArr)
			{
				for (int32 i = 0; i < itArr.Value().Num(); i++)
				{
					SortArray.Add(itArr.Value()[i]);
				}
			}
			Array = SortArray;

		}
		break;
		case ESortOrderCategory::GradeHigh:
		{
			Array.Sort([EquipID, DataList](const int64& A, const int64& B)
				{
					if (EquipID == A || EquipID == B)
					{
						return EquipID == A;
					}

					auto PetTableA = USDKTableManager::Get()->FindTableRow<FS_Pet>(ETableType::tb_Pet, FString::FromInt(DataList.GetPetTableIDByUniqueID(A)));
					auto PetTableB = USDKTableManager::Get()->FindTableRow<FS_Pet>(ETableType::tb_Pet, FString::FromInt(DataList.GetPetTableIDByUniqueID(B)));
					if (PetTableA != nullptr && PetTableB != nullptr)
					{
						if (DataList.IsContainsPetByUniqueID(A) && DataList.IsContainsPetByUniqueID(B))
						{
							if (PetTableA->Grade == PetTableB->Grade)
							{
								return A < B;
							}
							else
							{
								return PetTableA->Grade > PetTableB->Grade;
							}
						}
						else if (DataList.IsContainsPetByUniqueID(A) && DataList.IsContainsPetByUniqueID(B) == false)
						{
							return true;
						}
						else if (DataList.IsContainsPetByUniqueID(A) == false && DataList.IsContainsPetByUniqueID(B))
						{
							return false;
						}
						else
						{
							if (PetTableA->Grade == PetTableB->Grade)
							{
								return A < B;
							}
							else
							{
								return PetTableA->Grade > PetTableB->Grade;
							}
						}
					}

					return A < B;
				});
		}
		break;
		case ESortOrderCategory::GradeLow:
		{
			Array.Sort([EquipID, DataList](const int64& A, const int64& B)
				{
					if (EquipID == A || EquipID == B)
					{
						return EquipID == A;
					}

					auto PetTableA = USDKTableManager::Get()->FindTableRow<FS_Pet>(ETableType::tb_Pet, FString::FromInt(DataList.GetPetTableIDByUniqueID(A)));
					auto PetTableB = USDKTableManager::Get()->FindTableRow<FS_Pet>(ETableType::tb_Pet, FString::FromInt(DataList.GetPetTableIDByUniqueID(B)));
					if (PetTableA != nullptr && PetTableB != nullptr)
					{
						if (DataList.IsContainsPetByUniqueID(A) && DataList.IsContainsPetByUniqueID(B))
						{
							if (PetTableA->Grade == PetTableB->Grade)
							{
								return A < B;
							}
							else
							{
								return PetTableA->Grade < PetTableB->Grade;
							}
						}
						else if (DataList.IsContainsPetByUniqueID(A) && DataList.IsContainsPetByUniqueID(B) == false)
						{
							return true;
						}
						else if (DataList.IsContainsPetByUniqueID(A) == false && DataList.IsContainsPetByUniqueID(B))
						{
							return false;
						}
						else
						{
							if (PetTableA->Grade == PetTableB->Grade)
							{
								return A < B;
							}
							else
							{
								return PetTableA->Grade < PetTableB->Grade;
							}
						}
					}

					return A < B;
				});
		}
		break;
		case ESortOrderCategory::Friendship:
		{
			Array.Sort([EquipID, DataList](const int64& A, const int64& B)
				{
					if (EquipID == A || EquipID == B)
					{
						return EquipID == A;
					}

					auto DataA = DataList.GetPetDataByUniqueID(A);
					auto DataB = DataList.GetPetDataByUniqueID(B);

					if (DataList.IsContainsPetByUniqueID(A) && DataList.IsContainsPetByUniqueID(B))
					{
						int32 A_Level = FSDKHelpers::GetPetLevelByLike(DataA.GetPetTableID(), DataA.GetPetLike());
						int32 B_Level = FSDKHelpers::GetPetLevelByLike(DataB.GetPetTableID(), DataB.GetPetLike());
						if (A_Level == B_Level)
						{
							return A < B;
						}
						else
						{
							return A_Level > B_Level;
						}
					}
					else if (DataList.IsContainsPetByUniqueID(A) && DataList.IsContainsPetByUniqueID(B) == false)
					{
						return true;
					}
					else if (DataList.IsContainsPetByUniqueID(A) == false && DataList.IsContainsPetByUniqueID(B))
					{
						return false;
					}
					else
					{
						int32 A_Level = FSDKHelpers::GetPetLevelByLike(DataA.GetPetTableID(), DataA.GetPetLike());
						int32 B_Level = FSDKHelpers::GetPetLevelByLike(DataB.GetPetTableID(), DataB.GetPetLike());

						if (A_Level == B_Level)
						{
							return A < B;
						}
						else
						{
							return A_Level > B_Level;
						}
					}

					return A < B;
				});
		}
		break;
		case ESortOrderCategory::SameGrade:
		{
			TArray<int64> RemoveUniqueID;

			// 지워야하는 다른 등급 대상 찾기
			for (int32 i = 0; i < Array.Num(); i++)
			{
				if (SelectUniqueID == 0)
				{
					break;
				}
				if (Array[i] == SelectUniqueID)
				{
					continue;
				}

				auto PetTableA = USDKTableManager::Get()->FindTableRow<FS_Pet>(ETableType::tb_Pet, FString::FromInt(SelectTableID));
				auto PetTableB = USDKTableManager::Get()->FindTableRow<FS_Pet>(ETableType::tb_Pet, FString::FromInt(DataList.GetPetTableIDByUniqueID(Array[i])));

				if (PetTableA->Grade != PetTableB->Grade)
				{
					RemoveUniqueID.Add(Array[i]);
				}
			}

			// 다른 등급 지우기
			for (int32 i = 0; i < RemoveUniqueID.Num(); i++)
			{
				Array.Remove(RemoveUniqueID[i]);
			}
		}
		break;
		}
	}
}
