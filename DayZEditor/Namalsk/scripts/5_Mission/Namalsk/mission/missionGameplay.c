modded class MissionGameplay
{
	protected ref EventManager m_EventManagerClient;

	void ~MissionGameplay()
	{
		// We dont want memory leaks :D
		delete m_EventManagerClient;		
	}
	
	override void OnInit()
	{
		super.OnInit();

		if ( GetGame().IsMultiplayer() )
		{
			m_EventManagerClient = new EventManager();
		}
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate( timeslice );

		// event manager
		if ( GetGame().IsMultiplayer() )
		{
			m_EventManagerClient.OnUpdateClient( timeslice );
		}
	}
	
	EventManager GetEventManager()
	{
		return m_EventManagerClient;
	}
};