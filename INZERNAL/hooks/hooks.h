#pragma once
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shlwapi.lib")

#include <core/globals.h>
#include <core/minhook/hook.h>
#include <proton/NetHTTP.h>
#include <proton/Variant.h>
#include <string.h>

class hookinfo {
   public:
    int sig_num{};
    std::string name{};
    std::string pattern{};
    LPVOID address{};
    void* hooked;
    void* orig;
};
class hookmanager {
   public:
    std::vector<hookinfo*> hooks{};

    void add_hook(std::string name, std::string pattern, int type, void* hooked, int offset = 0 ) {
        auto address = sigs::add_pattern(name, pattern, type, offset);
        hookinfo* hk = new hookinfo();
        hk->address = (LPVOID)address;
        hk->name = name;
        hk->pattern = pattern;
        hk->sig_num = sigs::database.size() - 1;
        hk->hooked = hooked;
        hooks.push_back(std::move(hk));
    }

    void* orig(int i) {
        for (auto hook : hooks) {
            if (hook->sig_num == i)
                return hook->orig;
        }

        return nullptr;
    }
};
extern hookmanager* hookmgr;
namespace hooks {

    extern WNDPROC wndproc;
    extern LPVOID endscene;

    void init();
    void destroy();

    // clang-format off

	void	__cdecl		BaseApp_SetFPSLimit(BaseApp* ba, float fps);
	int		__cdecl		LogMsg(const char* msg, ...);
	bool	__cdecl		NetAvatar_CanMessageT4(NetAvatar* player);
	void	__cdecl		SendPacketRaw(int type, GameUpdatePacket* packet, int size, void* packetsender, ENetPeer* peer, int flag);
	void	__cdecl		HandleTouch(LevelTouchComponent* touch, CL_Vec2f pos, bool started);
    void    __cdecl     WorldCamera_OnUpdate(WorldCamera* camera, CL_Vec2f unk, CL_Vec2f unk2);
    void    __cdecl     SendPacket(int type, std::string& packet, ENetPeer* peer);
    void    __cdecl     ProcessTankUpdatePacket(GameLogic* logic, GameUpdatePacket* packet);
    bool    __cdecl     CanSeeGhosts(int id);
    void    __cdecl     NetAvatar_Gravity(NetAvatar* player);
    void    __cdecl     ProcessAcceleration(NetAvatar* player, float speed);
    void    __cdecl     NetHTTP_Update(NetHTTP* http);
    long    __stdcall   EndScene(IDirect3DDevice9* device);
    void    __cdecl     App_Update(App* app);

    LRESULT __stdcall   WndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

    // clang-format on

} // namespace hooks