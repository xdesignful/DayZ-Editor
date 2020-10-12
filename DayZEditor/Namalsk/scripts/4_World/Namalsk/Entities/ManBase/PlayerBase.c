modded class PlayerBase
{
	protected int m_EventInProgress;

	override void Init()
	{	
		super.Init();

		m_EventInProgress = -1;
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		switch(rpc_type)
		{
			case ERPCsNamalsk.NAM_RPC_EVENT_INPROGRESS:
				ref Param1<int> NamEventInfo;
				NamEventInfo = new Param1<int>(-1);

				Print("[Namalsk][EventManager] received event INPROGRESS");

				if (ctx.Read(NamEventInfo))
				{
					//Print(NamEventInfo.param1);
					m_EventInProgress = NamEventInfo.param1;
				}
			break;
		}
	}

	// for client event manager
	int GetActiveEventInfo()
	{
		return m_EventInProgress;
	}
	void ResetActiveEventInfo()
	{
		m_EventInProgress = -1;
	}
};