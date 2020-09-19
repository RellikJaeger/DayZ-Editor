
class EditorListItem: MVCLayout
{
	private int m_NestIndex;
	
	EditorListItemController GetListItemController() {
		return EditorListItemController.Cast(GetController());
	}
	
	static int COLOR_ON_SELECTED = COLOR_BLUE;
	static int COLOR_ON_DESELECTED = ARGB(140,35,35,35);
	static int COLOR_ON_HOVER = COLOR_SALMON;
	
	protected Widget EditorListItemHighlight;
	protected Widget EditorListItemContent;
	protected TextWidget EditorListItemLabel;
	protected ImageWidget EditorListItemIcon;
	protected WrapSpacerWidget EditorListItemChildren;
	protected ButtonWidget EditorListItemButton;
	protected ButtonWidget EditorListItemCollapse;
	
	void EditorListItem(Widget parent = null) 
	{ 
		EditorLog.Trace("EditorListItem");
		//GetListItemController().SetListItem(this);
		m_LayoutRoot.SetUserData(this);
	}
	
	void ~EditorListItem() 
	{ 
		EditorLog.Trace("~EditorListItem"); 
		m_LayoutRoot.Unlink();
	}
	
	void SetNestIndex(int index)
	{
		EditorLog.Trace("EditorListItem::SetNestIndex " + index);
		m_NestIndex = index;
		float x, y;
		Widget frame = m_LayoutRoot.FindAnyWidget("EditorListItemFrame");
		frame.GetSize(x, y);
		frame.SetSize(290 - 15 * m_NestIndex, y);
	}
	
	int GetNestIndex() {
		return m_NestIndex;
	}
		
	void OnMouseEnter(int x, int y) 
	{
		/*if (w != EditorListItemButton) 
			return false;*/
		EditorListItemContent.SetColor(COLOR_ON_HOVER);
	}
	
	void OnMouseLeave(int x, int y)
	{
		//if (w != EditorListItemButton) return false;
		if (!m_Selected)
			EditorListItemContent.SetColor(COLOR_ON_DESELECTED);
	}
	
	void ListItemExecute(ButtonCommandArgs args);
	void ListItemVisibleExecute(ButtonCommandArgs args);
	void ListItemCollapseExecute(ButtonCommandArgs args);
	
	private bool m_Selected;
	void Select() 
	{
		//EditorLog.Trace("EditorListItemController::Select");
		m_Selected = true;
		EditorListItemContent.SetColor(COLOR_ON_SELECTED);
		EditorListItemContent.Update();	
	}
	
	void Deselect() 
	{	
		//EditorLog.Trace("EditorListItemController::Deselect");
		m_Selected = false;
		EditorListItemContent.SetColor(COLOR_ON_DESELECTED);
		EditorListItemContent.Update();	
	}
	
	bool IsSelected() {
		return m_Selected;
	}
	
	override string GetLayoutFile() {
		return "DayZEditor/gui/Layouts/items/EditorListItem.layout";
	}

	override typename GetControllerType() {
		return EditorListItemController;
	}
}
