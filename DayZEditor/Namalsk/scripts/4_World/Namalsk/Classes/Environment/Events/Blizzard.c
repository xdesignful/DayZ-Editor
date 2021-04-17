class Blizzard extends EventBase
{
	protected float m_TempBonus;

	override void EventInit()
	{
		super.EventInit();

		m_EventID = 1;

		if (GetGame().IsServer())
		{
			m_InitPhaseLength = 600.0;
			m_MidPhaseLength = Math.RandomFloat(600, 800);
			m_EndPhaseLength = m_InitPhaseLength;
			m_TempBonus = -1.0 * Math.RandomFloat(12.0, 22.0);
			m_NeededOvercast = 0.25;
			m_overcastTarget = 0.85;
			m_fogTarget = 0.9;
			m_snowTarget = 1.0;
			m_windRelMinTarget = 0.7;
			m_windRelMaxTarget = 1.0;
		}

		/*if (GetGame().IsClient())
		{

		}*/
	}

	override bool EventActivateCondition()
	{
		if (m_wObject.GetOvercast().GetActual() >= m_NeededOvercast) {
			return true;
		} else {
			Print("[Namalsk][Blizzard] failed to activate (needed ov: " + m_NeededOvercast + ", actual ov: " + m_wObject.GetOvercast().GetActual() + ")");
			return false;
		}
	}

	override void InitPhaseClient() {}	
	override void InitPhaseServer()
	{
		Print("[Namalsk][Blizzard] init: " + m_InitPhaseLength + " (sec), mid: " + m_MidPhaseLength + " (sec), end: " + m_EndPhaseLength + " (sec), TempDrop: " + m_TempBonus + " (C)");

		// backup previous weather
		m_ovFore = m_wObject.GetOvercast().GetForecast();
		m_wObject.GetOvercast().GetLimits(m_ovMin, m_ovMax);
		m_fgFore = m_wObject.GetFog().GetForecast();
		m_wObject.GetFog().GetLimits(m_fgMin, m_fgMax);
		m_wObject.GetWindFunctionParams(m_windRelMin, m_windRelMax, m_windChangeSpeed);

		// set event (init) weather
		m_wObject.GetOvercast().SetLimits(0.0, 0.85);
		m_wObject.GetFog().SetLimits(0.0, 1.0);
		m_wObject.GetRain().SetLimits(0.0, 1.0);
		m_wObject.GetOvercast().Set(m_overcastTarget, m_InitPhaseLength, m_MidPhaseLength);
		m_wObject.GetFog().Set(m_fogTarget, m_InitPhaseLength, m_MidPhaseLength);
		m_wObject.GetRain().Set(m_snowTarget, m_InitPhaseLength, m_MidPhaseLength);
		m_wObject.SetWindFunctionParams(0.0, 0.0, m_windChangeSpeed);
	}
	override void InitPhaseServerLate() {}

	override void MidPhaseClient() {}
	override void MidPhaseServer()
	{
		m_wObject.SetWindFunctionParams(m_windRelMinTarget, m_windRelMaxTarget, m_windChangeSpeed);

		// add effect to WorldData
		GetGame().GetMission().GetWorldData().SetTemperatureEvent(m_TempBonus);
		GetGame().GetMission().GetWorldData().SetWetnessEvent(0.65);
	}
	override void MidPhaseServerLate() {}

	override void EndPhaseClient() {}
	override void EndPhaseServer()
	{
		m_wObject.GetOvercast().Set(m_ovFore, m_EndPhaseLength, 0);
		m_wObject.GetFog().Set(m_fgFore, m_EndPhaseLength, 0);
		m_wObject.GetRain().Set(0.0, m_EndPhaseLength / 1.5, (m_EndPhaseLength - (m_EndPhaseLength / 1.5)));
		m_wObject.SetWindFunctionParams(0.0, 0.0, m_windChangeSpeed);

		// remove effects from WorldData
		GetGame().GetMission().GetWorldData().SetTemperatureEvent(0.0);
		GetGame().GetMission().GetWorldData().SetWetnessEvent(0.0);
	}
	override void EndPhaseServerLate()
	{
		m_wObject.GetFog().SetLimits(m_fgMin, m_fgMax);
		m_wObject.GetOvercast().SetLimits(m_ovMin, m_ovMax);
		m_wObject.GetRain().SetLimits(0.0, 0.0);
	}

	override void OnEventEndServer()
	{
		m_wObject.SetWindFunctionParams(m_windRelMin, m_windRelMax, m_windChangeSpeed);
	}
	
	override string GetEventName() 
	{
		return "event-type-blizzard";
	}
};