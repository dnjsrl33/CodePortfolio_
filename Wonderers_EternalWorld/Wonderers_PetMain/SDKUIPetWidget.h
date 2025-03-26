// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SDKUserWidget.h"
#include "Share/SDKEnum.h"
#include "Share/SDKStructUI.h"
#include "SDKUIPetWidget.generated.h"

class UBorder;
class UGridPanel;
class UScrollBox;
class UVerticalBox;
class UTextBlock;
class URichTextBlock;
class ULevelStreamingDynamic;
class UWidgetSwitcher;
class CPetData;

class ASDKPet;
class USkeletalMeshComponent;

class USDKPetFeedWidget;
class USDKPetIconWidget;
class USDKComboBoxWidget;
class USDKCharacterStatusWidget;
class USDKPetInfoWidget;
class USDKButtonWidget;
class USDKPetCombineListWidget;
class USDKPetEggIncubatorWidget;
class USDKButtonParam;

UCLASS()
class ARENA_API USDKUIPetWidget : public USDKUserWidget
{
	GENERATED_BODY()

public:
	USDKUIPetWidget();

	// Begin SDKUserWidget Interface
	virtual void OpenUIProcess() override;
	virtual void CloseUIProcess() override;
	virtual void CreateUIProcess() override;
	virtual void SetID(const FString& ID) override;
	// End SDKUserWidget Interface

	UFUNCTION(BlueprintImplementableEvent)
	void CombineEffectStart(bool bMutant, FVector SpawnPos);

	UFUNCTION(BlueprintImplementableEvent)
	void CombineEffectEnd(bool bMutant, FVector SpawnPos);

	void UpdatePetList();
	void UpdatePetEggIncubator(FString UpdateIncubatorID);

	UFUNCTION(BlueprintCallable)
	void OnTouchRotationPet(const FVector2D InPosition);

	UFUNCTION()
	void OnClickedButtonSelectPet(USDKWidgetParam* param);

	UFUNCTION()
	void OnSelectSortOrder(ESortOrderCategory SelectOrderCategory);

	UFUNCTION()
	void OnClickedButtonCombineMenu(USDKWidgetParam* param);

	UFUNCTION()
	void OnClickedButtonPetInfoMenu();

	UFUNCTION()
	void OnClickedButtonCombine();

	UFUNCTION()
	void OnClickedButtonEquip(USDKWidgetParam* param);

	UFUNCTION()
	void OnClickedButtonIncubator();

	UFUNCTION()
	void OnClickedButtonTopBarBackFromCombine();

	UFUNCTION()
	void OnClickedButtonTopBarBackFromIncubator();

	UFUNCTION()
	TMap<int64, int32>& GetMapPetIndex() { return mapPetIndex; }
	UFUNCTION()
	TArray<USDKPetIconWidget*>& GetPetIconList() { return PetIconList; }


	UFUNCTION()
	void OnClickedGachaLinkButton();

	UFUNCTION()
	void OnClickedSetAutoCombine();

	void SetSwitchMenu(int32 index);
	void SetSelectPet(int32 TableID, int64 UniqueID);

	void SpawnSelectPet(int32 TableID);
	void SetVisibilitySelectPet(bool bVisible);

	UFUNCTION()
	void OnCompletedLoadPetLevel();

	void ClearMainPet();

	void LookCamRotateActor(AActor* RootActor);
	
	void UpdateInfoLock();

	void SetPetExpectAddedFeed(const TMap<FString, int32>& MapToy);

	void SetPetIconSelection(int64 SelectPetUniqueID);
	void SetPetIconSelectionFromCombine(int64 SelectPetUniqueID, bool bVisible);
	void SetModifyTopBarFromCombineMenu(bool bModify);
	void SetModifyTopBarFromIncubatorMenu(bool bModify);
	void SetVisibilityRotate(bool bVisible);
	void SetToggleViewTargetPet(bool bViewPet);
	void SetToggleViewTargetPetIncubator(bool bViewPet);
protected:
	void InitBPWidget();
	void CreatePetList();
	void CreatePetBoxWidget(const CPetData& Data);

private:
	void GetAllPetTableIDs(TArray<int64>& OutIDs);
	void SortPetList(TArray<int64>& Array, ESortOrderCategory SrotType);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstance* GlitchMatNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstance* GlitchMatMutant;

	FDelegateHandle CloseUIHandle;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxColumnCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxListCount;

	/* 합성 및 옵션 조합 시 카메라 이동 속도입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CamLerpSpeed;

	/* 합성 및 옵션 조합 시 펫 이동속도입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PetLerpSpeed;

	/* 펫 회전 시, 속도 입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PetRotateSpeed;

	/* 펫스폰 좌표에 더할 Z 조절 값입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MainPetZPos;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* SwitcherPetExist;

	// BP Pet List
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USDKUserWidget* SDKPetList;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextTotalCount;

	UPROPERTY()
	UScrollBox* ScrollBoxList;

	UPROPERTY()
	UGridPanel* GridPanelList;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* PetMenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	USDKPetInfoWidget* Pet_InfoMain;

	UPROPERTY(meta = (BindWidget))
	USDKPetCombineListWidget* PetCombineList;

	UPROPERTY(meta = (BindWidget))
	USDKPetEggIncubatorWidget* PetEggIncubatorWidget;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoHavePetText;

	UPROPERTY(meta = (BindWidget))
	USDKButtonWidget* GachaShopLinkButtonWidget;

	UPROPERTY(meta = (BindWidget))
	USDKButtonParam* ButtonIncubator;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextIncubatorTitle;


	UPROPERTY(meta = (BindWidget))
	UBorder* BorderRotate;

	UPROPERTY()
	TArray<FSkillDescript> SkillInfotList;

	UPROPERTY()
	TWeakObjectPtr<AActor> OldViewTarget;

	UPROPERTY()
	TWeakObjectPtr<ASDKPet> SDKPet;

	UPROPERTY()
	TWeakObjectPtr<USkeletalMeshComponent> SDKPetMesh;

	UPROPERTY()
	FTransform PetSpawnTransform;

	UPROPERTY()
	AActor* PetViewCamera;

	UPROPERTY()
	AActor* PetIncubatorCamera;
private:
	UPROPERTY()
	TMap<int64, int32> mapPetIndex;

	UPROPERTY()
	TArray<USDKPetIconWidget*> PetIconList;

	UPROPERTY()
	TMap<int64, int64> MapCombinePetList;

	UPROPERTY()
	TArray<FPetCombineData> PetCombineDataList;

	UPROPERTY()
	FVector DefaultCamPos;

	UPROPERTY()
	FVector CombineCamPos;

	UPROPERTY()
	FVector DestCamPos;

	UPROPERTY()
	FVector DestPetPos;

	UPROPERTY()
	FVector2D TouchPosition;

	UPROPERTY()
	UWidgetAnimation* WidgetOpen;

	UPROPERTY()
	UWidgetAnimation* InfoWidgetOpen;

	UPROPERTY()
	UWidgetAnimation* CombineWidgetOpen;

	bool bCamMove = false;
	bool PetActorMove = false;
	int32 WidgetIndex;
	int32 SelectTableID;
	int64 SelectUniqueID;

	int32 SelectSubTableID;
	int64 SelectSubUniqueID;
	ESortOrderCategory CurrentSortType;
	ESortOrderCategory PrevSortType;

	bool bPetCombineMode;

	FDelegateHandle CombineInputCloseHandle;
};
