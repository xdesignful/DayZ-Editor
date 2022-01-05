class EditorLocalEditorCommand: EditorCommand
{
	protected override bool Execute(Class sender, CommandArgs args)
	{
		super.Execute(sender, args);
		if (GetEditor().GetObjectManager().CurrentSelectedItem) {
			m_Editor.EditLocally(GetEditor().GetObjectManager().CurrentSelectedItem);
		}
		
		return true;
	}
	
	override string GetName() 
	{
		return "Edit Local...";
	}
}