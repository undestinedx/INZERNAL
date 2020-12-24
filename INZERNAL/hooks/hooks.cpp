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

#define ORIGINAL(x) types::x hooks::orig::##x{};
#define MAKEHOOK(x) MH_CreateHook(LPVOID(##x), hooks::##x, (void**)(&orig::##x));

//necessary to set original hook
ORIGINAL(App_GetVersion);
ORIGINAL(BaseApp_SetFPSLimit);
ORIGINAL(LogMsg);
ORIGINAL(NetAvatar_CanMessageT4);
ORIGINAL(CanPunchOrBuildNow);
ORIGINAL(ObjectMap_HandlePacket);
ORIGINAL(SendPacketRaw);
ORIGINAL(HandleTouch);
ORIGINAL(WorldCamera_OnUpdate);
ORIGINAL(UpdateFromNetAvatar);
ORIGINAL(SendPacket);
ORIGINAL(ProcessTankUpdatePacket);
ORIGINAL(CanSeeGhosts);
ORIGINAL(NetAvatar_Gravity);
ORIGINAL(NetHTTP_Update);
ORIGINAL(EndScene);
ORIGINAL(ProcessAcceleration);

WNDPROC hooks::orig::wndproc; //wndproc is special case

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
    //releasing device fucks up vtable if loading with patcher so removed that part.

    // clang-format off

	auto
		App_GetVersion                  = sigs::get(sig::app_getversion),
		BaseApp_SetFPSLimit             = sigs::get(sig::baseapp_setfpslimit),
		LogMsg                          = sigs::get(sig::logmsg),
		CanMessageT4                    = sigs::get(sig::canmessaget4),
		CanPunchOrBuildNow              = sigs::get(sig::canpunchorbuildnow),
		ObjectMap_HandlePacket          = sigs::get(sig::objectmap_handlepacket),
		SendPacketRaw                   = sigs::get(sig::sendpacketraw),
		HandleTouch                     = sigs::get(sig::handletouch),
        WorldCamera_OnUpdate            = sigs::get(sig::worldcamera_onupdate),
        UpdateFromNetAvatar             = sigs::get(sig::updatefromnetavatar),
        SendPacket                      = sigs::get(sig::sendpacket),
        ProcessTankUpdatePacket         = sigs::get(sig::processtankupdatepacket),
        CanSeeGhosts                    = sigs::get(sig::canseeghosts),
        NetAvatar_Gravity               = sigs::get(sig::gravity),
        NetHTTP_Update                  = sigs::get(sig::nethttp_update),
        ProcessAcceleration             = sigs::get(sig::processacceleration);

    MH_CreateHook(LPVOID(vtable[42]), EndScene, (void**)(&orig::EndScene));
	MAKEHOOK(App_GetVersion);
	MAKEHOOK(BaseApp_SetFPSLimit);
	MAKEHOOK(LogMsg);
	MAKEHOOK(NetAvatar_CanMessageT4);
	MAKEHOOK(CanPunchOrBuildNow);
	MAKEHOOK(ObjectMap_HandlePacket);
	MAKEHOOK(SendPacketRaw);
	MAKEHOOK(HandleTouch);
    MAKEHOOK(WorldCamera_OnUpdate);
    MAKEHOOK(UpdateFromNetAvatar);
    MAKEHOOK(SendPacket);
    MAKEHOOK(ProcessTankUpdatePacket);
    MAKEHOOK(CanSeeGhosts);
    MAKEHOOK(NetAvatar_Gravity);
    MAKEHOOK(NetHTTP_Update);
    MAKEHOOK(ProcessAcceleration);

	orig::wndproc = WNDPROC(SetWindowLongPtrW(global::hwnd, -4, LONG_PTR(WndProc)));

    // clang-format on

    //TODO - update alt server shit
    //*(bool*)((uintptr_t)global::gt + 0x5EA071) = opt::alt_server;

    MH_EnableHook(MH_ALL_HOOKS);
    utils::printc("93", "Hooks have been setup!");
}

void hooks::destroy() {
    SetWindowLongPtr(global::hwnd, -4, LONG_PTR(orig::wndproc));
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
    if (!global::app) {
        global::app = app;
        orig::BaseApp_SetFPSLimit((BaseApp*)app, opt::fps_limit);
        utils::unprotect_process();
        utils::printc("93", "Modified FPS limit!");
    }
    float version = opt::gt_version;
    static float real_ver = orig::App_GetVersion(app);
    if (real_ver > version) //dont spoof if we are running newer client. internal might just be outdated, avoid recompilation.
        version = real_ver;
    else
        utils::printc("93", "spoofed version: %.3f", version);

    return version;
}

void __cdecl hooks::BaseApp_SetFPSLimit(BaseApp* ba, float fps) {
    utils::printc("93", "[INZERNAL]\trequest to set fps to %.0f, setting to %.0f instead", fps, opt::fps_limit);
    orig::BaseApp_SetFPSLimit(ba, opt::fps_limit);
}

int __cdecl hooks::LogMsg(const char* msg, ...) {
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
    return orig::LogMsg(buf);
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

    return CallWindowProcW(hooks::orig::wndproc, wnd, msg, wparam, lparam);
}
bool __cdecl hooks::CanPunchOrBuildNow(AvatarRenderData* render_data) {
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

    return orig::CanPunchOrBuildNow(render_data);
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

    return orig::ObjectMap_HandlePacket(map, packet);
}

void __cdecl hooks::SendPacketRaw(int type, GameUpdatePacket* packet, int size, void* packetsender, ENetPeer* peer, int flag) {
    SendPacketRawHook::Execute(orig::SendPacketRaw, type, packet, size, packetsender, peer, flag);
}

void __cdecl hooks::HandleTouch(LevelTouchComponent* touch, CL_Vec2f pos, bool started) {
    if (opt::tp_click && GetAsyncKeyState(VK_CONTROL)) {
        //localplayer is guaranteed to be a valid pointer here according to xrefs
        auto local = sdk::GetGameLogic()->GetLocalPlayer();
        auto new_pos = pos - (local->GetSize() / 2.f);
        local->SetPos(new_pos);
        pos = new_pos;
    }
    else
        orig::HandleTouch(touch, pos, started);
}

void __cdecl hooks::WorldCamera_OnUpdate(WorldCamera* camera, CL_Vec2f unk, CL_Vec2f unk2) {
    if (opt::tp_click && GetAsyncKeyState(VK_CONTROL)) //if we dont do this then there is major sliding when teleporting.
        return;

    orig::WorldCamera_OnUpdate(camera, unk, unk2);
}

//for future usage
void __cdecl hooks::UpdateFromNetAvatar(AvatarRenderData* render_data, NetAvatar* player) {
    orig::UpdateFromNetAvatar(render_data, player);
}

void __cdecl hooks::SendPacket(int type, std::string& packet, ENetPeer* peer) {
    SendPacketHook::Execute(orig::SendPacket, type, packet, peer);
}

void __cdecl hooks::ProcessTankUpdatePacket(GameLogic* logic, GameUpdatePacket* packet) {
    ProcessTankUpdatePacketHook::Execute(orig::ProcessTankUpdatePacket, logic, packet);
}

bool __cdecl hooks::CanSeeGhosts(int id) {
    if (opt::see_ghosts)
        return true;
    return orig::CanSeeGhosts(id);
}

void __cdecl hooks::NetAvatar_Gravity(NetAvatar* player) {

    if (opt::cheat::gravity_on && player == sdk::GetGameLogic()->GetLocalPlayer()) {
        auto backup = player->gravity.get();
        player->gravity.set(opt::cheat::gravity_val);
        orig::NetAvatar_Gravity(player);
        player->gravity.set(backup);
    }
    else
        orig::NetAvatar_Gravity(player);
}

void __cdecl hooks::ProcessAcceleration(NetAvatar* player, float speed) {
    orig::ProcessAcceleration(player, speed);

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
    if (http->m_serverName.find("iap-mob.ubi.com") != -1) //block ubisoft iap http spam shit.
        return;

    //we dont know if its gt1 or gt2, so lets just do both, fuck the performance.
    if (opt::custom_server_on && http->m_serverName.find("growtopia") != -1) {
        utils::replace(http->m_serverName, "growtopia2.com", opt::custom_server_val);
        utils::replace(http->m_serverName, "growtopia1.com", opt::custom_server_val);
    }

    orig::NetHTTP_Update(http);
}

long __stdcall hooks::EndScene(IDirect3DDevice9* device) {
    menu::EndScene(device, active); //Imgui happens here
    return orig::EndScene(device);
}
