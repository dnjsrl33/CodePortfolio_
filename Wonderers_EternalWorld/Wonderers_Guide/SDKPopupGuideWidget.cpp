// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SDKPopupGuideWidget.h"

#include "BinkMediaPlayer.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/SizeBox.h"
#include "Components/WidgetSwitcher.h"

#include "Manager/SDKTableManager.h"
#include "Engine/SDKAssetManager.h"
#include "Engine/SDKGameInstance.h"

#include "Character/SDKHUD.h"
#include "Character/SDKOpenWorldController.h"

#include "UI/SDKButtonWidget.h"

#include "UI/SDKGuideMainMenuTapWidget.h"
#include "UI/SDKGuideSubMenuTapWidget.h"
#include "UI/SDKCheckBoxParam.h"
#include "UI/SDKPageIconWidget.h"


void USDKPopupGuideWidget::CreateUIProcess()
{
	Super::CreateUIProcess();

	if(IsValid(CloseButtonWidget) == false)
	{
		return;
	}

	CloseButton.SetButton(CloseButtonWidget);
	CloseButton.GetButton()->OnClicked.AddDynamic(this, &USDKPopupGuideWidget::OnClickedClose);

	ButtonWarp->GetButton()->OnClicked.AddDynamic(this, &USDKPopupGuideWidget::OnClickedGuideWarp);

	OpenAnim = GetWidgetAnimation(TEXT("On"));

	ButtonPrevContent->OnClicked.AddDynamic(this, &USDKPopupGuideWidget::OnClickedPrevGuidePage);
	ButtonNextContent->OnClicked.AddDynamic(this, &USDKPopupGuideWidget::OnClickedNextGuidePage);
}

void USDKPopupGuideWidget::OpenUIProcess()
{
	Super::OpenUIProcess();
	
	if (IsValid(OpenAnim))
	{
		PlayAnimation(OpenAnim);
	}
}

void USDKPopupGuideWidget::CloseUIProcess()
{
	Super::CloseUIProcess();

	if (IsValid(BinkMediaPlayer))
	{
		BinkMediaPlayer->Close();
		BinkMediaPlayer = nullptr;
	}

	ASDKHUD* SDKHUD = Cast<ASDKHUD>(GetOwningPlayer()->GetHUD());

	if (IsValid(SDKHUD) == false)
	{
		return;
	}

	if (InputCloseUIHandle.IsValid())
	{
		SDKHUD->CloseSubUIEvent.Remove(InputCloseUIHandle);
	}
	
}

void USDKPopupGuideWidget::OnClickedClose()
{
	ASDKHUD* SDKHUD = Cast<ASDKHUD>(GetOwningPlayer()->GetHUD());

	if (IsValid(SDKHUD) == false)
	{
		return;
	}

	SDKHUD->CloseUI(GetUI());
}

void USDKPopupGuideWidget::OnClickedPrevGuidePage()
{
	FS_InstantGuideUI* GuideTable = USDKTableManager::Get()->FindTableRow<FS_InstantGuideUI>(ETableType::tb_InstantGuideUI, CurrentGuideID);
	if (!GuideTable)
	{
		return;
	}

	CurrentGuideIndex = FMath::Clamp(CurrentGuideIndex - 1, 0, CurrentGuideIndexMax);

	if (GuideTable->GuideImagePath.Num() <= 0)
	{
		// 임시 : 테이블작업이 되어있지않아 스위쳐로 준비중 이미지 처리
		SetWidgetSwitcherActiveWidgetIndex(SwitcherEmpty, 1);
	}
	else
	{
		SetWidgetSwitcherActiveWidgetIndex(SwitcherEmpty, 0);
		if (GuideTable->GuideImagePath.IsValidIndex(CurrentGuideIndex))
		{
			SetImageTexturePath(ImageContent, GuideTable->GuideImagePath[CurrentGuideIndex].ToString());
		}
	}

	SetTextBlockTableText(TextDesc, GuideTable->GuideText[CurrentGuideIndex].ToString());

	SetButtonIsEnabled(ButtonPrevContent, CurrentGuideIndex <= 0 ? false : true);
	SetButtonIsEnabled(ButtonNextContent, CurrentGuideIndex >= CurrentGuideIndexMax ? false : true);
	SetActivePageIcon(CurrentGuideIndex);
}

void USDKPopupGuideWidget::OnClickedNextGuidePage()
{
	FS_InstantGuideUI* GuideTable = USDKTableManager::Get()->FindTableRow<FS_InstantGuideUI>(ETableType::tb_InstantGuideUI, CurrentGuideID);
	if (!GuideTable)
	{
		return;
	}

	CurrentGuideIndex = FMath::Clamp(CurrentGuideIndex + 1, 0, CurrentGuideIndexMax);

	if (GuideTable->GuideImagePath.Num() <= 0)
	{
		// 임시 : 테이블작업이 되어있지않아 스위쳐로 준비중 이미지 처리
		SetWidgetSwitcherActiveWidgetIndex(SwitcherEmpty, 1);
	}
	else
	{
		SetWidgetSwitcherActiveWidgetIndex(SwitcherEmpty, 0);

		if (GuideTable->GuideImagePath.IsValidIndex(CurrentGuideIndex))
		{
			SetImageTexturePath(ImageContent, GuideTable->GuideImagePath[CurrentGuideIndex].ToString());
		}
	}


	if (GuideTable->GuideText.IsValidIndex(CurrentGuideIndex))
	{
		SetTextBlockTableText(TextDesc, GuideTable->GuideText[CurrentGuideIndex].ToString());
	}
	
	SetButtonIsEnabled(ButtonPrevContent, CurrentGuideIndex <= 0 ? false : true);
	SetButtonIsEnabled(ButtonNextContent, CurrentGuideIndex >= CurrentGuideIndexMax ? false : true);

	SetActivePageIcon(CurrentGuideIndex);
}

void USDKPopupGuideWidget::OnClickedGuideWarp()
{
	FS_InstantGuideUI* GuideTable = USDKTableManager::Get()->FindTableRow<FS_InstantGuideUI>(ETableType::tb_InstantGuideUI, CurrentGuideID);
	if (!GuideTable)
	{
		return;
	}

	if (GuideTable->TrackingID <= 0)
	{
		return;
	}
	ASDKOpenWorldController* SDKOpenWorldController = GetOwningPlayer<ASDKOpenWorldController>();
	if (!IsValid(SDKOpenWorldController))
	{
		return;
	}

	SDKOpenWorldController->ClientTeleportStart(FString::FromInt(GuideTable->TrackingID));

}

void USDKPopupGuideWidget::OnKeyInputCloseAllGuide()
{
	//Close Dummy
	ASDKHUD* SDKHUD = Cast<ASDKHUD>(GetOwningPlayer()->GetHUD());

	if (IsValid(SDKHUD) == false)
	{
		return;
	}

	SDKHUD->CloseUI(GetUI());
}

void USDKPopupGuideWidget::SetActivePageIcon(int32 Index)
{
	for(int32 i = 0; i < PageIconList.Num(); i++)
	{
		PageIconList[i]->SetActiveIcon(i == Index);
	}
}

void USDKPopupGuideWidget::SetVisibilityBoxGuideMenu(bool bVisible)
{
	SetWidgetVisibility(BoxGuideMenu, bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void USDKPopupGuideWidget::SetGuideUI(const FString& Title, const TArray<FName>& Desc, const TArray<TSoftObjectPtr<UTexture2D>>& ImagePath)
{
	if(Title.IsEmpty())
	{
		return;
	}

	if(Desc.Num() <= 0)
	{
		return;
	}

	if(ImagePath.Num() < 0)
	{
		return;
	}

	SetTextBlockTableText(TextTitle, Title);
	SetTextBlockTableText(TextDesc, Desc[0].ToString());
	SetImageTexturePath(ImageContent, ImagePath[0].ToString());
}

void USDKPopupGuideWidget::SetGuideUI(int32 GuideTableID, bool bShowGuideMenu/* = false*/)
{
	auto WidgetTable = USDKTableManager::Get()->FindTableWidgetBluePrint(EWidgetBluePrint::PageIcon);
	if (nullptr == WidgetTable)
	{
		return;
	}

	if (WidgetTable->WidgetBluePrintPath.IsNull())
	{
		return;
	}

	TSubclassOf<USDKUserWidget> pWidget(*USDKAssetManager::Get().LoadSynchronous(WidgetTable->WidgetBluePrintPath));
	if (nullptr == pWidget)
	{
		return;
	}

	CurrentGuideID = FString::FromInt(GuideTableID);

	FS_InstantGuideUI* GuideTable = USDKTableManager::Get()->FindTableRow<FS_InstantGuideUI>(ETableType::tb_InstantGuideUI, CurrentGuideID);
	if (!GuideTable)
	{
		return;
	}


	PageIconList.Empty();
	BoxPageIconList->ClearChildren();

	SetTextBlockTableText(TextTitle, GuideTable->GuideCategoryName);
	SetTextBlockTableText(TextSubTitle, GuideTable->Title);

	SetVisibilityBoxGuideMenu(bShowGuideMenu);

	ASDKHUD* SDKHUD = Cast<ASDKHUD>(GetOwningPlayer()->GetHUD());

	if (IsValid(SDKHUD) == false)
	{
		return;
	}

	if (bShowGuideMenu && !InputCloseUIHandle.IsValid())
	{
		InputCloseUIHandle = SDKHUD->CloseSubUIEvent.AddUObject(this, &USDKPopupGuideWidget::OnKeyInputCloseAllGuide);
	}
	

	if (GuideTable->GuidePageCount > 0 && !GuideTable->IsMedia)
	{
		for (int32 i = 0; i < GuideTable->GuidePageCount; i++)
		{
			USDKPageIconWidget* IconWidget = CreateWidget<USDKPageIconWidget>(GetWorld(), pWidget);
			if (!IsValid(IconWidget))
			{
				return;
			}

			IconWidget->CreateUserWidget();
			IconWidget->SetPadding(WidgetTable->Padding);
			IconWidget->SetActiveIcon(false);

			PageIconList.Add(IconWidget);
			BoxPageIconList->AddChildToHorizontalBox(IconWidget);
		}

		CurrentGuideIndex = 0;
		CurrentGuideIndexMax = GuideTable->GuidePageCount - 1;

		SetWidgetVisibility(Page, CurrentGuideIndex == CurrentGuideIndexMax ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

		if (GuideTable->GuideImagePath.Num() <= 0)
		{
			// 임시 : 테이블작업이 되어있지않아 스위쳐로 준비중 이미지 처리
			SetWidgetSwitcherActiveWidgetIndex(SwitcherEmpty, 1);
		}
		else
		{
			SetWidgetSwitcherActiveWidgetIndex(SwitcherEmpty, 0);
			SetImageTexturePath(ImageContent, GuideTable->GuideImagePath[0].ToString());
		}
		
		SetTextBlockTableText(TextDesc, GuideTable->GuideText[0].ToString());
		PageIconList[0]->SetActiveIcon(true);


		SetWidgetVisibility(ButtonPrevContent, ESlateVisibility::Visible);
		SetWidgetVisibility(ButtonNextContent, ESlateVisibility::Visible);

		SetButtonIsEnabled(ButtonPrevContent, false);
		SetButtonIsEnabled(ButtonNextContent, true);
	}
	else
	{
		if(!GuideTable->GuideMediaPath.IsValid())
		{
			return;
		}

		FS_MediaPlayer* MediaTable = USDKTableManager::Get()->FindTableRow<FS_MediaPlayer>(ETableType::tb_MediaPlayer, GuideTable->GuideMediaPath.ToString());
		if (!MediaTable)
		{
			return;
		}

		if (IsValid(BinkMediaPlayer))
		{
			BinkMediaPlayer->Close();
			BinkMediaPlayer = nullptr;
		}

		BinkMediaPlayer = USDKAssetManager::Get().LoadSynchronous<UBinkMediaPlayer>(MediaTable->BinkMediaPlayer);
		if (IsValid(BinkMediaPlayer))
		{
			UMaterial* MediaMaterial = USDKAssetManager::Get().LoadSynchronous<UMaterial>(MediaTable->BinkMediaMaterial);
			if (IsValid(MediaMaterial))
			{
				ImageContent->SetBrushFromMaterial(MediaMaterial);
			}

			// 재생
			BinkMediaPlayer->OpenUrl(USDKGameInstance::ConvertPatchableMoviePath(MediaTable->BinkMediaUrl));
			BinkMediaPlayer->Play();
		}

		SetWidgetVisibility(ButtonPrevContent, ESlateVisibility::Collapsed);
		SetWidgetVisibility(ButtonNextContent, ESlateVisibility::Collapsed);
	}

	if(GuideTable->TrackingID > 0)
	{
		SetWidgetVisibility(ButtonWarp, GuideTable->TrackingID > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void USDKPopupGuideWidget::SetAllGuideMenu()
{
	TArray<FName> GuideUIMenuNames = USDKTableManager::Get()->GetRowNames(ETableType::tb_InstantGuideUIMenu);
	
	MapMenu.Empty();
	MainMenuTapList.Empty();
	SubMenuTapList.Empty();

	for(const auto& Iter : GuideUIMenuNames)
	{
		FString TableID = Iter.ToString();
		FS_InstantGuideUIMenu* GuideMenuTable = USDKTableManager::Get()->FindTableRow<FS_InstantGuideUIMenu>(ETableType::tb_InstantGuideUIMenu, TableID);

		if (!GuideMenuTable)
		{
			continue;
		}

		if (GuideMenuTable->GuideID != 0)
		{
			continue;
		}

		MapMenu.Emplace(TableID, GuideMenuTable->SubMenu);
	}


	auto MainTapWidgetTable = USDKTableManager::Get()->FindTableWidgetBluePrint(EWidgetBluePrint::GuideMainMenuTap);
	if (nullptr == MainTapWidgetTable)
	{
		return;
	}

	if (MainTapWidgetTable->WidgetBluePrintPath.IsNull())
	{
		return;
	}

	TSubclassOf<USDKUserWidget> pMainWidget(*USDKAssetManager::Get().LoadSynchronous(MainTapWidgetTable->WidgetBluePrintPath));
	if (nullptr == pMainWidget)
	{
		return;
	}

	auto SubTapWidgetTable = USDKTableManager::Get()->FindTableWidgetBluePrint(EWidgetBluePrint::GuideSubMenuTap);
	if (nullptr == MainTapWidgetTable)
	{
		return;
	}

	if (SubTapWidgetTable->WidgetBluePrintPath.IsNull())
	{
		return;
	}

	TSubclassOf<USDKUserWidget> pSubWidget(*USDKAssetManager::Get().LoadSynchronous(SubTapWidgetTable->WidgetBluePrintPath));
	if (nullptr == pSubWidget)
	{
		return;
	}

	BoxGuideMenuList->ClearChildren();
	
	for (auto& Iter : MapMenu)
	{
		USDKGuideMainMenuTapWidget* SDKMainTap = CreateWidget<USDKGuideMainMenuTapWidget>(GetWorld(), pMainWidget);
		if (nullptr == SDKMainTap)
		{
			continue;
		}

		FS_InstantGuideUIMenu* GuideUIMainMenuTable = USDKTableManager::Get()->FindTableRow<FS_InstantGuideUIMenu>(ETableType::tb_InstantGuideUIMenu, Iter.Key);
		
		if (!GuideUIMainMenuTable)
		{
			continue;
		}

		SDKMainTap->CreateUserWidget();
		SDKMainTap->SetMenuID(Iter.Key);
		SDKMainTap->SetMenuTitle(GuideUIMainMenuTable->MenuName);

		USDKButtonParam* MainMenuParam = SDKMainTap->GetButtonParam();

		USDKWidgetParamString* NewMainParam = NewObject<USDKWidgetParamString>(this, USDKWidgetParamString::StaticClass());
		if (IsValid(NewMainParam))
		{
			NewMainParam->SetValue(Iter.Key);
			MainMenuParam->SetClickedParam(NewMainParam);
			MainMenuParam->OnClickedParam.AddDynamic(this, &USDKPopupGuideWidget::OnClickedMainMenu);
		}


		SDKMainTap->GetBoxSubMenuTapList()->ClearChildren();

		MainMenuTapList.Emplace(Iter.Key, SDKMainTap);
		BoxGuideMenuList->AddChildToVerticalBox(SDKMainTap);
		
		for(int32 i = 0; i < Iter.Value.Num(); i++)
		{
			USDKGuideSubMenuTapWidget* SDKSubTap = CreateWidget<USDKGuideSubMenuTapWidget>(GetWorld(), pSubWidget);
			if (nullptr == SDKMainTap)
			{
				continue;
			}

			FS_InstantGuideUIMenu* GuideUISubMenuTable = USDKTableManager::Get()->FindTableRow<FS_InstantGuideUIMenu>(ETableType::tb_InstantGuideUIMenu, Iter.Value[i]);

			if (!GuideUISubMenuTable)
			{
				continue;
			}

			SDKSubTap->CreateUserWidget();
			SDKSubTap->SetMenuID(Iter.Value[i]);
			SDKSubTap->SetMenuTitle(GuideUISubMenuTable->MenuName);

			USDKButtonParam* SubMenuParam = SDKSubTap->GetButtonParam();

			USDKWidgetParamString* NewParam = NewObject<USDKWidgetParamString>(this, USDKWidgetParamString::StaticClass());
			if (IsValid(NewParam))
			{
				NewParam->SetValue(Iter.Value[i]);
				SubMenuParam->SetClickedParam(NewParam);
				SubMenuParam->OnClickedParam.AddDynamic(this, &USDKPopupGuideWidget::OnClickedSubMenu);
			}

			SDKMainTap->GetBoxSubMenuTapList()->AddChildToVerticalBox(SDKSubTap);
			SubMenuTapList.Emplace(Iter.Value[i], SDKSubTap);

		}

	}

	// 첫 페이지만 보여주도록 설정
	for(const auto& Iter : SubMenuTapList)
	{
		Iter.Value->GetButtonParam()->OnClickedParam.Broadcast(Iter.Value->GetButtonParam()->GetClickedParam());
		break;
	}
}

UButton* USDKPopupGuideWidget::GetCloseButton()
{
	return CloseButton.GetButton();
}
void USDKPopupGuideWidget::OnClickedMainMenu(USDKWidgetParam* Param)
{
	if (!IsValid(Param))
	{
		return;
	}

	USDKWidgetParamString* StringParam = Cast<USDKWidgetParamString>(Param);
	if (!IsValid(StringParam))
	{
		return;
	}
	FString GuideUIMenuID = StringParam->GetValue();

	for (const auto& Iter : MapMenu)
	{
		MainMenuTapList[Iter.Key]->SetVisibilityChecked(Iter.Key == GuideUIMenuID);

		for (int32 i = 0; i < Iter.Value.Num(); i++)
		{
			SubMenuTapList[Iter.Value[i]]->SetVisibilityChecked(false);
		}
	}
}

void USDKPopupGuideWidget::OnClickedSubMenu(USDKWidgetParam* Param)
{
	if (!IsValid(Param))
	{
		return;
	}

	USDKWidgetParamString* StringParam = Cast<USDKWidgetParamString>(Param);
	if (!IsValid(StringParam))
	{
		return;
	}

	FString GuideUIMenuID = StringParam->GetValue();

	FS_InstantGuideUIMenu* GuideUIMenuTable = USDKTableManager::Get()->FindTableRow<FS_InstantGuideUIMenu>(ETableType::tb_InstantGuideUIMenu, GuideUIMenuID);
	if (!GuideUIMenuTable)
	{
		return;
	}

	FS_InstantGuideUI* GuideTable = USDKTableManager::Get()->FindTableRow<FS_InstantGuideUI>(ETableType::tb_InstantGuideUI, FString::FromInt(GuideUIMenuTable->GuideID));

	if (!GuideTable)
	{
		return;
	}


	for (const auto& Iter : MapMenu)
	{
		for (int32 i = 0; i < Iter.Value.Num(); i++)
		{
			if (Iter.Value[i] == GuideUIMenuID)
			{
				MainMenuTapList[Iter.Key]->SetVisibilityChecked(true);
				SubMenuTapList[Iter.Value[i]]->SetVisibilityChecked(true);
				continue;
			}

			SubMenuTapList[Iter.Value[i]]->SetVisibilityChecked(false);
		}
	}

	SetGuideUI(GuideUIMenuTable->GuideID, true);
}
