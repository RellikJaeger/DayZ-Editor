
class EditorAction: Managed
{
	private string name;
	private bool undone = false;
	
	ref map<int, ref Param> UndoParameters = null;
	ref map<int, ref Param> RedoParameters = null;
	
	string m_UndoAction, m_RedoAction;
		
	void EditorAction(string undo_action, string redo_action)
	{
		name = undo_action;
		m_UndoAction = undo_action;
		m_RedoAction = redo_action;
		UndoParameters = new map<int, ref Param>();
		RedoParameters = new map<int, ref Param>();
	}
	
	void ~EditorAction()
	{
		Print("~EditorAction");
		
	}
	
	string GetName() { return name; }
	
	bool IsUndone() { return undone; }
	
	void CallUndo()
	{
		undone = true;
		Print("EditorAction::CallUndo");		
		foreach (int id, ref Param param: UndoParameters) {			
			GetGame().GameScript.Call(this, m_UndoAction, param);
		}
	}
	
	void CallRedo()
	{
		Print("EditorAction::CallRedo");
		undone = false;
		foreach (int id, ref Param param: RedoParameters) {
			GetGame().GameScript.Call(this, m_RedoAction, param);
		}
		
	}
	
	void InsertUndoParameter(EditorObject source, ref Param params)
	{
		Print("InsertUndoParameter");
		UndoParameters.Insert(source.GetID(), params);
	}	
	
	void InsertRedoParameter(EditorObject source, ref Param params)
	{
		Print("InsertRedoParameter");
		RedoParameters.Insert(source.GetID(), params);
	}
	
	
	void Create(Param1<int> params)
	{
		Print("EditorAction::Create");
		EditorObjectData data = GetEditor().GetObjectManager().GetSessionDataById(params.param1);
		GetEditor().GetObjectManager().SetSessionObjectById(params.param1, new EditorObject(data));
	}
	
	void Delete(Param1<int> params)
	{
		Print("EditorAction::Delete");		
		EditorObject editor_object = GetEditor().GetObjectManager().GetSessionObjectById(params.param1);
		GetEditor().GetObjectManager().SetSessionDataById(params.param1, editor_object.GetData());
		delete editor_object;
	}
	
	
	void SetTransform(Param3<int, vector, vector> params)
	{
		Print("EditorObject::SetTransform");
		EditorObject editor_object = GetEditor().GetObjectManager().GetSessionObjectById(params.param1);
		editor_object.SetPosition(params.param2);
		editor_object.SetOrientation(params.param3);
		editor_object.Update();
		
	}	
}