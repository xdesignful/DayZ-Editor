class EditorPrefab
{
	string Name;
	ref array<ref EditorObjectData> EditorObjects = {};
	
	void EditorPrefab()
	{
	}
	
	void ~EditorPrefab()
	{
		delete EditorObjects;
	}

	void SetName(string name)
	{
		Name = name;
	}
	
	void SetData(EditorObjectMap objects)
	{		
		if (objects.Count() == 0) {
			return;
		}		
		
		EditorObjects.Clear();
		
		vector avg_position;
		foreach (int id, EditorObject copy_object: objects) {
			avg_position += copy_object.GetPosition();
		}
		
		for (int i = 0; i < 3; i++) {
			avg_position[i] = avg_position[i] / objects.Count();
		}
		
		avg_position[1] = GetGame().SurfaceY(avg_position[0], avg_position[2]);
		
		foreach (int idx, EditorObject selected_object: objects) {
 			EditorObjectData data = EditorObjectData.Create(selected_object.GetType(), selected_object.GetPosition(), selected_object.GetOrientation(), selected_object.GetScale(), selected_object.GetFlags());
			data.Position = selected_object.GetPosition() - avg_position;
			data.Orientation = selected_object.GetOrientation();
			data.Flags = selected_object.GetFlags();
			data.BottomCenter = selected_object.GetBottomCenter();
			EditorObjects.Insert(data);
		}
	}
	
	static void SaveToFile(string file, EditorPrefab data)
	{
		JsonFileLoader<EditorPrefab>.JsonSaveFile(file, data);
	}
	
	static EditorPrefab LoadFromFile(string file)
	{
		EditorPrefab data();
		JsonFileLoader<EditorPrefab>.JsonSaveFile(file, data);
		return data;
	}
}