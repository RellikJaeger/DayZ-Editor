enum EditorClientModuleRPC
{
	INVALID = 36114,
	COUNT
};

class EditorClientModule: JMModuleBase
{
	protected int m_KonamiCodeProgress;
	protected float m_KonamiCodeCooldown;
	
	static const ref array<int> KONAMI_CODE = {
		KeyCode.KC_UP,
		KeyCode.KC_UP,
		KeyCode.KC_DOWN,
		KeyCode.KC_DOWN,
		KeyCode.KC_LEFT,
		KeyCode.KC_RIGHT,
		KeyCode.KC_LEFT,
		KeyCode.KC_RIGHT,
		KeyCode.KC_B,
		KeyCode.KC_A
	};
	
	void EditorClientModule() 
	{
		EditorLog.Info("EditorClientModule");
		g_Game.ReportProgress("Loading Client Module");
	}
	
	void ~EditorClientModule() 
	{
		EditorLog.Info("~EditorClientModule");
	}
	
	// JMModuleBase Overrides
	override void OnInit()
	{
		EditorLog.Trace("Editor::OnInit");
						
		// Keybinds
		RegisterBinding(new JMModuleBinding("OnEditorToggleActive", "EditorToggleActive"));
		RegisterBinding(new JMModuleBinding("OnEditorToggleCursor", "EditorToggleCursor"));
		RegisterBinding(new JMModuleBinding("OnEditorToggleUI", "EditorToggleUI"));
		RegisterBinding(new JMModuleBinding("OnEditorTeleportPlayerToCursor", "EditorTeleportPlayerToCursor"));
		
		RegisterBinding(new JMModuleBinding("OnEditorToggleMap", "EditorToggleMap"));
		RegisterBinding(new JMModuleBinding("OnEditorDeleteObject", "EditorDeleteObject"));
		
		RegisterBinding(new JMModuleBinding("OnEditorMoveObjectForward", "EditorMoveObjectForward"));
		RegisterBinding(new JMModuleBinding("OnEditorMoveObjectBackward", "EditorMoveObjectBackward"));
		RegisterBinding(new JMModuleBinding("OnEditorMoveObjectLeft", "EditorMoveObjectLeft"));
		RegisterBinding(new JMModuleBinding("OnEditorMoveObjectRight", "EditorMoveObjectRight"));
		RegisterBinding(new JMModuleBinding("OnEditorMoveObjectUp", "EditorMoveObjectUp"));
		RegisterBinding(new JMModuleBinding("OnEditorMoveObjectDown", "EditorMoveObjectDown"));
	}
		
	override void OnUpdate(float timeslice)
	{
		if (GetEditor()) {
			GetEditor().Update(timeslice);
		}
		
		// Konami suck
		/*if (m_KonamiCodeCooldown != 0) {
			m_KonamiCodeCooldown -= timeslice;
			m_KonamiCodeCooldown = Math.Clamp(m_KonamiCodeCooldown, 0, 100);
		}
		
		if (m_KonamiCodeProgress != -1 && KeyState(KONAMI_CODE[m_KonamiCodeProgress]) && m_KonamiCodeCooldown == 0) {
			m_KonamiCodeProgress++;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CheckKonamiCode, 1000, false, m_KonamiCodeProgress);
			m_KonamiCodeCooldown = 0.15;
		}
		
		if (m_KonamiCodeProgress >= KONAMI_CODE.Count()) {
			GetEditor().GetEditorHud().CreateNotification("Konami Code Complete!", ARGB(255, 255, 0, 255));
			GetEditor().KEgg = true;
			m_KonamiCodeProgress = -1;
		}*/
		
		/*
		if (GetEditor() && GetEditor().GetCamera() && !IsMissionOffline()) {
			ScriptRPC update_rpc = new ScriptRPC();
			update_rpc.Write(GetEditor().GetCamera().GetPosition());
			update_rpc.Write(GetEditor().GetCamera().GetOrientation());
			//update_rpc.Send(null, EditorServerModuleRPC.EDITOR_CLIENT_UPDATE, true);
		}*/
	}
	
	private void CheckKonamiCode(int progress)
	{
		if (m_KonamiCodeProgress == progress) {
			m_KonamiCodeProgress = 0;
		}
	}
	
	override bool IsServer() 
	{
		return false;
	}	
		
	override void OnMissionStart()
	{
		EditorLog.Trace("Editor::OnMissionStart");
		
		g_Game.ReportProgress("Loading Mission");
		vector center_pos = Editor.GetMapCenterPosition();
		PlayerBase player = Editor.CreateDefaultCharacter(Editor.GetSafeStartPosition(center_pos[0], center_pos[2], 500));
		if (!player) {
			g_Game.ReportProgress("Failed to create player, contact InclementDab");
			Error("Player was not created, exiting");
			return;
		}
		
		GetGame().SelectPlayer(null, player);
	}
	
	override void OnMissionFinish()
	{
		EditorLog.Trace("Editor::OnMissionFinish");
		Editor.Destroy();
	}
		
	override void OnMissionLoaded()
	{
		EditorLog.Trace("Editor::OnMissionLoaded");
		
		g_Game.ReportProgress("Mission Loaded");
		EditorLog.Info("Loading Offline Editor...");
		Editor editor = Editor.Create(PlayerBase.Cast(GetGame().GetPlayer()));
		editor.SetActive(true);
	}
	
	// Inputs
	private bool ShouldProcessInput(UAInput input)
	{
		// Check if LocalPress, Check if LControl is pressed, Check if game is focused
		return (GetEditor() && input.LocalPress() && !KeyState(KeyCode.KC_LCONTROL) && GetGame().GetInput().HasGameFocus());
	}
	
	private bool ShouldProcessQuickInput(UAInput input)
	{
		return (GetEditor() && input.LocalValue() && !KeyState(KeyCode.KC_LCONTROL) && GetGame().GetInput().HasGameFocus());
	}
	
	private void OnEditorToggleActive(UAInput input)
	{
		if (!ShouldProcessInput(input)) return;
		EditorLog.Trace("Editor::OnEditorToggleActive");
		
		string ban_reason;
		if (GetEditor().IsBannedClient(ban_reason)) {
			GetEditor().ShowBanDialog(ban_reason);
			return;
		}
		
		bool active = GetEditor().IsActive(); // weird syntax bug?
		GetEditor().SetActive(!active);
	}	
	
	private void OnEditorToggleCursor(UAInput input)
	{
		if (!ShouldProcessInput(input)) return;
		EditorLog.Trace("Editor::OnEditorToggleCursor");
		
		// Dont want to toggle cursor on map
		if (!GetEditor().IsActive() || GetEditor().GetEditorHud().EditorMapWidget.IsVisible() || (EditorHud.CurrentDialog && GetEditor().Settings.LockCameraDuringDialogs)) {
			return;
		}
		
		GetEditor().GetEditorHud().ToggleCursor();
	}	
	
	private void OnEditorToggleUI(UAInput input)
	{		
		if (!ShouldProcessInput(input)) return;
		EditorLog.Trace("Editor::OnEditorToggleUI");
		
		string ban_reason;
		if (GetEditor().IsBannedClient(ban_reason)) {
			GetEditor().ShowBanDialog(ban_reason);
			return;
		}
		
		if (GetEditor().IsInventoryEditorActive()) {
			GetEditor().GetInventoryEditorHud().GetLayoutRoot().Show(!GetEditor().GetInventoryEditorHud().GetLayoutRoot().IsVisible());
			return;
		}
		
		GetEditor().GetEditorHud().Show(!GetEditor().GetEditorHud().IsVisible());
		
		EditorObjectMap placed_objects =  GetEditor().GetPlacedObjects();
		foreach (int id, EditorObject editor_object: placed_objects) {
			EditorObjectMarker marker = editor_object.GetMarker();
			if (marker) {
				marker.Show(GetEditor().GetEditorHud().IsVisible());
			}
		}
		
		// If player is active
		if (!GetEditor().IsActive()) {
			GetEditor().GetEditorHud().ShowCursor(GetEditor().GetEditorHud().IsVisible());
			
			// A wacky way to disable motion while the UI is enabled
			if (GetGame().GetPlayer()) {
				GetGame().GetPlayer().DisableSimulation(GetEditor().GetEditorHud().IsVisible());
			}
		}
	}
	
	private void OnEditorToggleMap(UAInput input)
	{
		if (!ShouldProcessInput(input)) return;
		EditorLog.Trace("Editor::OnEditorToggleMap");
		
		if (!GetEditor().GetEditorHud().IsVisible()) return;
		
		EditorHud editor_hud = GetEditor().GetEditorHud();
		editor_hud.EditorMapWidget.Show(!editor_hud.EditorMapWidget.IsVisible());
		editor_hud.ShowCursor(true);
		
		EditorEvents.MapToggled(this, GetEditor().GetEditorHud().EditorMapWidget, GetEditor().GetEditorHud().EditorMapWidget.IsVisible());
	}	
	
	private void OnEditorDeleteObject(UAInput input)
	{
		if (!ShouldProcessInput(input)) return;
		EditorLog.Trace("Editor::OnEditorDeleteObject");
		
		EditorDeleteCommand command();
		CommandArgs args();
		args.Context = GetEditor().GetEditorHud();
		command.Execute(this, args);
	}
	
	private void OnEditorTeleportPlayerToCursor(UAInput input)
	{		
		if (!ShouldProcessInput(input)) return;
		EditorLog.Trace("Editor::OnEditorTeleportPlayerToCursor");
				
		GetEditor().TeleportPlayerToCursor();
	}
		
	private void QuickTransformObjects(vector relative_position)
	{
		EditorObjectMap selected_objects = GetEditor().GetSelectedObjects();
		foreach (int id, EditorObject editor_object: selected_objects) {
			editor_object.Position = relative_position + editor_object.GetPosition();
			editor_object.PropertyChanged("Position");
		}
	}
	
	private void OnEditorMoveObjectForward(UAInput input)
	{
		// nothing is selected and we are actively placing
		if (GetEditor() && GetEditor().GetSelectedObjects().Count() == 0 && GetEditor().IsPlacing() && input.LocalPress()) {
			ObservableCollection<ref EditorPlaceableListItem> placeables = GetEditor().GetEditorHud().GetTemplateController().LeftbarSpacerData;
			for (int i = 0; i < placeables.Count(); i++) {
				if (placeables[i].IsSelected()) {
					if (!placeables[i - 1]) {
						return;
					}
					
					placeables[i].Deselect();
					GetEditor().CreateInHand(placeables[i - 1].GetPlaceableItem());
					placeables[i - 1].Select();
					return;
				}
			}
		}
		
		if (!ShouldProcessQuickInput(input)) return;
		//EditorLog.Trace("Editor::OnEditorMoveObjectForward");
		
		float value = GetEditor().Settings.QuickMoveStepSize;
		if (GetGame().GetInput().LocalValue("EditorCameraTurbo")) {
			value *= 0.01;
		}
		
		QuickTransformObjects(Vector(0, 0, value));
	}

	private void OnEditorMoveObjectBackward(UAInput input)
	{
		// nothing is selected and we are actively placing
		if (GetEditor() && GetEditor().GetSelectedObjects().Count() == 0 && GetEditor().IsPlacing() && input.LocalPress()) {
			ObservableCollection<ref EditorPlaceableListItem> placeables = GetEditor().GetEditorHud().GetTemplateController().LeftbarSpacerData;
			for (int i = 0; i < placeables.Count(); i++) {
				if (placeables[i].IsSelected()) {
					if (!placeables[i + 1]) {
						return;
					}
					
					placeables[i].Deselect();
					GetEditor().CreateInHand(placeables[i + 1].GetPlaceableItem());
					placeables[i + 1].Select();
					return;
				}
			}
		}
		
		if (!ShouldProcessQuickInput(input)) return;
		//EditorLog.Trace("Editor::OnEditorMoveObjectBackward");
		
		float value = GetEditor().Settings.QuickMoveStepSize;
		if (GetGame().GetInput().LocalValue("EditorCameraTurbo")) {
			value *= 0.01;
		}
		
		QuickTransformObjects(Vector(0, 0, -value));
	}
	
	private void OnEditorMoveObjectLeft(UAInput input)
	{
		if (!ShouldProcessQuickInput(input)) return;
		//EditorLog.Trace("Editor::OnEditorMoveObjectLeft");
		
		float value = GetEditor().Settings.QuickMoveStepSize;
		if (GetGame().GetInput().LocalValue("EditorCameraTurbo")) {
			value *= 0.01;
		}
		
		QuickTransformObjects(Vector(-value, 0, 0));
	}	
	
	private void OnEditorMoveObjectRight(UAInput input)
	{
		if (!ShouldProcessQuickInput(input)) return;
		//EditorLog.Trace("Editor::OnEditorMoveObjectRight");
		
		float value = GetEditor().Settings.QuickMoveStepSize;
		if (GetGame().GetInput().LocalValue("EditorCameraTurbo")) {
			value *= 0.01;
		}
		
		QuickTransformObjects(Vector(value, 0, 0));
	}
	
	private void OnEditorMoveObjectUp(UAInput input)
	{
		if (!ShouldProcessQuickInput(input)) return;
		//EditorLog.Trace("Editor::OnEditorMoveObjectUp");
		
		float value = GetEditor().Settings.QuickMoveStepSize;
		if (GetGame().GetInput().LocalValue("EditorCameraTurbo")) {
			value *= 0.01;
		}
		
		QuickTransformObjects(Vector(0, value, 0));
	}	
	
	private void OnEditorMoveObjectDown(UAInput input)
	{
		if (!ShouldProcessQuickInput(input)) return;
		//EditorLog.Trace("Editor::OnEditorMoveObjectDown");
		
		float value = GetEditor().Settings.QuickMoveStepSize;
		if (GetGame().GetInput().LocalValue("EditorCameraTurbo")) {
			value *= 0.01;
		}
		
		QuickTransformObjects(Vector(0, -value, 0));
	}
	
	// RPC stuff
	override int GetRPCMin() 
	{
		return EditorClientModuleRPC.INVALID;
	}

	override int GetRPCMax()
	{
		return EditorClientModuleRPC.COUNT;
	}
	
	override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ref ParamsReadContext ctx)
	{
		switch (rpc_type) {
			
		
		}
	}
}