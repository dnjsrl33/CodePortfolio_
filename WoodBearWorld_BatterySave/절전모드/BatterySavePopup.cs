using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;

//  

public class BatterySavePopup : DefaultBGPopupBase
{

	#region Feild.

	// 기본 오브젝트 정보.
	BatterySavePopupUI popupUI;

	// 기본 정보.


	// 콜백 관련.
	public System.Action destoryCallBack = null;

	// 정상적으로 종료 했는지 여부 체크.
	bool isExitBtnCheck = false;
	public void SetIsExitBtnCheck(bool _isExitBtnCheck) { isExitBtnCheck = _isExitBtnCheck; }

	#endregion


	#region Process.

	public override void LoadCtrl()
	{
		FindGameObjects();
		InitScripts();

		base.LoadCtrl();
	}

	public override void DestoryCtrl()
	{
		destoryCallBack?.Invoke();
		destoryCallBack = null;

		base.DestoryCtrl();
	}

	public override void ClosePopup()
	{
		if(isExitBtnCheck == false)
		{
			popupUI.Button_Close();
		}

		base.ClosePopup();
	}

	protected override void OnBGClick()
	{
		Button_Close();
	}

	#endregion


	#region Setting.

	void FindGameObjects()
	{
		// 필수 오브젝트 정보.
		popupRootObj = GameObject.Instantiate(ResourceManager.Inst.LoadGameObject(POPUP_RES_PATH, "BatterySavePopup"), UIManager.Inst.PopupTrans) as GameObject;
		popupPanelRect = popupRootObj.transform.Find("Panel").GetComponent<RectTransform>();
		//popupBGBtn = popupRootObj.transform.Find("BG").GetComponent<Button>();

		// 기본 오브젝트 정보.
		popupUI = popupRootObj.AddComponent<BatterySavePopupUI>();
	}

	void InitScripts()
	{
		// 필수 오브젝트 정보.
		//popupBGBtn.onClick.AddListener(Button_Close);

		// 기본 오브젝트 정보.
		popupUI.popup = this;
		popupUI.SettingInit();

		SetBackKeyLock(true, null);
	}

	#endregion


	#region Button.

	public void Button_Close()
	{
		ClosePopup();
	}

	#endregion

}

