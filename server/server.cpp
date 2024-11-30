#include <iostream>
#include <enet/enet.h>
#include <algorithm>
#include <cstring>
#include <string>

#include "stdafx.h"

int main(int argc, char **argv)
{
    CNetwork::Init(6767);
    return 0;
}
