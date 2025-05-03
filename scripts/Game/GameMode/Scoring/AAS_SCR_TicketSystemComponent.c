class AAS_SCR_TicketSystemComponentClass : SCR_BaseGameModeComponentClass 
{
}

void OnFactionTicketsChangedDelegate(Faction faction, int tickets);
typedef func OnFactionTicketsChangedDelegate;
typedef ScriptInvokerBase<OnFactionTicketsChangedDelegate> OnFactionTicketsChangedInvoker;

class AAS_SCR_TicketSystemComponent : SCR_BaseGameModeComponent
{
	//! Map of tickets per faction.
	protected ref map<Faction, int> m_mFactionTickets = new map<Faction, int>();
	
	//! This invoker is invoked when tickets of provided faction changes.
	protected ref OnFactionTicketsChangedInvoker m_OnFactionTicketsChangedInvoker;
	
	//------------------------------------------------------------------------------------------------
	//! \return invoker that is invoked when tickets of provided faction changes.
	OnFactionTicketsChangedInvoker GetOnFactionTicketsChanged()
	{
		if (!m_OnFactionTicketsChangedInvoker)
			m_OnFactionTicketsChangedInvoker = new OnFactionTicketsChangedInvoker();

		return m_OnFactionTicketsChangedInvoker;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Internal helper method.
	//! \return faction from provided index.
	private Faction GetFactionByIndex(int factionIndex)
	{
		if (factionIndex < 0)
			return null;

		return GetGame().GetFactionManager().GetFactionByIndex(factionIndex);
	}

	//------------------------------------------------------------------------------------------------
	//! Internal helper method.
	//! \return index of provided faction.
	private int GetFactionIndex(Faction faction)
	{
		if (!faction)
			return -1;

		return GetGame().GetFactionManager().GetFactionIndex(faction);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnFactionTicketsChanged(Faction faction, int tickets)
	{
		if (m_OnFactionTicketsChangedInvoker)
			m_OnFactionTicketsChangedInvoker.Invoke(faction, tickets);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	private void RpcDo_SetFactionTickets(int factionIdx, int amount)
	{
		Faction faction = GetFactionByIndex(factionIdx);
		if (faction)
		{
			m_mFactionTickets.Set(faction, amount);
			OnFactionTicketsChanged(faction, amount);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set provided faction tickets to x amount
	//! Server-only, propagated to all clients via BC reliable RPC.
	//! \param[in] faction
	//! \param[in amount
	void SetFactionTickets(notnull Faction faction, int amount)
	{
		// Server-only
		if (!m_pGameMode.IsMaster())
			return;
		
		int factionIdx = GetFactionIndex(faction);
		RpcDo_SetFactionTickets(factionIdx, amount);
		Rpc(RpcDo_SetFactionTickets, factionIdx, amount);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	private void RpcDo_AddFactionTickets(int factionIdx, int count)
	{
		Faction faction = GetFactionByIndex(factionIdx);
		if (faction)
		{
			int prev_amount = m_mFactionTickets.Get(faction);
			int amount = prev_amount + count;
			m_mFactionTickets.Set(faction, amount);
			OnFactionTicketsChanged(faction, amount);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Add count x tickets to provided faction.
	//! Server-only, propagated to all clients via BC reliable RPC.
	//! \param[in] faction
	//! \param[in] count
	void AddFactionTickets(notnull Faction faction, int count = 1)
	{
		// Server only
		if (!m_pGameMode.IsMaster())
			return;

		int factionIdx = GetFactionIndex(faction);
		RpcDo_AddFactionTickets(factionIdx, count);
		Rpc(RpcDo_AddFactionTickets, factionIdx, count);
	}
	
	//------------------------------------------------------------------------------------------------
	//! \return tickets of provided faction or 0 if none
	int GetFactionTickets(notnull Faction faction)
	{
		if (!m_mFactionTickets.Contains(faction))
			return false;
		
		return m_mFactionTickets[faction];
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		FactionManager factionManager = GetGame().GetFactionManager();
		if (factionManager)
		{
			array<Faction> factions = {};
			factionManager.GetFactionsList(factions);

			foreach (Faction faction : factions)
			{
				m_mFactionTickets.Insert(faction, 0);
			}
		}

		#ifdef ENABLE_DIAG
			DiagMenu.RegisterBool(SCR_DebugMenuID.DEBUGUI_SCORING_SYSTEM, "", "Ticket System", "GameMode");
		#endif
	}
	
	//---------------------------------------------------------------------------------- --------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		SetEventMask(owner, EntityEvent.INIT);
		#ifdef ENABLE_DIAG
		ConnectToDiagSystem(owner);
		#endif
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		#ifdef ENABLE_DIAG
		DisconnectFromDiagSystem(owner);
		#endif
		
		super.OnDelete(owner);
	}
}
