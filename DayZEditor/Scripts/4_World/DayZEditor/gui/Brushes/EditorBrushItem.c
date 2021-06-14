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
		
		GetTemplateController().Type = m_Data.Name;
		GetTemplateController().Frequency = m_Data.Frequency;
		GetTemplateController().ZOffset = m_Data.ZOffset;
		GetTemplateController().NotifyPropertyChanged();
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
	
		GetTemplateController().Name = m_Data.Name;
		
		if (m_Data.BrushClassName) {
			GetTemplateController().IsClassType = true;
			GetTemplateController().ClassType = m_Data.BrushClassName.ToString();
		}
		
		foreach (EditorBrushObject brush_object: m_Data.PlaceableObjectTypes) {
			GetTemplateController().BrushObjects.Insert(new EditorBrushListItemObject(brush_object));
		}
		
		GetTemplateController().NotifyPropertyChanged();
	}
	
	void BrushAddChildObject(ButtonCommandArgs args)
	{
		GetTemplateController().BrushObjects.Insert(new EditorBrushListItemObject(new EditorBrushObject("type_goes_here", 1.0, 0.0)));
	}
	
	override string GetLayoutFile()
	{
		return "DayZEditor/GUI/layouts/brush/EditorBrushItem.layout";
	}
}