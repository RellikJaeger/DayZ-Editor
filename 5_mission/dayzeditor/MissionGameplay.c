// #include "Scripts/GUI/IngameHud.c"


// creates player in world
//GetGame().SelectPlayer(null, CreateCustomDefaultCharacter());

// heading model? mouse control
//bool			HeadingModel(float pDt, SDayZPlayerHeadingModel pModel);

static PlayerBase CreateDefaultCharacter()
{
    PlayerBase oPlayer = PlayerBase.Cast( GetGame().CreatePlayer( NULL, GetGame().CreateRandomPlayer(), "7500 0 7500", 0, "NONE") );
    oPlayer.GetInventory().CreateInInventory("AviatorGlasses");
    return oPlayer;
}

Mission EditorCreateCustomMission(string path)
{
	Print("DayZEditorGameplay::CreateCustomMission " + path);
	
	return new EditorMissionGameplay();
}

class EditorMissionServer: MissionServer
{

}

static ref EditorUI	m_EditorUI;	
static ref Editor m_Editor;
class EditorMissionGameplay: MissionGameplay
{
	EditorUI GetEditorUI() { return m_EditorUI; }
	bool ui_state = true;
    override void OnKeyPress(int key)
    {
		if (key == KeyCode.KC_F1) {
			delete m_Editor; delete m_EditorUI;
			m_Editor = new Editor();
			m_EditorUI = new EditorUI();
			
		}
		switch (key) {
			case KeyCode.KC_SPACE:
				if (!ui_state) {
					m_EditorUI.Show();
				} else
					m_EditorUI.Hide();
				ui_state = !ui_state;
				//m_EditorUI.Show(ui_state);
				
				break;
			
			case KeyCode.KC_ESCAPE:
				if (ui_state) {
					ui_state = false;	
					
				} else {
					// Pause menu	
				}
				break;
		}
		
		m_Editor.OnKeyPress(key);
    	super.OnKeyPress(key);        
        m_Hud.KeyPress(key);
    }

    override void OnInit()
	{
		super.OnInit();
		m_Editor = new Editor(); 
		m_EditorUI = new EditorUI();	
		
		GetGame().GetUIManager().ShowScriptedMenu(m_EditorUI, GetGame().GetUIManager().GetMenu());
		GetGame().GetUIManager().HideScriptedMenu(m_EditorUI);
		GetGame().GetUIManager().ShowScriptedMenu(m_EditorUI, GetGame().GetUIManager().GetMenu());
		//m_EditorUI.Init();
		//m_EditorUI.Show(ui_state);
	
		
	}
	
	
	


   
    override void ShowInventory()
    {
        UIScriptedMenu menu = GetUIManager().GetMenu();

        if (!menu)
        {
            GetUIManager().ShowScriptedMenu(m_EditorUI, menu);
        }
    }

    override void HideInventory()
    {
        if (m_EditorUI)
        {
			m_UIManager.HideScriptedMenu(m_EditorUI);
        }
    }

    void DestroyInventory()
    {
        if (m_InventoryMenu)
        {
            m_InventoryMenu.Close();
            m_InventoryMenu = NULL;
        }
    }

    override void ResetGUI()
    {
		Print("EditorMissionGameplay::ResetGUI");
        DestroyInventory();
        InitInventory();
		
		
    }

    override void ShowChat()
    {
        m_ChatChannelHideTimer.Stop();
        m_ChatChannelFadeTimer.Stop();
        m_ChatChannelArea.Show(false);
        m_UIManager.EnterScriptedMenu(MENU_CHAT_INPUT, NULL);

        int level = GetGame().GetVoiceLevel();
        UpdateVoiceLevelWidgets(level);

        PlayerControlDisable(INPUT_EXCLUDE_ALL);
    }

    override void RefreshCrosshairVisibility()
    {
        GetHudDebug().RefreshCrosshairVisibility();
    }

    override void HideCrosshairVisibility()
    {
        GetHudDebug().HideCrosshairVisibility();
    }

    override bool IsPaused()
    {
        return GetGame().GetUIManager().IsMenuOpen(MENU_INGAME);
    }

    override void Pause()
    {
        if (IsPaused() || (GetGame().GetUIManager().GetMenu() && GetGame().GetUIManager().GetMenu().GetID() == MENU_INGAME))
            return;

        if (g_Game.IsClient() && g_Game.GetGameState() != DayZGameState.IN_GAME)
            return;

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player && !player.IsPlayerLoaded() || IsPlayerRespawning())
            return;

        CloseAllMenus();

        // open ingame menu
        GetUIManager().EnterScriptedMenu(MENU_INGAME, GetGame().GetUIManager().GetMenu());
        PlayerControlDisable(INPUT_EXCLUDE_ALL);
    }

    override void Continue()
    {
        int menu_id = GetGame().GetUIManager().GetMenu().GetID();
        if (!IsPaused() || (menu_id != MENU_INGAME && menu_id != MENU_LOGOUT) || (m_Logout && m_Logout.layoutRoot.IsVisible()))
        {
            return;
        }

        PlayerControlEnable(true);
        GetUIManager().CloseMenu(MENU_INGAME);
        //GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(CloseInGameMenu,1,true);
    }

   
    override void AbortMission()
    {
#ifdef BULDOZER
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.RequestExit, IDC_MAIN_QUIT);
#else
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().AbortMission);
#endif
    }

    override void CreateLogoutMenu(UIMenuPanel parent)
    {
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

        // do not show logout screen if player's dead
        if (!player || player.IsDamageDestroyed())
        {
            // exit the game immediately
            AbortMission();
            return;
        }

        if (parent)
        {
            m_Logout = LogoutMenu.Cast(parent.EnterScriptedMenu(MENU_LOGOUT));

            if (m_Logout)
            {
                m_Logout.SetLogoutTime();
            }
        }
    }

    override void StartLogoutMenu(int time)
    {
        if (m_Logout)
        {
            if (time > 0)
            {
                // character will be deleted from server int "time" seconds
                m_Logout.SetTime(time);
                m_Logout.Show();

                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(m_Logout.UpdateTime, 1000, true);
            }
            else
            {
                // no waiting time -> player is most likely dead
                m_Logout.Exit();
            }
        }
    }

    override void CreateDebugMonitor()
    {
        if (!m_DebugMonitor)
        {
            m_DebugMonitor = new DebugMonitor();
            m_DebugMonitor.Init();
        }
    }


    override void UpdateVoiceLevelWidgets(int level)
    {
        for (int n = 0; n < m_VoiceLevelsWidgets.Count(); n++)
        {
            int voiceKey = m_VoiceLevelsWidgets.GetKey(n);
            ImageWidget voiceWidget = m_VoiceLevelsWidgets.Get(n);

            // stop fade timer since it will be refreshed
            ref WidgetFadeTimer timer = m_VoiceLevelTimers.Get(n);
            timer.Stop();

            // show widgets according to the level
            if (voiceKey <= level)
            {
                voiceWidget.SetAlpha(1.0); // reset from possible previous fade out
                voiceWidget.Show(true);

                if (!m_VoNActive && !GetUIManager().FindMenu(MENU_CHAT_INPUT))
                    timer.FadeOut(voiceWidget, 3.0);
            }
            else
                voiceWidget.Show(false);
        }

        // fade out microphone icon when switching levels without von on
        if (!m_VoNActive)
        {
            if (!GetUIManager().FindMenu(MENU_CHAT_INPUT))
            {
                m_MicrophoneIcon.SetAlpha(1.0);
                m_MicrophoneIcon.Show(true);

                m_MicFadeTimer.FadeOut(m_MicrophoneIcon, 3.0);
            }
        }
        else
        {
            // stop mic icon fade timer when von is activated
            m_MicFadeTimer.Stop();
        }
    }
    override bool IsVoNActive()
    {
        return m_VoNActive;
    }

    override void HideVoiceLevelWidgets()
    {
        for (int n = 0; n < m_VoiceLevelsWidgets.Count(); n++)
        {
            ImageWidget voiceWidget = m_VoiceLevelsWidgets.Get(n);
            voiceWidget.Show(false);
        }
    }

    override UIScriptedMenu GetNoteMenu()
    {
        return m_Note;
    };

    override void SetNoteMenu(UIScriptedMenu menu)
    {
        m_Note = NoteMenu.Cast(menu);
    };

    override void SetPlayerRespawning(bool state)
    {
        m_PlayerRespawning = state;
    }

    override bool IsPlayerRespawning()
    {
        return m_PlayerRespawning;
    }
}





static void ResetMission()
{
	GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().RestartMission);
}

modded class DayZIntroScene
{
	protected Object m_FunnyMeme;
	
	void DayZIntroScene()
	{
		delete m_Character;
		m_FunnyMeme = GetGame().CreateObject("DSLRCamera", m_CharacterPos, true);
		m_FunnyMeme.SetOrientation(m_CharacterRot);
	}
}


modded class MainMenu 
{
	override Widget Init()
	{
		super.Init();
		
		m_ChooseServer.Show(false);
		m_CustomizeCharacter.Show(false);
		m_Stats.HideStats();
		
		Widget c = layoutRoot.FindAnyWidget("character");
		c.Show(false);
		
		TextWidget tw = TextWidget.Cast(layoutRoot.FindAnyWidget("play_label"));
		tw.SetText("Open Editor");
		
		
		return layoutRoot;
	}
	
	override void Play()
	{
		GetGame().PlayMission("P:\\DayZ_Server\\dev\\DayZEditor\\mission\\DayZEditor.ChernarusPlus");
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if(IsFocusable(w)) {
			ColorHighlight( w );
			return true;
		}
		return false;
	}
}