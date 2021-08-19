class EditorHud: ScriptViewTemplate<EditorHudController>
{
	protected bool m_IsBoxSelectActive;
	
	// Layout Elements
	Widget NotificationFrame;
	Widget MapContainer;
	Widget LoggerFrame;
	
	CanvasWidget EditorCanvas;
	// for camera drawing i.e. rule of 3rds
	CanvasWidget EditorCameraCanvas;
	
	// ruler draws to this
	CanvasWidget EditorRulerCanvas;
	TextWidget EditorRulerReadout;
	
	ref EditorCameraMapMarker CameraMapMarker;
	
	// todo protect this and move all Map logic in here?
	MapWidget EditorMapWidget;
	
	EditBoxWidget LeftbarSearchBar;
	
	void EditorHud()
	{	
		EditorLog.Trace("EditorHud");
		EditorMapWidget.Show(false);
		
		ShowScreenLogs(GetEditor().Settings.ShowScreenLogs);
	}
	
	void ~EditorHud()
	{
		delete CameraMapMarker;
	}

	void Show(bool show) 
	{
		EditorLog.Trace("EditorHud::Show");
		if (m_LayoutRoot) {
			m_LayoutRoot.Show(show);
		}
		
		if (CurrentDialog) {
			CurrentDialog.GetLayoutRoot().Show(show);
		}
	}
	
	bool IsVisible() 
	{
		return m_LayoutRoot.IsVisible();
	}
	
	void ToggleCursor() 
	{		
		ShowCursor(!GetGame().GetUIManager().IsCursorVisible());
	}
	
	void ShowCursor(bool state) 
	{
		GetGame().GetUIManager().ShowCursor(state);
		
		if (!state) {
			delete CurrentTooltip;
			delete CurrentMenu;
			SetFocus(null);
		}
	}
		
	void ShowScreenLogs(bool state)
	{
		LoggerFrame.Show(state);
	}
		
	void CreateNotification(string text, int color = -4301218, float duration = 4)
	{
		EditorLog.Trace("EditorHud::CreateNotification");
		
		EditorNotification notification = new EditorNotification(text, color);
		notification.SetParent(NotificationFrame);
		notification.Play(duration);
	}
	
	bool IsMapVisible()
	{
		return EditorMapWidget.IsVisible();
	}
	
	bool IsSelectionBoxActive()
	{
		return m_IsBoxSelectActive;
	}
	
	void DelayedDragBoxCheck()
	{
		if (!IsVisible()) return;
		
		int x, y;
		x += 6;
		GetMousePos(x, y);
		thread _DelayedDragBoxCheck(x, y);
	}
	
	void ScrollToListItem(EditorListItem list_item)
	{
		
		//VScrollToWidget(list_item.GetLayoutRoot());
	}
		
	private void _DelayedDragBoxCheck(int start_x, int start_y)
	{
		int drag_box_color = GetEditor().Settings.SelectionColor;
		
		int a, r, g, b;
		InverseARGB(drag_box_color, a, r, g, b);
		int drag_box_color_fill = ARGB(50, r, g, b);			
		
		int current_x, current_y;
		while ((GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) && GetGame().GetInput().HasGameFocus()) {			
			m_IsBoxSelectActive = true;
			GetMousePos(current_x, current_y);
			// @Sumrak :ANGERY:
			current_x += 6;
			
			EditorCanvas.Clear();
			
			// Draw Drag Box
			if (Math.AbsInt(start_x - current_x) > DRAG_BOX_THRESHOLD || Math.AbsInt(start_y - current_y) > DRAG_BOX_THRESHOLD) {
				EditorCanvas.DrawLine(start_x, start_y, current_x, start_y, DRAG_BOX_THICKNESS, drag_box_color);
				EditorCanvas.DrawLine(start_x, start_y, start_x, current_y, DRAG_BOX_THICKNESS, drag_box_color);
				EditorCanvas.DrawLine(start_x, current_y, current_x, current_y, DRAG_BOX_THICKNESS, drag_box_color);
				EditorCanvas.DrawLine(current_x, start_y, current_x, current_y, DRAG_BOX_THICKNESS, drag_box_color);
				
				// Handles the fill operation
				int x_avg = (start_x + current_x) / 2;
				EditorCanvas.DrawLine(x_avg, start_y, x_avg, current_y, current_x - start_x, drag_box_color_fill);
							
				EditorObjectMap placed_objects = g_Editor.GetPlacedObjects();
				foreach (EditorObject editor_object: placed_objects) {
					
					if (editor_object.IsSelected()) continue;
					
					float marker_x, marker_y;
					EditorObjectMarker object_marker = editor_object.GetMarker();
					if (object_marker) {
						object_marker.GetPos(marker_x, marker_y);
						
						if ((marker_x < Math.Max(start_x, current_x) && marker_x > Math.Min(start_x, current_x)) && (marker_y < Math.Max(start_y, current_y) && marker_y > Math.Min(start_y, current_y))) {		
							g_Editor.SelectObject(editor_object);
						}
					}
				}		
			}
			
			Sleep(10);
		}
		
		m_IsBoxSelectActive = false;
		EditorCanvas.Clear();
	}
	
	void RunRuler()
	{
		thread _RunRuler();
	}
	
	private void _RunRuler()
	{
		Sleep(100);
		
		vector p1, p2;
		while (GetEditor().Ruler) {
			EditorRulerCanvas.Clear();
			EditorRulerReadout.Show(false);
			
			set<Object> collisions = new set<Object>();
			vector current_pos = MousePosToRay(collisions, null, 20000, 0, true);
			if (GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK) {
				p1 = current_pos;
			}
			
			if (GetMouseState(MouseState.RIGHT) & MB_PRESSED_MASK) {
				p2 = current_pos;
			}
						
			if (!p1 && !p2) {
				Sleep(10);
				continue;
			}
			
			vector s1 = GetGame().GetScreenPos(p1);
			vector s2 = GetGame().GetScreenPos(p2);
			vector s3 = GetGame().GetScreenPos(current_pos);
			
			//if (p1_assigned ^ p2_assigned)
			if (p1 != vector.Zero && p2 != vector.Zero) {
				
				// Draw static final line
				EditorRulerCanvas.DrawLine(s1[0], s1[1], s2[0], s2[1], 2, COLOR_RED);
				EditorRulerReadout.Show(true);
				// fuck mvc this is patrick
				EditorRulerReadout.SetText(vector.Distance(p1, p2).ToString());
				
				vector center_pos = AverageVectors(p1, p2);
				// center_pos.Perpend(); or something
				
				center_pos = GetGame().GetScreenPos(center_pos);
				
				EditorRulerReadout.SetPos(center_pos[0], center_pos[1]);
			}
			
			else if (p1 != vector.Zero || p2 != vector.Zero && !(p1 != vector.Zero && p2 != vector.Zero)) {
				if (p1) {
					EditorRulerCanvas.DrawLine(s3[0], s3[1], s1[0], s1[1], 2, COLOR_RED);
				} 
				
				if (p2) {
					EditorRulerCanvas.DrawLine(s3[0], s3[1], s2[0], s2[1], 2, COLOR_RED);
				}
			} 
			
			Sleep(10);
		}
		
		EditorRulerReadout.Show(false);
		EditorRulerCanvas.Clear();
	}
	
	void ShowRuleOfThirds(bool state)
	{
		if (!state) {
			EditorCameraCanvas.Clear();
			return;
		}
		
		int x, y;

		GetScreenSize(x, y);				
		EditorCameraCanvas.DrawLine(x / 3, 0, x / 3, y, 1, COLOR_BLACK);
		EditorCameraCanvas.DrawLine((x / 3) * 2, 0, (x / 3) * 2, y, 1, COLOR_BLACK);
		
		EditorCameraCanvas.DrawLine(0, y / 3, x, y / 3, 1, COLOR_BLACK);
		EditorCameraCanvas.DrawLine(0, (y / 3) * 2, x, (y / 3) * 2, 1, COLOR_BLACK);
	}
	
	override string GetLayoutFile() 
	{
		return "DayZEditor/gui/layouts/hud/EditorHud.layout";
	}
	
	// Modal Menu Control
	static ref EditorMenu CurrentMenu;
	
	// ToolTip Control
	protected ref ScriptView CurrentTooltip;
	void SetCurrentTooltip(ScriptView current_tooltip) 
	{
		delete CurrentTooltip;
		
		// Dont create a tooltip if conditions are met
		if (IsSelectionBoxActive()) {
			return;
		}
		
		CurrentTooltip = current_tooltip;
	}
		
	// Dialog Control
	static ref DialogBase CurrentDialog;
	
	static bool IsDialogCommand(Widget w) 
	{
		return (CurrentDialog && CurrentDialog.GetLayoutRoot() && CurrentDialog.GetLayoutRoot().FindAnyWidget(w.GetName()));
	}
	
	protected ref map<typename, vector> m_LastDialogPosition = new map<typename, vector>();
	
	void RegisterLastDialogPosition(ScriptView dialog)
	{
		float x, y;
		dialog.GetLayoutRoot().GetPos(x, y);
		m_LastDialogPosition[dialog.Type()] = Vector(x, y, 0);
	}
	
	vector GetLastDialogPosition(ScriptView dialog)
	{
		return m_LastDialogPosition[dialog.Type()];
	}
}
