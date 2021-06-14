class EditorBrushListItemObjectController: Controller
{
	string Type;
	float Frequency;
	float ZOffset;
}

class EditorBrushListItemObject: ScriptViewTemplate<EditorBrushListItemObjectController>
{
	protected EditorBrushObject m_Data;
	
	void EditorBrushListItemObject(EditorBrushObject data)
	{
		m_Data = data;
		
		m_TemplateController.Type = m_Data.Name;
		m_TemplateController.Frequency = m_Data.Frequency;
		m_TemplateController.ZOffset = m_Data.ZOffset;
		m_TemplateController.NotifyPropertyChanged();
	}
	
	void BrushDeleteChildObject(ButtonCommandArgs args)
	{
		delete this;
	}
	
	override string GetLayoutFile()
	{
		return "DayZEditor/GUI/layouts/brush/EditorBrushChildItem.layout";
	}
}

class EditorBrushListItemController: Controller
{
	string Name;
	
	// Only valid if IsClassType is true
	bool IsClassType;
	string ClassType;
	
	ref ObservableCollection<ref EditorBrushListItemObject> BrushObjects = new ObservableCollection<ref EditorBrushListItemObject>(this);
}

class EditorBrushListItem: ScriptViewTemplate<EditorBrushListItemController>
{
	protected EditorBrushData m_Data;
	
	void EditorBrushListItem(EditorBrushData data)
	{
		m_Data = data;
	
		m_TemplateController.Name = m_Data.Name;
		
		if (m_Data.BrushClassName) {
			m_TemplateController.IsClassType = true;
			m_TemplateController.ClassType = m_Data.BrushClassName.ToString();
		}
		
		foreach (EditorBrushObject brush_object: m_Data.PlaceableObjectTypes) {
			m_TemplateController.BrushObjects.Insert(new EditorBrushListItemObject(brush_object));
		}
		
		m_TemplateController.NotifyPropertyChanged();
	}
	
	void BrushAddChildObject(ButtonCommandArgs args)
	{
		m_TemplateController.BrushObjects.Insert(new EditorBrushListItemObject(new EditorBrushObject("type_goes_here", 1.0, 0.0)));
	}
	
	void SetName(string name)
	{
		m_TemplateController.Name = name;
		m_TemplateController.NotifyPropertyChanged("Name");
	}
	
	string GetName()
	{
		return m_TemplateController.Name;
	}
	
	override string GetLayoutFile()
	{
		return "DayZEditor/GUI/layouts/brush/EditorBrushItem.layout";
	}
}