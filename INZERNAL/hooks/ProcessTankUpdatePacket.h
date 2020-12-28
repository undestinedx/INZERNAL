#pragma once
#include <core/globals.h>
#include <core/gt.h>
#include <sdk/sdk.h>
#include <string>

class ProcessTankUpdatePacketHook {
   public:
    static void Execute(GameLogic* logic, GameUpdatePacket* packet) {
        static auto orig = decltype(&hooks::ProcessTankUpdatePacket)(hookmgr->orig(sig::processtankupdatepacket));

        if (logging::enabled) {
            if (logging::console & logging::processtank)
                printf("got raw packet: %d [%s]\n", packet->type, gt::get_type_string(packet->type).c_str());
        }

        auto local = sdk::GetGameLogic()->GetLocalPlayer();
        switch (packet->type) {
            case PACKET_CALL_FUNCTION: {
                if (logging::enabled && logging::console & logging::callfunction) {
                    variantlist_t varlist{};
                    if (varlist.serialize_from_mem(utils::get_extended(packet))) {
                        auto content = varlist.print();
                        printf("%s\n", content.c_str());
                    }
                }
            } break;

            case PACKET_SET_CHARACTER_STATE: {
                if (!local)
                    break;
                if (packet->netid == local->netid) {
                    global::state.copy_from_packet(packet);
                    if (opt::cheat::antighost && packet->gravity_out > 1150.0f && packet->speed_out < 150.0f)
                        return;
                }
            } break;
        }

        orig(logic, packet);
    }
};