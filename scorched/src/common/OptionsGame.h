////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#if !defined(__INCLUDE_OptionsGameh_INCLUDE__)
#define __INCLUDE_OptionsGameh_INCLUDE__

#include <common/OptionEntry.h>

class OptionsGame
{
public:
	OptionsGame();
	virtual ~OptionsGame();

	enum ScoreType
	{
		ScoreWins = 0,
		ScoreKills = 1,
		ScoreMoney = 2
	};
	enum TurnType
	{
		TurnSimultaneous = 0,
		TurnSequentialLooserFirst = 1,
		TurnSequentialRandom = 2
	};
	enum WindForce
	{
		WindRandom = 0,
		WindNone = 1,
		Wind1 = 2,
		Wind2 = 3,
		Wind3 = 4,
		Wind4 = 5,
		Wind5 = 6,
		WindBreezy = 7,
		WindGale = 8
	};
	enum WindType
	{
		WindOnRound = 0,
		WindOnMove = 1
	};
	enum WallType
	{
		WallRandom = 0,
		WallConcrete = 1,
		WallBouncy = 2,
		WallWrapAround = 3,
		WallNone = 4
	};
	enum WeapScale
	{
		ScaleSmall = 0,
		ScaleMedium = 1,
		ScaleLarge = 2
	};
	enum ResignType
	{
		ResignStart = 0,
		ResignEnd = 1,
		ResignDueToHealth = 2
	};
	enum MovementRestrictionType
	{
		MovementRestrictionNone = 0,
		MovementRestrictionLand = 1,
		MovementRestrictionLandOrAbove = 2
	};
	enum TeamBallanceType
	{
		TeamBallanceNone,
		TeamBallanceAuto,
		TeamBallanceBotsVs,
		TeamBallanceAutoByScore,
		TeamBallanceAutoByBots
	};
	
	const char *getTutorial() { return tutorial_; }
	OptionEntryString &getTutorialEntry() { return tutorial_; }

	int getPhysicsFPS() { return physicsFPS_; }
	OptionEntryInt &getPhysicsFPSEntry() { return physicsFPS_; }

	int getScorePerAssist() { return scorePerAssist_; }
	OptionEntryInt &getScorePerAssistEntry() { return scorePerAssist_; }

	int getScorePerKill() { return scorePerKill_; }
	OptionEntryInt &getScorePerKillEntry() { return scorePerKill_; }

	int getScoreWonForRound() { return scoreWonForRound_; }
	OptionEntryInt &getScoreWonForRoundEntry() { return scoreWonForRound_; }

	int getTeams() { return teams_; }
	OptionEntryInt &getTeamsEntry() { return teams_; }

	int getStartArmsLevel() { return startArmsLevel_; }
	OptionEntryInt &getStartArmsLevelEntry() { return startArmsLevel_; }

	int getEndArmsLevel() { return endArmsLevel_; }
	OptionEntryInt &getEndArmsLevelEntry() { return endArmsLevel_; }

	int getMaxNumberWeapons() { return maxNumberWeapons_; }
	OptionEntryInt &getMaxNumberWeaponsEntry() { return maxNumberWeapons_; }

	int getNoMaxPlayers() { return numberOfPlayers_; }
	OptionEntryInt &getNoMaxPlayersEntry() { return numberOfPlayers_; }

	int getNoMinPlayers() { return numberOfMinPlayers_; }
	OptionEntryInt &getNoMinPlayersEntry() { return numberOfMinPlayers_; }

	int getNoRounds() { return numberOfRounds_; }
	OptionEntryInt &getNoRoundsEntry() { return numberOfRounds_; }

	int getRemoveBotsAtPlayers() { return removeBotsAtPlayers_; }
	OptionEntryInt &getRemoveBotsAtPlayersEntry() { return removeBotsAtPlayers_; }

	int getNoMaxRoundTurns() { return maxRoundTurns_; }
	OptionEntryInt &getNoMaxRoundTurnsEntry() { return maxRoundTurns_; }

	int getStartMoney() { return moneyStarting_; }
	OptionEntryInt &getStartMoneyEntry() { return moneyStarting_; }

	int getInterest() { return moneyInterest_; }
	OptionEntryInt &getInterestEntry() { return moneyInterest_; }

	int getFreeMarketAdjustment() { return freeMarketAdjustment_; }
	OptionEntryInt &getFreeMarketAdjustmentEntry() { return freeMarketAdjustment_; }

	int getMaxLandscapeSize() { return maxLandscapeSize_; }
	OptionEntryInt &getMaxLandscapeSizeEntry() { return maxLandscapeSize_; }

	int getStartTime() { return startTime_; }
	OptionEntryInt &getStartTimeEntry() { return startTime_; }

	int getShotTime() { return shotTime_; }
	OptionEntryInt &getShotTimeEntry() { return shotTime_; }

	int getKeepAliveTime() { return keepAliveTime_; }
	OptionEntryInt &getKeepAliveTimeEntry() { return keepAliveTime_; }

	int getKeepAliveTimeoutTime() { return keepAliveTimeoutTime_; }
	OptionEntryInt &getKeepAliveTimeoutTimeEntry() { return keepAliveTimeoutTime_; }

	int getBuyingTime() { return buyingTime_; }
	OptionEntryInt &getBuyingTimeEntry() { return buyingTime_; }

	int getRoundScoreTime() { return roundScoreTime_; }
	OptionEntryInt &getRoundScoreTimeEntry() { return roundScoreTime_; }

	int getScoreTime() { return scoreTime_; }
	OptionEntryInt &getScoreTimeEntry() { return scoreTime_; }

	int getAllowedMissedMoves() { return allowedMissedMoves_; }
	OptionEntryInt &getAllowedMissedMovesEntry() { return allowedMissedMoves_; }

	int getIdleKickTime() { return idleKickTime_; }
	OptionEntryInt &getIdleKickTimeEntry() { return idleKickTime_; }

	int getIdleShotKickTime() { return idleShotKickTime_; }
	OptionEntryInt &getIdleShotKickTimeEntry() { return idleShotKickTime_; }

	int getMinFallingDistance() { return minFallingDistance_; }
	OptionEntryInt &getMinFallingDistanceEntry() { return minFallingDistance_; }

	int getMaxClimbingDistance() { return maxClimbingDistance_; }
	OptionEntryInt &getMaxClimbingDistanceEntry() { return maxClimbingDistance_; }

	int getPlayerLives() { return playerLives_; }
	OptionEntryInt &getPlayerLivesEntry() { return playerLives_; }

	int getGravity() { return gravity_; }
	OptionEntryInt &getGravityEntry() { return gravity_; }

	OptionEntryEnum getWindForce() { return windForce_; } // WindForce
	OptionEntryEnum &getWindForceEntry() { return windForce_; } // WindForce

	OptionEntryEnum getWindType() { return windType_; } // WindType
	OptionEntryEnum &getWindTypeEntry() { return windType_; } // WindType

	OptionEntryEnum getWallType() { return wallType_; } // WallType
	OptionEntryEnum &getWallTypeEntry() { return wallType_; } // WallType

	OptionEntryEnum getWeapScale() { return weapScale_; } // WeapScale
	OptionEntryEnum &getWeapScaleEntry() { return weapScale_; } // WeapScale
	
	OptionEntryEnum getTurnType() { return turnType_; } // TurnType
	OptionEntryEnum &getTurnTypeEntry() { return turnType_; } // TurnType
	 
	int getBuyOnRound() { return moneyBuyOnRound_; }
	OptionEntryInt &getBuyOnRoundEntry() { return moneyBuyOnRound_; }

	int getMoneyPerRound() { return moneyPerRound_; }
	OptionEntryInt &getMoneyPerRoundEntry() { return moneyPerRound_; }

	int getMoneyWonForRound() { return moneyWonForRound_; }
	OptionEntryInt &getMoneyWonForRoundEntry() { return moneyWonForRound_; }

	int getMoneyWonPerKillPoint() { return moneyPerKillPoint_; }
	OptionEntryInt &getMoneyWonPerKillPointEntry() { return moneyPerKillPoint_; }

	int getMoneyWonPerAssistPoint() { return moneyPerAssistPoint_; }
	OptionEntryInt &getMoneyWonPerAssistPointEntry() { return moneyPerAssistPoint_; }

	int getMoneyWonPerHitPoint() { return moneyPerHitPoint_; }
	OptionEntryInt &getMoneyWonPerHitPointEntry() { return moneyPerHitPoint_; }

	bool getMoneyPerHealthPoint() { return moneyPerHealthPoint_; }
	OptionEntryBool &getMoneyPerHealthPointEntry() { return moneyPerHealthPoint_; }

	bool getLimitPowerByHealth() { return limitPowerByHealth_; }
	OptionEntryBool &getLimitPowerByHealthEntry() { return limitPowerByHealth_; }
	
	OptionEntryEnum getTeamBallance() { return teamBallance_; } // TeamBallanceType
	OptionEntryEnum &getTeamBallanceEntry() { return teamBallance_; } // TeamBallanceType

	int getComputersDeathTalk() { return computersDeathTalk_; }
	OptionEntryInt &getComputersDeathTalkEntry() { return computersDeathTalk_; }

	int getComputersAttackTalk() { return computersAttackTalk_; }
	OptionEntryInt &getComputersAttackTalkEntry() { return computersAttackTalk_; }

	const char * getEconomy() { return economy_; }
	OptionEntryStringEnum &getEconomyEntry() { return economy_; }

	const char * getLandscapes() { return landscapes_; }
	OptionEntryString &getLandscapesEntry() { return landscapes_; }

	const char * getMasterListServer() { return masterListServer_; }
	OptionEntryString &getMasterListServerEntry() { return masterListServer_; }

	const char * getMasterListServerURI() { return masterListServerURI_; }
	OptionEntryString &getMasterListServerURIEntry() { return masterListServerURI_; }

	const char * getStatsLogger() { return statsLogger_; }
	OptionEntryString &getStatsLoggerEntry() { return statsLogger_; }

	const char * getServerFileLogger() { return serverFileLogger_; }
	OptionEntryString &getServerFileLoggerEntry() { return serverFileLogger_; }

	const char * getBotNamePrefix() { return botNamePrefix_; }
	OptionEntryString &getBotNamePrefixEntry() { return botNamePrefix_; }

	bool getGiveAllWeapons() { return giveAllWeapons_; }
	OptionEntryBool &getGiveAllWeaponsEntry() { return giveAllWeapons_; }

	bool getDelayedDefenseActivation() { return delayedDefenseActivation_; }
	OptionEntryBool &getDelayedDefenseActivationEntry() { return delayedDefenseActivation_; }

	bool getCycleMaps() { return cycleMaps_; }
	OptionEntryBool &getCycleMapsEntry() { return cycleMaps_; }

	int getResignMode() { return resignMode_; }
	OptionEntryInt &getResignModeEntry() { return resignMode_; }

	OptionEntryEnum getMovementRestriction() { return movementRestriction_; } // MovementRestrictionType
	OptionEntryEnum &getMovementRestrictionEntry() { return movementRestriction_; } // MovementRestrictionType

	bool getRandomizeBotNames() { return randomizeBotNames_; }
	OptionEntryBool &getRandomizeBotNamesEntry() { return randomizeBotNames_; }

	int getPortNo() { return portNo_; }
	OptionEntryInt &getPortNoEntry() { return portNo_; }

	int getManagementPortNo() { return managementPortNo_; }
	OptionEntryInt &getManagementPortNoEntry() { return managementPortNo_; }

	const char * getMod() { return mod_; }
	OptionEntryString &getModEntry() { return mod_; }

	const char * getMOTD() { return motd_; }
	OptionEntryString &getMOTDEntry() { return motd_; }

	int getModDownloadSpeed() { return modDownloadSpeed_; }
	OptionEntryInt &getModDownloadSpeedEntry() { return modDownloadSpeed_; }

	int getMaxAvatarSize() { return maxAvatarSize_; }
	OptionEntryInt &getMaxAvatarSizeEntry() { return maxAvatarSize_; }

	const char * getServerName() { return serverName_; }
	OptionEntryString &getServerNameEntry() { return serverName_; }

	const char * getServerPassword() { return serverPassword_; }
	OptionEntryString &getServerPasswordEntry() { return serverPassword_; }

	OptionEntryString &getPlayerType(int no) { DIALOG_ASSERT(no<24); return *playerType_[no]; }

	const char * getPublishAddress() { return publishAddress_; }
	OptionEntryString &getPublishAddressEntry() { return publishAddress_; }

	bool getAllowSameIP() { return allowSameIP_; }
	OptionEntryBool &getAllowSameIPEntry() { return allowSameIP_; }

	bool getAllowSameUniqueId() { return allowSameUniqueId_; }
	OptionEntryBool &getAllowSameUniqueIdEntry() { return allowSameUniqueId_; }

	bool getPublishServer() { return publishServer_; }
	OptionEntryBool &getPublishServerEntry() { return publishServer_; }

	bool getResidualPlayers() { return residualPlayers_; }
	OptionEntryBool &getResidualPlayersEntry() { return residualPlayers_; }

	const char * getAuthHandler() { return authHandler_; }
	OptionEntryString &getAuthHandlerEntry() { return authHandler_; }

	bool getRegisteredUserNames() { return registeredUserNames_; }
	OptionEntryBool &getRegisteredUserNamesEntry() { return registeredUserNames_; }

	// Fns used to save or restore the state of the options
	std::list<OptionEntry *> &getOptions();
	std::list<OptionEntry *> &getPlayerTypeOptions(); 
	virtual bool writeOptionsToXML(XMLNode *xmlNode);
	virtual bool readOptionsFromXML(XMLNode *xmlNode);
	virtual bool writeOptionsToFile(char *filePath);
	virtual bool readOptionsFromFile(char *filePath);
	virtual bool writeToBuffer(NetBuffer &buffer, bool useProtected, bool usePlayerTypes);
	virtual bool readFromBuffer(NetBufferReader &reader, bool useProtected, bool usePlayerTypes);

protected:
	std::list<OptionEntry *> options_;
	std::list<OptionEntry *> playerTypeOptions_;

	OptionEntryBoundedInt physicsFPS_;
	OptionEntryBoundedInt startArmsLevel_;
	OptionEntryBoundedInt endArmsLevel_;
	OptionEntryBoundedInt maxRoundTurns_;
	OptionEntryBoundedInt keepAliveTime_;
	OptionEntryBoundedInt keepAliveTimeoutTime_;
	OptionEntryBoundedInt shotTime_;
	OptionEntryBoundedInt startTime_;
	OptionEntryBoundedInt buyingTime_;
	OptionEntryBoundedInt roundScoreTime_;
	OptionEntryBoundedInt scoreTime_;
	OptionEntryBoundedInt allowedMissedMoves_;
	OptionEntryBoundedInt numberOfRounds_;
	OptionEntryBoundedInt maxNumberWeapons_;
	OptionEntryBoundedInt gravity_;
	OptionEntryBoundedInt minFallingDistance_;
	OptionEntryBoundedInt maxClimbingDistance_;
	OptionEntryBoundedInt playerLives_;
	OptionEntryBoundedInt teams_;
	OptionEntryBoundedInt numberOfPlayers_;
	OptionEntryBoundedInt numberOfMinPlayers_;
	OptionEntryBoundedInt removeBotsAtPlayers_;
	OptionEntryBoundedInt computersDeathTalk_;
	OptionEntryBoundedInt computersAttackTalk_;
	OptionEntryBoundedInt moneyBuyOnRound_;
	OptionEntryBoundedInt moneyWonForRound_;
	OptionEntryBoundedInt moneyPerKillPoint_;
	OptionEntryBoundedInt moneyPerAssistPoint_;
	OptionEntryBoundedInt moneyPerHitPoint_;
	OptionEntryBoundedInt moneyPerRound_;
	OptionEntryBool moneyPerHealthPoint_;
	OptionEntryBoundedInt scorePerAssist_;
	OptionEntryBoundedInt scorePerKill_;
	OptionEntryBoundedInt scoreWonForRound_;
	OptionEntryInt maxLandscapeSize_;
	OptionEntryInt freeMarketAdjustment_;
	OptionEntryBoundedInt moneyStarting_;
	OptionEntryBoundedInt idleKickTime_;
	OptionEntryBoundedInt idleShotKickTime_;
	OptionEntryEnum teamBallance_;
	OptionEntryBoundedInt moneyInterest_;
	OptionEntryBool limitPowerByHealth_;
	OptionEntryString tutorial_;
	OptionEntryBool cycleMaps_;
	OptionEntryBool delayedDefenseActivation_;
	OptionEntryEnum resignMode_;
	OptionEntryEnum movementRestriction_;
	OptionEntryEnum turnType_;
	OptionEntryEnum windForce_;
	OptionEntryEnum windType_;
	OptionEntryEnum wallType_;
	OptionEntryEnum weapScale_;
	OptionEntryBoundedInt modDownloadSpeed_;
	OptionEntryInt maxAvatarSize_;
	OptionEntryString mod_;
	OptionEntryString motd_;
	OptionEntryStringEnum economy_;
	OptionEntryString landscapes_;
	OptionEntryString masterListServer_;
	OptionEntryString masterListServerURI_;
	OptionEntryString statsLogger_;
	OptionEntryString serverFileLogger_;

	// Server only options
	OptionEntryString botNamePrefix_;
	OptionEntryBool residualPlayers_;
	OptionEntryBool randomizeBotNames_;
	OptionEntryBool giveAllWeapons_;
	OptionEntryBool registeredUserNames_;
	OptionEntryString authHandler_;
	OptionEntryString serverName_;
	OptionEntryString *playerType_[24];
	OptionEntryString serverPassword_;
	OptionEntryInt portNo_;
	OptionEntryInt managementPortNo_;
	OptionEntryString publishAddress_;
	OptionEntryBool publishServer_;
	OptionEntryBool allowSameIP_;
	OptionEntryBool allowSameUniqueId_;

	// Depricated (old) options
	OptionEntryEnum depricatedScoreType_;
	OptionEntryBool depricatedAutoBallanceTeams_;
	OptionEntryBoundedInt depricatedMaxArmsLevel_;
	OptionEntryString depricatedServerAdminPassword_;
};

class OptionsGameWrapper : public OptionsGame
{
public:
	OptionsGameWrapper();
	virtual ~OptionsGameWrapper();

	// Options that can be changed on the fly
	OptionsGame &getChangedOptions() { return changedOptions_; }
	void updateChangeSet();
	bool commitChanges();

protected:
	OptionsGame changedOptions_;

};

#endif
