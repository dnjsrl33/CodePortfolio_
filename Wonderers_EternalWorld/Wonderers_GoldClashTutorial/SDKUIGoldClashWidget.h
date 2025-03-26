// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/SDKUserWidget.h"
#include "UI/SDKInGameMainUIInterface.h"
#include "Share/SDKEnum.h"
#include "SDKUIGoldClashWidget.generated.h"

class USDKUserHotKeyWidget;
class UImage;
class UBorder;
class UButton;
class UOverlay;
class USizeBox;
class UScaleBox;
class UGridSlot;
class UGridPanel;
class UTextBlock;
class UCanvasPanel;
class UProgressBar;
class UHorizontalBox;
class UWidgetAnimation;
class UVerticalBox;

class USDKMiniMapWidget;
class USDKStaminaWidget;
class USDKDeathLogWidget;
class USDKLevelUpQuickWidget;
class USDKGoldClashScoreWidget;
class USDKGoldRankListWidget;
class USDKLevelUpBuffWidget;
class USDKRightButtonsWidget;
class USDKIngameUpdateNotifyWidget;
class USDKGoldClashShopArtifactBoxWidget;
class USDKButtonParam;
class USDKScreenEffectWidget;

UENUM(BlueprintType)
enum class ETutorialStepSingle : uint8
{
	None,
	TutorialBegin,
	LevelUpBuffGuideBegin,
	LevelUpBuffGuideEnd,
	GoldBoxGuideBegin,
	GoldShopGuideBegin,
	GoldShopGuideEnd,
	GoldShopSetArtifactGuideBegin,
	GoldShopSetArtifactGuideEnd,
	TutorialEnd,
};

UCLASS()
class ARENA_API USDKUIGoldClashWidget : public USDKUserWidget, public ISDKInGameMainUIInterface
{
	GENERATED_BODY()

public:
	// Begin UserWidget Interface
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// End UserWidget Interface

	// Begin SDKUserWidget Interface
	virtual void CreateUIProcess() override;
	virtual void OpenUIProcess() override;
	virtual void CloseUIProcess() override;
	virtual void TutorialUIProcess() override;
	virtual void Editor_SetPlatformTypeProcess(EPlatformType type) override;
	// End SDKUserWidget Interface

	// Begin SDKInGameMainUI Interace
	UFUNCTION(BlueprintCallable)
	virtual UGridPanel* GetBuffGridPanel(bool bIsBuff) const override;

	virtual USDKRightButtonsWidget* GetRightButtonsMainWidget() const override { return SDKRightButtonBase; }
	virtual USDKStaminaWidget* GetStaminaWidget() const override { return nullptr; }

	virtual void InitHudReferenceSetting() override;

	virtual void SetActiveInteractionButton(ERelationType Type = ERelationType::None) override;
	virtual void ActiveNotifyCollectItem(FString ItemID) override;

	virtual void ChangeEquipWeapon(int32 Index, int32 ItemID, int32 SkillID, bool bActiveItem) override;
	virtual void ChangedUnEquipWeapon(int32 ItemID, int32 SkillID, bool bActiveItem) override;

	virtual void SetDisableAttackButtons(bool bDisable) override;
	virtual void SetEnableDodgeButton(bool bEnable) override;

	virtual void UpdateGold(const int32 Gold) override;

	/*--- 스킬 -----------------------------------------------------------------------------------------*/
	virtual int32 GetTouchIndexSkillButton() const override;

	virtual void UpdateSkillCoolTime(int32 index, float RemainTime) override;
	virtual void TouchStartedSkillButton(int32 TouchpadIdx, FVector2D vStart, bool bAttackButton = false, int32 SkillIndex = -1) override;
	virtual void SetVisibilitySkillCancel(bool bVisible) override;
	UFUNCTION(BlueprintCallable)
	virtual void TouchSkillCancelButton(bool bCancel, const FGeometry InGeometry, const FPointerEvent InGestureEvent) override;

	virtual void SetVisibilityScreenEffect(EScreenEffectType InScreenEffectType) override;

	/*--- 레벨업 ---------------------------------------------------------------------------------------*/
	virtual bool GetVisibleyLevelUpCard() const override;
	virtual USDKLevelUpBuffWidget* GetLevelUpBuffListWidget() override;

	/** 퀵 레벨업 */
	virtual void ToggleQuickLevelUpWidget(bool bVisible) override;
	virtual void UpdateQuickLevelUpBuffCard(struct FLevelUpBuffData InBuffData) override;
	virtual void UpdateQuickLevelUpExp(float InPercent) override;
	virtual void UpdateQuickLevelUpRemainCount(int32 InCount) override;
	virtual void UseQuickLevelUpBuffCard(int32 InSlotIdx) override;
	virtual void PlayQuickLevelUpCloseAnim() override;
	/** 레벨업 */
	virtual void ToggleLevelUpRewardWidget(bool bVisible) override;
	virtual void UpdateHaveBuffList(int32 InTableID) override;
	virtual void ActiveLevelUpBuffCard(int32 Count) override;
	virtual void UpdateLevelUpBuffHaveList(const TArray<struct FBuffData>& BuffList) override;
	virtual void AddSelectedLevelupBuff(int32 BuffID) override;
	virtual void ClearLevelupBuffList() override;

	virtual void UpdateLevelUpBuffCardList(int32 First, int32 Second, int32 Third) override;
	virtual void UpdateLevelUpBuffCount(int32 InCount) override;
	virtual void CloseLevelUpBufCardfList() override;

	/*--- 버프 -----------------------------------------------------------------------------------------*/
	UFUNCTION(BlueprintCallable)
	virtual void AddBuff(int32 BuffID, USDKUserWidget* BuffWidget) override;
	UFUNCTION(BlueprintCallable)
	virtual void RemoveBuff(int32 BuffID, USDKUserWidget* BuffWidget) override;

	virtual void SortBuffIconList(UGridPanel* BuffGrid, TArray<UGridSlot*> BuffList) override;
	virtual void UpdateBuffRenderOpacity(EBuffType BuffType) override;

	/*--- 유물 -----------------------------------------------------------------------------------------*/
	virtual void UpdateArtifactList(const TArray<struct FItemSlotData>& UpdateInventory) override;
	virtual void UpdateSortAritfactWidgetList(const TArray<int32>& ArtifactIDs) override;

	/*--- Init ----------------------------------------------------------------------------------------*/
protected:
	/** 초기화 : 스트링 */
	virtual void InitTextBlockString() override;
	/** 초기화 : 팀 방향 이미지 */
	virtual void InitTeamMemberImages() override;
	/** 초기화 : 무기 및 스킬 */
	virtual void InitEquipData() override;
	/** 초기화 : 공통 버튼 바인딩 */
	virtual void InitBindButton() override;
	// End SDKInGameMainUI Interace

public:
	USDKMiniMapWidget* GetMinimapWidget() const { return SDKMinimap; }

	/** 시작 애니 재생 : 골클 목표 강조 애니 */
	UFUNCTION(BlueprintCallable)
	void StartPlayGoldClashGoalLoopAnim();

	/*--- 팀 멤버 --------------------------------------------------------------------------------------*/
	/** 설정 : 멤버 카운트 */
	void SetMemberCount(int32 count);

	/** 설정 : 매치팀 타입 */
	void SetIsMatchTypeTeam(bool bIsTeamType);

	/** 표시 설정 : 멤버 이미지 */
	void SetVisibilityMemberImages(bool bVisible);

	/** 파티 멤버 정보 설정 */
	void SetTeamMemberData(const TArray<struct FPartyMemberData>& Data);

	/** 업데이트 : 팀멤버 위치 */
	void UpdateTeamMemberPosition(const TMap<int32, FVector2D>& MemberPosition, const TMap<int32, float>& MemberRotation);

	/*--- 관전자 ---------------------------------------------------------------------------------------*/
	/** 관전자 상태 */
	UFUNCTION(BlueprintPure)
	bool IsOnlyASpectator();

	/** 설정 : 관전 UI 활성 여부 */
	void SetActiveSpectatorUI(bool bActive, int32 RespawnTime = 0);

	/** 설정 : 리스폰 타이머 */
	void SetRespawnTimer(int32 RespawnTime = 0);
	USDKGoldClashScoreWidget* GetGoldClashScoreWidget() { return SDKGoldClashScore; }

	/** 업데이트 : 부활 시간 */
	UFUNCTION()
	void UpdateRespwnTimer();

	/*--- 데스 로그 ------------------------------------------------------------------------------------*/
	/** 로드 : 데스 로그 */
	void LoadDeathWidget(const FDeathData& DeathData);

	/** 설정 : 데스 로그 타이머 */
	void SetDeathInfoTimerVisible(float fTime1, float fTime2);

	/*---- 골드 클래시 ---------------------------------------------------------------------------------*/
	/** 설정 : 핑과 FPS 설정값 적용 */
	void UpdateConfigPing();
	void UpdateConfigFPS();
	
	/** 설정 : 핑 */
	void SetTextGamePing(uint8 Ping);

	/** 설정 : FPS */
	void SetTextGameFPS();

	/** 설정 : 핑 Visibility */
	void SetPingVisibility(const bool InVisible);
	
	/** 설정 : FPS Visibility */
	void SetFPSVisibility(const bool InVisible);
	 
	/** 설정 : 접속 플레이어 카운트 */
	UFUNCTION(BlueprintCallable)
	void SetTextPlayerCount(int32 current, int32 max);

	/** 업데이트 : 골드크래시 1등 위치 */
	void UpdateGoldRankerPosition(bool bIsMe, FVector2D vPosition, float fRotator);

	/** 업데이트 : 골드 크러쉬 랭크 */
	void UpdateGoldClashRank(const TArray<struct FGoldClashResultInfo>& RankingList, const uint8 MyTeamID);

	/** 카운트 타이머 애니 */
	void StartEndSoonTimeAni();

	/** 업데이트 : 카운트 다운 시간*/
	UFUNCTION(BlueprintCallable)
	void UpdateCountDownTimer(int32 NewTime, bool bCountDown = false);

	/** 업데이트 : 골드 박스 리스폰 남은시간*/
	UFUNCTION(BlueprintCallable)
	void UpdateGoldBoxRespawnTimer(int32 NewTime);

	/** 업데이트 : 지형 붕괴 남은시간*/
	UFUNCTION(BlueprintCallable)
	void UpdateRegionDisappearTimer(int32 NewTime);

	/** 미니맵 속성 변경*/
	UFUNCTION(BlueprintCallable)
	void UpdateMapElementalIcon(int32 PositionNum, EElementalType Elemental);

	UFUNCTION(BlueprintCallable)
	void UpdateMapGoldChestIcon(int32 PositionNum, bool bActive);

	/** 골드 상자 표시 */
	void SetVisibilityGoldBoxSign(bool bVisible);

	/** 골드 상자 리스폰 타이머 표시 여부 */
	void ActiveGoldBoxRespawnTimer(bool bActive);

	/** 지역 낙하 타이머 표시 여부 */
	void ActiveRegionDisappearTimer(bool bActive);

	/** 골드 리젠 지역 표시 */
	void UpdateGoldRegenRegion(TArray<int32> RegionList);

	/** 1등 팀 표시 */
	void UpdateNo1TeamSign(bool bMyTeam);

	/** 유물 추천 */
	void UpdateArtifactRecommend(const TArray<FString>& ShopRelicIDs);
	void SetVisibleArtifactRecommend(bool bVisible);
	void UdpateArtifactRecommendGold(const int32 gold);

	/** 설정 : 항복 버튼 표시 여부 */
	void SetVisibilityButtonSurrender(bool bVisible);
	/** 항복 버튼 사용 가능 여부 */
	bool IsEnableButtonSurrender();

	/** 매크로 채팅 표시 */
	void ShowMacroChatMessage(const int32 HeroID, const ETeamColor Color, const FString& NotifyMessage);

	/*--- 바인드 이벤트 --------------------------------------------------------------------------------*/
	UFUNCTION()
	void OnClickedButtonOption();

	UFUNCTION()
	void OnClickedButtonStatus();

	UFUNCTION()
	void OnClickedLevelupCardDetail();

	UFUNCTION()
	void OnClickedButtonSticker();
	
	UFUNCTION()
	void OnClickedButtonMacro();

	UFUNCTION()
	void OnClickedButtonGoldClashShop();

	UFUNCTION()
	void OnClickedButtonSurrender();

	/*--- 블루 프린트 함수 -----------------------------------------------------------------------------*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnInitHudReferenceSetting();

	/** 활성화 설정 : 골드 스매시 튜토리얼 모드 */
	void SetActiveTutorialGoldClashMode(bool bActive);

	/*-- 골드 클래시 튜토리얼 -------------------------------------------------------------------------------*/

	/* 미니맵 활성화 여부 */
	UFUNCTION(BlueprintCallable)
	void SetVisibilityStatus(bool bVisible);

	/* 미니맵 활성화 여부 */
	UFUNCTION(BlueprintCallable)
	void SetVisibilityMap(bool bVisible);

	/* 이모티콘 버튼 활성화 여부 */
	UFUNCTION(BlueprintCallable)
	void SetVisibilityEmoticon(bool bVisible);

	/* 매크로 버튼 활성화 여부 */
	UFUNCTION(BlueprintCallable)
	void SetVisibilityMacro(bool bVisible);
	
	/* 상점 버튼 활성화 여부 */
	UFUNCTION(BlueprintCallable)
	void SetVisibilityShop(bool bVisible);

	/* 현재 골드 현황 위젯 활성화 여부 */
	UFUNCTION(BlueprintCallable)
	void SetVisibilityRankBoard(bool bVisible);

	/* 튜토리얼 : 회피 버튼 애니메이션 실행 */
	UFUNCTION(BlueprintCallable)
	void PlayAnimTutorialGuideDodge(bool bActivate);
	/* 튜토리얼 : 점프 버튼 애니메이션 실행 */
	UFUNCTION(BlueprintCallable)
	void PlayAnimTutorialGuideJump(bool bActivate);
	/* 튜토리얼 : 레벨업 버프 바로가기 버튼 애니메이션 실행 */
	UFUNCTION(BlueprintCallable)
	void PlayAnimTutorialGuideLevelUpQuick(bool bActivate);
	/* 튜토리얼 : 레벨업 버프 비저빌리티 설정 */
	UFUNCTION(BlueprintCallable)
	void SetTutorialVisibilityLevelUpBuffBox(bool bVisible);
	/* 튜토리얼 : 레벨업 버프 튜토리얼 준비 */
	UFUNCTION(BlueprintCallable)
	void SetTutorialGuideLevelUpBuff(int32 GuideID);
	/* 튜토리얼 : 상점 튜토리얼 준비 */
	UFUNCTION(BlueprintCallable)
	void SetTutorialGuideGoldShop(int32 GuideID);
	/* 튜토리얼 : 레벨업 버프 튜토리얼 다음 스탭 진행 */
	UFUNCTION(BlueprintCallable)
	void ActiveTutorialGuideLevelUpBuff();

	/* 튜토리얼 : 상점 튜토리얼 종료*/
	UFUNCTION(BlueprintCallable)
	void ActiveTutorialGuideGoldShop();

	UFUNCTION(BlueprintCallable)
	void SetTutorialGuideOpenShopAndBuyArtifact(int32 GuideID);

	UFUNCTION()
	void OnClickedSkipLevelUpBuffTutorial();

	UFUNCTION()
	void OnClickedTutorialBuyRecommnadedArtifact();

	UFUNCTION()
	void OnClickedButtonTutorialOpenShop(bool IsOpen);

	UFUNCTION()
	void OnTimerBroadcastOpenShop();

	UFUNCTION(BlueprintCallable)
	void OnEndGoldShopSetArtifactTutorial();
	UFUNCTION(BlueprintCallable)
	ETutorialStepSingle GetTutorialStepSingle() const { return TutorialStepSingle; }

	UFUNCTION(BlueprintCallable)
	void SetTutorialStepSingle(ETutorialStepSingle Step) { TutorialStepSingle = Step; }

	/* 튜토리얼 : 상점 튜토리얼 종료*/
	UFUNCTION(BlueprintCallable)
	void SetButtonVisibilityTutorial(bool bVisible);
	
protected:
	/** 초기화 : 보유 골드 */
	void InitGoldData();
	void InitSeasonArtifactData();

protected:
	/*--- 공용 -----------------------------------------------*/ 
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasHidden_OK;

	// 플랫폼에 따라 다르게 설정
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USDKRightButtonsWidget* SDKRightButtonBase;

	// 아이템 획득 알림
	UPROPERTY(meta = (BindWidget))
	USDKIngameUpdateNotifyWidget* SDKUpdateNotify;

	// 레벨업 버튼
	UPROPERTY(meta = (BindWidget))
	UScaleBox* ScaleBoxLevelUpQuick;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USDKLevelUpQuickWidget* SDKLevelUpQuick;
	UPROPERTY()
	UButton* ButtonLevelUp;
	UPROPERTY()
	UBorder* BorderPCGuid;

	// 레벨업 카드
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USDKLevelUpBuffWidget* SDKLevelUpCard;

	// 상태 버프
	UPROPERTY(EditDefaultsOnly, Category = UI)
	int32 MaxBuffIconColumn = 5;
	UPROPERTY(meta = (BindWidget))
	UGridPanel* GridPanelBuff;
	UPROPERTY(meta = (BindWidget))
	UGridPanel* GridPanelDebuff;

	// 보유 유물
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* HorizontalBoxArtifact;
	UPROPERTY()
	TArray<USDKUserWidget*> ArtifactIconList;

	// 버튼 : 상태창
	UPROPERTY(meta = (BindWidget))
	UOverlay* OverlayStatus;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonStatus;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextStatus;

	// 버튼 : 옵션(ESC)
	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBoxOption;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonOption;

	/*--------------------------------------------------------*/
	// 참여 파티 카운트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextPlayerCount;

	// 미니맵
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USDKMiniMapWidget* SDKMinimap;

	// 데스 메세지
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USDKDeathLogWidget* SDKDeathLog;

	// 스티커 버튼
	UPROPERTY(meta = (BindWidget))
	USizeBox* Emoticon;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonSticker;
	
	// 매크로 버튼
	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBoxMacro;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonMacro;

	// 매크로 채팅 리스트
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VerticalBoxMacroSlot;

	// 상점
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USDKGoldClashShopArtifactBoxWidget* RecommendArtifactSlot;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonShop;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextShop;

	// 관전
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UOverlay* OverlaySpectator;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorizontalBoxRespawn;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorizontalBoxViewButtons;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextSpectatorNoti;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextRespawnTitle;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextResapwnTime;

	// 점수 및 랭킹
	UPROPERTY(meta = (BindWidget))
	UOverlay* OverlayGoldClashScore;
	UPROPERTY(meta = (BindWidget))
	USDKGoldClashScoreWidget* SDKGoldClashScore;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USDKGoldRankListWidget* SDKGoldClashRank;

	// 마지막 카운트 
	UPROPERTY(meta = (BindWidget))
	UOverlay* OverlayLastTimer;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBarTimeGauge;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBarTimeGaugeEffect;
	UPROPERTY(meta = (BindWidget))
	USDKUserWidget* BPLastCountDown;
	UPROPERTY()
	UTextBlock* TextLastCountDown;
	UPROPERTY()
	UWidgetAnimation* CountDownAnim;
	
	// 튜토리얼용 강조 애니메이션
	UPROPERTY()
	UWidgetAnimation* TutorialLevelUpBuffPointAnim;

	// 핑
	UPROPERTY(meta = (BindWidget))
	USDKUserWidget* BPOutInfo;
	UPROPERTY()
	UTextBlock* TextPing;
	UPROPERTY()
	UTextBlock* TextFPS;

	UPROPERTY(meta = (BindWidget))
	USDKUserHotKeyWidget* HotKeyOpenShop;
	
	UPROPERTY(meta = (BindWidget))
	USDKUserHotKeyWidget* HotKeyOption;
	
	UPROPERTY(meta = (BindWidget))
	USDKUserHotKeyWidget* HotKeyState;

	UPROPERTY(meta = (BindWidget))
	USDKUserHotKeyWidget* HotKeyStateExpression;
	
	UPROPERTY(meta = (BindWidget))
	USDKUserHotKeyWidget* HotKeyMacro;
	
	// 항복
	UPROPERTY(meta = (BindWidget))
	USDKButtonParam* ButtonSurrender;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextSurrender;

	UPROPERTY(meta = (BindWidget))
	USDKScreenEffectWidget* ScreenEffectHide;

private:
	// 버프 목록
	UPROPERTY()
	TMap<int32, UGridSlot*> mapBuffList;
	UPROPERTY()
	TMap<int32, UGridSlot*> mapDebuffList;

	// 가이드 텍스트 목록
	UPROPERTY()
	TArray<UTextBlock*> TextGuidList;

	// 팀 방향 목록
	UPROPERTY()
	TArray<UImage*> MemberDirIcons;

	// 팀 스폰 넘버 목록
	UPROPERTY()
	TArray<int32> MemberSpawnNum;

	// 리스폰 시간
	int32 RemainRespawnTime;
	FTimerHandle RespawnTimerHandle;

	// DeathInfo
	FTimerHandle DeathInfoVisibleTimerHandle;
	FTimerHandle DeathInfoCollapsedTimerHandle;

	// 게임 종료 카운트 다운
	float MaxCountDown;
	float CountDeltaTime;
	float CountDownTick;
	ETutorialStepSingle TutorialStepSingle;
	FTimerHandle TutorialTimerHandle;

	TArray<FString> SeasonSet;
	TArray<FString> ModeSet;
};
