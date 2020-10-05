
// on refactor, editor object in constructor
class EditorPlacedListItem: EditorListItem
{
	protected EditorObject m_EditorObject;
	EditorObject GetData() { 
		return m_EditorObject; 
	}
	
	void SetEditorObject(EditorObject data) 
	{ 
		EditorLog.Trace("EditorPlacedListItem::SetEditorObject"); 
		m_EditorObject = data;
		
		m_TemplateController.Label = m_EditorObject.GetDisplayName();
		m_TemplateController.NotifyPropertyChanged("Label");
		
		//m_TemplateController.ListItemIcon = GetIconFromMod(m_EditorObject.GetData().ObjectMod);
		//m_TemplateController.NotifyPropertyChanged("ListItemIcon");
				
		m_EditorObject.OnObjectSelected.Insert(EditorObjectSelected);
		m_EditorObject.OnObjectDeselected.Insert(EditorObjectDeselected);	
	}
	
	
	void EditorObjectSelected(EditorObject data) {
		Select();
	}
	
	void EditorObjectDeselected(EditorObject data) {
		Deselect();
	}
	
	override bool IsSelected() {
		return m_EditorObject.IsSelected();
	}
	
	bool ListItemExecute(ButtonCommandArgs args)
	{
		switch (args.GetMouseButton()) {
			
			case 0: {
				// We want to Toggle selection if you are holding control
				if (KeyState(KeyCode.KC_LCONTROL)) {
					GetEditor().ToggleSelection(m_EditorObject);
					return true;
				} 
				
				if (!KeyState(KeyCode.KC_LSHIFT)) {
					GetEditor().ClearSelection();
				}
				
				GetEditor().SelectObject(m_EditorObject);
				
				
				break;
			} 
			
			case 1: {
				int x, y;
				GetMousePos(x, y);
				EditorPlacedContextMenu context_menu = new EditorPlacedContextMenu(x, y);
				break;
			}
		}
		
		return true;
	}
	
	bool ListItemVisibleExecute(ButtonCommandArgs args)
	{
		switch (args.GetMouseButton()) {
			
			case 0: {
				m_EditorObject.ShowWorldObject(args.GetButtonState());
				break;
			}
			
		}
		
		return true;
	}
}
