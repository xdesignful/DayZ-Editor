modded class MissionGameplay
{
	protected ref EventManager m_EventManagerClient;

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
};