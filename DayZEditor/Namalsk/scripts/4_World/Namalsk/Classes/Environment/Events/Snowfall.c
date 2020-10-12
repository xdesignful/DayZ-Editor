class Snowfall extends EventBase
{
	override void EventInit()
	{
		super.EventInit();

		m_EventID = 3;

		if (GetGame().IsServer())
		{
			m_InitPhaseLength = 600.0;
			m_MidPhaseLength = Math.RandomFloat(800, 2100);
			m_EndPhaseLength = m_InitPhaseLength;
			m_NeededOvercast = 0.2;
			m_overcastTarget = 0.75;
			m_fogTarget = 0.7;
			m_snowTarget = 0.25;
			m_windRelMinTarget = 0.0;
			m_windRelMaxTarget = 0.0;
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
			Print("[Namalsk][Snowfall] failed to activate (needed ov: " + m_NeededOvercast + ", actual ov: " + m_wObject.GetOvercast().GetActual() + ")");
			return false;
		}
	}

	override void InitPhaseClient() {}
	override void InitPhaseServer()
	{
		Print("[Namalsk][Snowfall] init: " + m_InitPhaseLength + " (sec), mid: " + m_MidPhaseLength + " (sec), end: " + m_EndPhaseLength + " (sec)");

		// backup previous weather
		m_ovFore = m_wObject.GetOvercast().GetForecast();
		m_wObject.GetOvercast().GetLimits(m_ovMin, m_ovMax);
		m_fgFore = m_wObject.GetFog().GetForecast();
		m_wObject.GetFog().GetLimits(m_fgMin, m_fgMax);
		m_wObject.GetWindFunctionParams(m_windRelMin, m_windRelMax, m_windChangeSpeed);

		// set event (init) weather
		m_wObject.GetOvercast().SetLimits(0.0, 0.75);
		m_wObject.GetFog().SetLimits(0.0, 1.0);
		m_wObject.GetRain().SetLimits(0.0, 0.25);
		m_wObject.GetOvercast().Set(m_overcastTarget, m_InitPhaseLength, m_MidPhaseLength);
		m_wObject.GetFog().Set(m_fogTarget, m_InitPhaseLength, m_MidPhaseLength);
		m_wObject.GetRain().Set(m_snowTarget, m_InitPhaseLength, m_MidPhaseLength);
		m_wObject.SetWindFunctionParams(m_windRelMinTarget, m_windRelMaxTarget, m_windChangeSpeed);
	}
	override void InitPhaseServerLate() {}

	override void MidPhaseClient() {}
	override void MidPhaseServer()
	{
		// add effect to WorldData
		GetGame().GetMission().GetWorldData().SetWetnessEvent(0.35);
	}
	override void MidPhaseServerLate() {}

	override void EndPhaseClient() {}
	override void EndPhaseServer()
	{
		m_wObject.GetOvercast().Set(m_ovFore, m_EndPhaseLength, 0);
		m_wObject.GetFog().Set(m_fgFore, m_EndPhaseLength, 0);
		m_wObject.GetRain().Set(0.0, m_EndPhaseLength / 1.5, 0);
		m_wObject.SetWindFunctionParams(m_windRelMinTarget, m_windRelMaxTarget, m_windChangeSpeed);

		// remove effects from WorldData
		GetGame().GetMission().GetWorldData().SetWetnessEvent(0.0);
	}
	override void EndPhaseServerLate()
	{
		m_wObject.GetOvercast().SetLimits(m_ovMin, m_ovMax);
		m_wObject.GetFog().SetLimits(m_fgMin, m_fgMax);
		m_wObject.GetRain().SetLimits(0.0, 0.0);
	}
	override void OnEventEndServer()
	{
		m_wObject.SetWindFunctionParams(m_windRelMin, m_windRelMax, m_windChangeSpeed);
	}
}