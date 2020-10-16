class EVRStorm: EventBase
{
	protected APSI m_APSI;
	
	private bool m_MissionWeatherState;
	
	protected autoptr ref MaterialEffect m_MatGlow;
	protected autoptr ref MaterialEffect m_MatBlur;
	protected autoptr ref MaterialEffect m_MatChroma;
	protected autoptr ref MaterialEffect m_MatColors;
		
	protected vector m_Position;
	
	protected ref array<ref AbstractWave> m_AlarmSounds = {};
	
	protected int m_WaveCount = 10; // 'Bang' count
	protected float m_TimeBetweenWaves = 1;	
	
	protected float m_BlowoutSize = 10000; // Radius in which players can be affected
	protected int m_BlowoutCount = 3;	
	
	void EVRStorm(vector position)
	{
		m_Position = position;
		
		m_MissionWeatherState = m_wObject.GetMissionWeather();
		m_wObject.MissionWeather(false);
	}
	
	void ~EVRStorm()
	{
		m_wObject.MissionWeather(m_MissionWeatherState);
	}
	
	override void EventInit()
	{
		super.EventInit();

		m_EventID = 4;
		
		m_Player = GetGame().GetPlayer();

		if (GetGame().IsServer())
		{
			m_InitPhaseLength = 60.0;
			m_MidPhaseLength = 30.0;
			m_EndPhaseLength = m_InitPhaseLength;
			m_snowTarget = 0.25;
			m_windRelMinTarget = 0.0;
			m_windRelMaxTarget = 0.0;
		}

		/*if (GetGame().IsClient())
		{

		}*/
	}
	
	void DebugInit()
	{
		thread _DebugInit();
	}
	
	private void _DebugInit()
	{
		InitPhaseClient();
		Sleep(m_InitPhaseLength * 1000);
		MidPhaseClient();
		Sleep(m_MidPhaseLength * 1000);
		EndPhaseClient();
		Sleep(m_EndPhaseLength * 1000);
		Print("Debug Client Done!");
	}
	
	override void InitPhaseClient() 
	{
		thread StartBlowoutClient();
	}
	
	override void MidPhaseClient()
	{
		thread MidBlowoutClient();
	}
	
	override void EndPhaseClient()
	{
		if (m_APSI && m_APSI.IsSwitchedOn()) {
			m_APSI.SwitchOff();
		}
		
		foreach (AbstractWave alarm: m_AlarmSounds) {
			if (alarm) {
				alarm.Stop();
			}
		}
	}
	
	private void StartBlowoutClient()
	{
		// Init stuff
		m_MatBlur = new MaterialEffect("graphics/materials/postprocess/gauss");
		m_MatGlow = new MaterialEffect("graphics/materials/postprocess/glow");
		m_MatChroma = new MaterialEffect("graphics/materials/postprocess/chromaber");
		m_MatColors = new MaterialEffect("graphics/materials/postprocess/colors");
		
		m_wObject.SetStorm(0, 1, 3000);
		m_wObject.GetFog().Set(0.5, m_InitPhaseLength, m_InitPhaseLength);
		m_wObject.GetOvercast().Set(1, m_InitPhaseLength, m_InitPhaseLength);
		
		
		//thread LerpFunction(g_Game, "SetEVValue", 0, -3, m_InitPhaseLength);				
		
		g_Game.SetEVValue(-3);
		
		float timepassed;
		while (timepassed < m_InitPhaseLength * 1000) {
			
			float pregame_phase = 1 / (m_InitPhaseLength * 1000) * timepassed;			
			float dt = 3000;
			timepassed += dt;
			float inverse_phase = Math.AbsFloat(pregame_phase - 1);
			inverse_phase *= 100;
			
			PlayEnvironmentSound(BlowoutSound.Blowout_Drone, m_Position, pregame_phase * 0.1);
			PlayEnvironmentSound(BlowoutSound.Blowout_Voices, RandomizeVector(GetGame().GetPlayer().GetPosition(), inverse_phase, inverse_phase + 50), pregame_phase * 0.2);
			Sleep(dt);
		}
	}
		
	private void MidBlowoutClient()
	{
		
		PlayEnvironmentSound(BlowoutSound.Blowout_Bass, m_Position, 1.5);
		
		EntityAI headgear = GetGame().GetPlayer().GetInventory().FindAttachment(InventorySlots.HEADGEAR);
		if (Class.CastTo(m_APSI, headgear)) {
			m_APSI.SwitchOn();
		}
		
		Sleep(5000);
		ref array<vector> alarm_positions = GetAlarmPositions();
		foreach (vector pos: alarm_positions) {
			m_AlarmSounds.Insert(PlayEnvironmentSound(BlowoutSound.Blowout_Alarm, pos, 1, 0));
		}
		
	
		thread StartHitPhase();		
		
		for (int j = 0; j < m_BlowoutCount; j++) {
			
			float phase = (1 / m_BlowoutCount) * j;
			phase = Math.Clamp(phase, 0.25, FLT_MAX);
			
			PlaySoundOnPlayer(BlowoutSound.Blowout_Contact, phase);
			//Sleep(vector.Distance(m_Player.GetPosition(), m_Position) * 0.343);
			thread CreateBlowout(phase);
			Sleep(3000 * Math.RandomFloat(0.7, 1.2));
		}
		
		
		// Final Blowout
		PlayEnvironmentSound(BlowoutSound.Blowout_Begin, m_Position, 1);
		Sleep(3000); // 3 Seconds delay for sound lol
		
		// Delay for distance from camera
		Sleep(vector.Distance(m_Position, m_Player.GetPosition()) * 0.343);
		CreateCameraShake(0.8);
		
		PlaySoundOnPlayer(BlowoutSound.Blowout_NearImpact);
		Sleep(1700);
		m_Player.AddHealth("", "Shock", -15); // 15 shock damage
		PlaySoundOnPlayer(BlowoutSound.Blowout_Contact);
		Sleep(100);
		
		thread CreateBlowout(1);
		
		if (m_APSI && m_APSI.IsSwitchedOn()) {
			
		} else {
			if (m_Player.m_Environment.IsSafeFromEVR()) {		
				return;
			}
			
			PlaySoundOnPlayer(BlowoutSound.Blowout_FullWave, 0.25);
			m_Player.StartCommand_Unconscious(0);
			PPEffects.SetUnconsciousnessVignette(true);
		}
		
		//thread LerpFunction(g_Game, "SetEVValue", -3, 0, m_BlowoutDelay);
	}
		
	private void StartHitPhase()
	{
		for (int i = 0; i < m_WaveCount; i++) {
			thread CreateHit(1 / m_WaveCount * i);
			Sleep(m_TimeBetweenWaves * 1000 * Math.RandomFloat(0.7, 1.2));
		}
	}
	
	private void CreateHit(float intensity)
	{	
		Print("CreateHit " + intensity);
		intensity = Math.Clamp(intensity, 0.3, 1);
		//intensity *= CalculateIntensity(vector.Distance(m_Player.GetPosition(), m_Position));
		
		
		float phase = intensity;
		phase *= 100;
		vector pos = RandomizeVector(m_Player.GetPosition(), phase, phase + 25);
		Sleep(vector.Distance(pos, m_Player.GetPosition()) * 0.343);
		
		
		//m_Player.GetStaminaHandler().DepleteStamina(EStaminaModifiers.JUMP);
		//CreateCameraShake(intensity);
		CreateLightning(m_Position, intensity * 3);
		
		if (m_APSI && m_APSI.IsSwitchedOn()) {
			
		} else {
			//m_MatBlur.LerpParam("Intensity", 0.4 * intensity, 0.1, 0.75);
			//m_MatGlow.LerpParam("Vignette", 0.4 * intensity, 0, 0.75);
			//m_MatChroma.LerpParam("PowerX", 0.5 * intensity, 0, 1);
		}
	}
	
	private void CreateBlowout(float intensity)
	{	
		m_Player.GetStaminaHandler().DepleteStamina(EStaminaModifiers.JUMP);		
		CreateCameraShake(intensity * 2);
		if (m_APSI && m_APSI.IsSwitchedOn()) {
			
		} else {
			
			m_MatBlur.LerpParam("Intensity", 0.8 * intensity, m_MatBlur.GetParamValue("Intensity") + 0.04, 0.75);
			m_MatGlow.LerpParam("Vignette", 1 * intensity, m_MatBlur.GetParamValue("Vignette") + 0.25, 0.75);
			m_MatChroma.LerpParam("PowerX", 0.3 * intensity, 0, 2.5);
			m_MatGlow.LerpParam("Saturation", 0.2, 1, 1);
		}
	}
	
	private float CalculateIntensity(float distance)
	{
		float x = (1 / m_BlowoutSize) * distance;
		return Math.Clamp(x, 0.1, 1);
	}
	
	private void CreateCameraShake(float intensity)
	{
		GetGame().GetPlayer().GetCurrentCamera().SpawnCameraShake(Math.Clamp(intensity, 0.2, 1), 2, 5, 3.5);
	}
		

	private AbstractWave PlaySoundOnPlayer(BlowoutSound sound, float volume = 1)
	{
		SoundObjectBuilder builder = new SoundObjectBuilder(new SoundParams(typename.EnumToString(BlowoutSound, sound)));
		SoundObject sound_object = builder.BuildSoundObject();
	
		sound_object.SetKind(WaveKind.WAVEENVIRONMENTEX);
		sound_object.SetPosition(GetGame().GetPlayer().GetPosition());
		AbstractWave wave = GetGame().GetSoundScene().Play2D(sound_object, builder);
		wave.SetVolume(volume);
		wave.Play();
		return wave;
	}
	
	private AbstractWave PlayEnvironmentSound(BlowoutSound sound, vector position, float volume = 1, float frequency_random = 0)
	{
		SoundObjectBuilder builder = new SoundObjectBuilder(new SoundParams(typename.EnumToString(BlowoutSound, sound)));
		SoundObject sound_object = builder.BuildSoundObject();
		sound_object.SetKind(WaveKind.WAVEENVIRONMENTEX);
		sound_object.SetPosition(position);
		
		AbstractWave wave = GetGame().GetSoundScene().Play3D(sound_object, builder);
		wave.SetFrequency(wave.GetFrequency() * Math.RandomFloat(1 - frequency_random, 1 + frequency_random));
		wave.SetVolume(volume);
		wave.Play();
		return wave;
	}
	
	
	void LerpFunction(Class inst, string function, float start, float finish, float duration)
	{		
		int i = 0;
		while (i < duration * 1000) {
			g_Script.CallFunctionParams(inst, function, null, new Param1<float>(Math.Lerp(start, finish, (1 / duration) * i / 1000)));
			Sleep(10);
			i += 10;
		}
	}
	

	void CreateLightning(vector position, int intensity)
	{
		for (int i = 0; i < Math.RandomFloat(1, 3) * intensity; i++) {
			thread CreateBolt(position);
			Sleep(Math.RandomInt(0, 100));
		}
	}
	
	void CreateBolt(vector position)
	{
		position = RandomizeVector(position, 10, 50);
		PlayEnvironmentSound(BlowoutSound.Blowout_Hit, position, 0.35);
		//position[1] = GetGame().SurfaceY(position[0], position[2]);
		Object bolt = GetGame().CreateObject(BOLT_TYPES[Math.RandomInt(0, 1)], position);
		bolt.SetOrientation(Vector(0, Math.RandomFloat(0, 360), 0));
		
		position[1] = position[1] + 50;
		InclementDabLightning m_Light = InclementDabLightning.Cast(ScriptedLightBase.CreateLight(InclementDabLightning, position));
		
		Sleep(Math.RandomInt(15, 150));
		m_Light.Destroy();
		GetGame().ObjectDelete(bolt);
	}
	
	static ref array<vector> GetAlarmPositions()
	{
		
		ref array<vector> alarm_positions = {};
		string world_name;
		GetGame().GetWorldName(world_name);
		string cfg = "CfgWorlds " + world_name + " Names";		
		
		string allowed_types = "Capital City";
		
		for (int i = 0; i < GetGame().ConfigGetChildrenCount(cfg); i++) {
			string city;
			GetGame().ConfigGetChildName(cfg, i, city);			
			vector city_position = GetGame().ConfigGetVector(string.Format("%1 %2 position", cfg, city));
			if (allowed_types.Contains(GetGame().ConfigGetTextOut(string.Format("%1 %2 type", cfg, city)))) {
				alarm_positions.Insert(city_position);
			}
		}
		
		return alarm_positions;
	}
	
	// add min and max to this
	private vector RandomizeVector(vector in, float rand)
	{
		for (int i = 0; i < 3; i++)
			in[i] = Math.RandomFloat(in[i] - rand, in[i] + rand);
		
		return in;
	}
	
	private vector RandomizeVector(vector in, float min, float max)
	{
		for (int i = 0; i < 3; i++) {
			in[i] = Math.RandomFloat(in[i] + Math.RandomFloat(-min, min), in[i] + Math.RandomFloat(-max, max));
		}
		
		return in;
	}
	
	override string GetEventName() 
	{
		return "event-type-evr-storm";
	}
}