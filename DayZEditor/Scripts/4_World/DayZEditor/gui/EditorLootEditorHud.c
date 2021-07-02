class EditorLootEditorHudController: ViewController
{
	
}

class EditorLootEditorHud: ScriptViewTemplate<EditorLootEditorHudController>
{
	override string GetLayoutFile()
	{
		return "DayZEditor/GUI/layouts/looteditor/LootEditorHud.layout";
	}
}

ref EditorLootEditorHud EditorLootEditorHudTest;