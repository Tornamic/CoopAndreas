#pragma once
class CNetworkPlayerStats
{
public:
	std::array<float, 83> m_aStatsFloat{};
	std::array<int, 224> m_aStatsInt{};

	CNetworkPlayerStats();

    float& operator[](eStats i)
    {
        if (i >= 120)
        {
            return reinterpret_cast<float&>(m_aStatsInt[i - 120]);
        }
        else
        {
            return m_aStatsFloat[i];
        }
    }

    float GetFloatStat(size_t index) const;
    void SetFloatStat(size_t index, float value);
    int GetIntStat(size_t index) const;
    void SetIntStat(size_t index, int value);
};

