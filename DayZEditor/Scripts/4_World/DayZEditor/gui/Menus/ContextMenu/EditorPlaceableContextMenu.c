class EditorPlaceableContextMenu: EditorContextMenu
{
	void EditorPlaceableContextMenu(float x, float y)
	{
		EditorLog.Trace("EditorPlacedContextMenu");
		AddMenuButton(m_Editor.CommandManager.AddToFavoritesCommand);
		AddMenuButton(m_Editor.CommandManager.LootEditorCommand);
	}
}