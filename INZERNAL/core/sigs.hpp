#pragma once
#include <string>
#include <vector>

namespace sig {
    enum : int {
        banbypass = 0,
        gamelogic = 1,
        world2screen,
        enetclient,
        baseapp_setfpslimit,
        logmsg,
        canmessaget4,
        canpunchorbuildnow,
        sendpacketraw,
        handletouch,
        worldcamera_onupdate,
        sendpacket,
        processtankupdatepacket,
        canseeghosts,
        gravity,
        nethttp_update,
        processacceleration,
        app_update
    };

    namespace type {
        enum : int {
            direct, //utils::find_pattern
            fstart, //utils::find_func_start
            call,   //detail::get_call
            //leaving out some unused methods (like utils::find_func_end) of getting these for now
        };
    }

} // namespace sig

namespace sigs {
    extern std::vector<uintptr_t> database;
    uintptr_t add_pattern(std::string name, std::string pattern, int type, int offset = 0, bool ignore = false);
    void init();
    __forceinline uintptr_t get(int num) {
        return database[num];
    }
} // namespace sigs