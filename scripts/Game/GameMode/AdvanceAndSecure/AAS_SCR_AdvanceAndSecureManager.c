class SCR_AdvanceAndSecureManager : SCR_BaseGameModeComponent
{	
	[Attribute("-30", UIWidgets.Slider, "Tickets awarded to the faction that held a control point prior to change in ownership.", params: "-100 100 1", category: "AdvanceAndSecure")]
	int m_iRewardOnControlLost;
	
	protected int m_iStartingTicketsBLUFOR;
	
	protected int m_iStartingTicketsOPFOR;
	
	override void OnGameModeStart()
	{
		super.OnGameModeStart();
		
		SCR_MilitaryBaseSystem baseSystem = SCR_MilitaryBaseSystem.Cast(GetGame().GetGameMode().FindComponent(SCR_MilitaryBaseSystem));
		if (!baseSystem)
			return;
		
		array<SCR_MilitaryBaseComponent> bases = {};
		baseSystem.GetBases(bases);
		
		foreach (SCR_MilitaryBaseComponent base : bases)
		{
			SCR_SeizingComponent seizing = SCR_SeizingComponent.Cast(base.FindComponent(SCR_SeizingComponent));
			if (seizing) 
				seizing.GetOnCaptureFinish().Insert(OnControlPointCaptured);
		
		}
	}
	
	void OnControlPointCaptured(SCR_Faction faction, SCR_SeizingComponent seizingComponent)
	{
		SCR_Faction previousOwner = seizingComponent.m_FactionControl.GetAffiliatedFaction();
		
		if (previousOwner)
		{
			AAS_SCR_TicketSystemComponent ticketSystem = AAS_SCR_TicketSystemComponent.Cast(FindComponent(AAS_SCR_TicketSystemComponent));
			if (scoring)
				ticketSystem.AddFactionTickets(previousOwner, m_iRewardOnControlLost);
		}
	}
}