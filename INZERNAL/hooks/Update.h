#pragma once
#include <core/globals.h>
#include <core/gt.h>
#include <hooks/hooks.h>

class UpdateHook {
   public:
    //for code that needs to be handled on injection as well
    static void OnInject(App* app) {
        static auto orig_fps = decltype(&hooks::BaseApp_SetFPSLimit)(hookmgr->orig(sig::baseapp_setfpslimit));
        orig_fps((BaseApp*)app, opt::fps_limit);
        utils::printc("93", "Modified FPS limit!");
        global::app = app;
        utils::unprotect_process();

        auto gamelogic = sdk::GetGameLogic();
        if (!gamelogic)
            return;

        if (opt::cheat::punch_cooldown_on) {
        //   opt::cheat::punch_cooldown_val)
        }
        auto player = gamelogic->GetLocalPlayer();
        if (!player)
            return;

        player->SetModStatus(opt::mod_zoom, opt::cheat::dev_zoom);
    }

    static void OnUpdateInWorld(App* app, NetAvatar* local) {

    }
    //any kind of continuous code can be ran here
    static void Execute(App* app) {
        static auto orig = decltype(&hooks::App_Update)(hookmgr->orig(sig::app_update));
        if (!global::app && app)
            OnInject(app);

        auto gamelogic = sdk::GetGameLogic();
        if (gamelogic) {
            auto player = gamelogic->GetLocalPlayer();
            if (player)
                OnUpdateInWorld(app, player);
        }

        orig(app);
    }
};