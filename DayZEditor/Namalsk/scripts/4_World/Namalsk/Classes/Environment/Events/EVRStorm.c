class BlowoutLight: ScriptedLightBase
{
	void BlowoutLight()
	{
		SetLightType(LightSourceType.PointLight);
		SetVisibleDuringDaylight(true);
		SetRadiusTo(1000);
		SetBrightnessTo(1);
		SetCastShadow(false);
		SetDiffuseColor(0.5, 1.0, 0.5);		
		SetFlickerSpeed(0.5);
		SetFlickerAmplitude(0.5);
	}
}

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
	
	private bool m_Rumble = true;
	
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
		InitPhaseClient();		
		if (GetGame().IsServer()) {
			InitPhaseServer();
		}
		
		Sleep(m_InitPhaseLength * 1000);
		
		MidPhaseClient();		
		if (GetGame().IsServer()) {
			MidPhaseServer();
		}
		
		Sleep(m_MidPhaseLength * 1000);
		
		EndPhaseClient();	
		
		if (GetGame().IsServer()) {
			EndPhaseServer();
		}
		
		
		Sleep(m_EndPhaseLength * 1000);
		Print("Debug Client Done!");
	}
	
	override void InitPhaseClient() 
	{
		Print("EVRStorm InitPhaseClient");
		
		// Init stuff
		m_MatBlur = new MaterialEffect("graphics/materials/postprocess/gauss");
		m_MatGlow = new MaterialEffect("graphics/materials/postprocess/glow");
		m_MatChroma = new MaterialEffect("graphics/materials/postprocess/chromaber");
		m_MatColors = new MaterialEffect("graphics/materials/postprocess/colors");
				
		// Starts Lighting at site
		thread StartAmbientRumble();
		
		// Makes shit darker :)
		thread LerpFunction(g_Game, "SetEVValue", 0, -1, m_InitPhaseLength);	
		
		thread StartBlowoutClient();
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
		thread MidBlowoutClient();
	}
	
	override void EndPhaseClient()
	{
		Print("EVRStorm EndPhaseClient");
		
		// Cleaning up Mat Effects
		m_MatBlur.LerpParamTo("Intensity", m_EndPhaseLength, 0);
		m_MatGlow.LerpParamTo("Vignette", m_EndPhaseLength, 0);
		
		// Makes shit brighter
		thread LerpFunction(g_Game, "SetEVValue", -3, 0, 10);
		
		thread EndBlowoutClient();
	}
	
	private void StartBlowoutClient()
	{		
		thread StartHitPhase(m_MidPhaseLength);
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
		
		Sleep(5000);
		ref array<vector> alarm_positions = GetAlarmPositions();
		foreach (vector pos: alarm_positions) {
			m_AlarmSounds.Insert(PlayEnvironmentSound(BlowoutSound.Blowout_Alarm, pos, 1, 0));
		}
	
		if (GetAPSI()) {
			GetAPSI().SwitchOn();
		}
	}
		
	private void MidBlowoutClient()
	{
		Sleep(m_MidPhaseLength * 1000);
	
		// Actual Blowout Event			
		PlaySoundOnPlayer(BlowoutSound.Blowout_Contact, 0.5);
		thread CreateBlowout(0.5);
		
		// Final Blowout
		m_BlowoutLight = ScriptedLightBase.CreateLight(BlowoutLight, m_Position + Vector(0, 1500, 0));		
		AnimateLight(m_BlowoutLight, 3000);
			
		// Delay for distance from camera
		Sleep(vector.Distance(m_Position, m_Player.GetPosition()) * 0.343);
		m_Player.AddHealth("", "Shock", -15);
		PlaySoundOnPlayer(BlowoutSound.Blowout_Contact);
		CreateCameraShake(0.8);
		Sleep(100);
		
		thread CreateBlowout(1);
		
		if (GetAPSI() && GetAPSI().IsSwitchedOn()) {
			return;
		} 
		
		if (m_Player.m_Environment.IsSafeFromEVR()) {		
			return;
		}
		
		PlaySoundOnPlayer(BlowoutSound.Blowout_FullWave, 0.25);
		m_Player.StartCommand_Unconscious(0);
		PPEffects.SetUnconsciousnessVignette(true);
		
		// Welp son. you fucked up
		if (DistanceFromCenter() < 200) {
			m_Player.SetHealth("", "Health", 0);
		}
		
		m_Rumble = false;
	}
	
	private void EndBlowoutClient()
	{		
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
		
	private void AnimateLight(BlowoutLight blowout_light, float time)
	{
		vector position = blowout_light.GetPosition();
		float distance = vector.Distance(m_Position, position);		
		float factor = distance / time;
		float surface_y = GetGame().SurfaceY(position[0], position[2]);
		
		bool near_impact_played;
		while (time >= 0) {
	
			blowout_light.SetPosition(Vector(position[0], surface_y + (time * factor), position[2]));
			blowout_light.Update();
			Sleep(10);
			time -= 10;
			
			if (!near_impact_played && time < 1000) {
				near_impact_played = true;
				PlayEnvironmentSound(BlowoutSound.Blowout_NearImpact, m_Position);
			}
		}
		
		PlayEnvironmentSound(BlowoutSound.Blowout_Begin, blowout_light.GetPosition(), 1);
		Particle particle = Particle.PlayInWorld(ParticleList.BLOWOUT_SHOCKWAVE, blowout_light.GetPosition());
		
		blowout_light.Destroy();
		
		CreateCameraShake(1);
	}
	
	private void StartAmbientRumble()
	{
		while (m_Rumble) {
			PlayEnvironmentSound(BlowoutSound.Blowout_Ambient, m_Position, 0.6);
			Sleep(20000);
		}
	}
	
	private void StartHitPhase(float time)
	{
		Print("Starting hit phase " + time);
		// Need milliseconds
		time *= 1000;
		float start_time = time;
		while (time > 0) {
			Print(time);
			int factor = Math.Clamp(start_time / time, 0, 100);
			int rand = Math.RandomInt(0, 200);
			
			if (factor == rand) {
				CreateLightning(m_Position, factor);
				
				// If player is within the "Danger Zone".... fuck em up
				if (DistanceFromCenter() < 200 && !(GetAPSI() && GetAPSI().IsSwitchedOn())) {
					float intensity = Math.Clamp(factor, 0.3, 1);
					m_Player.AddHealth("", "Health", -5);
					m_MatBlur.LerpParam("Intensity", 0.2 * intensity, 0.1, 0.75);
					m_MatGlow.LerpParam("Vignette", 0.2 * intensity, 0, 0.75);
					m_MatChroma.LerpParam("PowerX", 0.25 * intensity, 0, 1);
				}
			}
			
			time -= 10;
			Sleep(10);
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
	}
	
	private void CreateBlowout(float intensity)
	{	
		m_Player.GetStaminaHandler().DepleteStamina(EStaminaModifiers.JUMP);		
		CreateCameraShake(intensity * 2);
		
		if (!(GetAPSI() && GetAPSI().IsSwitchedOn())) {
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
	
	void LerpFunction(Class inst, string function, float start, float finish, float duration)
	{		
		int i = 0;
		while (i < duration * 1000) {
			g_Script.CallFunction(inst, function, null, Math.Lerp(start, finish, (1 / duration) * i / 1000));
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
		PlayEnvironmentSound(BlowoutSound.Blowout_Hit, position, 1);
		//position[1] = GetGame().SurfaceY(position[0], position[2]);
		Object bolt = GetGame().CreateObject(BOLT_TYPES[Math.RandomInt(0, 1)], position);
		bolt.SetOrientation(Vector(0, Math.RandomFloat(180, 360), 0));
		
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