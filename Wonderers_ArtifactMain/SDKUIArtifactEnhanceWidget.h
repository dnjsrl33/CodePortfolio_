// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SDKUIMainWidget.h"
#include "SDKUIArtifactEnhanceWidget.generated.h"

class UGridPanel;
class UScrollBox;
class UTextBlock;
class UWidgetSwitcher;

class USDKTileView;
class USDKPopupArtifactInfoWidget;
class USDKArtifactEnhanceSetListWidget;
class USDKArtifactEnhanceIconWidget;

class USDKArtifactGradeInfoWidget;
class USDKArtifactDeckListWidget;
class USDKArtifactDeckSettingWidget;
class USDKArtifactFiliterWidget;

UCLASS()
class ARENA_API USDKUIArtifactEnhanceWidget : public USDKUserWidget
{
	GENERATED_BODY()

public:
	USDKUIArtifactEnhanceWidget();

	virtual void SetID(const FString& ID);
	virtual void CreateUIProcess();
	virtual void OpenUIProcess();
	virtual void CloseUIProcess();

	void SetBackUI(EUI InBackUI) { BackUI = InBackUI; }
	EUI GetBackUI() const { return BackUI; }
	
	void InitWidget();
	bool GetDeckSetting() const { return bStateDeckSetting; }
	void SetDeckSetting(bool bDeckSetting) { bStateDeckSetting = bDeckSetting; }

	/* 유물, 유물 세트 아이콘 생성*/
	void CreateArtifactList();

	/* 유물 정렬*/
	void SortArtifactSetList();

	/* 유물 아이콘 클릭 시*/
	void SetCurrentSelectedArtifact(USDKWidgetParamArtifactEnhance* Param);

	/* 유물 변경사항 적용 함수 */
	UFUNCTION()
	void LoadArtifactData();

	/* 유물 강화 */
	void UpgradeArtifact(FArtifactData ArtifactData);
	void ClearListItemOverlay(USDKWidgetParamArtifactEnhance* Param);
	void ClearAllListItemOverlay();

	void UpdateArtifactDeck();

	void ActivateDeckSettingState(int32 SelectedDeckIndex);
	void DeactiveDeckSettingState();

	/* 덱 이름 변경 관련 EditBox Event */
	UFUNCTION()
	void OnTextCommittedDeckName(const FText& Text, ETextCommit::Type CommitMethod);

	/* 필터 클릭 시*/
	UFUNCTION()
	void OnClickedFiliterVisibility();

	/* 세트 아이콘 클릭 시 */
	UFUNCTION()
	void SetSelectSetList(USDKArtifactEnhanceSetListWidget* SetList);

	UFUNCTION()
	void OnClickedDeckSettingClear();

	/* 유물 등급 위젯에서 덱 관리 버튼 클릭 시 */
	UFUNCTION()
	void OnClickedDeckManageButton();

	/* 덱 리스트 위젯에서 돌아가기 버튼 클릭 시 */
	UFUNCTION()
	void OnClickedBackGradeInfo();

	/* 덱 설정 위젯에서 돌아가기 버튼 클릭 시 */
	UFUNCTION()
	void OnClickedBackGradeInfoInDeckSetting();

	/* 덱 수정 중 위젯 종료 시 안내 팝업 */
	UFUNCTION()
	void OnClosePopupDeckModifyAlert();

	/* 덱 수정 중 위젯 종료 시 안내 팝업 : Yes */
	UFUNCTION()
	void OnClickedReturnYesButtonDeckModify();

	/* 덱 수정 중 위젯 종료 시 안내 팝업 : No */
	UFUNCTION()
	void OnClickedReturnNoButtonDeckModify();

	/* 덱 수정 중 종료 상태 변경*/
	void SetDeckModifyTopBar(bool bModity);

	/* 덱 슬롯 추가 시 덱 변경 상태로 전환*/
	void UpdateAddedArtifactDeck();

	void ClearSelectSetList();

	FArtifactData FindArtifactData(int32 InArtifactID);
	void EquipDeckArtifact();
	void RemoveDeckArtifact(FArtifactData ArtifactData);
	void CancelDeckSetting();
	void ApplyArtifactFilter(TArray<int32> EnhanceFilter, TArray<EItemGrade> GradeFilter, bool bSeasonArtifact, bool bUnknown, bool bEnhanceAvaliable);

public:
	FArtifactData GetCurrentArtifactData() { return CurrentArtifactParam->GetArtifactData(); }

private:
	void CloseWidget();

	void OnCloseCheckBackUI();

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextSetTitle;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ArtifactSetListBox;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher_GradeOrDeck;

	UPROPERTY(meta = (BindWidget))
	USDKArtifactGradeInfoWidget* ArtifactGradeInfo;

	UPROPERTY(meta = (BindWidget))
	USDKArtifactDeckListWidget* ArtifactDeckList;

	UPROPERTY(meta = (BindWidget))
	USDKArtifactDeckSettingWidget* ArtifactDeckSetting;

	UPROPERTY(meta = (BindWidget))
	USDKButtonParam* ButtonFilter;

	UPROPERTY(meta = (BindWidget))
	USDKArtifactFiliterWidget* ArtifactFilter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextFilterTitle;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextSeasonInfoTitle;

private:
	UPROPERTY()
	USDKWidgetParamArtifactEnhance* CurrentArtifactParam;

	/* 유물 위젯에서 보이는 리스트, 유물ID, 만들어진 유물 아이콘*/
	UPROPERTY()
	TMap<int32, USDKWidgetParamArtifactEnhance*> ArtifactIconList;

	/* 유물 세트 위젯에서 보이는 리스트, 세트아이디, 만들어진 세트아이콘*/
	UPROPERTY()
	TMap<int32, USDKArtifactEnhanceSetListWidget*> ArtifactSetIconList;

	/* 필터 : 강화등급, 유물 리스트*/
	UPROPERTY()
	TMap<int32, USDKArtifactEnhanceSetListWidget*> FilterEnhanceIconList;

	/* 필터 : 유물 등급, 유물 리스트*/
	UPROPERTY()
	TMap<EItemGrade, USDKArtifactEnhanceSetListWidget*> FilterGradeIconList;

	UPROPERTY()
	USDKArtifactEnhanceSetListWidget* SelectSetList;

	//현재 해금되어있는 유물 리스트
	TArray<FArtifactData> ArtifactList;

	/* 세트 ID, 세트 대응 유물 리스트*/
	TMap<int32, TArray<FArtifactData>> ArtifactSetList;

	// <유물등급, (현재수량,전체수량)>
	TMap<EItemGrade, FIntPoint> MapArtifactGradeData;

	FArtifactDeck CurrentArtifactDeck;

	bool bUpgrade;

	bool bStateDeckSetting;

	EUI BackUI;
};
