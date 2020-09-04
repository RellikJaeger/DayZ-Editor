
modded class MVC
{
	override void RegisterConversionTemplates(out TypeConverterHashMap type_conversions)
	{
		super.RegisterConversionTemplates(type_conversions);
		type_conversions.Insert(StringEvaluater, TypeConversionEquation);
		type_conversions.Insert(EditorWidget, TypeConversionEditorWidget);
		type_conversions.Insert(EditorBrushData, TypeConversionBrush);
		
		type_conversions.Insert(EditorPrefabEditText, TypeConversionPrefabEditText);
		type_conversions.Insert(EditorPrefabPosition, TypeConversionPrefabPosition);
		
	}
}



class TypeConversionEquation: TypeConversionTemplate<StringEvaluater>
{
	
	override void SetString(string value) {
		m_Value = string.Empty;
		for (int i = 0; i < value.Length(); i++) {
			int ascii = value[i].Hash();
			if (ascii >= 40 && ascii <= 57 || ascii == 32 || ascii == 94)
				m_Value += value[i];
		}
	}
	
	override string GetString() {
		return m_Value;
	}	
}


class EditorWidget
{
	protected Widget m_LayoutRoot;
	Widget GetLayoutRoot() { 
		return m_LayoutRoot; 
	}
	
	void SetLayoutRoot(Widget layout_root) {
		m_LayoutRoot = layout_root;
	}
}


class TypeConversionEditorWidget: TypeConversionTemplate<EditorWidget>
{
		
	override Widget GetWidget() {
		return m_Value.GetLayoutRoot();
	}
	
	override void SetWidget(Widget value) {
		m_Value.SetLayoutRoot(value);
	}
	
}


class TypeConversionBrush: TypeConversionTemplate<ref EditorBrushData>
{	
	override void SetString(string value) {
		m_Value.Name = value;
	}
	
	override string GetString() {
		return m_Value.Name;
	}
}


class TypeConversionPrefabEditText: TypeConversionTemplate<ref EditorPrefabEditText>
{
	override void SetString(string value) {
		m_Value.SetText(value);
	}
	
	override string GetString() {
		return m_Value.GetText();
	}
}

class TypeConversionPrefabPosition: TypeConversionTemplate<ref EditorPrefabPosition>
{
	
	override void SetVector(vector value) {
		m_Value.SetVector(value);
	}
	
	override vector GetVector() {
		return m_Value.GetVector();
	}
}