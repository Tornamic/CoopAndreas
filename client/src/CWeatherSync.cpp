#include "CWeatherSync.h"

void CWeatherSync::SyncCurrentState()
{
	if (!CLocalPlayer::m_bIsHost)
	{
		return;
	}

	Packets::World::GameWeatherTime packet{};
	packet.newWeather = static_cast<eWeatherType>(CWeather::NewWeatherType);
	packet.oldWeather = static_cast<eWeatherType>(CWeather::OldWeatherType);
	//packet.forcedWeather = CWeather::ForcedWeatherType;
	packet.currentMonth = CClock::ms_nGameClockMonth;
	packet.currentDay = CClock::CurrentDay;
	packet.currentHour = CClock::ms_nGameClockHours;
	packet.currentMinute = CClock::ms_nGameClockMinutes;
	GetPacketFactory().Send(packet);
}

void CWeatherSync::HandlePacket(Packets::World::GameWeatherTime* pGameWeatherTime)
{
	CWeather::OldWeatherType = pGameWeatherTime->oldWeather;
	CWeather::NewWeatherType = pGameWeatherTime->newWeather;
	//CWeather::ForcedWeatherType = pGameWeatherTime->forcedWeather;
	CClock::ms_nGameClockMonth = pGameWeatherTime->currentMonth;
	CClock::CurrentDay = pGameWeatherTime->currentDay;
	CClock::ms_nGameClockHours = pGameWeatherTime->currentHour;
	CClock::ms_nGameClockMinutes = pGameWeatherTime->currentMinute;
}

void CWeatherSync::ServerTimeRecalculated(server_time_t serverTime)
{
	CWaterLevel::m_nWaterTimeOffset = CTimer::m_snTimeInMilliseconds - serverTime;
}
