class EditorHudController: EditorControllerBase
{
	string SearchBarData;
	string SearchBarIcon = "set:dayz_editor_gui image:search";
	string Version = Editor.Version;
	
	string ObjectReadoutName;
	
	bool CategoryPlacements = true;
	bool CategoryDeletions;
	
	float cam_x, cam_y, cam_z;	
	float obj_x, obj_y, obj_z;
	
	// Main data
	ref EditorHudToolbar EditorHudToolbarView;
	
	ref ObservableCollection<ref EditorPlaceableListItem> LeftbarSpacerData = new ObservableCollection<ref EditorPlaceableListItem>(this);
	
	//
	ref ObservableCollection<EditorListItem> RightbarPlacedData 		= new ObservableCollection<EditorListItem>(this);
	ref ObservableCollection<EditorListItem> RightbarDeletionData 		= new ObservableCollection<EditorListItem>(this);
	
	// Logger
	static const int MAX_LOG_ENTRIES = 15;
	ref ObservableCollection<ref EditorLogEntry> EditorLogEntries 			= new ObservableCollection<ref EditorLogEntry>(this);
	
	// Camera bindings
	float CameraSmoothing = 50.0;
	ref ObservableCollection<EditorCameraTrackListItem> CameraTrackData = new ObservableCollection<EditorCameraTrackListItem>(this);

	// View Properties
	protected Widget LeftbarFrame;
	protected ImageWidget LeftbarHideIcon;
	
	protected ScrollWidget LeftbarScroll;
	protected ScrollWidget RightbarScroll;
	
	protected Widget RightbarFrame;
	protected ImageWidget RightbarHideIcon;
	
	protected WrapSpacerWidget RightbarPlacementsList;
	protected WrapSpacerWidget RightbarDeletionsList;
	
	protected GridSpacerWidget InfobarObjPosFrame;
		
	protected WrapSpacerWidget LeftbarPanelSelectorWrapper;
	protected EditBoxWidget LeftbarSearchBar;
	
	protected ButtonWidget CinematicCameraButton;
	protected ButtonWidget BrushToggleButton;
	protected ButtonWidget PlacementsTabButton;
	protected ButtonWidget DeletionsTabButton;
	protected ButtonWidget LeftbarPanelSearchBarIconButton;
	
	// Camera Track
	protected Widget CameraTrackWrapper;
	protected ButtonWidget CameraTrackRunButton;
	protected Widget CameraTrackButtonOutline;
	
	// Temp until sub ViewControllers can be properties of parent ViewController
	EditorHudToolbarController GetToolbarController() 
	{
		return EditorHudToolbarController.Cast(EditorHudToolbarView.GetController());
	}
	
	void EditorHudController() 
	{
		EditorLog.Trace("EditorHudController");
		
#ifndef COMPONENT_SYSTEM		
		EditorLog.OnLog.Insert(OnEditorLog);
		
		GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Insert(Update);
#endif
	}
	
	void ~EditorHudController() 
	{
		EditorLog.Trace("~EditorHudController");
		
#ifndef COMPONENT_SYSTEM		
		EditorLog.OnLog.Remove(OnEditorLog);
		
		GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Remove(Update);
#endif
	}
	
	override void OnWidgetScriptInit(Widget w)
	{
		super.OnWidgetScriptInit(w);
		
		// Reload Placeables
#ifndef COMPONENT_SYSTEM
		array<ref EditorPlaceableItem> placeable_items = LoadPlaceableObjects();
		foreach (EditorPlaceableItem placeable_item: placeable_items) {				
			// Makes stuff look good when first loading
			if (GetEditor().Settings.PreloadObjects) {
				GetGame().ObjectDelete(GetGame().CreateObjectEx(placeable_item.Type, vector.Zero, ECE_NONE));				
			}
			
			LeftbarSpacerData.Insert(new EditorPlaceableListItem(placeable_item));
		}
		
		EditorLog.Info("Loaded %1 Placeable Objects", placeable_items.Count().ToString());
#endif
		
		EditorHudToolbarView = new EditorHudToolbar();
		NotifyPropertyChanged("EditorHudToolbarView");
		
		PlacementsTabButton.SetColor(m_Editor.Settings.SelectionColor);
		DeletionsTabButton.SetColor(ARGB(255, 60, 60, 60));
	}
		
	void Update()
	{
		//Debug.DestroyAllShapes();
/*
		array<EditorCameraTrackListItem> camera_tracks = GetCameraTracks();
		for (int i = 0; i < camera_tracks.Count(); i++) {
			EditorCameraTrackListItemController start_ctrl = camera_tracks[i].GetData();
			if (!camera_tracks[i + 1]) {
				continue;
			}
			
			EditorCameraTrackListItemController end_ctrl = camera_tracks[i + 1].GetData();
			
			float value = 0;
			vector last_position = start_ctrl.GetPosition();
			while (value <= 1.0) {
				vector position = EditorMath.LerpVector(start_ctrl.GetPosition(), end_ctrl.GetPosition(), value);
				Debug.DrawLine(last_position, position, COLOR_WHITE);
				last_position = position;
				value += 0.05;
			}
		}*/
	}
			
	void InsertMapMarker(EditorMarker map_marker)
	{
		EditorLog.Trace("EditorHudController::InsertMapObject " + map_marker.GetLayoutRoot().GetName());
		m_Editor.GetEditorHud().EditorMapWidget.AddChild(map_marker.GetLayoutRoot());
	}
	
	static array<ref EditorPlaceableItem> LoadPlaceableObjects() 
	{ 
		EditorLog.Trace("EditorHudController::LoadPlaceableObjects");
		g_Game.ReportProgress("Loading Placeable Objects");
		
		array<ref EditorPlaceableItem> placeable_items();
		TStringArray config_paths = {};
		config_paths.Insert(CFG_VEHICLESPATH);
		config_paths.Insert(CFG_WEAPONSPATH);
		config_paths.Insert(CFG_MAGAZINESPATH);
		
		foreach (string path: config_paths) {
			for (int i = 0; i < GetGame().ConfigGetChildrenCount(path); i++) {
				string type;
		        GetGame().ConfigGetChildName(path, i, type);
				if (GetGame().ConfigGetInt(path + " " + type + " scope") < 1) {
					continue;
				}
				
				EditorPlaceableItem placeable_item = EditorPlaceableItem.Create(path, type);

				if (!placeable_item || IsForbiddenItem(placeable_item.Type)) {
					continue;
				}
				
				placeable_items.Insert(placeable_item);
		    }
		}
		
		placeable_items.Insert(EditorPlaceableItem.Create(EditorPointLight));
		placeable_items.Insert(EditorPlaceableItem.Create(EditorSpotLight));
		
		return placeable_items;
	}
	
	static bool IsForbiddenItem(string Model)
	{
		//! In theory should be safe but just in case
		if (Model.Contains("Fx")) return true;
		if (Model == "ItemOptics") return true;

		//! Cursed items
		if (Model == "AKM_TESTBED") return true;
		if (Model == "Red9") return true;
		if (Model == "QuickieBow") return true;
		if (Model == "LargeTentBackPack") return true;
		if (Model == "SurvivorMale_Base" || Model == "SurvivorFemale_Base") return true;
		if (GetGame().IsKindOf(Model, "GP25Base")) return true;
		if (GetGame().IsKindOf(Model, "M203Base")) return true;
	
		//! Everything is fine... I hope... :pain:
		return false;
	}

	override void PropertyChanged(string property_name)
	{
		EditorLog.Trace("EditorHudController::PropertyChanged: %1", property_name);
		
		switch (property_name) {
					
			case "SearchBarData": {
				for (int j = 0; j < LeftbarSpacerData.Count(); j++) {
					EditorPlaceableListItem placeable_item = LeftbarSpacerData[j];
					placeable_item.GetLayoutRoot().Show(placeable_item.FilterType(SearchBarData)); 
				}
				
				LeftbarScroll.VScrollToPos(0);
				
				if (SearchBarData.Length() > 0) {
					SearchBarIcon = "set:dayz_gui image:icon_x";
				} else {
					SearchBarIcon = "set:dayz_editor_gui image:search";
				}
				
				NotifyPropertyChanged("SearchBarIcon");
				
				break;
			}			
			
			case "cam_x":
			case "cam_y":
			case "cam_z": {				
				GetEditor().GetCamera().SetPosition(Vector(cam_x, cam_y, cam_z));
				break;
			}
			
			case "CategoryPlacements": {
				CategoryDeletions = false;
				NotifyPropertyChanged("CategoryDeletions", false);
				// forcing to be true, otherwise it will just show nothing
				CategoryPlacements = true;
				
				RightbarPlacementsList.Show(CategoryPlacements);
				RightbarDeletionsList.Show(CategoryDeletions);
				
				PlacementsTabButton.SetColor(m_Editor.Settings.SelectionColor);
				DeletionsTabButton.SetColor(ARGB(255, 60, 60, 60));
				RightbarScroll.VScrollToPos(0);
				break;
			}
			
			case "CategoryDeletions": {				
				CategoryPlacements = false;				
				NotifyPropertyChanged("CategoryPlacements", false);				
				// forcing to be true, otherwise it will just show nothing
				CategoryDeletions = true;
				RightbarPlacementsList.Show(CategoryPlacements);
				RightbarDeletionsList.Show(CategoryDeletions);
				
				PlacementsTabButton.SetColor(ARGB(255, 60, 60, 60));
				DeletionsTabButton.SetColor(m_Editor.Settings.SelectionColor);
				RightbarScroll.VScrollToPos(0);
				break;
			}
			
			case "SearchBarIcon": {
				// this could probably be a command with SetCanExecute but im not feeling it 
				LeftbarPanelSearchBarIconButton.Enable(SearchBarData.Length() > 0);
				break;
			}
		}
	}
	
	void LeftbarHideExecute(ButtonCommandArgs args) 
	{
		LeftbarFrame.Show(!args.GetButtonState());
		
		if (args.GetButtonState()) {
			LeftbarHideIcon.SetFlags(WidgetFlags.FLIPU);
		} else {
			LeftbarHideIcon.ClearFlags(WidgetFlags.FLIPU);
		}
	}
	
	void RightbarHideExecute(ButtonCommandArgs args) 
	{
		RightbarFrame.Show(!args.GetButtonState());
				
		if (args.GetButtonState()) {
			RightbarHideIcon.SetFlags(WidgetFlags.FLIPU);
		} else {
			RightbarHideIcon.ClearFlags(WidgetFlags.FLIPU);
		}
	}
		
	void ButtonCreateFolderExecute(ButtonCommandArgs args) 
	{
		EditorLog.Trace("EditorHudController::ButtonCreateFolderExecute");
		EditorCollapsibleListItem category(null);
		RightbarPlacedData.Insert(category);
	}	
	
	void CameraTrackToggleExecute(ButtonCommandArgs args) 
	{
		EditorLog.Trace("EditorHudController::CameraTrackToggleExecute");
		CameraTrackWrapper.Show(!CameraTrackWrapper.IsVisible());
	}

	void CameraTrackInsertNode(ButtonCommandArgs args)
	{
		EditorLog.Trace("EditorHudController::CameraTrackInsertNode");
		string name = "CameraTrack" + CameraTrackData.Count();
		GetEditor().GetCameraTrackManager().InsertCameraTrack(GetEditor().GetCamera(), 1.0, name);
	}

	void OnSearchButtonPress(ButtonCommandArgs args)
	{
		EditorLog.Trace("EditorHudController::OnSearchButtonPress");
		if (SearchBarData.Length() > 0) {
			SearchBarData = string.Empty;
			NotifyPropertyChanged("SearchBarData");
		}
	}
	
	void OnCameraTrackStart()
	{
		CameraTrackRunButton.SetText("Stop");
		CameraTrackRunButton.SetColor(COLOR_RED);
		CameraTrackButtonOutline.SetColor(COLOR_RED);
	}
	
	void OnCameraTrackStop()
	{
		CameraTrackRunButton.SetText("Start");
		CameraTrackRunButton.SetColor(COLOR_WHITE_A);
		CameraTrackButtonOutline.SetColor(COLOR_WHITE);
		CameraTrackRunButton.SetState(1);
	}
		
	void DoMultiSelect(int index_0, int index_1, ObservableCollection<EditorListItem> list)
	{
		int bottom, top;
		bottom = Math.Min(index_0, index_1);
		top = Math.Max(index_0, index_1);
		
		if (list.Count() < top) {
			EditorLog.Error("Could not multi select, top was out of range of list array");
			return;
		}
		
		for (int i = bottom; i < top; i++) {
			EditorPlacedListItem placed_list_item;
			// this is bad and wont work.. well it will but i dont like it
			if (Class.CastTo(placed_list_item, list[i])) {
				GetEditor().SelectObject(placed_list_item.GetEditorObject());
			}
			
			EditorDeletedListItem deleted_list_item;
			if (Class.CastTo(deleted_list_item, list[i])) {
				GetEditor().SelectHiddenObject(deleted_list_item.GetDeletedObject());
			}
		}
	}
	
	// im not adding a trace to this lol
	void OnEditorLog(LogLevel level, string message)
	{
		if (!m_Editor || !m_Editor.Settings || level < m_Editor.Settings.SelectedLogLevel) {
			return;
		}
		
		EditorLogEntries.Insert(new EditorLogEntry(level, message));
		if (EditorLogEntries.Count() > MAX_LOG_ENTRIES) {
			// todo: this is booming my mind for whatever reason
			//delete EditorLogEntries[1];
			//EditorLogEntries.Remove(1);
		}
	}
	
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		EditorLog.Trace("EditorHudController::OnMouseButtonDown");
		
		if (button == MouseState.RIGHT) {
		
			switch (w) {
				case LeftbarSearchBar: {
					SearchBarData = string.Empty;
					NotifyPropertyChanged("SearchBarData");
					break;
				}
			}	
		}
	
		return super.OnMouseButtonDown(w, x, y, button);
	}
		
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		//EditorLog.Trace("EditorHudController::OnMouseEnter");

		switch (w) {
			case PlacementsTabButton: {
				EditorHud.SetCurrentTooltip(EditorTooltip.CreateOnButton("" + GetEditor().GetPlacedObjects().Count() + " #STR_EDITOR_PLACEMENTS", w, TooltipPositions.BOTTOM_LEFT));
				break;
			}
			
			case DeletionsTabButton: {
				EditorHud.SetCurrentTooltip(EditorTooltip.CreateOnButton("" + GetEditor().GetDeletedObjects().Count() + " #STR_EDITOR_DELETIONS", w, TooltipPositions.BOTTOM_LEFT));
				break;
			}
			
			case CinematicCameraButton: {
				EditorHud.SetCurrentTooltip(EditorTooltip.CreateOnButton("#STR_EDITOR_CINEMATIC_CAMERA", w, TooltipPositions.TOP_LEFT));
				break;
			}
		}
				
		return super.OnMouseEnter(w, x, y);
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		//EditorLog.Trace("EditorHudController::OnMouseLeave");
		EditorHud.SetCurrentTooltip(null);
		
		return super.OnMouseLeave(w, enterW, x, y);
	}
	
	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (RecursiveGetParent(w, ScrollWidget)) {
			if (KeyState(KeyCode.KC_LCONTROL)) {
				ScrollWidget.Cast(w).VScrollStep(wheel * 10);
			}
		}

		return false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		EditorLog.Trace("EditorHud::OnFocus");
		
		switch (w) {
			
			case LeftbarSearchBar: {
				m_Editor.GetCamera().MoveEnabled = false;
				break;
			}
		}
		
		return false;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		EditorLog.Trace("EditorHud::OnFocusLost");
		
		switch (w) {
			
			case LeftbarSearchBar: {
				m_Editor.GetCamera().MoveEnabled = true;
				break;
			}
		}
		
		return false;
	}
	
	void SetInfoObjectPosition(vector position)
	{
		obj_x = position[0];
		obj_y = position[1];
		obj_z = position[2];
		NotifyPropertyChanged("obj_x");
		NotifyPropertyChanged("obj_y");
		NotifyPropertyChanged("obj_z");
	}
}