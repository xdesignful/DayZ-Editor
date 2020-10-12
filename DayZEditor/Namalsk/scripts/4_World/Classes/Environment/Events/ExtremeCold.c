class ExtremeCold extends EventBase
{
	protected float ppDelta;
	protected float m_TempDrop;

	override void EventInit()
	{
		super.EventInit();

		m_EventID = 2;

		if (GetGame().IsServer())
		{
			m_InitPhaseLength = 100.0;
			m_MidPhaseLength = Math.RandomFloat(600, 900);
			m_EndPhaseLength = m_InitPhaseLength;
			m_TempDrop = -1.0 * Math.RandomFloat(10.0, 20.0);
			m_NeededOvercast = 0.15;
		}

		if (GetGame().IsClient())
		{
			ppDelta = 0.0;
		}
	}

	override bool EventActivateCondition()
	{
		if (m_wObject.GetOvercast().GetActual() <= m_NeededOvercast) {
			return true;
		} else {
			Print("[Namalsk][ExtremeCold] failed to activate (needed ov: " + m_NeededOvercast + ", actual ov: " + m_wObject.GetOvercast().GetActual() + ")");
			return false;
		}
	}

	override void InitPhaseClient()
	{
		m_ClientTickRequired = 0.01;
		ppDelta = Math.Min(ppDelta + (m_ClientTickRequired / 25.0), 1.0);
		//PPEffects.SetColorizationDynEvent(Math.Lerp(1.0, 0.95, ppDelta), 1.0, Math.Lerp(1.0, 1.5, ppDelta));
	}
	override void InitPhaseServer()
	{
		Print("[Namalsk][ExtremeCold] init: " + m_InitPhaseLength + " (sec), mid: " + m_MidPhaseLength + " (sec), end: " + m_EndPhaseLength + " (sec), TempDrop: " + m_TempDrop + " (C)");

		// get weather state
		m_ovFore = m_wObject.GetOvercast().GetForecast();
		m_wObject.GetOvercast().GetLimits(m_ovMin, m_ovMax);

		// set weather state
		m_wObject.GetOvercast().SetLimits(0.0, 1.0);
		m_wObject.GetOvercast().Set(0.0, m_InitPhaseLength, m_MidPhaseLength);

		// add effect to WorldData
		GetGame().GetMission().GetWorldData().SetTemperatureEvent(m_TempDrop);
	}
	override void InitPhaseServerLate() {}

	override void MidPhaseClient()
	{
		m_ClientTickRequired = -1.0;
		ppDelta = 0;
		//PPEffects.SetColorizationDynEvent(0.95, 1.0, 1.5);
	}
	override void MidPhaseServer() {}
	override void MidPhaseServerLate() {}

	override void EndPhaseClient()
	{
		m_ClientTickRequired = 0.01;
		ppDelta = Math.Min(ppDelta + (m_ClientTickRequired / 25.0), 1.0);
		//PPEffects.SetColorizationDynEvent(Math.Lerp(0.95, 1.0, ppDelta), 1.0, Math.Lerp(1.5, 1.0, ppDelta));
	}
	override void OnEventEndClient()
	{
		//PPEffects.SetColorizationDynEvent(1.0, 1.0, 1.0);
	}

	override void EndPhaseServer()
	{
		m_wObject.GetOvercast().Set(m_ovFore, m_EndPhaseLength, 0);
	}
	override void EndPhaseServerLate()
	{
		m_wObject.GetOvercast().SetLimits(m_ovMin, m_ovMax);
	}
	override void OnEventEndServer()
	{	
		// remove effect from WorldData
		GetGame().GetMission().GetWorldData().SetTemperatureEvent(0.0);
	}
}