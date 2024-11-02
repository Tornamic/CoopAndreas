#pragma once
#include <cstdint>

class Modelable
{
private:
    int16_t m_nModelId;

protected:
    Modelable(int16_t modelId) : m_nModelId(modelId) {}

public:
    virtual ~Modelable() = default;

    int16_t GetModelId() const { return m_nModelId; }
    void SetModelId(int16_t modelId) { m_nModelId = modelId; }
};
