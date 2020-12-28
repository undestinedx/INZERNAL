#pragma once
#include <core/utils.h>
#include <hooks/hooks.h>
#include <proton/clanlib/vec2.h>
#include <sdk/EntityComponent.h>
#include <sdk/sdk.h>
#include <proton/variant2.hpp>
#include <string>
#include <core/gt.h>

#pragma pack(push, 1)
GTClass EncryptedFloat {
   private:
    float enc_first;
    float decrypted; //this is the straight up value for other players, they dont have encrypted values
    float enc_second;
    float weight;

   public:
    //basically gt sets weight to random float between 3 and 29384
    //enc_half has 0.5*weight + the actual value
    //enc         has weight + the actual value
    //gt checks both enc_first and enc_second and the weight in their anti hack checks and so on

    float get(bool local = true) {
        return local ? (enc_second - weight) : decrypted;
    }
    //this is just smth i used for my testing to make sure everythings right, gt also uses this formula to deduce hacks
    float bakery() {
        return enc_second - ((enc_first - weight * 0.5f) + weight);
    }
    void set(float value, bool local = true) {
        if (local) {
            enc_second = weight + value;
            enc_first = (weight * 0.5f) + value;
        }
        else
            decrypted = value;
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
GTClass NetAvatar { //how fucking annoying it is to get align to work
   public:
    void* vtable;
    CL_Vec2f pos;
    CL_Vec2f size;
    CL_Vec2f pos_enc; //use this one for local
    CL_Vec2f size_enc;
    std::string name;
    uint32_t netid;
    uint32_t destroy;
    EntityComponent* entitycomponent;
    void* unk0;
    byte unk1;
    byte facing_left;
    char pad[2];
    void* subpixelmemory;
    char pad0a[20];
    EncryptedFloat punch_cooldown;
    uint32_t skin_color;
    char pad1a[20];
    std::string unkstr4; //didnt find anything about this
    float jump_progress;
    float unk3;
    uint32_t tile_x;
    uint32_t tile_y;
    char unk02[12];
    EncryptedFloat velocity_x; //could be fun to do all kinds of things with messing with these values
    EncryptedFloat velocity_y;
    char unk6[8];
    int emotion;
    float emotion_time;
    char unk7[4];
    void* NetControllerLocal; //280
    void* AvatarPacketReceiver;
    void* AvatarPacketSender;
    bool jump_state;
    char unk7a[3];
    uint32_t flags;
    int freeze_state;
    int userid;
    char unk7b[16];
    std::string country;
    char unk8[84];
    EncryptedFloat gravity;
    EncryptedFloat accel;
    EncryptedFloat speed;
    float water_speed;
    int unk_smth;
    EncryptedFloat punch_strength; //the one which dragon and etc increases
    int unk50;
    std::string unkstr1; //these get set by NetAvatar::OnWepSfx, but i have no clue what that is either
    std::string unkstr2;
    std::string unkstr3;
    char unk10[16];
    int client_hack_type; //ban report type for SendMessageT4

    //TODO: somehow differentiate between local and others so you can use SetPos, GetPos for other players too
    //(since they dont use encrypted coords too)

    void SetPos(const CL_Vec2f& n) {
        SetPos(n.x, n.y);
    }
    void SetPos(float x, float y) {
        pos.x = x;
        pos.y = y;
        pos_enc.x = x / 6.5999999;
        pos_enc.y = y / 1.3;
    }
    void SetPosAtTile(int x, int y) {
        SetPos(x * 32 + 8, y * 32);
    }
#if __has_include("code1.txt")
    //Promised my friend to keep this a secret for now, so I'm not adding this until its public knowledge
    void do_stuff2(int x, int y) {
        #include "code1.txt"
    }
#endif
    //for local only
    CL_Vec2f GetPos() {
        //returning the encrypted one cuz its 100% what the server has
        return CL_Vec2f(pos_enc.x * 6.5999999f, pos_enc.y * 1.3f);
    }
    void SetSize(const CL_Vec2f& n) {
        SetSize(n.x, n.y);
    }
    void SetSize(float x, float y) {
        size.x = x;
        size.y = y;
        size_enc.x = x / 6.5999999;
        size_enc.y = y / 1.3;
    }

    CL_Vec2f GetSize() {
        return size;
    }

    void SetModStatus(bool mod, bool supermod) {
        *(bool*)(uintptr_t(this) + 377) = mod;
        *(bool*)(uintptr_t(this) + 378) = supermod;

        //for now OnDataConfig for some reason sets 378th bit as true even though if its not, idk why
        /*  static std::vector<const char*> ondataconfig_patt{ "C7 43 ?? 05 00 00 00 C7 43 ?? 00", "40 0F ?? ?? 83", "0F 95 C0 40" };
        static auto OnDataConfig = types::OnDataConfig(utils::find_func_has(ondataconfig_patt, 250));
        
        printf("dataconfig (EXACT): %llx\n", (uintptr_t)OnDataConfig);*/
        /* variantlist_t va{};
        va[0] = uint32_t(mod);
        va[1] = uint32_t(supermod);

        OnDataConfig(this, &va);*/
    }
};
#pragma pack(pop)
