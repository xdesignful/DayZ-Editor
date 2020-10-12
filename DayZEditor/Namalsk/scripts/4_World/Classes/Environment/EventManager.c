class EventManager
{
	protected float m_Timer;
	protected float m_TimerEventPhase;
	protected int m_NextEventIn;
	protected int m_NextEventType;	// rpc
	protected bool m_EventRunning;
	protected float m_ActiveEventPhaseLength;
	protected int m_EventPhaseClientPrev;
	protected ref EventBase m_ActiveEvent;
	protected bool m_ServerLateUpdate;
	protected int m_EventFreqMin;
	protected int m_EventFreqMax;
	protected bool m_ServerTickEnabled;

	void EventManager()
	{
		m_Timer = 0.0;
		m_NextEventType = -1;
		m_EventRunning = false;
		m_EventPhaseClientPrev = -1;

		m_ServerLateUpdate = false;

		if (GetGame().IsServer())
		{
			m_EventFreqMin = 600;
			m_EventFreqMax = 800;
			m_ServerTickEnabled = true;
			m_TimerEventPhase = 0.0;
			m_ActiveEventPhaseLength = -1.0;
			m_NextEventIn = 700;
		}

		/*if (GetGame().IsClient())
		{

		}*/
	}

	void OnInitServer(bool state, int min, int max)
	{
		if (state)
		{
			m_EventFreqMin = min;
			m_EventFreqMax = max;
			m_NextEventIn = Math.RandomInt(m_EventFreqMin, m_EventFreqMax);

			Print("[Namalsk][EventManager] is now running");
			Print("[Namalsk][EventManager] next selection will occur in " + m_NextEventIn + " seconds");
		} else {
			m_ServerTickEnabled = false;

			Print("[Namalsk][EventManager] disabled");
		}
	}

	void OnUpdateClient(float timeslice)
	{
		m_Timer += timeslice;
		if (m_EventRunning)
		{
			if (GetDayZGame().GetActiveNamalskEventPhase() != m_EventPhaseClientPrev)
			{
				m_EventPhaseClientPrev = GetDayZGame().GetActiveNamalskEventPhase();
				// switch phase logic on client
				m_ActiveEvent.SwitchPhase();

				if (m_ActiveEvent.GetActivePhaseID() == -1)
				{
					m_ActiveEvent.OnEventEndClient();
					GetDayZGame().SetActiveNamalskEventPhase(-1);
					m_EventPhaseClientPrev = -1;
					m_EventRunning = false;
				}
			}

			// continuous updates in selected rate
			float reqTickRate = m_ActiveEvent.GetRequiredClientTickRate();
			if (reqTickRate != -1.0)
			{
				if (m_Timer > reqTickRate)
				{
					m_Timer = 0.0;
					m_ActiveEvent.ContinuousClientEventTick();
				}
			}
		} else {
			if (m_Timer > 5.0)
			{
				m_Timer = 0.0;

				// check for changes in variable NextEventType every 5 seconds
				// check for changes in EventInfo (player connected in middle of an event)

				bool isInProgressOnServerAlready = false;
				PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
				if (player)
				{
					if (player.GetActiveEventInfo() != -1)
						isInProgressOnServerAlready = true;
				}

				if ((GetDayZGame().GetActiveNamalskEventID() != m_NextEventType) || isInProgressOnServerAlready)
				{
					int eventPhase;

					if (isInProgressOnServerAlready)
					{
						int eventInfo = player.GetActiveEventInfo();
						player.ResetActiveEventInfo();
						m_NextEventType = eventInfo / 10;
						eventPhase = eventInfo - (m_NextEventType * 10);
					} else {
						m_EventPhaseClientPrev = -1;
						m_NextEventType = GetDayZGame().GetActiveNamalskEventID();
						GetDayZGame().SetActiveNamalskEventID(-1);
					}

					m_EventRunning = true;
					switch (m_NextEventType)
					{
						case 0:
						{
							// aurora
							m_ActiveEvent = new Aurora();
							break;
						}
						case 1:
						{
							// blizzard
							m_ActiveEvent = new Blizzard();
							break;
						}
						case 2:
						{
							// extreme cold
							m_ActiveEvent = new ExtremeCold();
							break;
						}
						case 3:
						{
							// snowfall
							m_ActiveEvent = new Snowfall();
							break;
						}
						default: {
							// should not happen!
						}
					}
					if (isInProgressOnServerAlready)
					{
						switch (eventPhase)
						{
							case 0:
							{
								// init
								m_ActiveEvent.SwitchPhase();
								break;	
							}
							case 1:
							{
								// middle
								m_ActiveEvent.SwitchPhase();
								m_ActiveEvent.SwitchPhase();
								break;
							}
							case 2:
							{
								// end
								m_ActiveEvent.SwitchPhase();
								m_ActiveEvent.SwitchPhase();
								m_ActiveEvent.SwitchPhase();
								break;
							}
						}
					}
					Print("[Namalsk][EventManager] an event now running");
					m_NextEventType = -1;
				}
			}
		}
	}

	void OnUpdateServer(float timeslice)
	{
		m_Timer += timeslice;

		// 5 second tick
		if ((m_Timer > 5.0) && (!m_EventRunning))
		{
			m_Timer = 0.0;
			m_NextEventIn -= 5.0;

			// is it time for another event?
			if (m_NextEventIn <= 0.0)
			{
				// pick time for the next event
				m_NextEventIn = Math.RandomInt(m_EventFreqMin, m_EventFreqMax);
				m_TimerEventPhase = 0.0;
				m_ActiveEventPhaseLength = -1.0;
				m_EventRunning = false;

				int throwDice = Math.RandomInt(0, 4);
				int plannedEventType = -1;
				switch (throwDice)
				{
					case 0:
					{
						// aurora
						plannedEventType = 0;
						m_ActiveEvent = new Aurora();
						break;
					}
					case 1:
					{
						// blizzard
						plannedEventType = 1;
						m_ActiveEvent = new Blizzard();
						break;
					}
					case 2:
					{
						// extreme cold
						plannedEventType = 2;
						m_ActiveEvent = new ExtremeCold();
						break;
					}
					case 3:
					{
						// snowfall
						plannedEventType = 3;
						m_ActiveEvent = new Snowfall();
						break;
					}
					default: {
						// should not happen!
					}
				}

				Print("[Namalsk][EventManager] selected " + ConvertEventIDToString(plannedEventType) + " as next");

				if (m_ActiveEvent.EventActivateCondition())
				{
					m_NextEventType = plannedEventType;
					m_EventRunning = true;
					Print("[Namalsk][EventManager] " + ConvertEventIDToString(plannedEventType) + " is now running");
					SendActiveEventID(m_NextEventType);
				} else {
					m_EventRunning = false;
					Print("[Namalsk][EventManager] next selection will occur in " + m_NextEventIn + " seconds");
				}
			}
		} else {
			// event in progress
			// dont lower the m_NextEventIn variable while event is up

			// 5 second tick in event (server event tick - not per frame actions done)
			if (m_Timer > 5.0)
			{
				m_TimerEventPhase += 5.0;	// this is timer for the whole length of an event phase
				m_Timer = 0;				// reset active manager timer

				if (m_ServerLateUpdate)
				{
					m_ServerLateUpdate = false;
					m_ActiveEvent.PhaseLateUpdateServer();
				}

				if ( m_TimerEventPhase > m_ActiveEventPhaseLength)
				{
					m_TimerEventPhase = 0.0;
					m_ActiveEvent.SwitchPhase();
					m_ActiveEventPhaseLength = m_ActiveEvent.GetCurrentPhaseLength();
					SendActiveEventPhase(m_ActiveEvent.GetActivePhaseID());
					m_ServerLateUpdate = true;
				}

				if (m_ActiveEventPhaseLength == -1) {
					m_ActiveEvent.OnEventEndServer();
					m_EventRunning = false;
					Print("[Namalsk][EventManager] end of " + ConvertEventIDToString(m_NextEventType));
					Print("[Namalsk][EventManager] next selection will occur in " + m_NextEventIn + " seconds");
				}
			}
		}
	}

	protected string ConvertEventIDToString(int eventid)
	{
		switch (eventid)
		{
			case 0:
			{
				// aurora
				return "event-type-aurora";
			}
			case 1:
			{
				// blizzard
				return "event-type-blizzard";
			}
			case 2:
			{
				// extreme cold
				return "event-type-extremecold";
			}
			case 3:
			{
				// snowfall
				return "event-type-snowfall";
			}
			default: {
				// should not happen!
				return "event-type-none";
			}
		}
		return "event-type-none";
	}
	protected void SendActiveEventID(int eventid)
	{
		Param1<int> p = new Param1<int>(eventid);
		GetGame().RPCSingleParam(null, ERPCsNamalsk.NAM_RPC_EVENTID_UPDATE, p, true, null);
	}
	protected void SendActiveEventPhase(int phaseid)
	{
		Param1<int> p = new Param1<int>(phaseid);
		GetGame().RPCSingleParam(null, ERPCsNamalsk.NAM_RPC_EVENTPHASE_UPDATE, p, true, null);
	}

	bool IsEventActive()
	{
		return m_EventRunning;
	}
	void SendInProgressEventInfo(PlayerBase pBase)
	{
		int eventInfo = (m_ActiveEvent.GetEventID() * 10) + m_ActiveEvent.GetActivePhaseID();
		Param1<int> p = new Param1<int>(eventInfo);
		GetGame().RPCSingleParam(pBase, ERPCsNamalsk.NAM_RPC_EVENT_INPROGRESS, p, true, pBase.GetIdentity());
	}

	bool IsServerTickEnabled()
	{
		// server only
		return m_ServerTickEnabled;
	}
};