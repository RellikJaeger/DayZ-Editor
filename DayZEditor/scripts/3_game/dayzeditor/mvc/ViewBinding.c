


class ViewBinding: ScriptedWidgetEventHandler
{
	protected Widget m_LayoutRoot;
	
	protected reference string Binding_Name;
	protected reference int Binding_Index;
	protected reference bool Two_Way_Binding;
	protected reference string Command_Execute;
	protected reference string Command_CanExecute;
	
	
	Widget GetRoot() { return m_LayoutRoot; }
	string GetBindingName() { return Binding_Name; }
	int GetBindingIndex() { return Binding_Index; }
	
	protected ref WidgetController m_WidgetController;
	protected ref TypeConverter m_PropertyDataConverter;
	protected typename m_PropertyDataType;
	protected Controller m_Controller;
	
	void ViewBinding() { /*EditorLog.Trace("ViewBinding");*/ }
	void ~ViewBinding() { /*EditorLog.Trace("~ViewBinding");*/ }
	
	void SetController(Controller controller) 
	{ 
		EditorLog.Trace("ViewBinding::SetController");
		m_Controller = controller;
		m_PropertyDataType = m_Controller.GetPropertyType(Binding_Name);
		
		if (m_PropertyDataType.IsInherited(Observable)) {
			m_PropertyDataConverter = MVC.GetTypeConversion(Observable.Cast(m_PropertyDataType.Spawn()).GetType());
		} else {
			m_PropertyDataConverter = MVC.GetTypeConversion(m_PropertyDataType);
		}
		
		// Updates the view on first load
		if (m_PropertyDataConverter) {
			OnPropertyChanged();
		} else {
			EditorLog.Warning(string.Format("[%1] Data Converter not found!", m_LayoutRoot.GetName()));
		}
	}
	
	void OnWidgetScriptInit(Widget w)
	{
		EditorLog.Trace("ViewBinding::Init");
		m_LayoutRoot = w;
		
		if (Binding_Name == string.Empty) {
			Binding_Name = m_LayoutRoot.GetName();
		}
		
		m_WidgetController = GetWidgetController(m_LayoutRoot);
		if (!m_WidgetController) {
			MVC.UnsupportedTypeError(m_LayoutRoot.Type());
		}
		
		// Check for two way binding support
		if (Two_Way_Binding && !m_WidgetController.CanTwoWayBind()) {
			MVC.ErrorDialog(string.Format("Two Way Binding for %1 is not supported!", m_LayoutRoot.Type()));
		}
		

		m_LayoutRoot.SetHandler(this);
	}
	
	void OnPropertyChanged()
	{
		EditorLog.Trace("ViewBinding::OnPropertyChanged " + Binding_Name);		
		UpdateCommand();
		
		if (!m_PropertyDataType) {
			EditorLog.Warning(string.Format("Binding not found: %1", Binding_Name));
			return;
		}
		
		if (!m_PropertyDataConverter) {
			MVC.ErrorDialog(string.Format("Could not find TypeConversion for Type %1\nUse TypeConverter.RegisterTypeConversion to register custom types", m_PropertyDataType));
			return;
		}

		m_PropertyDataConverter.GetFromController(m_Controller, Binding_Name, Binding_Index);
		EditorLog.Debug(string.Format("[%1] Updating View...", m_LayoutRoot.Type()));
		
		m_WidgetController.SetData(m_PropertyDataConverter);
	}
	
	void UpdateModel()
	{
		EditorLog.Trace("ViewBinding::UpdateModel");
		if (!Two_Way_Binding || !m_WidgetController.CanTwoWayBind()) 
			return;
		
		if (!m_PropertyDataConverter) {
			MVC.ErrorDialog(string.Format("Could not find TypeConversion for Type %1\nUse TypeConverter.RegisterTypeConversion to register custom types", m_PropertyDataType));
			return;
		}
		
		EditorLog.Debug(string.Format("[%1] Updating Model...", m_LayoutRoot.Type()));
		
		m_WidgetController.GetData(m_PropertyDataConverter);
		m_PropertyDataConverter.SetToController(m_Controller, Binding_Name, Binding_Index);
		m_Controller.NotifyPropertyChanged(Binding_Name);
	}
	
	
	void OnCollectionChanged(ref CollectionChangedEventArgs args)
	{
		EditorLog.Trace("ViewBinding::OnCollectionChanged " + Binding_Name);
		
		if (!m_PropertyDataType) {
			MVC.ErrorDialog(string.Format("Binding not found: %1", Binding_Name));
			return;
		}
		
		if (!m_PropertyDataConverter) {
			MVC.ErrorDialog(string.Format("Could not find TypeConversion for Type %1\nUse TypeConverter.RegisterTypeConversion to register custom types", m_PropertyDataType));
			return;			
		}
		
		if (!m_WidgetController) {
			MVC.UnsupportedTypeError(m_LayoutRoot.Type());
			return;
		}

		EditorLog.Debug(string.Format("[%1] Updating Collection View...", m_LayoutRoot.Type()));
			
		// Anonymouse Data Setter
		m_PropertyDataConverter.SetParam(args.param4);
		switch (args.param2) {
						
			
			case NotifyCollectionChangedAction.Add: {
				m_WidgetController.InsertData(args.param3, m_PropertyDataConverter);
				break;
			}
			
			case NotifyCollectionChangedAction.Remove: {
				m_WidgetController.RemoveData(args.param3, m_PropertyDataConverter);
				break;
			}
			
			case NotifyCollectionChangedAction.Set: {
				m_WidgetController.ReplaceData(args.param3, m_PropertyDataConverter);
				break;
			}
			
			case NotifyCollectionChangedAction.Move: {
				// this ones a weird case /shrug
				m_WidgetController.MoveData(args.param3, Param1<int>.Cast(args.param4).param1);
				break;	
			}
			
			case NotifyCollectionChangedAction.Clear: {
				m_WidgetController.ClearData();
				break;
			}
		}
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		
		EditorLog.Trace("ViewBinding::OnClick " + w.Type());
		
		switch (w.Type()) {
			case ButtonWidget: {
				UpdateModel();
				InvokeCommand(new ButtonCommandArgs(ButtonWidget.Cast(w), button, ButtonWidget.Cast(w).GetState()));
				break;
			}
			

		}
		
		return false;
	}
	
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		EditorLog.Trace("ViewBinding::OnChange");
		
		
		switch (w.Type()) {
			
			case CheckBoxWidget: {
				InvokeCommand(new CheckBoxCommandArgs(CheckBoxWidget.Cast(w), CheckBoxWidget.Cast(w).IsChecked()));
				break;
			}
			
			case XComboBoxWidget: {
				InvokeCommand(new XComboBoxCommandArgs(XComboBoxWidget.Cast(w), XComboBoxWidget.Cast(w).GetCurrentItem()));
				break;
			}
			
		}
		
		UpdateModel();
		

		return super.OnChange(w, x, y, finished);
	}
		
	
	
	private bool InvokeCommand(Param params)
	{
		if (Command_Execute == string.Empty) {
			return false;
		}
		
		EditorLog.Trace("ViewBinding::InvokeCommand");
		
		bool result;
		g_Script.CallFunction(m_Controller, Command_Execute, result, params);
		return result;
	}
	
	private void UpdateCommand()
	{
		if (Command_CanExecute == string.Empty) {
			return;
		}
		
		EditorLog.Trace("ViewBinding::UpdateCommand");
		
		bool result;
		g_Script.CallFunction(m_Controller, Command_CanExecute, result, null);
		
		if (result) {
			m_LayoutRoot.ClearFlags(WidgetFlags.IGNOREPOINTER);
		} else {
			m_LayoutRoot.SetFlags(WidgetFlags.IGNOREPOINTER);
			if (GetFocus() == m_LayoutRoot) {
				SetFocus(null);
			}
		}
	}
}


