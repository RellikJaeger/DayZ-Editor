enum ExportMode 
{
	TERRAINBUILDER,
	EXPANSION,
	COMFILE,
	VPP
}

enum ImportMode
{
	TERRAINBUILDER,
	EXPANSION, 
	COMFILE,
	VPP
}

enum HeightType 
{
	ABSOLUTE,
	RELATIVE
}

class ExportSettings
{
	ExportMode ExportFileMode;
	HeightType ExportHeightType;
	bool ExportSelectedOnly;
	vector ExportOffset;
	string ExportSetName;
}

enum FileDialogResult
{
	SUCCESS = 0,
	NOT_FOUND = 1,
	IN_USE = 2,
	NOT_SUPPORTED = 3,
	UNKNOWN_ERROR = 100
}



typedef FileSerializer Cerealizer;

static FileDialogResult ImportVPPData(out ref EditorObjectDataSet data, string filename)
{
	Cerealizer file = new Cerealizer();
	if (!FileExist(filename)) {
		return FileDialogResult.NOT_FOUND;
	}
	
	ref VPPToEditorBuildingSet bSet;
	if (file.Open(filename, FileMode.READ)) {
		file.Read(bSet);
		file.Close();
		
	} else return FileDialogResult.UNKNOWN_ERROR;
	
	if (!bSet) return FileDialogResult.UNKNOWN_ERROR;
	
	ref array<ref VPPToEditorSpawnedBuilding> spawned_buildings = new array<ref VPPToEditorSpawnedBuilding>();
	bSet.GetSpawnedBuildings(spawned_buildings);
	foreach (ref VPPToEditorSpawnedBuilding building: spawned_buildings) {
		string name = building.GetName();
		TStringArray name_split = new TStringArray();
		name.Split("-", name_split);
		data.InsertEditorData(EditorObjectData.Create(name_split.Get(0), building.GetPosition(), building.GetOrientation(), EditorObjectFlags.ALL));
	}
	
	return FileDialogResult.SUCCESS;
}

static FileDialogResult ExportVPPData(ref EditorObjectDataSet data, string filename, string set_name = "DayZEditor Export")
{
	Cerealizer file = new Cerealizer();
	
	ref VPPToEditorBuildingSet bSet = new VPPToEditorBuildingSet(set_name);
	
	foreach (EditorObjectData object_data: data) {
		bSet.AddBuilding(object_data.Type, object_data.Position, object_data.Orientation, true);
		bSet.SetActive(true);
	}
	
	
	if (file.Open(filename, FileMode.APPEND)) {
		file.Write(bSet);
		file.Close();	
	}
	
	return FileDialogResult.SUCCESS;
}

static FileDialogResult ExportExpansionData(ref EditorObjectDataSet data, string filename)
{
	FileHandle handle = OpenFile(filename, FileMode.WRITE);
	
	if (!handle) {
		return FileDialogResult.IN_USE;
	}
	
	foreach (EditorObjectData editor_object: data) {
		// Land_Construction_House2|13108.842773 10.015385 6931.083984|-101.999985 0.000000 0.000000
		string line = string.Format("%1|%2 %3 %4|%5 %6 %7", editor_object.Type, editor_object.Position[0], editor_object.Position[1], editor_object.Position[2], editor_object.Orientation[0], editor_object.Orientation[1], editor_object.Orientation[2]);
		FPrintln(handle, line);
	}
	
	return FileDialogResult.SUCCESS;
}

static FileDialogResult ExportCOMData(ref EditorObjectDataSet data, string filename)
{
	FileHandle handle = OpenFile(filename, FileMode.WRITE);
	
	if (!handle) {
		return FileDialogResult.IN_USE;
	}
	
	foreach (EditorObjectData editor_object: data) {
		// SpawnObject("Land_Construction_House2", "6638.935547 7.190318 6076.024414", "146.000015 0.000000 0.000000")
		string line = string.Format("SpawnObject(\"%1\", \"%2\", \"%3\");", editor_object.Type, editor_object.Position.ToString(false), editor_object.Orientation.ToString(false));
		FPrintln(handle, line);
	}
	
	return FileDialogResult.SUCCESS;
}

static FileDialogResult ExportTBData(ref EditorObjectDataSet data, string filename, ExportSettings export_settings)
{
	FileHandle handle = OpenFile(filename, FileMode.WRITE);
	
	if (!handle) {
		return FileDialogResult.IN_USE;
	}
	
	vector terrainbuilder_offset = Vector(200000, 0, 0);
	foreach (EditorObjectData editor_object: data) {
		// "construction_house2";206638.935547;6076.024414;146.000015;0.000000;0.000000;1.000000;
		// Name, X, Y, Yaw, Pitch, Roll, Scale, Relative Height										
		//if (height_type == HeightType.RELATIVE)
		//position[1] = position[1] - GetGame().SurfaceY(position[0], position[2]);

		vector position = editor_object.Position + terrainbuilder_offset;
		vector orientation = editor_object.Orientation;
		string line = string.Format("\"%1\";%2;%3;%4;%5;%6;%7;%8;", GetGame().GetModelName(editor_object.Type), position[0], position[2], orientation[0], orientation[1], orientation[2], editor_object.Scale, position[1]);
		FPrintln(handle, line);
	}
	
	return FileDialogResult.SUCCESS;
}

class ExpansionImportData
{

	static void ReadFromFile(out ref EditorObjectDataSet data, string filename)
	{
		FileHandle handler = OpenFile(filename, FileMode.READ);
		
		string name;
		vector position, rotation;
		while (GetObjectFromFile(handler, name, position, rotation)) {	
			
			string model_name = GetGame().GetModelName(name);
			if (model_name == "UNKNOWN_P3D_FILE") {
				EditorLog.Warning("%1 is not a valid Object Type!", name);
				continue;
			}
					
			data.InsertEditorData(EditorObjectData.Create(name, position, rotation, EditorObjectFlags.OBJECTMARKER | EditorObjectFlags.LISTITEM));
		}
		
		CloseFile(handler);
	}
	
    private static bool GetObjectFromFile(FileHandle file, out string name, out vector position, out vector rotation, out string special = "false")
    {                
        string line;
        int lineSize = FGets( file, line );
        
        if ( lineSize < 1 )
            return false;
        
        ref TStringArray tokens = new TStringArray;
        line.Split( "|", tokens );

        name = tokens.Get( 0 );        
        position = tokens.Get( 1 ).ToVector();
        rotation = tokens.Get( 2 ).ToVector();    
        special = tokens.Get( 3 );

        return true;
    }
}

class COMImportData
{
	string name;
	ref array<ref Param3<string, vector, vector>> m_SceneObjects;
	
	void COMImportData()
	{
		m_SceneObjects = new array<ref Param3<string, vector, vector>>();
	}
}



class EditorSaveData
{
	bool Binarized = 1;
	string MapName = "ChernarusPlus";
	vector CameraPosition[4];
	ref EditorObjectDataSet EditorObjects = new EditorObjectDataSet();
	
	static EditorSaveData CreateFromEditor(Editor editor)
	{
		EditorSaveData save_data = new EditorSaveData();
		
		// Save world name
		save_data.MapName = GetGame().GetWorldName();
		
		// Save Camera Position
		editor.GetCamera().GetTransform(save_data.CameraPosition);
		
		// Save Objects
		EditorObjectSet placed_objects = editor.GetPlacedObjects();
		if (placed_objects) {
			foreach (EditorObject editor_object: placed_objects) {
				save_data.EditorObjects.InsertEditorData(editor_object.GetData());
			}
		}
		
		return save_data;
	}
}


class EditorFileManager
{

	static FileDialogResult Save(ref EditorSaveData data, string file_name)
	{		
		if (FileExist(file_name) && !DeleteFile(file_name)) {
			return FileDialogResult.IN_USE;
		}
		
		Cerealizer file_serializer = new Cerealizer();
		if (!file_serializer.Open(file_name, FileMode.WRITE)) {
			return FileDialogResult.IN_USE;
		}
		
		if (!file_serializer.Write(data)) {
			file_serializer.Close();
			return FileDialogResult.UNKNOWN_ERROR;
		}
		
		file_serializer.Close();
		
		return FileDialogResult.SUCCESS;
	}
	
	static FileDialogResult Open(out EditorSaveData data, string file_name)
	{
		Cerealizer file_serializer = new Cerealizer();

		if (!FileExist(file_name)) {
			return FileDialogResult.NOT_FOUND;
		}
		
		if (!file_serializer.Open(file_name, FileMode.READ)) {
			return FileDialogResult.IN_USE;
		}
		
		if (!file_serializer.Read(data)) {
			file_serializer.Close();
			return FileDialogResult.UNKNOWN_ERROR;
		}
		
		file_serializer.Close();
		
		return FileDialogResult.SUCCESS;
	}
	
	static FileDialogResult Import(out EditorSaveData data, string file_name, ImportMode mode)
	{		
		if (!FileExist(file_name)) {
			return FileDialogResult.NOT_FOUND;
		}
		
		switch (mode) {
			
			case (ImportMode.COMFILE): {
				
				COMImportData com_data = new COMImportData();
				JsonFileLoader<COMImportData>.JsonLoadFile(file_name, com_data);
				
				foreach (ref Param3<string, vector, vector> param: com_data.m_SceneObjects) {
					EditorLog.Debug("ImportFromFile::COMFILE::Import " + param.param1);					
					data.EditorObjects.InsertEditorData(EditorObjectData.Create(param.param1, param.param2, param.param3));
				}
				
				break;
			}
			
			case (ImportMode.EXPANSION): {
				EditorLog.Debug("EditorFileManager::Import::EXPANSION");
				ExpansionImportData.ReadFromFile(data.EditorObjects, file_name);
				break;
			}
			
			case (ImportMode.VPP): {
				EditorLog.Debug("EditorFileManager::Import::VPP");
				return ImportVPPData(data.EditorObjects, file_name);
			}
			
			default: {
				
				EditorLog.Debug(string.Format("%1 not implemented!", typename.EnumToString(ImportMode, mode)));
				break;
			}
		}

		
		return FileDialogResult.SUCCESS;
		
	}
	
	static FileDialogResult Export(ref EditorSaveData data, string file_name, ref ExportSettings export_settings)
	{
		Print("EditorFileManager::Export");		
		DeleteFile(file_name);
		
		switch (export_settings.ExportFileMode) {
			
			case ExportMode.TERRAINBUILDER: {
				return ExportTBData(data.EditorObjects, file_name, export_settings);
			}
			
			case ExportMode.EXPANSION: {
				return ExportExpansionData(data.EditorObjects, file_name);
			}			
			
			case ExportMode.COMFILE: {
				return ExportCOMData(data.EditorObjects, file_name);
			}			
			
			case ExportMode.VPP: {
				return ExportVPPData(data.EditorObjects, file_name);
			}
			
			default: {
				EditorLog.Error("ExportMode not supported! " + export_settings.ExportFileMode);
				return FileDialogResult.NOT_SUPPORTED;
			}
		}
		
		return FileDialogResult.NOT_SUPPORTED;		
	}
}