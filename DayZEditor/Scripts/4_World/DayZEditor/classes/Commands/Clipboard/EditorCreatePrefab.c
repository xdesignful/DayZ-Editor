class EditorCreatePrefabCommand: EditorCommand
{
	protected override void Call(Class sender, CommandArgs args) 
	{
		m_Editor.Undo();
	}
}