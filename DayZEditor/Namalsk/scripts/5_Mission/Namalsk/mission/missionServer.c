modded class MissionServer
{
	protected ref EventManager m_EventManagerServer;

	override void OnInit()
	{
		super.OnInit();

		m_EventManagerServer = new EventManager();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		if (m_EventManagerServer.IsServerTickEnabled())
			m_EventManagerServer.OnUpdateServer(timeslice);
	}

	override void OnEvent(EventType eventTypeId, Param params) 
	{
		PlayerIdentity identity;
		PlayerBase player;
		int counter = 0;
		
		switch(eventTypeId)
		{
			case ClientNewEventTypeID:
				ClientNewEventParams newParams;
				Class.CastTo(newParams, params);
				
				player = OnClientNewEvent(newParams.param1, newParams.param2, newParams.param3);
				if (!player)
				{
					Debug.Log("ClientNewEvent: Player is empty");
					return;
				}
				identity = newParams.param1;
				InvokeOnConnect(player,identity );
				SyncEvents.SendPlayerList();
				ControlPersonalLight(player);
				SyncGlobalLighting(player);

				// modded
				if (m_EventManagerServer.IsEventActive())
					m_EventManagerServer.SendInProgressEventInfo(player);
				break;
				
			case ClientReadyEventTypeID:
				ClientReadyEventParams readyParams;
				Class.CastTo(readyParams, params);
				
				identity = readyParams.param1;
				Class.CastTo(player, readyParams.param2);
				if (!player)
				{
					Debug.Log("ClientReadyEvent: Player is empty");
					return;
				}
				
				OnClientReadyEvent(identity, player);
				InvokeOnConnect(player, identity);
				// Send list of players at all clients
				SyncEvents.SendPlayerList();
				ControlPersonalLight(player);
				SyncGlobalLighting(player);

				// modded
				if (m_EventManagerServer.IsEventActive())
					m_EventManagerServer.SendInProgressEventInfo(player);
				break;

			default:
				super.OnEvent(eventTypeId, params);
				break;
		}
	}
};