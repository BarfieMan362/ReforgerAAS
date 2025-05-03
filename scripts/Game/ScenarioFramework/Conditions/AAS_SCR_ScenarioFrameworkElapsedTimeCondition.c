// File: SCR_ScenarioFrameworkElapsedTimeCondition.c

[BaseContainerProps(configRoot: true)]
class SCR_ScenarioFrameworkElapsedTimeCondition : SCR_ScenarioFrameworkActivationConditionBase
{
    [Attribute(defvalue: "30", desc: "Delay before the layer activates (seconds)", params: "0 100000 1", category: "Time")]
    int m_iDelay;

	override bool Init(IEntity entity)
	{
		SCR_BaseGameMode baseGM = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		SCR_ScenarioFrameworkLayerBase layer = SCR_ScenarioFrameworkLayerBase.Cast(entity.FindComponent(SCR_ScenarioFrameworkLayerBase));
		baseGM.GetOnGameStart().Insert(OnGameStart);
	}
	
	
    //------------------------------------------------------------------------------------------------
    override static bool EvaluateConditions(SCR_EScenarioFrameworkLogicOperators operator, notnull array<ref SCR_ScenarioFrameworkActivationConditionBase> activationConditions, IEntity entity)
    {
        // if (m_bDebug)
        //    Print("[SCR_ScenarioFrameworkElapsedTimeCondition.Init] debug line (" + FILE + " L" + LINE + ")", LogLevel.WARNING);

		SCR_BaseGameMode baseGame = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		float secs = baseGame.GetElapsedTime();
        return secs >= m_iDelay;
    }
	
	protected 
}




	//------------------------------------------------------------------------------------------------
	//! Repetitive spawning with timer.
	protected void RepeatedSpawn()
	{
		if (!m_bEnableRepeatedSpawn || (m_iRepeatedSpawnNumber != -1 && m_iRepeatedSpawnNumber <= 0))
			return;

		//This calls the RepeatedSpawnCalled with set delay and is set in a way that it
		//Can be both queued that way or called manually from different place (pseudo-looped CallLater)
		SCR_ScenarioFrameworkSystem.GetCallQueuePausable().CallLater(RepeatedSpawnCalled, 1000 * m_fRepeatedSpawnTimer);
	}

	//------------------------------------------------------------------------------------------------
	//! Repetitive spawning logic with countdown and condition checks.
	protected void RepeatedSpawnCalled()
	{
		if (m_iRepeatedSpawnNumber != -1)
			m_iRepeatedSpawnNumber--;

		SpawnChildren(true);

		if (!m_bEnableRepeatedSpawn || (m_iRepeatedSpawnNumber != -1 && m_iRepeatedSpawnNumber <= 0))
			return;

		RepeatedSpawn();
	}

	