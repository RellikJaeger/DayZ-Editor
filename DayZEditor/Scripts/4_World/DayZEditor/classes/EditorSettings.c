class EditorSettings: ProfileSettings
{	
	// General Settings
	float ViewDistance 				= 8000;
	float ObjectViewDistance 		= 1500;
	int AutoSaveTimer 				= 240;
	float QuickMoveStepSize			= 0.1;

	// Camera Settings
	float CameraSpeed				= 60;
	bool RuleOfThirds				= false;
	
	// Advanced Settings
	bool LockCameraDuringDialogs 	= true;
	bool ShowBoundingBoxes 			= true;
	bool PreloadObjects				= false;
	bool ShowScreenLogs				= true;
	bool HighPrecisionCollision		= false;
	bool ExportBackupFile 			= true; // this is temporary until we know that the bin files are stable
	bool DebugMode 					= false;
	
	// Marker Settings
	bool MarkerTooltips				= true;
	bool MarkerGroundOcclusion		= false;
	float MarkerViewDistance 		= 1000;
	int MarkerPrimaryColor			= COLOR_WHITE_A;
	
	// Theme Settings
	int HighlightColor				= COLOR_SALMON;
	int SelectionColor				= COLOR_BLUE;
	
	// Brush Settings
	bool BrushedObjectMarkers 		= false;
	bool BrushedListItems			= false;
	string EditorBrushFile 			= Editor.ROOT_DIRECTORY + "EditorBrushes.xml";
	
	// Loot Settings
	string EditorProtoFile 			= Editor.ROOT_DIRECTORY + "MapGroupProto.xml";
	
	LogLevel SelectedLogLevel 		= LogLevel.INFO;

	// Its a pseduo-controller, preferences dialogs!!
	void PropertyChanged(string property_name)
	{
		switch (property_name) {
						
			case "SelectedLogLevel": {
				EditorLog.Warning("Changed log level to %1", typename.EnumToString(LogLevel, SelectedLogLevel));
				break;
			}
			
			case "ViewDistance":
			case "ObjectViewDistance": {
				// This might break, not sure
				GetGame().GetWorld().SetPreferredViewDistance(ViewDistance);
				
				GetGame().GetWorld().SetViewDistance(ViewDistance);
				GetGame().GetWorld().SetObjectViewDistance(ObjectViewDistance);
				break;
			}
			
			case "ShowScreenLogs": {
				GetEditor().GetEditorHud().ShowScreenLogs(ShowScreenLogs);
				break;
			}
			
			case "MarkerColor": {
				EditorObjectMap editor_objects = GetEditor().GetObjectManager().GetPlacedObjects();
				foreach (int id, EditorObject editor_object: editor_objects) {
					EditorObjectMarker marker = editor_object.GetMarker();
					if (marker) {
						marker.Update();
					}
				}
				
				break;
			}
			
			case "CameraSpeed": {
				GetEditor().GetCamera().Speed = CameraSpeed;
				break;
			}
			
			case "RuleOfThirds": {
				GetEditor().GetEditorHud().ShowRuleOfThirds(RuleOfThirds);
				break;
			}
		}
	}
	
	void SetDefaults()
	{
		QuickMoveStepSize			= 0.1;
		CameraSpeed					= 60;
		ViewDistance 				= 8000;
		ObjectViewDistance 			= 1500;
		MarkerViewDistance 			= 1000;
		
		AutoSaveTimer 				= 240;

		LockCameraDuringDialogs 	= true;
		ShowBoundingBoxes 			= true;
		PreloadObjects				= false;
		ShowScreenLogs				= true;
		DebugMode 					= false;
		
		MarkerPrimaryColor			= COLOR_WHITE_A;
		HighlightColor				= COLOR_SALMON;
		SelectionColor				= COLOR_BLUE;
	}
}