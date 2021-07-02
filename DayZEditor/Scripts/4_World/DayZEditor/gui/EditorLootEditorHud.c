class EditorLootEditorHudController: ViewController
{
	
}

class EditorLootEditorHud: ScriptViewTemplate<EditorLootEditorHudController>
{
	override string GetLayoutRoot()
	{
		return "DayZEditor/GUI/layouts/looteditor/LootEditorHud.layout";
	}
}

ref EditorLootEditorHud EditorLootEditorHudTest;