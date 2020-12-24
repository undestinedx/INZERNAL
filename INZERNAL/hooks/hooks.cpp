#pragma once
#include <core/gt.h>
#include <core/utils.h>
#include <hooks/ProcessTankUpdatePacket.h>
#include <hooks/SendPacket.h>
#include <hooks/SendPacketRaw.h>
#include <hooks/hooks.h>
#include <intrin.h>
#include <menu\menu.h>
#include <sdk/sdk.h>
#include <stdio.h>
#include <iomanip>
#include <thread>

WNDPROC hooks::wndproc; //special cases which dont follow normal pattern
LPVOID hooks::endscene;

hookmanager* hookmgr = new hookmanager();
IDirect3DDevice9* device = nullptr;
bool canrender = false;

void hooks::init() {
    global::hwnd = FindWindowA(nullptr, "Growtopia");

    const auto base = global::gt;
    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!pD3D)
        return;

    D3DPRESENT_PARAMETERS d3dpp{ 0 };
    d3dpp.hDeviceWindow = global::hwnd;
    d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
    d3dpp.Windowed = TRUE;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; //vsync
    d3dpp.FullScreen_RefreshRateInHz = 0;
    if (FAILED(pD3D->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &device))) {
        printf("fail: creating device\n");
        pD3D->Release();
        return;
    }

    auto vtable = *reinterpret_cast<void***>(device);

   
    MH_CreateHook(LPVOID(vtable[42]), EndScene, (void**)(&hooks::endscene));
    
    //hooks can now be found in sigs.cpp, they are directly set up there

    for (auto hk : hookmgr->hooks) 
        MH_CreateHook(hk->address, hk->hooked, &hk->orig);
    

    wndproc = WNDPROC(SetWindowLongPtrW(global::hwnd, -4, LONG_PTR(WndProc)));

    //TODO - update alt server shit
    //*(bool*)((uintptr_t)global::gt + 0x5EA071) = opt::alt_server;

    MH_EnableHook(MH_ALL_HOOKS);
    utils::printc("93", "Hooks have been setup!");
}

void hooks::destroy() {
    SetWindowLongPtr(global::hwnd, -4, LONG_PTR(wndproc));
RETRY:
    if (MH_OK != MH_DisableHook(MH_ALL_HOOKS)) {
        auto res = MessageBox(GetForegroundWindow(), L"Hooks could not be reverted", L"Internal", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_TOPMOST);
        switch (res) {
            case IDABORT:
                while (!TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS))
                    ;
            case IDRETRY: goto RETRY;
            case IDIGNORE: // just ignore
                break;
        }
    }
}

float __cdecl hooks::App_GetVersion(App* app) {
    static auto orig = decltype(&hooks::App_GetVersion)(hookmgr->orig(sig::app_getversion));
    if (!global::app) {
        global::app = app;
        static auto orig_fps = decltype(&hooks::BaseApp_SetFPSLimit)(hookmgr->orig(sig::baseapp_setfpslimit));
        orig_fps((BaseApp*)app, opt::fps_limit);
        utils::unprotect_process();
        utils::printc("93", "Modified FPS limit!");
    }
    float version = opt::gt_version;
    static float real_ver = orig(app);
    if (real_ver > version) //dont spoof if we are running newer client. internal might just be outdated, avoid recompilation.
        version = real_ver;
    else
        utils::printc("93", "spoofed version: %.3f", version);

    return version;
}

void __cdecl hooks::BaseApp_SetFPSLimit(BaseApp* ba, float fps) {
    static auto orig = decltype(&hooks::BaseApp_SetFPSLimit)(hookmgr->orig(sig::baseapp_setfpslimit));
    utils::printc("93", "[INZERNAL]\trequest to set fps to %.0f, setting to %.0f instead", fps, opt::fps_limit);
    orig(ba, opt::fps_limit);
}

int __cdecl hooks::LogMsg(const char* msg, ...) {
    static auto orig = decltype(&hooks::LogMsg)(hookmgr->orig(sig::logmsg));
    char buf[0x1000u];
    int len = -1;
    va_list va;
    va_start(va, msg);
    if (0 > (len = _vsnprintf_s(buf, sizeof(buf), msg, va))) {
        va_end(va);
        return 0;
    }
    va_end(va);

    if (len < 90 && len > 2) {
        if (logging::enabled && logging::console & logging::logmsg)
            utils::printc("88", "LogMsg\t%s", buf);
    }
    return orig(buf);
}

bool __cdecl hooks::NetAvatar_CanMessageT4(NetAvatar* player) {
    printf("CanMessageT4 called!!!\n");
    return false;
}

bool active = false;
LRESULT __stdcall hooks::WndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    active = GetActiveWindow() == global::hwnd;

    //TODO: with imgui
    if (menu::WndProc(wnd, msg, wparam, lparam))
        return true;

    if (msg == WM_KEYDOWN && (wparam == VK_CONTROL || wparam == VK_LCONTROL || wparam == VK_RCONTROL))
        return true;

    if (msg == WM_KEYUP && wparam == VK_F3)
        global::unload = true;

    return CallWindowProcW(hooks::wndproc, wnd, msg, wparam, lparam);
}
bool __cdecl hooks::CanPunchOrBuildNow(AvatarRenderData* render_data) {
    //grr i wanted to get type of self func automatically but didnt find a way to do
    static auto orig = decltype(&hooks::CanPunchOrBuildNow)(hookmgr->orig(sig::canpunchorbuildnow));
    if (opt::cheat::punch_cooldown_on) {
        static auto time_before = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_sec = std::chrono::system_clock::now() - time_before;
        if (elapsed_sec.count() > opt::cheat::punch_cooldown_val) {
            time_before = std::chrono::system_clock::now();
            return true;
        }
        else
            return false;
    }

    return orig(render_data);
}

bool __cdecl hooks::ObjectMap_HandlePacket(WorldObjectMap* map, GameUpdatePacket* packet) {
    //CL_Vec2f pos_obj{ packet->pos_x, packet->pos_y };
    //auto player = sdk::gamelogic()->GetLocalPlayer();
    //auto ret = orig::WorldObjectMap_HandlePacket(map, packet);
    //if (player && pos_obj.x > 0.1f && pos_obj.y > 0.1f && ret) {

    //	/*	player->set_pos(packet->pos_x - 4.f, packet->pos_y - 4.f);
    //		player->set_pos(packet->pos_x, packet->pos_y - 4.f);
    //		player->set_pos(packet->pos_x - 4.f, packet->pos_y);
    //		player->set_pos(packet->pos_x , packet->pos_y );
    //		player->set_pos(packet->pos_x + 4.f, packet->pos_y + 4.f);
    //		player->set_pos(packet->pos_x, packet->pos_y + 4.f);
    //		player->set_pos(packet->pos_x + 4.f, packet->pos_y);*/

    //		/*pickup.active = true;
    //		pickup.pos = pos_obj;
    //		pickup.netid = packet->netid;
    //		time_ = std::chrono::system_clock::now() + std::chrono::milliseconds(150);*/

    //		/*	auto pos_pl = player->get_pos();
    //			utils::print("%f %f\n", pos_pl.x, pos_pl.y);*/
    //			//utils::print("%f %f\n", player->m_pos.x, player->m_pos.y);
    //	packet->debug_print("packet");
    //	utils::print("%d %f %f\n", packet->netid, packet->pos_x, packet->pos_y);
    //}
    static auto orig = decltype(&hooks::ObjectMap_HandlePacket)(hookmgr->orig(sig::objectmap_handlepacket));
    return orig(map, packet);
}

void __cdecl hooks::SendPacketRaw(int type, GameUpdatePacket* packet, int size, void* packetsender, ENetPeer* peer, int flag) {
    SendPacketRawHook::Execute(type, packet, size, packetsender, peer, flag);
}

void __cdecl hooks::HandleTouch(LevelTouchComponent* touch, CL_Vec2f pos, bool started) {
    static auto orig = decltype(&hooks::HandleTouch)(hookmgr->orig(sig::handletouch));
    if (opt::tp_click && GetAsyncKeyState(VK_CONTROL)) {
        //localplayer is guaranteed to be a valid pointer here according to xrefs
        auto local = sdk::GetGameLogic()->GetLocalPlayer();
        auto new_pos = pos - (local->GetSize() / 2.f);
        local->SetPos(new_pos);
        pos = new_pos;
    }
    else
        orig(touch, pos, started);
}

void __cdecl hooks::WorldCamera_OnUpdate(WorldCamera* camera, CL_Vec2f unk, CL_Vec2f unk2) {
    static auto orig = decltype(&hooks::WorldCamera_OnUpdate)(hookmgr->orig(sig::worldcamera_onupdate));
    if (opt::tp_click && GetAsyncKeyState(VK_CONTROL)) //if we dont do this then there is major sliding when teleporting.
        return;

    orig(camera, unk, unk2);
}

//for future usage
void __cdecl hooks::UpdateFromNetAvatar(AvatarRenderData* render_data, NetAvatar* player) {
    static auto orig = decltype(&hooks::UpdateFromNetAvatar)(hookmgr->orig(sig::updatefromnetavatar));
    orig(render_data, player);
}

void __cdecl hooks::SendPacket(int type, std::string& packet, ENetPeer* peer) {
    SendPacketHook::Execute(type, packet, peer);
}

void __cdecl hooks::ProcessTankUpdatePacket(GameLogic* logic, GameUpdatePacket* packet) {
    ProcessTankUpdatePacketHook::Execute(logic, packet);
}

bool __cdecl hooks::CanSeeGhosts(int id) {
    static auto orig = decltype(&hooks::CanSeeGhosts)(hookmgr->orig(sig::canseeghosts));
    if (opt::see_ghosts)
        return true;
    return orig(id);
}

void __cdecl hooks::NetAvatar_Gravity(NetAvatar* player) {
    static auto orig = decltype(&hooks::NetAvatar_Gravity)(hookmgr->orig(sig::gravity));
    if (opt::cheat::gravity_on && player == sdk::GetGameLogic()->GetLocalPlayer()) {
        auto backup = player->gravity.get();
        player->gravity.set(opt::cheat::gravity_val);
        orig(player);
        player->gravity.set(backup);
    }
    else
        orig(player);
}

void __cdecl hooks::ProcessAcceleration(NetAvatar* player, float speed) {
    static auto orig = decltype(&hooks::ProcessAcceleration)(hookmgr->orig(sig::processacceleration));
    orig(player, speed);

    if (player == sdk::GetGameLogic()->GetLocalPlayer()) {
        if (opt::cheat::movespeed_on) {
            if (speed != 0.f) {
                if ((opt::cheat::movespeed_start || (!opt::cheat::movespeed_start && fabsf(player->velocity_x.get()) >= 250.f))) {
                    player->velocity_x.set(speed > 0.f ? opt::cheat::movespeed_val : -opt::cheat::movespeed_val);
                }
            }
            else if (opt::cheat::movespeed_stop)
                player->velocity_x.set(0.f);
        }
    }
}

void __cdecl hooks::NetHTTP_Update(NetHTTP* http) {
    static auto orig = decltype(&hooks::NetHTTP_Update)(hookmgr->orig(sig::nethttp_update));
    if (http->m_serverName.find("iap-mob.ubi.com") != -1) //block ubisoft iap http spam shit.
        return;

    //we dont know if its gt1 or gt2, so lets just do both, fuck the performance.
    if (opt::custom_server_on && http->m_serverName.find("growtopia") != -1) {
        utils::replace(http->m_serverName, "growtopia2.com", opt::custom_server_val);
        utils::replace(http->m_serverName, "growtopia1.com", opt::custom_server_val);
    }

    orig(http);
}

long __stdcall hooks::EndScene(IDirect3DDevice9* device) {
    static auto orig = decltype(&hooks::EndScene)(endscene);
    menu::EndScene(device, active); //Imgui happens here
    return orig(device);
}
