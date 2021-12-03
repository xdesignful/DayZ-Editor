class EditorPlaneLockCommand: EditorCommand
{	
	protected override bool Execute(Class sender, CommandArgs args)
	{
		super.Execute(sender, args);
		ButtonCommandArgs button_args = ButtonCommandArgs.Cast(args);
		if (!button_args || button_args.Source.GetName() != "PlaneLockButton") {
			m_Editor.PlaneLockToggle = !m_Editor.PlaneLockToggle;
			m_Editor.GetEditorHud().GetTemplateController().GetToolbarController().NotifyPropertyChanged("m_Editor.PlaneLockToggle");
		}
		
		return true;
	}
	
	override string GetName() 
	{
		return "Plane Lock";
	}

	override ShortcutKeys GetShortcut() 
	{
		return { KeyCode.KC_LCONTROL, KeyCode.KC_6 };
	}
}