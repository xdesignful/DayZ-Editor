class EditorDZEFile: EditorFileType
{
	protected ref FileSerializer _serializer;
	protected ref EditorSaveData _data;

	void EditorDZEFile()
	{
		_serializer = new FileSerializer();
		_data = new EditorSaveData();
	}

	protected bool OnRead(ParamsReadContext ctx, int version)
	{
		if (!ctx.Read(_data.MapName))
			return false;

		if (!ctx.Read(_data.CameraPosition))
			return false;

		if (!ctx.Read(_data.EditorObjects))
			return false;

		return true;
	}

	protected void OnWrite(ParamsReadContext ctx, int version)
	{
		ctx.Write(_data.MapName);
		ctx.Write(_data.CameraPosition);
		ctx.Write(_data.EditorObjects);
	}

	override EditorSaveData Import(string file, ImportSettings settings)
	{
		int version;

		if (!FileExist(file))
		{
			EditorLog.Error("File not found %1", file);
			return _data;
		}
		
		if (ConvertFromJson(file, _data)) // test shit
		{
			_serializer.Close();
			return _data;
		}

		if (!_serializer.Open(file, FileMode.READ))
		{
			_serializer.Close();

			EditorLog.Error("File in use %1", file);
			return _data;
		}

		if (!_serializer.Read(version))
		{
			EditorLog.Warning("File is unversioned, attempting conversion");

			//if (!ConvertFromJson(file)) // read from 4head json save
			//	_serializer.Read(_data); // read from old unversioned bin

			_serializer.Close();
			return _data;
		}
		
		

		if (!OnRead(_serializer, version))
		{
			_serializer.Close();

			EditorLog.Error("Unknown File Error %1", file);
			return _data;
		}

		_serializer.Close();
		return _data;
	}

	override void Export(EditorSaveData data, string file, ExportSettings settings)
	{
		if (_serializer.Open(file, FileMode.WRITE))
		{
			_data = data;

			int version = EDITOR_DZE_VERSION_SAVE;
			_serializer.Write(version);

			OnWrite(_serializer, version);

			_serializer.Close();
		}
	}

	private bool ConvertFromJson(string file, out EditorSaveData data)
	{
		// Temporary fix, Binarize always = 0
		JsonFileLoader<EditorSaveData>.JsonLoadFile(file, data);

		if (data.EditorObjects.Count() <= 0)
			return false;

		// bugfix to fix the id not inclementing
		EditorSaveData editor_save = new EditorSaveData();
		foreach (EditorObjectData object_data: data.EditorObjects) {
			editor_save.EditorObjects.Insert(EditorObjectData.Create(object_data.Type, object_data.Position, object_data.Orientation, object_data.Flags));
		}

		editor_save.MapName = data.MapName;
		editor_save.CameraPosition = data.CameraPosition;

		return true;
	}

	override string GetExtension() {
		return ".dze";
	}
}