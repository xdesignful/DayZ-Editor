modded class DayZGame
{
	protected int m_NamEventID = -1;
	protected int m_NamEventPhase = -1;

	override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, target, rpc_type, ctx);

		switch (rpc_type)
		{
			case ERPCsNamalsk.NAM_RPC_EVENTID_UPDATE:
				ref Param1<int> NamEventID;
				NamEventID = new Param1<int>(-1);

				Print("[Namalsk][EventManager] received event ID update");

				if (ctx.Read(NamEventID))
				{
					SetActiveNamalskEventID(NamEventID.param1);
				}
				break;

			case ERPCsNamalsk.NAM_RPC_EVENTPHASE_UPDATE:
				ref Param1<int> NamPhaseID;
				NamPhaseID = new Param1<int>(-1);

				Print("[Namalsk][EventManager] received event PHASE update");

				if (ctx.Read(NamPhaseID))
				{
					SetActiveNamalskEventPhase(NamPhaseID.param1);
				}
				break;
		}
	}

	void SetActiveNamalskEventID(int eventID)
	{
		m_NamEventID = eventID;
	}
	int GetActiveNamalskEventID()
	{
		return m_NamEventID;
	}

	void SetActiveNamalskEventPhase(int phaseID)
	{
		m_NamEventPhase = phaseID;
	}
	int GetActiveNamalskEventPhase()
	{
		return m_NamEventPhase;
	}
};