#pragma once
#include "stdafx.h"

#include "CNetworkVehicle.h"

class Driveable
{
private:
    uint8_t m_nSeatId = -1;
    CNetworkVehicle* m_pNetworkVehicle = nullptr;

protected:
    Driveable() {}

public:
    virtual ~Driveable() = default;

    virtual uint8_t GetSeatId() { return m_nSeatId; }
    virtual void SetSeatId(uint8_t seatId) { m_nSeatId = seatId; }

    virtual CNetworkVehicle* GetVehicle() { return m_pNetworkVehicle; }

    uint16_t GetVehicleId() { if (m_pNetworkVehicle) return m_pNetworkVehicle->GetId(); return -1; }

    virtual void SetVehicle(CNetworkVehicle* networkVehicle) { m_pNetworkVehicle = networkVehicle; }
};