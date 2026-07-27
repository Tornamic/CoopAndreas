#include <windows.h>
#include <string.h>
#include <iomanip>
#include <sstream>
#include "registry.h"

class ComputerID
{
private:
    static const int m_length = 8;

    std::string CreateGuid()
    {
        GUID guid;
        if (FAILED(CoCreateGuid(&guid)))
            return "DEADCODE-AAAA-BBBB-CCCC-123456123456";

        char buffer[37];
        sprintf_s(buffer, "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6],
            guid.Data4[7]);

        return buffer;
    }

    std::string ComputeMD5(const std::string& input)
    {
        HCRYPTPROV hProv = 0;
        HCRYPTHASH hHash = 0;
        BYTE rgbHash[16];
        DWORD cbHash = 16;
        std::string hashString = "";

        if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
        {
            if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
            {
                if (CryptHashData(
                        hHash, reinterpret_cast<const BYTE*>(input.c_str()), static_cast<DWORD>(input.length()), 0))
                {
                    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
                    {
                        std::stringstream ss;
                        for (DWORD i = 0; i < (DWORD)(m_length / 2); i++)
                        {
                            ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)rgbHash[i];
                        }
                        hashString = ss.str();
                    }
                }
                CryptDestroyHash(hHash);
            }
            CryptReleaseContext(hProv, 0);
        }
        return hashString;
    }

public:
    ComputerID() {}

    std::string GetUniqueSystemId()
    {
        char buffer[37];
        std::string pcid;
        if (Registry::GetField(buffer, sizeof(buffer), "pcid"))
        {
            pcid = std::string(buffer);
        }
        else
        {
            pcid = CreateGuid();
            Registry::SetField(pcid.c_str(), "pcid");
        }
        return ComputeMD5(pcid);
    }
};
