// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SDKUIGoldClashWidget.h"

#include "Arena/Arena.h"

#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/Overlay.h"
#include "Components/ScaleBox.h"
#include "Components/GridSlot.h"
#include "Components/GridPanel.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBox.h"

#include "Kismet/GameplayStatics.h"

#include "Character/SDKHUD.h"
#include "Character/SDKMyInfo.h"
#include "Character/SDKPlayerState.h"
#include "Character/SDKPlayerCharacter.h"
#include "Character/SDKInGameController.h"
#include "Character/SDKMultiGameController.h"

#include "Engine/SDKAssetManager.h"
#include "Engine/SDKGameInstance.h"
#include "Engine/SDKBlueprintFunctionLibrary.h"

#include "GameMode/SDKGameState.h"
#include "Manager/SDKTableManager.h"
#include "Manager/SDKTutorialGuideManager.h"
#include "Manager/SDKPopupWidgetManager.h"

#include "Share/SDKData.h"
#include "Share/SDKStruct.h"
#include "Share/SDKHelper.h"
#include "Share/SDKDataTable.h"

#include "UI/SDKButtonParam.h"
#include "UI/SDKMiniMapWidget.h"
#include "UI/SDKDeathLogWidget.h"
#include "UI/SDKGoldClashScoreWidget.h"
#include "UI/SDKLevelUpQuickWidget.h"
#include "UI/SDKGoldRankListWidget.h"
#include "UI/SDKLevelUpBuffWidget.h"
#include "UI/SDKRightButtonsWidget.h"
#include "UI/SDKIngameUpdateNotifyWidget.h"
#include "UI/SDKGoldClashShopArtifactBoxWidget.h"
#include "UI/SDKEventMissionSummaryWidget.h"
#include "UI/SDKGoldClashShopPopupWidget.h"
#include "UI/SDKUserHotKeyWidget.h"
#include "UI/SDKMacroChatListWidget.h"
#include "UI/SDKScreenEffectWidget.h"

#include "Engine/LevelScriptActor.h"
#include "Misc/OutputDeviceNull.h"
#include "GameMode/SDKGoldClashState.h"
#include "UI/SDKGameConfigPopupWidget.h"

DECLARE_CYCLE_STAT_EXTERN(TEXT("SDK UIGoldClash Tick"), STAT_SDK_UIGoldClash_Tick, STATGROUP_SDKUserWidget, );
DEFINE_STAT(STAT_SDK_UIGoldClash_Tick);

void USDKUIGoldClashWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_SDK_UIGoldClash_Tick);
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("USDKUIGoldClashWidget::NativeTick"));
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (MaxCountDown != 0)
	{
		if (IsValid(ProgressBarTimeGauge) && IsValid(ProgressBarTimeGaugeEffect))
		{
			CountDeltaTime -= InDeltaTime;
			float CountPercent = CountDeltaTime / MaxCountDown;
			ProgressBarTimeGauge->SetPercent(CountPercent);
			ProgressBarTimeGaugeEffect->SetPercent(CountPercent);

			CountDownTick += InDeltaTime;
			if (CountDownTick >= 1.f)
			{
				CountDownTick = 0.f;
				SetTextBlockText(TextLastCountDown, NumberToText(FMath::RoundToInt(CountDeltaTime), 2));
				BPLastCountDown->PlayWidgetAnimation(CountDownAnim);
			}

			if (CountDeltaTime <= 0.0f)
			{
				SetWidgetVisibility(OverlayLastTimer, ESlateVisibility::Collapsed);
			}
		}
	}
}

void USDKUIGoldClashWidget::CreateUIProcess()
{
	Super::CreateUIProcess();

	if (IsValid(BPOutInfo))
	{
		if (FSDKHelpers::GetGameMode(GetWorld()) == EGameMode::GoldClash)
		{
			SetWidgetVisibility(BPOutInfo, ESlateVisibility::SelfHitTestInvisible);
		}

		TextPing = Cast<UTextBlock>(BPOutInfo->GetWidgetFromName(FName("TextPing")));
		TextFPS = Cast<UTextBlock>(BPOutInfo->GetWidgetFromName(FName("TextFPS")));
	}

	if (IsValid(BPLastCountDown))
	{
		TextLastCountDown = Cast<UTextBlock>(BPLastCountDown->GetWidgetFromName(FName("TextBlock_32")));
		CountDownAnim = BPLastCountDown->GetWidgetAnimation(TEXT("Loop"));
	}

	// Init
	InitTextBlockString();
	InitTeamMemberImages();
	InitEquipData();
	InitBindButton();
	InitGoldData();
	InitSeasonArtifactData();

	ASDKGoldClashState* GoldClashState = Cast<ASDKGoldClashState>(GetWorld()->GetGameState());
	if(IsValid(GoldClashState))
	{
		bool bVisible = GoldClashState->IsCustomMatch();
		SetWidgetVisibility(ButtonSurrender, bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void USDKUIGoldClashWidget::OpenUIProcess()
{
	ASDKInGameController* SDKIngameController = GetOwningPlayer<ASDKInGameController>();
	if (IsValid(SDKIngameController) && IsOnlyASpectator())
	{
		SDKIngameController->ClientActiveSpectator(true);
	}

	ASDKGoldClashState* GoldClashState = Cast<ASDKGoldClashState>(GetWorld()->GetGameState());

	if (IsValid(GoldClashState))
	{
		if (FSDKHelpers::GetGameMode(GetWorld()) == EGameMode::GoldClashTutorial && GoldClashState->GetCurrentMatchType() == EMatchType::Solo)
		{
			SetActiveTutorialGoldClashMode(true);

			// 싱글 튜토리얼 LevelBP에서 활성화.
			if (TutorialStepSingle <= ETutorialStepSingle::TutorialBegin)
			{
				SetVisibilityStatus(false);
				SetVisibilityMap(false);
				SetTutorialVisibilityLevelUpBuffBox(false);
				SetVisibilityShop(false);
				SetVisibilityRankBoard(false);
				SetVisibilityEmoticon(false);
				SetVisibilityMacro(false);

				SetWidgetVisibility(TextSpectatorNoti, ESlateVisibility::Collapsed);
			}
		}
	}

	UpdateConfigPing();
	UpdateConfigFPS();

	ToggleQuickLevelUpWidget(true);

	Super::OpenUIProcess();
}

void USDKUIGoldClashWidget::CloseUIProcess()
{
	Super::CloseUIProcess();
}

void USDKUIGoldClashWidget::TutorialUIProcess()
{
	switch (TutorialStepSingle)
	{
		case ETutorialStepSingle::LevelUpBuffGuideBegin:
		{
			USDKTutorialGuideManager::Get()->StartGuide();
		}
		break;
		case ETutorialStepSingle::LevelUpBuffGuideEnd:
		{
			USDKTutorialGuideManager::Get()->EndGuide();
			TutorialStepSingle = ETutorialStepSingle::GoldBoxGuideBegin;
		}
		break;

		case ETutorialStepSingle::GoldShopGuideBegin:
		{
			USDKTutorialGuideManager::Get()->StartGuide();
		}
		break;

		case ETutorialStepSingle::GoldShopSetArtifactGuideBegin:
		{
			USDKTutorialGuideManager::Get()->StartGuide();
		}
		break;
	}
}

void USDKUIGoldClashWidget::Editor_SetPlatformTypeProcess(EPlatformType type)
{
	Super::Editor_SetPlatformTypeProcess(type);

	if (type == EPlatformType::Windows)
	{
		SetWidgetVisibility(BorderPCGuid, ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SetWidgetVisibility(BorderPCGuid, ESlateVisibility::Collapsed);
	}

	ASDKInGameController* IngameController = GetOwningPlayer<ASDKInGameController>();
	if (IsValid(IngameController))
	{
		SetDisableAttackButtons(IngameController->GetDisableAttackLevel());
	}

	SetWidgetVisibility(SDKRightButtonBase, ESlateVisibility::SelfHitTestInvisible);
}

UGridPanel* USDKUIGoldClashWidget::GetBuffGridPanel(bool bIsBuff) const
{
	if (IsValid(SDKRightButtonBase))
	{
		if (bIsBuff)
		{
			return GridPanelBuff;
		}
		else
		{
			return GridPanelDebuff;
		}
	}

	return nullptr;
}

void USDKUIGoldClashWidget::InitHudReferenceSetting()
{
	OnInitHudReferenceSetting();
}

void USDKUIGoldClashWidget::SetActiveInteractionButton(ERelationType Type /*= ERelationType::None*/)
{
	if (IsValid(SDKRightButtonBase))
	{
		SDKRightButtonBase->SetActiveInteractionButton(Type);
	}
}

void USDKUIGoldClashWidget::ActiveNotifyCollectItem(FString ItemID)
{
	const FS_Item* ItemTable = USDKTableManager::Get()->FindTableRow<FS_Item>(ETableType::tb_Item, ItemID);
	if (!ItemTable)
	{
		const FS_Pet* PetTable = USDKTableManager::Get()->FindTableRow<FS_Pet>(ETableType::tb_Pet, ItemID);
		if (PetTable)
		{
			SDKUpdateNotify->SetNotiftyPetData(ItemID);
		}

		return;
	}

	if (ItemTable->ItemType == EItemType::Orb || ItemTable->ItemType == EItemType::Use)
	{
		return;
	}

	if (IsValid(SDKUpdateNotify))
	{
		SDKUpdateNotify->SetNotiftyItemData(ItemID);
	}
}

void USDKUIGoldClashWidget::ChangeEquipWeapon(int32 Index, int32 ItemID, int32 SkillID, bool bActiveItem)
{
	if (IsValid(SDKRightButtonBase))
	{
		FString strItemID = TEXT("");
		if (ItemID > 0)
		{
			strItemID = FString::FromInt(ItemID);
		}
		FString strSkillID = TEXT("");
		if (SkillID > 0)
		{
			strSkillID = FString::FromInt(SkillID);
		}

		// 공격
		SDKRightButtonBase->UpdateAttackButton(strItemID, bActiveItem);

		// 스킬
		int32 SubIndex = Index == 0 ? 1 : 0;

		if (bActiveItem)
		{
			SDKRightButtonBase->UpdateSkillButton(Index, strSkillID, bActiveItem);
			SDKRightButtonBase->UpdateSkillButton(SubIndex, strSkillID, bActiveItem);
		}
		else
		{
			// 무기의 스킬 레벨 조회 (무속성 -> -1) / (속성무기 -> 등급)
			int32 SkillLevel = 0;
			const FS_Item* ItemTable = USDKTableManager::Get()->FindTableRow<FS_Item>(ETableType::tb_Item, FString::FromInt(ItemID));
			if (ItemTable)
			{
				SkillLevel = (ItemTable->Elemental == EElementalType::None) ? -1 : static_cast<int32>(ItemTable->Grade);
			}

			SDKRightButtonBase->UpdateSkillButton(Index, strSkillID, bActiveItem, SkillLevel);
		}

		// 캐릭터 스킬
		if (!bActiveItem)
		{
			auto ItemTable = USDKTableManager::Get()->FindTableRow<FS_Item>(ETableType::tb_Item, strItemID);
			if (ItemTable != nullptr)
			{
				if (ItemTable->CharacterSkillID.Num() > 0)
				{
					auto itFirst = ItemTable->CharacterSkillID.CreateIterator();
					SDKRightButtonBase->UpdateSkillButton(3, FString::FromInt(*itFirst), bActiveItem);
				}
			}
		}
		else
		{
			SDKRightButtonBase->UpdateSkillButton(3, TEXT(""), bActiveItem);
		}
	}
}

void USDKUIGoldClashWidget::ChangedUnEquipWeapon(int32 ItemID, int32 SkillID, bool bActiveItem)
{
	if (IsValid(SDKRightButtonBase))
	{
		// 공격 및 무기 스킬
		SDKRightButtonBase->UpdateAttackButton(FString::FromInt(ItemID), bActiveItem);

		CAdditionCalculation AdditionCalculation;
		AdditionCalculation.SetOwner(GetOwningPlayer());
		AdditionCalculation.SetItemID(FString::FromInt(ItemID));

		TMap<ESkillType, TArray<int32>> mapSkillID = AdditionCalculation.GetItemSkillID(ESkillType::ActiveSkill);
		int32 MainSkillID = (mapSkillID.Contains(ESkillType::ActiveSkill) == false) ? 0 : mapSkillID[ESkillType::ActiveSkill][0];

		SDKRightButtonBase->UpdateSkillButton(0, FString::FromInt(MainSkillID), bActiveItem);
		SDKRightButtonBase->UpdateSkillButton(1, SkillID > 0 ? FString::FromInt(SkillID) : TEXT(""), bActiveItem);

		if (!bActiveItem)
		{
			auto ItemTable = USDKTableManager::Get()->FindTableRow<FS_Item>(ETableType::tb_Item, FString::FromInt(ItemID));
			if (ItemTable != nullptr)
			{
				if (ItemTable->CharacterSkillID.Num() > 0)
				{
					auto itFirst = ItemTable->CharacterSkillID.CreateIterator();
					SDKRightButtonBase->UpdateSkillButton(3, FString::FromInt(*itFirst), bActiveItem);
				}
			}
		}
	}
}

void USDKUIGoldClashWidget::SetDisableAttackButtons(bool bDisable)
{
	if (IsValid(SDKRightButtonBase))
	{
		for (int32 i = 0; i < MAX_SKILL + 1; ++i)
		{
			SDKRightButtonBase->SetActiveSkillButton(i, !bDisable);
		}
	}
}

void USDKUIGoldClashWidget::SetEnableDodgeButton(bool bEnable)
{
	if (IsValid(SDKRightButtonBase))
	{
		SDKRightButtonBase->SetEnableDodgeButton(bEnable);
	}
}

void USDKUIGoldClashWidget::UpdateGold(const int32 Gold)
{

}

/*** 스킬 ********************************************************************************************************************************************/
int32 USDKUIGoldClashWidget::GetTouchIndexSkillButton() const
{
	if (IsValid(SDKRightButtonBase))
	{
		return SDKRightButtonBase->GetSkillTouchpadIndex();
	}

	return -1;
}

void USDKUIGoldClashWidget::UpdateSkillCoolTime(int32 index, float RemainTime)
{
	if (IsValid(SDKRightButtonBase))
	{
		// 무기 및 캐릭터 스킬
		SDKRightButtonBase->UpdateSkillCoolTime(index, RemainTime);
	}
}

void USDKUIGoldClashWidget::TouchStartedSkillButton(int32 TouchpadIdx, FVector2D vStart, bool bAttackButton /*= false*/, int32 SkillIndex /*= -1*/)
{
	if (IsValid(SDKRightButtonBase))
	{
		if (bAttackButton)
		{
			SDKRightButtonBase->SetTouchStartAttackButton(TouchpadIdx, vStart);
		}
		else
		{
			SDKRightButtonBase->SetTouchStartSkillButton(TouchpadIdx, vStart, SkillIndex);
		}

		SetVisibilitySkillCancel(SkillIndex >= 0);
	}
}

void USDKUIGoldClashWidget::SetVisibilitySkillCancel(bool bVisible)
{
	if (!IsValid(SDKRightButtonBase))
	{
		return;
	}

	bool bSkillVisibility = bVisible;
#if WITH_EDITOR
	// Test용
	USDKGameInstance* SDKGameInstance = GetGameInstance<USDKGameInstance>();
	if (IsValid(SDKGameInstance))
	{
		if (SDKGameInstance->GetPlatformName() != TEXT("Android"))
		{
			bSkillVisibility = false;
		}
	}
#endif

#if PLATFORM_DESKTOP
	bSkillVisibility = false;
#endif

	SDKRightButtonBase->SetVisibilitySkillCancel(bSkillVisibility);
}

void USDKUIGoldClashWidget::TouchSkillCancelButton(bool bCancel, const FGeometry InGeometry, const FPointerEvent InGestureEvent)
{
	if (IsValid(SDKRightButtonBase))
	{
		SDKRightButtonBase->TouchSkillCancelButton(bCancel, GetTouchPosition(InGeometry, InGestureEvent));
		SetVisibilitySkillCancel(false);
	}
}

void USDKUIGoldClashWidget::SetVisibilityScreenEffect(EScreenEffectType InScreenEffectType)
{
	// 추후 스크린 이펙트가 추가되면 위젯 스위쳐로 변경
	switch (InScreenEffectType)
	{
	case EScreenEffectType::None:
	{
		if (IsValid(ScreenEffectHide))
		{
			ScreenEffectHide->CloseScreenEffect();
		}
	}
	break;
	case EScreenEffectType::Hide:
	{
		if (IsValid(ScreenEffectHide))
		{
			ScreenEffectHide->OpenUserWidget();
		}
	}
	break;
	default:
		break;
	}
}

/*** 레벨업 ******************************************************************************************************************************************/
void USDKUIGoldClashWidget::UseQuickLevelUpBuffCard(int32 InSlotIdx)
{
	// 레벨업 튜토리얼 과정은 Level BP에서 처리되도록 처리.
	if (FSDKHelpers::GetGameMode(GetWorld()) == EGameMode::GoldClashTutorial && ScaleBoxLevelUpQuick->GetVisibility() == ESlateVisibility::Collapsed)
	{
		return;
	}

	if (IsValid(SDKLevelUpQuick))
	{
		SDKLevelUpQuick->UseApplyLevelUpBuffCard(InSlotIdx);
	}
}

void USDKUIGoldClashWidget::PlayQuickLevelUpCloseAnim()
{
	if (IsValid(SDKLevelUpQuick))
	{
		SDKLevelUpQuick->CloseLevelupBuffCard();
	}
}

void USDKUIGoldClashWidget::ToggleLevelUpRewardWidget(bool bVisible)
{
	if (bVisible)
	{
		SetWidgetVisibility(SDKLevelUpCard, ESlateVisibility::SelfHitTestInvisible);

		if (FSDKHelpers::GetGameMode(GetWorld()) == EGameMode::GoldClashTutorial && TutorialStepSingle == ETutorialStepSingle::LevelUpBuffGuideBegin)
		{
			ActiveTutorialGuideLevelUpBuff();
		}
	}
	else
	{
		SetWidgetVisibility(SDKLevelUpCard, ESlateVisibility::Collapsed);

		if (FSDKHelpers::GetGameMode(GetWorld()) == EGameMode::GoldClashTutorial && TutorialStepSingle == ETutorialStepSingle::LevelUpBuffGuideBegin)
		{
			TutorialStepSingle = ETutorialStepSingle::LevelUpBuffGuideEnd;
			TutorialUIProcess();
		}
	}

	if (IsValid(SDKLevelUpCard) && bVisible)
	{
		SDKLevelUpCard->UpdateLevelUpBuffList();
	}
}

void USDKUIGoldClashWidget::UpdateHaveBuffList(int32 InTableID)
{
	if (IsValid(SDKLevelUpCard))
	{
		SDKLevelUpCard->AddHaveLevelUpBuffList(InTableID);
	}
}

void USDKUIGoldClashWidget::ActiveLevelUpBuffCard(int32 Count)
{
	UE_LOG(LogArena, Log, TEXT("ActiveLevelUpBuffCard"));

	if (Count < 0)
	{
		UE_LOG(LogArena, Log, TEXT("LevelUpBuffCard_UIOpen Failed : OpenCount <= 0"));
		return;
	}

	ASDKPlayerCharacter* SDKPlayerCharacter = GetOwningPlayerPawn<ASDKPlayerCharacter>();
	if (!IsValid(SDKPlayerCharacter))
	{
		UE_LOG(LogArena, Error, TEXT("RaidLog_ActiveLevelUpBuffCard_Error: SDKPlayerCharacter == nullptr"));
		return;
	}

	//if (SDKPlayerCharacter->IsDie() || SDKPlayerCharacter->IsGroggy())
	//{
	//	UE_LOG(LogArena, Error, TEXT("ActiveLevelUpBuffCard_Error: die() or groggy()"));
	//	return;
	//}

	ASDKInGameController* SDKIngameController = GetOwningPlayer<ASDKInGameController>();
	if (!IsValid(SDKIngameController))
	{
		UE_LOG(LogArena, Error, TEXT("RaidLog_ActiveLevelUpBuffCard_Error: SDKIngameController == nullptr"));
		return;
	}

	bool bOpened = SDKLevelUpCard->GetVisibility() != ESlateVisibility::Collapsed;
	if(bOpened)
	{
		// 레벨업버프를 다시 닫는 상황
		UE_LOG(LogArena, Log, TEXT("IngameLog_ActiveLevelUpBuffCard_ClientCloseLevelUpBuff"));

		// 레벨업 버프가 닫힐때는 점프 잠금 해제
		SDKPlayerCharacter->ClientEnableJump(true);

		ToggleLevelUpRewardWidget(false);
	}
	else
	{
		// 레벨업 버프가 열렸을때는 점프 잠금
		SDKPlayerCharacter->ClientEnableJump(false);

		UE_LOG(LogArena, Log, TEXT("IngameLog_ActiveLevelUpBuffCard_ClientOpenLevelUpBuff"));
		SDKIngameController->CreateLevelUpBuffCard(FLevelUpBuffData(), true);
	}

	SDKIngameController->CheckGoldClashWidgetClose();
}

void USDKUIGoldClashWidget::UpdateLevelUpBuffHaveList(const TArray<FBuffData>& BuffList)
{
	if (IsValid(SDKRightButtonBase))
	{
		SDKRightButtonBase->UpdateLevelupBuffList(BuffList);
	}
}

void USDKUIGoldClashWidget::AddSelectedLevelupBuff(int32 BuffID)
{
	if (IsValid(SDKRightButtonBase))
	{
		SDKRightButtonBase->AddSelectedLevelupBuff(BuffID);
	}
}

void USDKUIGoldClashWidget::ClearLevelupBuffList()
{
	if (IsValid(SDKRightButtonBase))
	{
		SDKRightButtonBase->ClearLevelupBuffList();
	}
}

void USDKUIGoldClashWidget::UpdateLevelUpBuffCardList(int32 First, int32 Second, int32 Third)
{
	if (IsValid(SDKLevelUpCard))
	{
		SDKLevelUpCard->SetLevelUpBuffID(First, Second, Third);
	}
}

void USDKUIGoldClashWidget::UpdateLevelUpBuffCount(int32 InCount)
{
	if (IsValid(SDKLevelUpCard))
	{
		SDKLevelUpCard->SetRemainLevelUpBuffCount(InCount);
	}
}

void USDKUIGoldClashWidget::CloseLevelUpBufCardfList()
{
	if (IsValid(SDKLevelUpCard))
	{
		SDKLevelUpCard->CloseChoiceData();
	}
}

/*** 버프 ********************************************************************************************************************************************/
void USDKUIGoldClashWidget::AddBuff(int32 BuffID, USDKUserWidget* BuffWidget)
{
	if (BuffID <= 0)
	{
		UE_LOG(LogWidget, Warning, TEXT("No Have TableID : %d"), BuffID);
		return;
	}

	if (!IsValid(BuffWidget))
	{
		UE_LOG(LogWidget, Warning, TEXT("BuffWidget Is NULL"));
		return;
	}

	auto BuffTable = USDKTableManager::Get()->FindTableRow<FS_Buff>(ETableType::tb_Buff, FString::FromInt(BuffID));
	if (BuffTable != nullptr)
	{
		int32 Count = 0;
		UGridSlot* pBuffSlot = nullptr;

		if (BuffTable->BuffType == EBuffType::Buff)
		{
			if (IsValid(GridPanelBuff))
			{
				Count = GridPanelBuff->GetChildrenCount();
				pBuffSlot = GridPanelBuff->AddChildToGrid(BuffWidget, Count / MaxBuffIconColumn, Count % MaxBuffIconColumn);
			}

			if (!mapBuffList.Contains(BuffID))
			{
				mapBuffList.Add(BuffID, pBuffSlot);
			}
			else
			{
				mapBuffList[BuffID] = pBuffSlot;
			}
		}
		else
		{
			if (IsValid(GridPanelDebuff))
			{
				Count = GridPanelDebuff->GetChildrenCount();
				pBuffSlot = GridPanelDebuff->AddChildToGrid(BuffWidget, Count / MaxBuffIconColumn, Count % MaxBuffIconColumn);
			}

			if (!mapDebuffList.Contains(BuffID))
			{
				mapDebuffList.Add(BuffID, pBuffSlot);
			}
			else
			{
				mapDebuffList[BuffID] = pBuffSlot;
			}
		}

		UpdateBuffRenderOpacity(BuffTable->BuffType);
	}
}

void USDKUIGoldClashWidget::RemoveBuff(int32 BuffID, USDKUserWidget* BuffWidget)
{
	if (BuffID <= 0)
	{
		UE_LOG(LogWidget, Warning, TEXT("No Have TableID : %d"), BuffID);
		return;
	}

	auto BuffTable = USDKTableManager::Get()->FindTableRow<FS_Buff>(ETableType::tb_Buff, FString::FromInt(BuffID));
	if (BuffTable != nullptr)
	{
		int32 Count = 0;
		TArray<UGridSlot*> BuffGridSlots;

		if (BuffTable->BuffType == EBuffType::Buff)
		{
			if (IsValid(GridPanelBuff))
			{
				Count = GridPanelBuff->GetChildrenCount();
				if (Count > 0)
				{
					GridPanelBuff->RemoveChild(BuffWidget);
				}
			}

			if (mapBuffList.Contains(BuffID))
			{
				mapBuffList.Remove(BuffID);
			}

			mapBuffList.GenerateValueArray(BuffGridSlots);
			SortBuffIconList(GridPanelBuff, BuffGridSlots);
		}
		else
		{
			if (IsValid(GridPanelDebuff))
			{
				Count = GridPanelDebuff->GetChildrenCount();
				if (Count > 0)
				{
					GridPanelDebuff->RemoveChild(BuffWidget);
				}
			}

			if (mapDebuffList.Contains(BuffID))
			{
				mapDebuffList.Remove(BuffID);
			}

			mapDebuffList.GenerateValueArray(BuffGridSlots);
			SortBuffIconList(GridPanelDebuff, BuffGridSlots);
		}

		UpdateBuffRenderOpacity(BuffTable->BuffType);
	}
}

void USDKUIGoldClashWidget::SortBuffIconList(UGridPanel* BuffGrid, TArray<UGridSlot*> BuffList)
{
	if (BuffList.Num() <= 0)
	{
		return;
	}

	for (auto itBuff = BuffList.CreateIterator(); itBuff; ++itBuff)
	{
		int32 index = itBuff.GetIndex();
		if (IsValid(*itBuff))
		{
			(*itBuff)->SetRow(index / MaxBuffIconColumn);
			(*itBuff)->SetColumn(index % MaxBuffIconColumn);
		}
	}
}

void USDKUIGoldClashWidget::UpdateBuffRenderOpacity(EBuffType BuffType)
{
	TArray<UWidget*> BuffWidgetList = GridPanelDebuff->GetAllChildren();
	if (BuffType == EBuffType::Buff)
	{
		GridPanelBuff->GetAllChildren();
	}
	
	if (BuffWidgetList.Num() > 0)
	{
		int32 CurMaxColumn = BuffWidgetList.Num() / MaxBuffIconColumn;

		for (int32 i = BuffWidgetList.Num() - 1; i >= 0; --i)
		{
			int32 CurColumn = i / MaxBuffIconColumn;
			if (CurMaxColumn == CurColumn)
			{
				BuffWidgetList[i]->SetRenderOpacity(1.f);
			}
			else if ((CurMaxColumn - 1) == CurColumn)
			{
				BuffWidgetList[i]->SetRenderOpacity(0.8f);
			}
			else if ((CurMaxColumn - 2) == CurColumn)
			{
				BuffWidgetList[i]->SetRenderOpacity(0.4f);
			}
			else
			{
				BuffWidgetList[i]->SetRenderOpacity(0.f);
			}
		}
	}
}

/*** 유물 ********************************************************************************************************************************************/
void USDKUIGoldClashWidget::UpdateArtifactList(const TArray<struct FItemSlotData>& UpdateInventory)
{
	// [hajang] 나중에 세트 별로 정리 ID 순서 정리

	TArray<int32> ArtifactIDs;
	for (auto& iter : UpdateInventory)
	{
		ArtifactIDs.AddUnique(iter.TableID);
	}

	UpdateSortAritfactWidgetList(ArtifactIDs);
}

void USDKUIGoldClashWidget::UpdateSortAritfactWidgetList(const TArray<int32>& ArtifactIDs)
{
	if (IsValid(HorizontalBoxArtifact) == false)
	{
		return;
	}

	int32 MaxCount = FMath::Max(ArtifactIDs.Num(), ArtifactIconList.Num());

	for (int32 i = 0; i < MaxCount; ++i)
	{
		bool bHaveData = false;
		if (ArtifactIconList.IsValidIndex(i))
		{
			bHaveData = ArtifactIDs.IsValidIndex(i) == true;
			SetWidgetVisibility(ArtifactIconList[i], bHaveData ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
		else
		{
			if (ArtifactIDs.IsValidIndex(i))
			{
				auto WidgetTable = USDKTableManager::Get()->FindTableWidgetBluePrint(EWidgetBluePrint::Artifact_IngameIcon);
				if (WidgetTable != nullptr)
				{
					TSubclassOf<USDKUserWidget> WidgetClass(*USDKAssetManager::Get().LoadSynchronous(WidgetTable->WidgetBluePrintPath));
					if (IsValid(WidgetClass))
					{
						auto IconWidget = CreateWidget<USDKUserWidget>(GetOwningPlayer(), WidgetClass);
						if (IsValid(IconWidget))
						{
							bHaveData = true;
							ArtifactIconList.Add(IconWidget);

							auto pHorizontalSlot = HorizontalBoxArtifact->AddChildToHorizontalBox(IconWidget);
							if (IsValid(pHorizontalSlot))
							{
								pHorizontalSlot->SetPadding(WidgetTable->Padding);
							}
						}
					}
				}
			}
		}

		if (bHaveData && IsValid(ArtifactIconList[i]))
		{
			auto ItemTable = USDKTableManager::Get()->FindTableRow<FS_Item>(ETableType::tb_Item, FString::FromInt(ArtifactIDs[i]));
			if (ItemTable != nullptr)
			{
				auto pImageIcon = Cast<UImage>(ArtifactIconList[i]->GetWidgetFromName(FName("ImageIcon")));
				SetImageTexturePath(pImageIcon, ItemTable->IconPath.ToString());
			}
		}
	}
}

/*** 초기화 ******************************************************************************************************************************************/
void USDKUIGoldClashWidget::InitTextBlockString()
{
	// 팀원 생존
	SetTextBlockTableText(TextSpectatorNoti, TEXT("UIText_UpdateWaiting"));
	SetTextBlockTableText(TextRespawnTitle, TEXT("UIText_RevivalRemainTime"));
	SetTextBlockTableText(TextSurrender, TEXT("UIText_GoldClash_Button_Surrender"));

	for (int32 i = 1; i <= 2; ++i)
	{
		UTextBlock* TextGuid = Cast<UTextBlock>(GetWidgetFromName(*FString::Printf(TEXT("TextkGuide_%d"), i)));
		TextGuidList.Add(TextGuid);
	}

	// 모바일일때 단축키 텍스트 비활성화
#if PLATFORM_ANDROID || PLATFORM_IOS
	SetWidgetVisibility(TextStatus, ESlateVisibility::Collapsed);
	if (TextGuidList.Num() > 0)
	{
		for (int32 i = 0; i < TextGuidList.Num(); ++i)
		{
			SetWidgetVisibility(TextGuidList[i], ESlateVisibility::Collapsed);
		}
	}
#endif
}

void USDKUIGoldClashWidget::InitTeamMemberImages()
{
	for (int32 i = 1; i <= 5; i++)
	{
		UImage* ImageTeam = Cast<UImage>(GetWidgetFromName(*FString::Printf(TEXT("Team_%d"), i)));
		if (IsValid(ImageTeam))
		{
			SetWidgetVisibility(ImageTeam, ESlateVisibility::Collapsed);

			MemberDirIcons.Emplace(ImageTeam);
		}
	}
}

void USDKUIGoldClashWidget::SetActiveTutorialGoldClashMode(bool bActive)
{
	SetWidgetVisibility(OverlayStatus, bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::SelfHitTestInvisible);
	SetWidgetVisibility(SDKGoldClashRank, bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::SelfHitTestInvisible);
	SetWidgetVisibility(OverlayGoldClashScore, bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::SelfHitTestInvisible);
}


void USDKUIGoldClashWidget::PlayAnimTutorialGuideDodge(bool bActivate)
{
	if (IsValid(SDKRightButtonBase))
	{
		if (bActivate)
		{
			SDKRightButtonBase->SetDodgeButtonVisibility(true);
			SDKRightButtonBase->PlayAnimDodgeButtonTutorial();
		}
		else
		{
			SDKRightButtonBase->StopAnimDodgeButtonTutorial();
		}
	}
}

void USDKUIGoldClashWidget::PlayAnimTutorialGuideJump(bool bActivate)
{
	if (IsValid(SDKRightButtonBase))
	{
		if (bActivate)
		{
			SDKRightButtonBase->SetJumpButtonVisibility(true);
			SDKRightButtonBase->PlayAnimJumpButtonTutorial();
		}
		else
		{
			SDKRightButtonBase->StopAnimJumpButtonTutorial();
		}

	}
}

void USDKUIGoldClashWidget::PlayAnimTutorialGuideLevelUpQuick(bool bActivate)
{
	if (IsValid(SDKLevelUpQuick))
	{
		if (bActivate)
		{
			SDKLevelUpQuick->PlayAnimLevelUpQuickButtonTutorial();
		}
		else
		{
			SDKLevelUpQuick->StopAnimLevelUpQuickButtonTutorial();
		}
	}
}

void USDKUIGoldClashWidget::SetTutorialVisibilityLevelUpBuffBox(bool bVisible)
{
	SetWidgetVisibility(ScaleBoxLevelUpQuick, bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);

	if(bVisible)
	{
		SDKLevelUpQuick->PlayOpenAnimation();
	}
}

void USDKUIGoldClashWidget::SetTutorialGuideLevelUpBuff(int32 GuideID)
{
	if (FSDKHelpers::GetGameMode(GetWorld()) == EGameMode::GoldClashTutorial)
	{
		USDKGameInstance* SDKGameInstance = GetGameInstance<USDKGameInstance>();
		if (IsValid(SDKGameInstance))
		{
			if (TutorialWidgetList.Num() <= 0)
			{
				AddTutorialWidgetList(SDKLevelUpQuick->GetWidgetFromName(FName("ButtonDetail")));
				AddTutorialWidgetList(SDKLevelUpCard->GetWidgetFromName(FName("WidgetSwitcherList")));
			}

			TutorialStepSingle = ETutorialStepSingle::LevelUpBuffGuideBegin;
			// 튜토리얼 가이드 메시지 등록
			USDKTutorialGuideManager::Get()->SetInstantGuideID(GuideID);
			USDKTutorialGuideManager::Get()->EndGuideDelegate.BindUFunction(this, FName("OnClickedSkipLevelUpBuffTutorial"));
			TutorialUserWidget();
		}
	}
}

void USDKUIGoldClashWidget::SetTutorialGuideGoldShop(int32 GuideID)
{
	if (FSDKHelpers::GetGameMode(GetWorld()) == EGameMode::GoldClashTutorial)
	{
		USDKGameInstance* SDKGameInstance = GetGameInstance<USDKGameInstance>();
		if (IsValid(SDKGameInstance))
		{
			if (TutorialWidgetList.Num() <= 0)
			{
				AddTutorialWidgetList(GetWidgetFromName(FName("ScaleBox_2")));
			}
			
			RecommendArtifactSlot->GetSelectButton()->OnClicked.AddDynamic(this, &USDKUIGoldClashWidget::OnClickedTutorialBuyRecommnadedArtifact);
			USDKTutorialGuideManager::Get()->EndGuideDelegate.BindUFunction(this, FName("OnClickedTutorialBuyRecommnadedArtifact"));
			USDKTutorialGuideManager::Get()->SetInstantGuideID(GuideID);
			ToggleLevelUpRewardWidget(false);
			TutorialStepSingle = ETutorialStepSingle::GoldShopGuideBegin;

			TutorialUserWidget();
		}
	}
}

void USDKUIGoldClashWidget::ActiveTutorialGuideLevelUpBuff()
{
	if (USDKTutorialGuideManager::Get()->IsActiveGuide())
	{
		USDKTutorialGuideManager::Get()->NextGuide(0.3f);

		USDKGameInstance* SDKGameInstance = GetGameInstance<USDKGameInstance>();
		if (IsValid(SDKGameInstance))
		{
			// 레벨 함수 호출
			const ULevel* Level = GetWorld()->GetLevel(0);
			if (IsValid(Level) && IsValid(Level->GetLevelScriptActor()))
			{
				FOutputDeviceNull ar;
				Level->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("OnClickedTutorialLevelUpBuff"), ar, NULL, true);
			}
		}
	}
}

void USDKUIGoldClashWidget::ActiveTutorialGuideGoldShop()
{
	TutorialStepSingle = ETutorialStepSingle::GoldShopGuideEnd;
	USDKTutorialGuideManager::Get()->EndGuide();
}

void USDKUIGoldClashWidget::SetTutorialGuideOpenShopAndBuyArtifact(int32 GuideID)
{
	if (FSDKHelpers::GetGameMode(GetWorld()) == EGameMode::GoldClashTutorial)
	{
		USDKGameInstance* SDKGameInstance = GetGameInstance<USDKGameInstance>();
		if (IsValid(SDKGameInstance))
		{
			if (TutorialWidgetList.Num() <= 0)
			{
				AddTutorialWidgetList(ButtonShop);
			}

			USDKGameDelegate::Get()->OnOpenGoldClashShop.AddDynamic(this, &USDKUIGoldClashWidget::OnClickedButtonTutorialOpenShop);
			//ButtonShop->OnClicked.AddDynamic(this, &USDKUIGoldClashWidget::OnClickedButtonTutorialOpenShop);
			USDKTutorialGuideManager::Get()->EndGuideDelegate.BindUFunction(this, FName("OnEndGoldShopSetArtifactTutorial"));
			USDKTutorialGuideManager::Get()->SetInstantGuideID(GuideID);
			TutorialStepSingle = ETutorialStepSingle::GoldShopSetArtifactGuideBegin;
			SetWidgetVisibility(ButtonShop, ESlateVisibility::Visible);
			USDKTutorialGuideManager::Get()->StartGuide();
		}
	}
}

void USDKUIGoldClashWidget::OnClickedSkipLevelUpBuffTutorial()
{
	TutorialStepSingle = ETutorialStepSingle::GoldBoxGuideBegin;
}

void USDKUIGoldClashWidget::OnClickedTutorialBuyRecommnadedArtifact()
{
	TutorialStepSingle = ETutorialStepSingle::GoldShopGuideEnd;
	RecommendArtifactSlot->GetSelectButton()->OnClicked.RemoveDynamic(this, &USDKUIGoldClashWidget::OnClickedTutorialBuyRecommnadedArtifact);
	SetWidgetVisibility(RecommendArtifactSlot, ESlateVisibility::HitTestInvisible);
	SetWidgetVisibility(ButtonShop, ESlateVisibility::HitTestInvisible);
	USDKTutorialGuideManager::Get()->EndGuideDelegate.Unbind();
	USDKTutorialGuideManager::Get()->EndGuide();
}

void USDKUIGoldClashWidget::OnClickedButtonTutorialOpenShop(bool IsOpen)
{
	USDKGameDelegate::Get()->OnOpenGoldClashShop.RemoveDynamic(this, &USDKUIGoldClashWidget::OnClickedButtonTutorialOpenShop);
	TutorialStepSingle = ETutorialStepSingle::GoldShopSetArtifactGuideBegin;
	USDKTutorialGuideManager::Get()->EndGuideDelegate.Unbind();
	USDKTutorialGuideManager::Get()->EndGuide();
	SetWidgetVisibility(RecommendArtifactSlot, ESlateVisibility::Visible);
	TutorialTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &USDKUIGoldClashWidget::OnTimerBroadcastOpenShop);
}

void USDKUIGoldClashWidget::OnTimerBroadcastOpenShop()
{
	USDKGameDelegate::Get()->OnGoldClashShopTutorial.Broadcast();
	GetWorld()->GetTimerManager().ClearTimer(TutorialTimerHandle);
}

void USDKUIGoldClashWidget::OnEndGoldShopSetArtifactTutorial()
{
	TutorialStepSingle = ETutorialStepSingle::GoldShopSetArtifactGuideEnd;

	SetWidgetVisibility(RecommendArtifactSlot, ESlateVisibility::Visible);

	if (USDKGameDelegate::Get()->OnOpenGoldClashShop.Contains(this, TEXT("OnClickedButtonTutorialOpenShop")))
	{
		USDKGameDelegate::Get()->OnOpenGoldClashShop.RemoveDynamic(this, &USDKUIGoldClashWidget::OnClickedButtonTutorialOpenShop);
	}
}

void USDKUIGoldClashWidget::SetButtonVisibilityTutorial(bool bVisible)
{
	if (IsValid(SDKRightButtonBase))
	{
		SDKRightButtonBase->SetButtonVisibilityFromTutorial(bVisible);
	}
}