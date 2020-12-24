#pragma once
#include <core/globals.h>
#include <core/gt.h>
#include <proton\MiscUtils.h>
#include <sdk/sdk.h>
#include <proton/RTParam.hpp>
#include <string>

class SendPacketHook {
   public:
    static void Execute(int type, std::string& packet, ENetPeer* peer) {
        static auto orig = decltype(&hooks::SendPacket)(hookmgr->orig(sig::sendpacket));
        if (type == NET_MESSAGE_CLIENT_LOG_RESPONSE) //dont send crash log to gt.
            return;

        if (logging::enabled && logging::console & logging::sendpacket)
            printf("sending text packet: %d [%s]\n", type, packet.c_str());

        if (packet.find("game_version|") != -1 && opt::spoof_login) {
            auto var = RTVar::parse(packet);
            auto mac = gt::generate_mac();
            auto hash_str = mac + "RT";
            auto hash2 = HashString(hash_str.c_str(), hash_str.length());

            var.set("mac", mac);
            var.set("wk", gt::generate_rid());
            var.set("rid", gt::generate_rid());
            var.set("hash", std::to_string(utils::random(INT_MIN, INT_MAX)));
            var.set("hash2", std::to_string(hash2));
            var.set("meta", gt::generate_meta());

            if (opt::spoof_win == WinSpoof::wsRandom) {
                var.set("fz", std::to_string(utils::random(INT_MIN, INT_MAX)));
                var.set("zf", std::to_string(utils::random(INT_MIN, INT_MAX)));
            }
            else if (opt::spoof_win == WinSpoof::wsMimic) { //win-specific file values, mismatching is logged as fake windows.
                var.set("fz", "7013544");                   //last update: 2.950
                var.set("zf", "-1553185796");
            }

            if (opt::flag_mode == FlagMode::fmRandom)
                var.set("country", gt::get_random_flag());
            else if (opt::flag_mode == FlagMode::fmCustom)
                var.set("country", opt::flag);

            //never really paid attention to this before but this indeed could be used to track too
            var.set("player_age", std::to_string(utils::random(21, 60)));

            if (opt::spoof_name)
                var.set("requestedName", utils::rnd(utils::random(4, 10)));

            packet = var.serialize();
        }

        orig(type, packet, peer);
    }
};