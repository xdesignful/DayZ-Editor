class BlowoutLight: ScriptedLightBase
{
	void BlowoutLight()
	{
		SetLightType(LightSourceType.PointLight);
		SetVisibleDuringDaylight(true);
		SetRadiusTo(1000);
		SetBrightnessTo(1);
		SetCastShadow(true);
		SetDiffuseColor(0.5, 1.0, 0.5);		
		SetFlickerSpeed(0.5);
		SetFlickerAmplitude(0.5);
	}
}

// add some methods to BlowoutLight to create sound on the object
// right now all sounds are based on the ground

class EVRStorm: EventBase
{	
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
	
	protected BlowoutLight m_BlowoutLight;	
	protected ref array<Object> m_ZeroGravityBuildings = {};
	
	private bool m_Rumble = true;
	private bool m_Lightning = true;
	
	void EVRStorm(vector position)
	{
		m_Position = position;
		m_Position[1] = GetGame().SurfaceY(m_Position[0], m_Position[2]);
		
		m_MissionWeatherState = m_wObject.GetMissionWeather();
		m_wObject.MissionWeather(false);
		
		ref array<CargoBase> proxy_data = {};
		GetGame().GetObjectsAtPosition(m_Position, 25, m_ZeroGravityBuildings, proxy_data);
		
		// Init stuff
		m_MatBlur = new MaterialEffect("graphics/materials/postprocess/gauss");
		m_MatGlow = new MaterialEffect("graphics/materials/postprocess/glow");
		m_MatChroma = new MaterialEffect("graphics/materials/postprocess/chromaber");
		m_MatColors = new MaterialEffect("graphics/materials/postprocess/colors");
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
			m_MidPhaseLength = 60.0;
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
		EventInit();
		
		thread InitPhaseClient();		
		if (GetGame().IsServer()) {
			thread InitPhaseServer();
		}
		
		Sleep(m_InitPhaseLength * 1000);
		
		thread MidPhaseClient();		
		if (GetGame().IsServer()) {
			thread MidPhaseServer();
		}
		
		Sleep(m_MidPhaseLength * 1000);
		
		thread EndPhaseClient();	
		
		if (GetGame().IsServer()) {
			thread EndPhaseServer();
		}
		
		
		Sleep(m_EndPhaseLength * 1000);
		Print("Debug Client Done!");
	}
	
	override void InitPhaseClient() 
	{
		Print("EVRStorm InitPhaseClient");
					
		// Starts Lighting at site
		thread StartAmbientRumble();
		
		// Makes shit darker :)
		thread LerpFunction(g_Game, "SetEVValue", 0, -1, m_InitPhaseLength);	
		
		if (GetAPSI()) {
			GetAPSI().SwitchOn();
		}
		
		ref array<vector> alarm_positions = GetAlarmPositions();
		foreach (vector pos: alarm_positions) {
			//m_AlarmSounds.Insert(PlayEnvironmentSound(BlowoutSound.Blowout_Alarm, pos, 1, 0));
		}	
		
		m_BlowoutLight = ScriptedLightBase.CreateLight(BlowoutLight, m_Position - "0 5 0", 5);
		thread HandleBlowoutLight(m_BlowoutLight);
		thread StartGraviRumble(m_BlowoutLight);
		
		float timepassed;
		while (timepassed < m_InitPhaseLength * 1000) {
			float pregame_phase = 1 / (m_InitPhaseLength * 1000) * timepassed;			
			float dt = 10000;
			timepassed += dt;
			float inverse_phase = Math.AbsFloat(pregame_phase - 1);
			inverse_phase *= 100;
			
			PlayEnvironmentSound(BlowoutSound.Blowout_Drone, m_Position, pregame_phase * 0.1);
			PlayEnvironmentSound(BlowoutSound.Blowout_Voices, RandomizeVector(GetGame().GetPlayer().GetPosition(), inverse_phase, inverse_phase + 50), pregame_phase * 0.2);
			Sleep(dt);
		}
		
		PlayEnvironmentSound(BlowoutSound.Blowout_Bass, m_Position, 1);
		LerpPosition(m_BlowoutLight, m_Position, m_Position + Vector(0, 50, 0), 10);
/*
		// omfg floating objects
		foreach (Object zero_g_object: m_ZeroGravityBuildings) {
			Print(zero_g_object);
			if (zero_g_object && zero_g_object != m_BlowoutLight) {	
				thread LerpPosition(zero_g_object, zero_g_object.GetPosition(), RandomizeVector(zero_g_object.GetPosition(), 10), 10);
				//Sleep(3);
				//thread LerpOrientation(zero_g_object, zero_g_object.GetOrientation(), Math3D.GetRandomDir(), 10);
			}
			
			Sleep(10);
		}
		
		*/
		
		// ominously sit there*
		Sleep(10000);
		PlayEnvironmentSound(BlowoutSound.Blowout_NearImpact, m_Position);
		// Delay for sound effect
		Sleep(1000);
		LerpPosition(m_BlowoutLight, m_Position + Vector(0, 50, 0), m_Position + Vector(1500, 1500, 1500), 2);
		
		// Blowout in sky
		PlaySoundOnPlayer(BlowoutSound.Blowout_Contact, 0.5);
		thread CreateBlowout(0.5);
		m_Lightning = false;
	}
	
	override void InitPhaseServer()
	{
		Print("EVRStorm InitPhaseServer");
		m_wObject.SetStorm(0, 1, 3000);
		m_wObject.GetFog().SetLimits(0, 1);
		m_wObject.GetOvercast().SetLimits(0, 1);
		m_wObject.GetFog().Set(0.5, m_InitPhaseLength, m_InitPhaseLength);
		m_wObject.GetOvercast().Set(1, m_InitPhaseLength, m_InitPhaseLength);
	}	
	
	override void MidPhaseClient()
	{
		Print("EVRStorm MidPhaseClient");
		thread StartHitPhase(m_MidPhaseLength);
		Sleep(m_MidPhaseLength * 1000);

		// Final BlowoutLight
		thread LerpPosition(m_BlowoutLight, m_BlowoutLight.GetPosition(), m_Position, 3.4);
		Sleep(1400);
		PlayEnvironmentSound(BlowoutSound.Blowout_Reentry, m_Position, 1);
		///LerpPosition(m_BlowoutLight, m_BlowoutLight.GetPosition(), m_Position, 1.40);
		PlayEnvironmentSound(BlowoutSound.Blowout_Begin, m_Position, 1);
		Particle.PlayInWorld(ParticleList.BLOWOUT_SHOCKWAVE, m_Position);
		m_BlowoutLight.Destroy();
		
		// Actual Blowout Event			
		PlaySoundOnPlayer(BlowoutSound.Blowout_Contact, 0.5);
		thread CreateBlowout(0.65);
		
		// Delay for distance from camera
		Sleep(DistanceFromCenter() * 0.343);
		
		m_Player.AddHealth("", "Shock", -15);
		CreateCameraShake(0.8);
		Sleep(100);
		
		thread CreateBlowout(1);
		
		if (GetAPSI() && GetAPSI().IsSwitchedOn()) {
			return;
		} 
		
		if (m_Player.m_Environment.IsSafeFromEVR()) {		
			return;
		}
		
		PlaySoundOnPlayer(BlowoutSound.Blowout_FullWave, 0.5);
		m_Player.StartCommand_Unconscious(0);
		//PPEffects.SetUnconsciousnessVignette(true);
		
		// Welp son. you fucked up
		if (DistanceFromCenter() < 200) {
			//m_Player.SetHealth("", "Health", 0);
		}
		
		m_Rumble = false;
	}
	
	override void EndPhaseClient()
	{
		Print("EVRStorm EndPhaseClient");
		
		// Cleaning up Mat Effects
		m_MatBlur.LerpParamTo("Intensity", m_EndPhaseLength, 0);
		m_MatGlow.LerpParamTo("Vignette", m_EndPhaseLength, 0);
		
		// Makes shit brighter
		thread LerpFunction(g_Game, "SetEVValue", -1, 0, m_EndPhaseLength);
		
		Sleep(m_EndPhaseLength * 1000);
		
		if (GetAPSI() && GetAPSI().IsSwitchedOn()) {
			GetAPSI().SwitchOff();
		}
		
		foreach (AbstractWave alarm: m_AlarmSounds) {
			if (alarm) {
				alarm.Stop();
			}
		}
	}
				
	private void StartAmbientRumble()
	{
		while (m_Rumble) {
			PlayEnvironmentSound(BlowoutSound.Blowout_Ambient, m_Position, 0.6);
			Sleep(20000);
		}
	}
	
	private void StartGraviRumble(BlowoutLight blowout_light)
	{
		while (blowout_light) {
			PlayEnvironmentSound(BlowoutSound.Blowout_GraviIdle, blowout_light.GetPosition(), 1);
			Sleep(24000);
		}
	}
	
	private void StartHitPhase(float time)
	{
		Print("Starting hit phase " + time);
		// Need milliseconds
		time *= 1000;
		float start_time = time;
		while (time > 0 && m_Lightning) {
			int factor = Math.Clamp(start_time / time, 0, 100);
			int rand = Math.RandomInt(0, 100);
			
			if (factor == rand) {				
				thread CreateBolt(m_Position);
				
				// If player is within the "Danger Zone".... fuck em up
				if (DistanceFromCenter() < 200 && !(GetAPSI() && GetAPSI().IsSwitchedOn())) {
					float intensity = Math.Clamp(factor, 0.3, 1);
					m_Player.AddHealth("", "Health", -2.5);
					m_MatBlur.LerpParam("Intensity", 0.2 * intensity, 0, Math.RandomFloat01());
					m_MatGlow.LerpParam("Vignette", 0.2 * intensity, 0, Math.RandomFloat01());
					m_MatChroma.LerpParam("PowerX", 0.25 * intensity, 0, Math.RandomFloat01());
				}
			}
			
			time -= 10;
			Sleep(10);
		}
	}
		
	private void CreateBlowout(float intensity)
	{	
		m_Player.GetStaminaHandler().DepleteStamina(EStaminaModifiers.JUMP);		
		CreateCameraShake(intensity * 2);
		
		if (!(GetAPSI() && GetAPSI().IsSwitchedOn())) {
			m_MatBlur.LerpParam("Intensity", 1 * intensity, 0, 0.75);
			m_MatGlow.LerpParam("Vignette", 0.75 * intensity, 0, 0.75);
			m_MatChroma.LerpParam("PowerX", 0.3 * intensity, 0, 2.5);
			m_MatGlow.LerpParam("Saturation", 0.2, 1, 1);
		}
	}
	
	private void HandleBlowoutLight(BlowoutLight blowout_light)
	{
		while (blowout_light) {
			vector position = blowout_light.GetPosition();
			if (position[1] < GetGame().SurfaceY(position[0], position[2])) {
				blowout_light.SetBrightnessTo(0);
				blowout_light.SetRadiusTo(0);
				blowout_light.SetDiffuseColor(0, 0, 0);		
			} else {
				blowout_light.SetBrightnessTo(1);
				blowout_light.SetRadiusTo(1000);
				blowout_light.SetDiffuseColor(0.5, 1.0, 0.5);				
			}
			
			Sleep(10);
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
	
	void LerpFunction(Class inst, string function, float start, float finish, float duration)
	{		
		int i = 0;
		while (i < duration * 1000) {
			g_Script.CallFunction(inst, function, null, Math.Lerp(start, finish, (1 / duration) * i / 1000));
			Sleep(10);
			i += 10;
		}
	}
	
	void LerpFunction(Class inst, string function, vector start, vector finish, float duration)
	{
		int i = 0;
		while (i < duration * 1000) {
			g_Script.CallFunction(inst, function, null, Vector(Math.Lerp(start[0], finish[0], (1 / duration) * i / 1000), Math.Lerp(start[1], finish[1], (1 / duration) * i / 1000), Math.Lerp(start[2], finish[2], (1 / duration) * i / 1000)));
			g_Script.Call(inst, "Update", null);
			Sleep(10);
			i += 10;
		}
	}
	
	void LerpPosition(Object obj, vector start, vector finish, float duration)
	{
		Object obj_ref = obj;
		int i = 0;
		while (i < duration * 1000) {
			obj_ref.SetPosition(Vector(Math.Lerp(start[0], finish[0], (1 / duration) * i / 1000), Math.Lerp(start[1], finish[1], (1 / duration) * i / 1000), Math.Lerp(start[2], finish[2], (1 / duration) * i / 1000)));
			obj_ref.Update();
			Sleep(10);
			i += 10;
		}
	}
	
	void LerpOrientation(Object obj, vector start, vector finish, float duration)
	{
		Object obj_ref = obj;
		int i = 0;
		while (i < duration * 1000) {
			obj_ref.SetOrientation(Vector(Math.Lerp(start[0], finish[0], (1 / duration) * i / 1000), Math.Lerp(start[1], finish[1], (1 / duration) * i / 1000), Math.Lerp(start[2], finish[2], (1 / duration) * i / 1000)));
			obj_ref.Update();
			Sleep(10);
			i += 10;
		}
	}
		
	void CreateBolt(vector position)
	{
		position = RandomizeVector(position, 10, 50);
		PlayEnvironmentSound(BlowoutSound.Blowout_Hit, position, 0.6);
		//position[1] = GetGame().SurfaceY(position[0], position[2]);
		Object bolt = GetGame().CreateObject(BOLT_TYPES[Math.RandomInt(0, 1)], position);
		InclementDabLightning m_Light = InclementDabLightning.Cast(ScriptedLightBase.CreateLight(InclementDabLightning, position));
		bolt.AddChild(m_Light, -1);
		bolt.SetOrientation(Vector(0, Math.RandomFloat(180, 360), 0));
		position[1] = position[1] + 50;
		
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
	
	private float DistanceFromCenter()
	{
		return (vector.Distance(m_Player.GetPosition(), m_Position));
	}
	
	private APSI GetAPSI()
	{
		APSI apsi;
		Class.CastTo(apsi, GetGame().GetPlayer().GetInventory().FindAttachment(InventorySlots.HEADGEAR));		
		return apsi;
	}
	
	override string GetEventName() 
	{
		return "event-type-evr-storm";
	}
}