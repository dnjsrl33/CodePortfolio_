using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.EventSystems;

//  

public class BatterySavePopupUI : MonoBehaviour
{

	#region Field.



	// 팝업 제어 본체.
	public BatterySavePopup popup = null;

	// 기본 오브젝트 정보.
	RectTransform panelRect = null;

	// 절전 모드 관련 값
	float wakeUpValue = 0.0f;
	const float WAKE_UP_SLIDER_MAX = 100.0f;
	const float WAKE_UP_POINT = 80.0f;
	const int ONDEMAND_VALUE = 20;

	//Test 멤버
	Button mainCamRenderOnOffBtn = null;
	Button stopBatterySaveBtn = null;
	Slider onDeMandValueSlider = null;
	Slider bgAlphaSlider = null;
	//////////////////////////////


	Slider wakeUpSlider = null;
	Camera mainCam = null;
	Image bgImg = null;
	bool isOn = true;

	EventTrigger sliderTrigger;

	Animator animator_handle;

	MagicaCloth.MagicaPhysicsManager clothManager;

	Coroutine playerDieWaitCoroutineInst = null;

	#endregion


	#region Process.

	public void SettingInit()
	{
		FindGameObjects();
		InitScripts();
		SetUI();
	}

	IEnumerator PlayerDieEndProcess()
	{
		while (true)
		{
			if (PlayerManager.Inst.Player == null)
			{
				yield break;
			}

			if (PlayerManager.Inst.Player.CharState == Enums.CharState.Die)
			{
				break;
			}

			yield return null;
		}

		//yield return new WaitUntil(() => PlayerManager.Inst.Player.CharState == Enums.CharState.Die);

		Button_Close();
	}

	#endregion


	#region Setting.

	void FindGameObjects()
	{
		// 기본 오브젝트 정보.
		panelRect = transform.Find("Panel").GetComponent<RectTransform>();

		mainCamRenderOnOffBtn = panelRect.Find("Button_MainRender").GetComponent<Button>();
		stopBatterySaveBtn = panelRect.Find("Button_StopBatterySave").GetComponent<Button>();
		onDeMandValueSlider = panelRect.Find("Slider_OnDemandValue").GetComponent<Slider>();
		bgImg = transform.Find("BG").GetComponent<Image>();
		bgAlphaSlider = panelRect.Find("Slider_BgAlpha").GetComponent<Slider>();

		wakeUpSlider = panelRect.Find("Slider_wakeup").GetComponent<Slider>();
		sliderTrigger = wakeUpSlider.gameObject.AddComponent<EventTrigger>();

		animator_handle = wakeUpSlider.transform.Find("Handle Slide Area/Handle").GetComponent<Animator>();
		mainCam = CameraManager.Inst.GetMainCamera();

		clothManager = FindObjectOfType<MagicaCloth.MagicaPhysicsManager>();
		//SoundManager.Inst.
	}
	void InitScripts()
	{
		mainCamRenderOnOffBtn.onClick.AddListener(Button_SetMainRender);
		stopBatterySaveBtn.onClick.AddListener(Button_Close);

		//Test Slider
		onDeMandValueSlider.wholeNumbers = true;
		onDeMandValueSlider.minValue = 1;
		onDeMandValueSlider.maxValue = 10;
		onDeMandValueSlider.value = 10;
		onDeMandValueSlider.onValueChanged.AddListener(Slider_ValueChanged);

		bgAlphaSlider.wholeNumbers = true;
		bgAlphaSlider.maxValue = 125;
		bgAlphaSlider.maxValue = 255;
		bgAlphaSlider.value = 255;
		bgAlphaSlider.onValueChanged.AddListener(Slider_BgAlphaChanged);
		bgImg.color = Color.black;

		wakeUpSlider.minValue = 0;
		wakeUpSlider.maxValue = WAKE_UP_SLIDER_MAX;
		wakeUpSlider.onValueChanged.AddListener(Slider_WakeUpCheckValue);

		// 절전모드 슬라이더에 이벤트 추가
		EventTrigger.Entry pointUpTrigger = new EventTrigger.Entry();
		EventTrigger.Entry pointEnterTrigger = new EventTrigger.Entry();
		pointUpTrigger.eventID = EventTriggerType.PointerUp;

		pointUpTrigger.callback.AddListener((eventData) =>
		{
			if (wakeUpValue > WAKE_UP_POINT)
				Button_Close();
			else
			{
				wakeUpSlider.value = 0.0f;
				animator_handle.enabled = false;
				SetDemandRenderingValue(ONDEMAND_VALUE);
			}
		});


		pointEnterTrigger.eventID = EventTriggerType.PointerDown;
		pointEnterTrigger.callback.AddListener((eventData) =>
		{
			animator_handle.enabled = true;
			SetDemandRenderingValue(1);
		});

		sliderTrigger.triggers.Add(pointEnterTrigger);
		sliderTrigger.triggers.Add(pointUpTrigger);

		animator_handle.enabled = false;

		// 카메라 종료
		isOn = mainCam.enabled = false;


		// 온 디멘드 랜더링 적용.
		SetDemandRenderingValue(ONDEMAND_VALUE);

		// 오브젝트 천 물리처리 연산 중지
		if (clothManager != null)
			clothManager.enabled = false;


		// 플레이어 사망 시, 절전모드 종료.
		playerDieWaitCoroutineInst = StartCoroutine(PlayerDieEndProcess());
	}

	void SetDemandRenderingValue(int interval)
	{
		OnDemandRendering.renderFrameInterval = interval;
	}
	void SetUI()
	{

	}

	#endregion


	#region Ctrl.



	#endregion


	#region Button.


	public void Button_Close()
	{
		popup.SetIsExitBtnCheck(true);

		UIManager.Inst.SoundPlay_DefaultBtnClick();
		SetDemandRenderingValue(1);
		CameraManager.Inst.GetMainCamera().enabled = true;
		popup.Button_Close();

		if (clothManager != null)
			clothManager.enabled = true;

		if (playerDieWaitCoroutineInst != null)
			StopCoroutine(playerDieWaitCoroutineInst);
	}
	void Button_SetMainRender()
	{
		isOn = !isOn;
		mainCam.enabled = isOn;
	}
	#endregion

	#region Slider
	void Slider_ValueChanged(float value)
	{
		SetDemandRenderingValue((int)value);
	}

	void Slider_BgAlphaChanged(float value)
	{
		bgImg.color = new Color(0, 0, 0, value / 255);
	}

	void Slider_WakeUpCheckValue(float value)
	{
		wakeUpValue = value;
	}
	#endregion


}

