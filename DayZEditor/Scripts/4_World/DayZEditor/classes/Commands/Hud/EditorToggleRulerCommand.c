class EditorToggleRulerCommand: EditorCommand
{
	protected override bool Execute(Class sender, CommandArgs args)
	{
		super.Execute(sender, args);
		
		ButtonCommandArgs button_args = ButtonCommandArgs.Cast(args);
		if (!button_args || button_args.Source.GetName() != "RulerButton") {
			m_Editor.Ruler = !m_Editor.Ruler;
			m_Editor.GetEditorHud().GetTemplateController().GetToolbarController().NotifyPropertyChanged("m_Editor.Ruler");
		}
		
		return true;
	}
	
	override string GetName() 
	{
		return "Toggle Ruler";
	}
	
	override ShortcutKeys GetShortcut()
	{
		return { KeyCode.KC_LCONTROL, KeyCode.KC_6 };
	}
}