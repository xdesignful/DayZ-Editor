class EditorBrushListItemObjectController: Controller
{
	string Type;
	float Frequency;
	float ZOffset;
}

class EditorBrushListItemObject: ScriptViewTemplate<EditorBrushListItemObjectController>
{
	
}

class EditorBrushListItemController: Controller
{
	ref ObservableCollection<ref EditorBrushListItemObject> BrushObjects = new ObservableCollection<ref EditorBrushListItemObject>(this);
}

class EditorBrushListItem: ScriptViewTemplate<EditorBrushListItemController>
{
	
}