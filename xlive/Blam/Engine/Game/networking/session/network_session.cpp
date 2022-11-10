#include "stdafx.h"
#include "network_session.h"

#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "H2MOD/Modules/Shell/ServerConsole.h"


namespace network_session
{
	// New Carto Functions.
	void vip_lock(e_game_life_cycle state)
	{
		if (state == _life_cycle_post_game)
		{
			ServerConsole::ClearVip();
			*Memory::GetAddress<byte*>(0, 0x534850) = 0;
			//ServerConsole::SendCommand2(1, L"vip", L"clear");
			//ServerConsole::SendCommand2(1, L"Privacy", L"Open");
		}
		if (state == _life_cycle_in_game)
		{
			for (auto i = 0; i < NetworkSession::GetPeerCount(); i++)
			{
				ServerConsole::AddVip(NetworkSession::GetPeerPlayerName(i));
				//ServerConsole::SendCommand2(2, L"vip", L"add", NetworkSession::getPeerPlayerName(i));
			}
			//ServerConsole::SendCommand2(1, L"Privacy", L"VIP");
			*Memory::GetAddress<byte*>(0, 0x534850) = 2;
		}
	}
}
