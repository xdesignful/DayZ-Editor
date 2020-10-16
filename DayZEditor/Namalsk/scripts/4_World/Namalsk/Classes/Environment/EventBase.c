class EventBase
{
	protected int m_EventID;
	protected float m_InitPhaseLength;
	protected float m_MidPhaseLength;
	protected float m_EndPhaseLength;
	protected int m_EventState;
	protected float m_ClientTickRequired;
	protected float m_NeededOvercast;
	protected float m_overcastTarget;
	protected float m_fogTarget;
	protected float m_snowTarget;
	protected float m_windRelMinTarget;
	protected float m_windRelMaxTarget;
	protected float m_ovFore, m_ovMin, m_ovMax;
	protected float m_fgFore, m_fgMin, m_fgMax;
	protected float m_windRelMin, m_windRelMax, m_windChangeSpeed;
	
	protected Weather m_wObject;
	protected PlayerBase m_Player;

	void EventBase()
	{
		EventInit();
	}

	protected void EventInit()
	{
		m_EventState = -1;

		if (GetGame().IsServer())
		{
			m_InitPhaseLength = 5.0;
			m_MidPhaseLength = 10.0;
			m_EndPhaseLength = 5.0;
			m_wObject = GetGame().GetWeather();
		}

		if (GetGame().IsClient())
		{
			m_ClientTickRequired = -1.0;
			m_Player = PlayerBase.Cast(GetGame().GetPlayer());
		}
	}
	
	// Abstract methods
	protected void InitPhaseClient();
	protected void InitPhaseServer();
	protected void InitPhaseServerLate();
	protected void MidPhaseClient();
	protected void MidPhaseServer();
	protected void MidPhaseServerLate();
	protected void EndPhaseClient();
	protected void EndPhaseServer();
	protected void EndPhaseServerLate();

	void OnEventEndClient();
	void OnEventEndServer();
		
	float GetCurrentPhaseLength()
	{
		switch (m_EventState)
		{
			case 0:
			{
				return m_InitPhaseLength;
			}
			case 1:
			{
				return m_MidPhaseLength;
			}
			case 2:
			{
				return m_EndPhaseLength;
			}
			default:
			{
				return -1;
			}
		}
		return -1;
	}
	
	float GetRequiredClientTickRate()
	{
		return m_ClientTickRequired;
	}
	
	int GetActivePhaseID()
	{
		return m_EventState;
	}
	
	void SwitchPhase()
	{
		m_EventState += 1;
		if (m_EventState > 2)
		{
			// event end, destroy this somehow?
			m_EventState = -1;
		} else {
			// do stuff..
			if (GetGame().IsServer())
			{
				switch (m_EventState)
				{
					case 0:
					{
						InitPhaseServer();
						break;
					}
					case 1:
					{
						MidPhaseServer();
						break;
					}
					case 2:
					{
						EndPhaseServer();
						break;
					}
				}
			}
			if (GetGame().IsClient())
			{
				switch (m_EventState)
				{
					case 0:
					{
						InitPhaseClient();
						break;
					}
					case 1:
					{
						MidPhaseClient();
						break;
					}
					case 2:
					{
						EndPhaseClient();
						break;
					}
				}
			}
		}
	}
	
	void ContinuousClientEventTick()
	{
		switch (m_EventState)
		{
			case 0:
			{
				InitPhaseClient();
				break;
			}
			case 1:
			{
				MidPhaseClient();
				break;
			}
			case 2:
			{
				EndPhaseClient();
				break;
			}
		}
	}
	
	void PhaseLateUpdateServer()
	{
		switch (m_EventState)
		{
			case 0:
			{
				InitPhaseServerLate();
				break;
			}
			case 1:
			{
				MidPhaseServerLate();
				break;
			}
			case 2:
			{
				EndPhaseServerLate();
				break;
			}
		}
	}
	
	bool EventActivateCondition()
	{
		return true;
	}
	
	bool GetEventID()
	{
		return m_EventID;
	}
	
	string GetEventName() 
	{
		return "event-type-none";
	}
};