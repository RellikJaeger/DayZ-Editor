class TypeConversionEditorWidget: TypeConversionTemplate<EditorWidget>
{	
	override Widget GetWidget() 
	{
		return m_Value.GetLayoutRoot();
	}
	
	override void SetWidget(Widget value) 
	{
		m_Value.SetLayoutRoot(value);
	}	
}

class TypeConversionBrush: TypeConversionTemplate<EditorBrushData>
{	
	override void SetString(string value) 
	{
		m_Value.Name = value;
	}
	
	override string GetString() 
	{
		return m_Value.Name;
	}
}

class TypeConversionBrushObject: TypeConversionTemplate<EditorBrushObject>
{
	override void SetString(string value) 
	{
		m_Value.Name = value;
	}
	
	override string GetString() 
	{
		return m_Value.Name;
	}
	
	override Widget GetWidget() 
	{
		return GetGame().GetWorkspace().CreateWidgets("DayZEditor/gui/Layouts/options/EditorDialogOptionEditBrush.layout");
	}
}

class TypeConversionEditorFile: TypeConversionTemplate<EditorFile>
{
	override void SetString(string value) 
	{
		if (m_Value) {
			m_Value.FileName = value;
		}
	}
	
	override string GetString() 
	{
		if (m_Value) {
			return m_Value.FileName;
		}
		
		return string.Empty;
	}
}

class DropdownListPrefabItemConverter: TypeConversionTemplate<DropdownListPrefabItemBase>
{
	override string GetString() 
	{
		if (m_Value) {
			return m_Value.GetText();
		}
		
		return string.Empty;
	}
	
	override Widget GetWidget() 
	{
		// Todo: why can this be null? not sure
		if (m_Value) {
			return m_Value.GetLayoutRoot();
		}
		
		return null;
	}
	
	override void SetWidget(Widget value) 
	{
		m_Value.OnWidgetScriptInit(value);
	}
}