// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Settings/AlienSettingCategoryWidget.h"

#include "UI/Components/Text/AlienTextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "UI/Components/Button/AlienButtonBase.h"
#include "Paper2D/Classes/PaperSprite.h"
#include "Core/PlayerController/AlienOfflinePlayerController.h"
#include "Common/AlienUIStruct.h"
#include "Common/AlienGamePlayStatics.h"

void UAlienSettingCategoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UAlienSettingCategoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAlienSettingCategoryWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UAlienSettingCategoryWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
}

void UAlienSettingCategoryWidget::SetDisable()
{
	m_Button->SetIsEnabled(false);
}

void UAlienSettingCategoryWidget::SetTitleTableText(FString InStr)
{
	m_CategoryNameText->SetStringID(InStr);
	m_CategoryNameText->Update();
}

void UAlienSettingCategoryWidget::SetImageTexture2D(const FString& TexturePath)
{
	UPaperSprite* LoadedIcon = LoadObject<UPaperSprite>(nullptr, *TexturePath);

	if (!IsValid(LoadedIcon))
	{
		return;
	}

	if (IsValid(m_IconImage))
	{
		//ItemImage->SetBrushResourceObject(LoadedIcon);
		const FSlateAtlasData SpriteAtlasData = LoadedIcon->GetSlateAtlasData();
		const FVector2D SpriteSize = SpriteAtlasData.GetSourceDimensions();

		FSlateBrush Brush;
		Brush.SetResourceObject(LoadedIcon);
		Brush.ImageSize = FVector2D(SpriteSize.X, SpriteSize.Y);
		m_IconImage->SetBrush(Brush);
	}
}
