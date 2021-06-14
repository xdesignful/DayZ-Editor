class EditorBrushListItemObjectController: Controller
{
	string Type;
	float Frequency;
	float ZOffset;
}

class EditorBrushListItemObject: ScriptViewTemplate<EditorBrushListItemObjectController>
{
	override string GetLayoutFile()
	{
		return "DayZEditor/GUI/layouts/brush/EditorBrushChildItem.layout";
	}
}

class EditorBrushListItemController: Controller
{
	ref ObservableCollection<ref EditorBrushListItemObject> BrushObjects = new ObservableCollection<ref EditorBrushListItemObject>(this);
}

class EditorBrushListItem: ScriptViewTemplate<EditorBrushListItemController>
{
	override string GetLayoutFile()
	{
		return "DayZEditor/GUI/layouts/brush/EditorBrushItem.layout";
	}
}