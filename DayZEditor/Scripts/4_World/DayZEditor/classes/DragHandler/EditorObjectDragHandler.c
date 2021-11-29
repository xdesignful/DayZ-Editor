class EditorObjectDragHandler: EditorDragHandler
{
	protected float m_LastAngle;
	
	override void OnDragging(out vector transform[4], notnull EditorObject target)
	{
		// Welcome to vector copying
		vector original_position[4] = {
			transform[0].ToString(false).ToVector(),
			transform[1].ToString(false).ToVector(),
			transform[2].ToString(false).ToVector(),
			transform[3].ToString(false).ToVector(),
		};
		
		m_Editor.ObjectInHand = target;
		
		vector cursor_pos = Editor.CurrentMousePosition;
		vector size, ground_position, surface_normal, local_dir, local_ori;
		vector deltapos = m_EditorObject.GetPosition();
		size = m_EditorObject.GetSize();
		float scale = m_EditorObject.GetScale();
		ground_position = GetGroundPosition(transform);
		surface_normal = GetGame().SurfaceGetNormal(ground_position[0], ground_position[2]);
		float angle;
		int i;
		
		// handle plane lock
		if (m_Editor.PlaneLockToggle) {
			switch (m_Editor.PlaneLockMode) {
				case EditorPlaneLockMode.AXIS_X: {
					cursor_pos[1] = transform[3][1];
					cursor_pos[2] = transform[3][2];
					break;
				}
				
				case EditorPlaneLockMode.AXIS_Y: {		
					cursor_pos = GetGame().GetCurrentCameraPosition() + GetGame().GetPointerDirection() * vector.Distance(GetGame().GetCurrentCameraPosition(), m_EditorObject.GetBottomCenter());	
					cursor_pos[1] = cursor_pos[1] + size[1] / 2;
					transform[3][1] = cursor_pos[1];
					break;
				}
				
				case EditorPlaneLockMode.AXIS_Z: {
					cursor_pos[0] = transform[3][0];
					cursor_pos[1] = transform[3][1];
					break;
				}
				
				case EditorPlaneLockMode.PLANE_XY: {
					transform[3][2] = original_position[3][2];
					break;
				}				
				
				case EditorPlaneLockMode.PLANE_XZ: {
					transform[3][1] = original_position[3][1];
					break;
				}				
				
				case EditorPlaneLockMode.PLANE_YZ: {
					transform[3][0] = original_position[3][0];
					break;
				}
			}
		}
		
		// Handle Z-Only motion
		// Todo will people want this as a keybind?
		if (KeyState(KeyCode.KC_LMENU)) {
			cursor_pos = GetGame().GetCurrentCameraPosition() + GetGame().GetPointerDirection() * vector.Distance(GetGame().GetCurrentCameraPosition(), m_EditorObject.GetBottomCenter());
			cursor_pos[1] = cursor_pos[1] + size[1] / 2;
			if (m_Editor.MagnetMode) {
				transform[3] = ground_position + transform[1] * vector.Distance(ground_position, cursor_pos + GetGame().GetCurrentCameraDirection() * 1);
			} else {
				transform[3][1] = cursor_pos[1];
			}
		}
		
		// Handle XY Rotation
		else if (KeyState(KeyCode.KC_LSHIFT)) {
			vector cursor_delta = ground_position - Editor.CurrentMousePosition;
			local_ori = m_EditorObject.GetOrientation();
			angle = Math.Atan2(cursor_delta[0], cursor_delta[2]);
			local_ori[0] = local_ori[0] + ((angle - m_LastAngle) * Math.RAD2DEG);
			local_ori.RotationMatrixFromAngles(transform);
			
			for (i = 0; i < 3; i++) {
				transform[i] = transform[i] * scale;
			}
			
			if (m_Editor.MagnetMode) {
				local_dir = vector.Direction(ground_position, cursor_pos);
				local_dir.Normalize();
				transform[0] = surface_normal * local_dir;
				transform[1] = surface_normal;
				transform[2] = surface_normal * (local_dir * vector.Up);
				
			} else {
				if (m_Editor.GroundMode) {
					transform[3] = ground_position + transform[1] * vector.Distance(ground_position, transform[3]);
				}
			}
		}
		
		// Handle regular motion
		else {
			if (m_Editor.MagnetMode) {
				local_ori = m_EditorObject.GetWorldObject().GetDirection();
				transform[0] = surface_normal * local_ori;
				transform[1] = surface_normal;
				transform[2] = surface_normal * (local_ori * vector.Up);
			}
			
			if (m_Editor.GroundMode) {
				if (m_Editor.MagnetMode) {
					transform[3] = cursor_pos + surface_normal * vector.Distance(ground_position, transform[3]);				
				} else {
					transform[3] = cursor_pos + transform[1] * vector.Distance(ground_position, transform[3]);
				}
				
			} else {
				transform[3] = cursor_pos;
				transform[3][1] = transform[3][1] + m_EditorObject.GetYDistance();
			} 			
		}
		
		deltapos = transform[3] - deltapos;
		
		// Handle all child objects
		EditorObjectMap selected_objects = GetEditor().GetSelectedObjects();	
		foreach (EditorObject selected_object: selected_objects) {
			if (selected_object == target) { 
				continue; 
			}
			
			vector selected_pos = selected_object.GetPosition();
			vector selected_ori = selected_object.GetOrientation();
			
			// Handle Z-Only motion
			if (KeyState(KeyCode.KC_LMENU)) {
				selected_object.SetPosition(selected_object.GetPosition() + deltapos);
			}
			
			else if (KeyState(KeyCode.KC_LSHIFT)) {
				if (angle - m_LastAngle == 0) {
					continue;
				}
				
				selected_object.SetPosition(EditorMath.RotateAroundPoint(transform[3], selected_object.GetPosition(), vector.Up, Math.Cos(angle - m_LastAngle), Math.Sin(angle - m_LastAngle)));
				vector new_ori = selected_object.GetOrientation();
				new_ori[0] = new_ori[0] + ((angle - m_LastAngle) * Math.RAD2DEG);
				selected_object.SetOrientation(new_ori);
			} else {
				selected_object.SetPosition(selected_object.GetPosition() + deltapos);
			}
		}
		
		m_LastAngle = angle;
	}
	
	vector GetAveragePosition(EditorObjectMap objects)
	{
		vector avg_position;
		foreach (int id, EditorObject copy_object: objects) {
			avg_position += copy_object.GetPosition();
		}
		
		for (int i = 0; i < 3; i++) {
			avg_position[i] = avg_position[i] / objects.Count();
		}
		
		avg_position[1] = GetGame().SurfaceY(avg_position[0], avg_position[2]);
		
		return avg_position;
	}
}