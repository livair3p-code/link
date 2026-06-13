#include "Files/Helper/Includes.h"

#define PKG_GL "com.tencent.ig"
#define PKG_VNG "com.vng.pubgmobile"
#define PKG_KR "com.pubg.krmobile"
#define PKG_TW "com.rekoo.pubgm"
#define PKG_IN "com.pubg.imobile"

bool isAPKRunning(const char* packageName) {
    DIR* dir = opendir("/proc/");
    if (!dir) return false;
    struct dirent* ptr;
    char filepath[50], filetext[128];
    while ((ptr = readdir(dir)) != nullptr) {
        if (ptr->d_type != DT_DIR) continue;
        snprintf(filepath, sizeof(filepath), "/proc/%s/cmdline", ptr->d_name);
        FILE* fp = fopen(filepath, "r");
        if (fp) {
            fgets(filetext, sizeof(filetext), fp);
            fclose(fp);
            if (strcmp(filetext, packageName) == 0) {
                closedir(dir);
                return true;
            }
        }
    }
    closedir(dir);
    return false;
}

const char* GetPackageName() {
    if (isAPKRunning(PKG_GL)) return PKG_GL;
    if (isAPKRunning(PKG_VNG)) return PKG_VNG;
    if (isAPKRunning(PKG_KR)) return PKG_KR;
    if (isAPKRunning(PKG_TW)) return PKG_TW;
    if (isAPKRunning(PKG_IN)) return PKG_IN;
    return "unknown pkg";
}

int GetAndroidSdkVersion() {
    int sdk = 0;
    FILE *fp = fopen("/system/build.prop", "r");
    if (fp) {
        char line[128];
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "ro.build.version.sdk=%d", &sdk) == 1) {
                break;
            }
        }
        fclose(fp);
    }
    return sdk;
}

using namespace std;
namespace settings {
    inline ImVec4 particleColour = ImVec4(255, 255, 255, 255);
}
 
int32_t f_mprotect(uintptr_t addr, size_t len, int32_t prot) {
    size_t page_size = static_cast<size_t>(4096);
    void* start = reinterpret_cast<void*>(addr & -page_size);
    uintptr_t end = (addr + len + page_size - 1) & -page_size;
    return mprotect((void *)start, end - reinterpret_cast<uintptr_t>(start), prot);
}

static int XSuits;
bool Marcoo = false;
int AutoTargetChoose = 0;
ImVec4 accentcolor = ImColor(122, 41, 39);
ImColor frame = (accentcolor);

namespace Active {
    inline bool SpinBot = false;
    inline float RotatorSpinBot = 0.0f;
    inline bool SpinW = false;
    inline float SpinWeapon = 0.0f;
    inline float RotatorSpinBotCar = 0;
    inline bool SpinBotCar = false;
    inline int SkinCarDefault = 0;
    inline int SkinCarMod = 0;
    inline int SkinCarNew = 0;
}

#include <ctime>
bool infinitycar;
bool SPINCHECK;
float SpinCar360;
bool WallHackCar;
using json = nlohmann::json;
using namespace SDK;
static std::unordered_set<uint32_t> AlreadyChangedSet;

uintptr_t GetVirtualFunctionAddress(uintptr_t clazz, uintptr_t index) {
    if (!clazz) return 0;
    uintptr_t vtablePtr = *(uintptr_t*)clazz;
    if (!vtablePtr) return 0;
    if (index < 0) return 0;
    return *((uintptr_t*)vtablePtr + index);
}

void ChangeItemAVc(uintptr_t thiz, int InItemID) {
    if (thiz) {
        auto PrechangeitemAvatar_addr = GetVirtualFunctionAddress(thiz, 184);
        if (PrechangeitemAvatar_addr) {
            return ((void(*)(uintptr_t, int, bool))PrechangeitemAvatar_addr)(thiz, InItemID, true);
        }
    }
}

#include <vector>
#include <unordered_set>

static UEngine* GEngine = nullptr;
static USTExtraGameInstance* ExtraGameInstance = nullptr;
static UWorld* GWorld = nullptr;
static ULocalPlayer* LocalPlayerInstance = nullptr;
static std::unordered_set<AActor*> UniqueActors;
static std::vector<AActor*> CachedActors;

UWorld* GetWorld() {
    if (!GEngine) {
        GEngine = UObject::FindObject<UEngine>("UAEGameEngine Transient.UAEGameEngine_1");
    }
    if (!GEngine) return nullptr;
    if (GEngine->GameViewport && GEngine->GameViewport->World) {
        if (!ExtraGameInstance) {
            ExtraGameInstance = UObject::FindObject<USTExtraGameInstance>("STExtraGameInstance Transient.UAEGameEngine_1.STExtraGameInstance_1");
        }
        GWorld = GEngine->GameViewport->World;
        return GWorld;
    }
    if (!LocalPlayerInstance) {
        LocalPlayerInstance = UObject::FindObject<ULocalPlayer>("LocalPlayer Transient.UAEGameEngine_1.LocalPlayer_1");
    }
    if (LocalPlayerInstance) {
        auto ViewPort = LocalPlayerInstance->ViewportClient;
        if (ViewPort) {
            return ViewPort->World;
        }
    }
    return nullptr;
}

static UGameViewportClient *GameViewport = nullptr;
UGameViewportClient *GetGameViewport() {
    while (!GameViewport) {
        GameViewport = UObject::FindObject<UGameViewportClient>("GameViewportClient Transient.UAEGameEngine_1.GameViewportClient_1");
        sleep(1);
    }
    return GameViewport;
}

std::chrono::steady_clock::time_point lastChangeTime;
std::chrono::steady_clock::time_point lastWeaponChangeTime;

ImGuiWindow* g_window = NULL;
bool initImGui = false;
int screenWidth = 0, glWidth, screenHeight = 0, glHeight;
float density = -1;
std::string g_Token, g_Auth;
bool bValid = false;
#define IM_PI 3.14159265358979323846f
#define RAD2DEG(x) ((float)(x) * (float)(180.f / IM_PI))
#define DEG2RAD(x) ((float)(x) * (float)(IM_PI / 180.f))

auto start = std::chrono::steady_clock::now();
bool TotalEnemy = true;
std::vector<FVector> BombLocation = {};
std::vector<FVector> LineBomb = {};

struct sRegion {
    uintptr_t start, end;
};

std::vector<sRegion> trapRegions;
int Style;

std::string GetRedZone(std::string Name) {
    if (Name.find("BP_AirAttack_C") != std::string::npos) return "Bomb";
    if (Name.find("AirAttackBomb_Livik_C") != std::string::npos || Name.find("AirAttackBomb_C") != std::string::npos) return "-";
    return "Unknown";
}

int32_t (*orig_ANativeWindow_getWidth)(ANativeWindow *window);
int32_t _ANativeWindow_getWidth(ANativeWindow *window) {
    screenWidth = orig_ANativeWindow_getWidth(window);
    return orig_ANativeWindow_getWidth(window);
}

int32_t (*orig_ANativeWindow_getHeight)(ANativeWindow *window);
int32_t _ANativeWindow_getHeight(ANativeWindow *window) {
    screenHeight = orig_ANativeWindow_getHeight(window);
    return orig_ANativeWindow_getHeight(window);
}

UWorld *GetFullWorld() {
    while (!GEngine) {
        GEngine = UObject::FindObject<UEngine>("UAEGameEngine Transient.UAEGameEngine_1"); 
        sleep(1);
    }
    if (GEngine) {
        auto ViewPort = GEngine->GameViewport;
        if (ViewPort) {
            return ViewPort->World;
        }
    }
    return 0;
}

#include <atomic>
TNameEntryArray * GetGNames() {
    return ((TNameEntryArray * ( *)())(Data::Cheat::UE4 + Data::Offset::GNames_Offset))();
}

std::string getObjectPath(UObject* Object) {
    std::string s;
    for (auto super = Object->ClassPrivate; super; super = (UClass*)super->SuperStruct) {
        if (!s.empty())
            s += ".";
        s += super->NamePrivate.GetName();
    }
    return s;
}

bool EnemyAiming = false;
bool checkaiming = false;

bool isObjectInvalid(UObject *obj) {
    if (!Tools::IsPtrValid(obj)) return true;
    if (!Tools::IsPtrValid(obj->ClassPrivate)) return true;
    if (obj->InternalIndex <= 0) return true;
    if (obj->NamePrivate.ComparisonIndex <= 0) return true;
    if ((uintptr_t)(obj) % sizeof(uintptr_t) != 0x0 && (uintptr_t)(obj) % sizeof(uintptr_t) != 0x4) return true;
    if (std::any_of(trapRegions.begin(), trapRegions.end(), [obj](sRegion region) { return ((uintptr_t) obj) >= region.start && ((uintptr_t) obj) <= region.end; }) ||
        std::any_of(trapRegions.begin(), trapRegions.end(), [obj](sRegion region) { return ((uintptr_t) obj->ClassPrivate) >= region.start && ((uintptr_t) obj->ClassPrivate) <= region.end; })) {
        return true;
    }
    return false;
}

std::vector<AActor *> NecessaryActors;
std::mutex necessaryActorsMutex;

struct ActorArray {
    uintptr_t base; 
    int count; 
    int max; 
};

std::vector<AActor*>& GetNecessaryActors() {
    static std::vector<AActor*> result;
    result.clear();
    UWorld* world = GetWorld();
    if (!world || !Tools::IsPtrValid(world->PersistentLevel)) return result;
    auto func = reinterpret_cast<ActorArray* (*)(uintptr_t)>(Data::Cheat::UE4 + Data::Offset::GetActorArray);
    ActorArray* array = func ? func((uintptr_t)world->PersistentLevel) : nullptr;
    if (!array || array->count <= 0 || !array->base) return result;
    AActor** raw = reinterpret_cast<AActor**>(array->base);
    for (int i = 0; i < array->count && result.size() < 512; ++i) {
        AActor* actor = raw[i];
        if (!actor || isObjectInvalid(actor)) continue;
        if (actor->IsA(ASTExtraPlayerCharacter::StaticClass())) {
            auto* player = static_cast<ASTExtraPlayerCharacter*>(actor);
            if (!player || player->bDead) continue;
            result.push_back(actor);
        } else if (actor->IsA(ASTExtraPlayerController::StaticClass())) {
            result.push_back(actor);
        } else if (actor->IsA(ASTExtraGrenadeBase::StaticClass())) {
            result.push_back(actor);
        } else if (actor->IsA(APickUpListWrapperActor::StaticClass())) {
            result.push_back(actor);
        } else if (actor->IsA(ASTExtraVehicleBase::StaticClass())) {
            result.push_back(actor);
        }   
    }
    return result;
}

static std::unordered_map<std::string, bool> isDumped;

void ActorDump() {
    auto Actors = *(((TArray<AActor *> * (*)(uintptr_t))(Data::Cheat::UE4 + Data::Offset::GetActorArray))(reinterpret_cast<uintptr_t>(GetFullWorld()->PersistentLevel)));
    for (int i = 0; i < Actors.Num(); i++) {
        auto Actor = Actors[i];
        if (Actor) {
            if (!isDumped[Actor->NamePrivate.GetName()]) {
                FILE *fp = fopen("/sdcard/Android/obb/com.tencent.ig/ActorDump.txt", "a");
                fprintf(fp, "ObjectFlags(%d), InternalIndex(%d), NamePrivate(%s), FullName(%s)\n", 
                        Actor->ObjectFlags, Actor->InternalIndex, Actor->NamePrivate.GetName(), Actor->GetFullName().c_str());
                fclose(fp);
                isDumped[Actor->NamePrivate.GetName()] = true;
            }
        }
    }
}

ImFont *pRegularFont;
ImFont *Icon;

struct TeamColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

ImU32 Black = IM_COL32(0, 0, 0, 255);
ImU32 Green = IM_COL32(0, 255, 0, 255);
ImU32 Yellow = IM_COL32(255, 255, 0, 255);
ImU32 Red = IM_COL32(255, 0, 0, 255);
ImU32 Red2 = IM_COL32(255, 0, 0, 255);
ImU32 White = IM_COL32(255, 255, 255, 255);

struct VECTOR3 {
    float X;
    float Y;
    float Z;
};

ImDrawList* pImGuiDraw;

void DrawBorderString(ImFont *pFont, float FontSize, string Text, float X, float Y, ImColor Color, bool Center) {
    pImGuiDraw = ImGui::GetBackgroundDrawList();
    ImVec2 TextSize = pFont->CalcTextSizeA(FontSize, FLT_MAX, 0.0f, Text.c_str());
    pImGuiDraw->AddText(pFont, FontSize, ImVec2(X - TextSize.x / 2.0f, Y), Color, Text.c_str());
}

struct Matrix {
    float m[4][4];
};

Matrix GetMatrix() {
    static UCanvas* Canvas = nullptr;
    if (Canvas == nullptr) {
        Canvas = UObject::FindObject<UCanvas>("Canvas Transient.DebugCanvasObject");
    }
    if (!Canvas) return Matrix{};
    uintptr_t lodalele;
#if defined(__LP64__)
    lodalele = 0x270;
#else
    lodalele = 0x200;
#endif
    return *(Matrix*)((uintptr_t)Canvas + lodalele);
}

bool WorldToScreenPlayer(FVector pos, FVector &screen, int *distance) {
    Matrix ViewMatrix = GetMatrix();
    screen = {};
    *distance = 0;
    float screenW = (ViewMatrix.m[0][3] * pos.X) + (ViewMatrix.m[1][3] * pos.Y) + (ViewMatrix.m[2][3] * pos.Z + ViewMatrix.m[3][3]);
    if (screenW < 0.0001f) return false;
    *distance = (screenW / 100);
    float screenY = (ViewMatrix.m[0][1] * pos.X) + (ViewMatrix.m[1][1] * pos.Y) + (ViewMatrix.m[2][1] * (pos.Z + 85) + ViewMatrix.m[3][1]);
    float screenX = (ViewMatrix.m[0][0] * pos.X) + (ViewMatrix.m[1][0] * pos.Y) + (ViewMatrix.m[2][0] * pos.Z + ViewMatrix.m[3][0]);
    screen.Y = (glHeight / 2.f) - (glHeight / 2.f) * screenY / screenW;
    screen.X = (glWidth / 2.f) + (glWidth / 2.f) * screenX / screenW;
    float y1 = (glHeight / 2.f) - (ViewMatrix.m[0][1] * pos.X + ViewMatrix.m[1][1] * pos.Y + ViewMatrix.m[2][1] * (pos.Z - 95) + ViewMatrix.m[3][1]) * (glHeight / 2.f) / screenW;
    screen.Z = y1 - screen.Y;
    return !(screen.X == 0 && screen.Y == 0 && screen.Z == 0);
}

bool WorldToScreenBone(FVector Position, FVector2D *Screen) {
    Matrix ViewMatrix = GetMatrix();
    float ScreenW = (ViewMatrix.m[0][3] * Position.X) + (ViewMatrix.m[1][3] * Position.Y) + (ViewMatrix.m[2][3] * Position.Z + ViewMatrix.m[3][3]);
    if (ScreenW < 0.0001f) return false;
    ScreenW = 1 / ScreenW;
    float SightX = (glWidth / 2);
    float SightY = (glHeight / 2);
    Screen->X = SightX + (ViewMatrix.m[0][0] * Position.X + ViewMatrix.m[1][0] * Position.Y + ViewMatrix.m[2][0] * Position.Z + ViewMatrix.m[3][0]) * ScreenW * SightX;
    Screen->Y = SightY - (ViewMatrix.m[0][1] * Position.X + ViewMatrix.m[1][1] * Position.Y + ViewMatrix.m[2][1] * Position.Z + ViewMatrix.m[3][1]) * ScreenW * SightY;
    return true;
}

bool WorldToScreen(FVector Position, FVector& Screen, int* Distance) {
    Matrix ViewMatrix = GetMatrix();
    float ScreenW = (ViewMatrix.m[0][3] * Position.X) + (ViewMatrix.m[1][3] * Position.Y) + (ViewMatrix.m[2][3] * Position.Z + ViewMatrix.m[3][3]);
    *Distance = (ScreenW / 100);
    if (ScreenW < 0.0001f) return false;
    ScreenW = 1 / ScreenW;
    float SightX = (glWidth / 2);
    float SightY = (glHeight / 2);
    Screen.X = SightX + (ViewMatrix.m[0][0] * Position.X + ViewMatrix.m[1][0] * Position.Y + ViewMatrix.m[2][0] * Position.Z + ViewMatrix.m[3][0]) * ScreenW * SightX;
    Screen.Y = SightY - (ViewMatrix.m[0][1] * Position.X + ViewMatrix.m[1][1] * Position.Y + ViewMatrix.m[2][1] * Position.Z + ViewMatrix.m[3][1]) * ScreenW * SightY;
    return true;
}

void DrawFilledRectangle(float X, float Y, float Width, float Height, ImColor Color) {
    pImGuiDraw = ImGui::GetBackgroundDrawList();
    pImGuiDraw->AddRectFilled(ImVec2(X, Y), ImVec2(X - Width, Y - Height), Color);
}
      
void test_DrawLine(ImDrawList* pImGuiDraw, float X1, float Y1, float X2, float Y2, ImColor Color, float Thickness) {
    pImGuiDraw->AddLine(ImVec2(X1, Y1), ImVec2(X2, Y2), Color, Thickness);
}

void CornerBox(ImDrawList* pImGuiDraw, int x, int y, int w, int h, int thickness, float cornered, ImColor color) {
    test_DrawLine(pImGuiDraw, x, y, x + (w * cornered), y, color, thickness);
    test_DrawLine(pImGuiDraw, x, y, x, y + (h * cornered), color, thickness);
    test_DrawLine(pImGuiDraw, x, y + h, x + (w * cornered), y + h, color, thickness);
    test_DrawLine(pImGuiDraw, x, y + h, x, (y + h) - (h * cornered), color, thickness);
    test_DrawLine(pImGuiDraw, x + w, y, (x + w) - (w * cornered), y, color, thickness);
    test_DrawLine(pImGuiDraw, x + w, y, x + w, y + (h * cornered), color, thickness);
    test_DrawLine(pImGuiDraw, x + w, y + h, (x + w) - (w * cornered), y + h, color, thickness);
    test_DrawLine(pImGuiDraw, x + w, y + h, x + w, (y + h) - (h * cornered), color, thickness);
}

bool fileExists(const std::string &filePath) {
    std::ifstream file(filePath);
    return file.good();
}

void rect_filled(float X, float Y, float Width, float Height, ImColor Color) {
    pImGuiDraw = ImGui::GetBackgroundDrawList();
    pImGuiDraw->AddRectFilled(ImVec2(X, Y), ImVec2(X - Width, Y - Height), Color);
}

void rect_filled_2(float X, float Y, float Width, float Height, ImColor Color) {
    pImGuiDraw = ImGui::GetBackgroundDrawList();
    pImGuiDraw->AddRectFilled(ImVec2(X, Y), ImVec2(X + Width, Y + Height), Color);
}

#define IM_CLAMP(V, MN, MX) ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

namespace Settings {
    static int Tab = 1;
}

static bool VR = false;

void Theme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8, 8);
    style.WindowRounding = 3.5f;
    style.FramePadding = ImVec2(4, 4);
    style.ScrollbarSize = 0.0f;
    style.FrameRounding = 3.5f;
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.GrabMinSize = 20.f;
    style.WindowTitleAlign = ImVec2(0.5, 0.5);
    style.ButtonTextAlign = ImVec2(0.5,0.5);
    style.Colors[ImGuiCol_WindowBg]              = ImColor(0, 0, 0, 190);
    style.Colors[ImGuiCol_FrameBg]               = ImColor(0, 0, 0, 215);
    style.Colors[ImGuiCol_FrameBgActive]         = ImColor(0, 0, 0, 215);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImColor(0, 0, 0, 255);
    style.Colors[ImGuiCol_PopupBg]               = ImColor(122, 41, 39);
    style.Colors[ImGuiCol_Button]                = ImColor(122, 41, 39);
    style.Colors[ImGuiCol_ButtonActive]          = ImColor(122, 41, 39);
    style.Colors[ImGuiCol_ButtonHovered]         = ImColor(122, 41, 39);
    style.Colors[ImGuiCol_TitleBg]               = ImColor(122, 41, 39);
    style.Colors[ImGuiCol_TitleBgActive]         = ImColor(122, 41, 39);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImColor(122, 41, 39);
    style.Colors[ImGuiCol_CheckMark]             = ImColor(0, 255, 0, 255);
    style.Colors[ImGuiCol_SliderGrab]            = ImColor(122, 41, 39);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImColor(122, 41, 39);
}

void AddUnderLine(ImColor col_) {
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    min.y = max.y;
    ImGui::GetWindowDrawList()->AddLine(min, max, col_, 1.0f);
}

void linevertical() {
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();
}

bool iLogin = true;
#define SLEEP_TIME 1000LL / 90LL
static bool opened = true;
static bool activehack = true;
bool ifismembotenable = false;
int selectedTab = 1;
using namespace ImGui;

char buffer[256];
const char* text = "Welcome to Mod Menu Cheat, please login first...";
int length = strlen(text);
float timer = 0.0f;
int index = 0;

#include <GLES3/gl3.h>
#include <GLES2/gl2.h>

size_t WriteMemoryCallback1(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    std::vector<unsigned char> *mem = (std::vector<unsigned char> *)userp;
    unsigned char *ptr = (unsigned char*)contents;
    mem->insert(mem->end(), ptr, ptr + realsize);
    return realsize;
}

#include <dirent.h>
//
// Created by Administrator on 3/4/2024.
//

#ifndef NTH_SKIN_H
#define NTH_SKIN_H
#define ID_AKM 101001
#define ID_AKM1 1010011
#define ID_AKM2 1010012
#define ID_AKM3 1010013
#define ID_AKM4 1010014
#define ID_AKM5 1010015
#define ID_AKM6 1010016
#define ID_AKM7 1010017

#define ID_M16A4 101002
#define ID_M16A41 1010021
#define ID_M16A42 1010022
#define ID_M16A43 1010023
#define ID_M16A44 1010024
#define ID_M16A45 1010025
#define ID_M16A46 1010026
#define ID_M16A47 1010027

#define ID_SCARL 101003
#define ID_SCARL1 1010031
#define ID_SCARL2 1010032
#define ID_SCARL3 1010033
#define ID_SCARL4 1010034
#define ID_SCARL5 1010035
#define ID_SCARL6 1010036
#define ID_SCARL7 1010037

#define ID_M416 101004
#define ID_M4161 1010041
#define ID_M4162 1010042
#define ID_M4163 1010043
#define ID_M4164 1010044
#define ID_M4165 1010045
#define ID_M4166 1010046
#define ID_M4167 1010047

#define ID_GROZA 101005
#define ID_GROZA1 1010051
#define ID_GROZA2 1010052
#define ID_GROZA3 1010053
#define ID_GROZA4 1010054
#define ID_GROZA5 1010055
#define ID_GROZA6 1010056
#define ID_GROZA7 1010057

#define ID_AUG 101006
#define ID_AUG1 1010061
#define ID_AUG2 1010062
#define ID_AUG3 1010063
#define ID_AUG4 1010064
#define ID_AUG5 1010065
#define ID_AUG6 1010066
#define ID_AUG7 1010067

#define ID_QBZ 101007
#define ID_QBZ1 1010071
#define ID_QBZ2 1010072
#define ID_QBZ3 1010073
#define ID_QBZ4 1010074
#define ID_QBZ5 1010075
#define ID_QBZ6 1010076
#define ID_QBZ7 1010077

#define ID_M762 101008
#define ID_M7621 1010081
#define ID_M7622 1010082
#define ID_M7623 1010083
#define ID_M7624 1010084
#define ID_M7625 1010085
#define ID_M7626 1010086
#define ID_M7627 1010087

#define ID_MK47 101009
#define ID_MK471 1010091
#define ID_MK472 1010092
#define ID_MK473 1010093
#define ID_MK474 1010094
#define ID_MK475 1010095
#define ID_MK476 1010096
#define ID_MK477 1010097

#define ID_G36C 101010
#define ID_G36C1 1010101
#define ID_G36C2 1010102
#define ID_G36C3 1010103
#define ID_G36C4 1010104
#define ID_G36C5 1010105
#define ID_G36C6 1010106
#define ID_G36C7 1010107

#define ID_ACVAL 101011
#define ID_ACVAL1 1010111
#define ID_ACVAL2 1010112
#define ID_ACVAL3 1010113
#define ID_ACVAL4 1010114
#define ID_ACVAL5 1010115
#define ID_ACVAL6 1010116
#define ID_ACVAL7 1010117

#define ID_HONEY_BADGER 101012
#define ID_HONEY_BADGER1 1010121
#define ID_HONEY_BADGER2 1010122
#define ID_HONEY_BADGER3 1010123
#define ID_HONEY_BADGER4 1010124
#define ID_HONEY_BADGER5 1010125
#define ID_HONEY_BADGER6 1010126
#define ID_HONEY_BADGER7 1010127

#define ID_FAMAS 101100
#define ID_FAMAS1 1011001
#define ID_FAMAS2 1011002
#define ID_FAMAS3 1011003
#define ID_FAMAS4 1011004
#define ID_FAMAS5 1011005
#define ID_FAMAS6 1011006
#define ID_FAMAS7 1011007

#define ID_ASM 101101
#define ID_ASM1 1011011
#define ID_ASM2 1011012
#define ID_ASM3 1011013
#define ID_ASM4 1011014
#define ID_ASM5 1011015
#define ID_ASM6 1011016
#define ID_ASM7 1011017

#define ID_ACE32 101102
#define ID_ACE321 1011021
#define ID_ACE322 1011022
#define ID_ACE323 1011023
#define ID_ACE324 1011024
#define ID_ACE325 1011025
#define ID_ACE326 1011026
#define ID_ACE327 1011027

#define ID_UZI 102001
#define ID_UZI1 1020011
#define ID_UZI2 1020012
#define ID_UZI3 1020013
#define ID_UZI4 1020014
#define ID_UZI5 1020015
#define ID_UZI6 1020016
#define ID_UZI7 1020017

#define ID_UMP45 102002
#define ID_UMP451 1020021
#define ID_UMP452 1020022
#define ID_UMP453 1020023
#define ID_UMP454 1020024
#define ID_UMP455 1020025
#define ID_UMP456 1020026
#define ID_UMP457 1020027

#define ID_VECTOR 102003
#define ID_VECTOR1 1020031
#define ID_VECTOR2 1020032
#define ID_VECTOR3 1020033
#define ID_VECTOR4 1020034
#define ID_VECTOR5 1020035
#define ID_VECTOR6 1020036
#define ID_VECTOR7 1020037

#define ID_THOMPSON 102004
#define ID_THOMPSON1 1020041
#define ID_THOMPSON2 1020042
#define ID_THOMPSON3 1020043
#define ID_THOMPSON4 1020044
#define ID_THOMPSON5 1020045
#define ID_THOMPSON6 1020046
#define ID_THOMPSON7 1020047

#define ID_PP19 102005
#define ID_PP191 1020051
#define ID_PP192 1020052
#define ID_PP193 1020053
#define ID_PP194 1020054
#define ID_PP195 1020055
#define ID_PP196 1020056
#define ID_PP197 1020057

#define ID_MP5K 102007
#define ID_MP5K1 1020071
#define ID_MP5K2 1020072
#define ID_MP5K3 1020073
#define ID_MP5K4 1020074
#define ID_MP5K5 1020075
#define ID_MP5K6 1020076
#define ID_MP5K7 1020077

#define ID_P90 102105
#define ID_P901 1021051
#define ID_P902 1021052
#define ID_P903 1021053
#define ID_P904 1021054
#define ID_P905 1021055
#define ID_P906 1021056
#define ID_P907 1021057

#define ID_KAR98K 103001
#define ID_KAR98K1 1030011
#define ID_KAR98K2 1030012
#define ID_KAR98K3 1030013
#define ID_KAR98K4 1030014
#define ID_KAR98K5 1030015
#define ID_KAR98K6 1030016
#define ID_KAR98K7 1030017

#define ID_M24 103002
#define ID_M241 1030021
#define ID_M242 1030022
#define ID_M243 1030023
#define ID_M244 1030024
#define ID_M245 1030025
#define ID_M246 1030026
#define ID_M247 1030027

#define ID_AWM 103003
#define ID_AWM1 1030031
#define ID_AWM2 1030032
#define ID_AWM3 1030033
#define ID_AWM4 1030034
#define ID_AWM5 1030035
#define ID_AWM6 1030036
#define ID_AWM7 1030037

#define ID_SKS 103004
#define ID_SKS1 1030041
#define ID_SKS2 1030042
#define ID_SKS3 1030043
#define ID_SKS4 1030044
#define ID_SKS5 1030045
#define ID_SKS6 1030046
#define ID_SKS7 1030047

#define ID_VSS 103005
#define ID_VSS1 1030051
#define ID_VSS2 1030052
#define ID_VSS3 1030053
#define ID_VSS4 1030054
#define ID_VSS5 1030055
#define ID_VSS6 1030056
#define ID_VSS7 1030057

#define ID_MINI14 103006
#define ID_MINI141 1030061
#define ID_MINI142 1030062
#define ID_MINI143 1030063
#define ID_MINI144 1030064
#define ID_MINI145 1030065
#define ID_MINI146 1030066
#define ID_MINI147 1030067

#define ID_MK14 103007
#define ID_MK141 1030071
#define ID_MK142 1030072
#define ID_MK143 1030073
#define ID_MK144 1030074
#define ID_MK145 1030075
#define ID_MK146 1030076
#define ID_MK147 1030077

#define ID_WIN94 103008
#define ID_WIN941 1030081
#define ID_WIN942 1030082
#define ID_WIN943 1030083
#define ID_WIN944 1030084
#define ID_WIN945 1030085
#define ID_WIN946 1030086
#define ID_WIN947 1030087

#define ID_SLR 103009
#define ID_SLR1 1030091
#define ID_SLR2 1030092
#define ID_SLR3 1030093
#define ID_SLR4 1030094
#define ID_SLR5 1030095
#define ID_SLR6 1030096
#define ID_SLR7 1030097

#define ID_QBU 103010
#define ID_QBU1 1030101
#define ID_QBU2 1030102
#define ID_QBU3 1030103
#define ID_QBU4 1030104
#define ID_QBU5 1030105
#define ID_QBU6 1030106
#define ID_QBU7 1030107

#define ID_MOSIN_NAGANT 103011
#define ID_MOSIN_NAGANT1 1030111
#define ID_MOSIN_NAGANT2 1030112
#define ID_MOSIN_NAGANT3 1030113
#define ID_MOSIN_NAGANT4 1030114
#define ID_MOSIN_NAGANT5 1030115
#define ID_MOSIN_NAGANT6 1030116
#define ID_MOSIN_NAGANT7 1030117

#define ID_AMR 103012
#define ID_AMR1 1030121
#define ID_AMR2 1030122
#define ID_AMR3 1030123
#define ID_AMR4 1030124
#define ID_AMR5 1030125
#define ID_AMR6 1030126
#define ID_AMR7 1030127

#define ID_MK12 103100
#define ID_MK121 1031001
#define ID_MK122 1031002
#define ID_MK123 1031003
#define ID_MK124 1031004
#define ID_MK125 1031005
#define ID_MK126 1031006
#define ID_MK127 1031007

#define ID_S686 104001
#define ID_S6861 1040011
#define ID_S6862 1040012
#define ID_S6863 1040013
#define ID_S6864 1040014
#define ID_S6865 1040015
#define ID_S6866 1040016
#define ID_S6867 1040017

#define ID_S1897 104002
#define ID_S18971 1040021
#define ID_S18972 1040022
#define ID_S18973 1040023
#define ID_S18974 1040024
#define ID_S18975 1040025
#define ID_S18976 1040026
#define ID_S18977 1040027

#define ID_S12K 104003
#define ID_S12K1 1040031
#define ID_S12K2 1040032
#define ID_S12K3 1040033
#define ID_S12K4 1040034
#define ID_S12K5 1040035
#define ID_S12K6 1040036
#define ID_S12K7 1040037

#define ID_DBS 104004
#define ID_DBS1 1040041
#define ID_DBS2 1040042
#define ID_DBS3 1040043
#define ID_DBS4 1040044
#define ID_DBS5 1040045
#define ID_DBS6 1040046
#define ID_DBS7 1040047

#define ID_SPAS12 104100
#define ID_SPAS121 1041001
#define ID_SPAS122 1041002
#define ID_SPAS123 1041003
#define ID_SPAS124 1041004
#define ID_SPAS125 1041005
#define ID_SPAS126 1041006
#define ID_SPAS127 1041007

#define ID_M1014 104101
#define ID_M10141 1041011
#define ID_M10142 1041012
#define ID_M10143 1041013
#define ID_M10144 1041014
#define ID_M10145 1041015
#define ID_M10146 1041016
#define ID_M10147 1041017

#define ID_NS2000 104102
#define ID_NS20001 1041021
#define ID_NS20002 1041022
#define ID_NS20003 1041023
#define ID_NS20004 1041024
#define ID_NS20005 1041025
#define ID_NS20006 1041026
#define ID_NS20007 1041027

#define ID_M249 105001
#define ID_M2491 1050011
#define ID_M2492 1050012
#define ID_M2493 1050013
#define ID_M2494 1050014
#define ID_M2495 1050015
#define ID_M2496 1050016
#define ID_M2497 1050017

#define ID_DP28 105002
#define ID_DP281 1050021
#define ID_DP282 1050022
#define ID_DP283 1050023
#define ID_DP284 1050024
#define ID_DP285 1050025
#define ID_DP286 1050026
#define ID_DP287 1050027

#define ID_PAN 108004
#define ID_PAN1 1080041
#define ID_PAN2 1080042
#define ID_PAN3 1080043
#define ID_PAN4 1080044
#define ID_PAN5 1080045
#define ID_PAN6 1080046
#define ID_PAN7 1080047

#define ID_MG3 105010
#define ID_MG31 1050101
#define ID_MG32 1050102
#define ID_MG33 1050103
#define ID_MG34 1050104
#define ID_MG35 1050105
#define ID_MG36 1050106
#define ID_MG37 1050107

#define ID_KNIFE 108001
#define ID_KNIFE1 1080011
#define ID_KNIFE2 1080012
#define ID_KNIFE3 1080013
#define ID_KNIFE4 1080014
#define ID_KNIFE5 1080015
#define ID_KNIFE6 1080016
#define ID_KNIFE7 1080017

#define ID_KNIFE1 108005
#define ID_KNIFE11 1080051
#define ID_KNIFE12 1080052
#define ID_KNIFE13 1080053
#define ID_KNIFE14 1080054
#define ID_KNIFE15 1080055
#define ID_KNIFE16 1080056
#define ID_KNIFE17 1080057


#endif //NTH_SKIN_H

//
// Created by Administrator on 3/4/2024.
//

#ifndef HakmeDev_SKIN_DATA_H
#define HakmeDev_SKIN_DATA_H
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <codecvt>
#include <locale>

struct ItemData {
    int itemId;
    std::wstring itemName;
};
static std::unordered_map<int, ItemData> itemDatabase;
static std::unordered_set<int> loggedIDs;
static bool showKeyboard1 = false;
static std::string clipboardText = "";

std::string WStringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

void DumpSkin() {
    auto backpack = (UBackpackUtils*)UBackpackUtils::StaticClass();
    if (backpack) {
        auto items = backpack->GetItemIDs();
        for (int i = 0; i < items.Num(); ++i) {
            int itemId = items[i];
            if (loggedIDs.find(itemId) == loggedIDs.end()) {
                auto itemRecord = backpack->GetItemRecord(itemId);
                ItemData data;
                data.itemId = itemId;
                data.itemName = itemRecord.ItemName.ToWString();
                itemDatabase[itemId] = data;
                loggedIDs.insert(itemId);
            }
        }
    }
}

typedef unsigned long DWORD;
class CModBase
{
};

class CWeapAttack : public CModBase
{
public:
    DWORD Default;
};

class CGunSkinSetting : public CModBase
{
public:
    DWORD AKM = 1101001213;
    DWORD M16A4 = 1101002081;
    DWORD SCARL = 1101003188;
    DWORD M416 = 1101004046;
    DWORD GROZA = 1101005052;
    DWORD AUG = 1101006062;
    DWORD QBZ = 1101007046;
    DWORD M762 = 1101008081;
    DWORD HONEY = 0;
    DWORD ACE32 = 1101102017;
    // smg
    DWORD UZI = 1102001024;
    DWORD UMP45 = 1102002136;
    DWORD VECTOR = 1102003080;
    DWORD TOMMY = 1102004018;
    // snip
    DWORD KAR98 = 1103001179;
    DWORD M24 = 1103002087;
    DWORD AWM = 1103003062;
    DWORD MINI14 = 1103006030;
    DWORD MK14 = 1103007020;
    DWORD AMR = 1103012010;
    // light-machine
    DWORD M249 = 1105001048;
    DWORD DP28 = 1105002063;
    DWORD MG3 = 1101102017;
    // SHOTGUN
    DWORD DBS = 1104101001;
    DWORD S12K = 1104003027;
    DWORD XM1014 = 1104101001;
    // other
    DWORD PAN = 1108004356;
    DWORD P90 = 1102105012;
    DWORD S1897,S686,MK12,QBU,SLR,WIN94,VSS,SKS,MP5K,PP19,FAMAS,ACVAL,G36C,MK47 = 0;
};

class CClothSetting : public CModBase
{
public:
    DWORD SHIRT = 1405628;
    DWORD HAT,FACE,MASK;
    DWORD GLOVES;
    DWORD PANT;
    DWORD SHOE;
    DWORD PARACHUTE;
    DWORD EMOTE1,EMOTE2,EMOTE3;
    DWORD HELMET = 1502003023;
    DWORD HELMET1 = 1502001023,BACKPACK1 = 1501001220, HELMET2 = 1502002023, BACKPACK2 = 1501002220, HELMET3 = 1502003023,BACKPACK3 = 1501003220;
    DWORD BACKPACK = 1501003220;
    DWORD GLIDER;
    DWORD HAE,MOLOTOV,SMOKE;
};

class CVehicleSetting : public CModBase
{
public:
    DWORD MOTOR = 1901047;
    DWORD COUPE = 1961015;
    DWORD UAZ = 1908067;
    DWORD DACIA = 1903075;
    DWORD MIRADO = 1915010;
    DWORD BUGGY = 1907054;
};

class CModSkinSetting
{
public:
    DWORD dwPassword = 0xBEEF;
    bool bEnable = false;
    bool bGunSkin = true;
    bool bClothSkin = true;
    bool bVehicleSkin = true;
    bool bKillMsg = true;
    bool bDeadbox = false;
    bool bMetro = false;
    bool bEmote = false;
    bool bBackPack = false;
    //
    CGunSkinSetting *gun;
    CClothSetting *cloth;
    CVehicleSetting *vehicle;
    CModSkinSetting()
    {
        gun = new CGunSkinSetting();
        cloth = new CClothSetting();
        vehicle = new CVehicleSetting();
    }
};

static CModSkinSetting skin;

struct FSkinItem
{
    std::string name2;
    DWORD value;
    std::string ToStructString()
    {
        // return "{ \"" + name + " \", " + std::to_string(value) + " },";
        return std::to_string(value);
    }
};

extern std::vector<FSkinItem> skinItems, filteredItems;
class CSkinsInfo
{
public:
    DWORD *out;
    bool bFirstInit = false;
    int current_index = 0;
    std::vector<FSkinItem> items;
    CSkinsInfo()
    {
        items = {};
    }
    void Init(std::vector<FSkinItem> _items, DWORD *_out)
    {
        out = _out;
        items = _items;
        if (out && items.size() > 0)
        {
            *out = items[0].value;
        }
    }
    void Init(std::vector<FSkinItem> _items)
    {
        items = _items;
    }
    DWORD At(int index)
    {
        if (index > items.size())
            return 0;
        return items[index].value;
    }
    DWORD Get()
    {
        return At(current_index);
    }
};

class SkinData
{
public:
    CSkinsInfo
        m416,
        m16,
        akm,
        scarl,
        aug,
        m762,
        groza,
        ace32,
        honey,
        qbz;
    CSkinsInfo
        ump45,
        vector,
        tommy,
        uzi;
    CSkinsInfo
        kar98,
        m24,
        awm,
        amr,
        mk14,
        mini14,
        m249,
        dp28,
        mg3,
        pan,        
s1897,
s686,
mk12,
qbu,
slr,
win94,
vss,
sks,
mp5k,
pp19,
famas,
acval,
g36c,
mk47,
p90;
    CSkinsInfo
        s12k,
        xm1014,
        dbs;
    CSkinsInfo
        cloth,
        hat,
        face,
        mask,
        pant,
        shoe,
        gloves,
        parachute,
        glider,
        backpack,
        helmet,
        emote1,
        emote2,
        emote3,
        hae,molotov,smoke,
        coupe,
        uaz,
        dacia,
        motor,
        buggy,
        mirado,
        backpack1, helmet1, backpack2,helmet2, backpack3, helmet3;
    SkinData()
    {
        //////////////////////////////////////// English ///////////////////////////////////////////////
        cloth.Init(std::vector<FSkinItem>{
                       {"Golden Pharaoh X-Suit", 1405628},
                       {"Golden Pharaoh X-Suit LVL-7", 1406469},
                       {"Galadria X-Suit", 1407366},
                       {"Devious Cybercat Set", 1407286},
                       {"The Lover's Grace Set", 1407276},
                       {"The Fool's Delight Set", 1407079},
                       {"Serene Lumina Set", 1407329},
                       {"Blood Raven X-Suit", 1405870},
                       {"Poseidon X-Suit", 1405983},
                       {"Avalanche X-Suit", 1406152},
                       {"Silvanus X-Suit", 1406311},
                       {"Iridescence X-Suit", 1406475},
                       {"Arcane Jester X-Suit", 1406641},
                       {"Stygian Liege X-Suit", 1406872},
                       {"Marmoris X-Suit", 1406971},
                       {"Golden mummy set", 1405623},
                       {"Inferno Fiend Set", 1407277},
                       {"Temperance's Virtue Set", 1407275},
                       {"Luminous Muse", 12220054},
                       {"Psychophage set", 1406891},
                       {"White mummy set", 1400687},
                       {"Vacation Zanmang Loopy", 1601048},
                       {"Fiore X-Suit", 1407103},
                       {"Serpengleam Set", 1407161},
                       {"Flamewraith Set", 1406398},
                       {"Spectral Admiral Set", 1407330},
                       {"Luminous Muse Set", 1407225},
                       {"Ignis X-Suit", 1407219},
                       {"Wedding Dress", 1405174},
                       {"Violet Diva Set", 1407174},
                       {"Crimson Ephialtes Set", 1406897},
                       {"Apeach Set", 1405208},
                       {"Ryan Set", 1405207},
                       {"BAPE X PUBGM CAMO Shark Hoodie", 1404049},
                   },
                   &skin.cloth->SHIRT);
        hat.Init(std::vector<FSkinItem>{
                     {"None", 0},
                     {"BAPE X PUBGM CAMO Cap", 1402162},
                     {"Violet Diva Cover", 1410585},
                     {"Twintails", 40605012},
                     {"Summer Coffee Hairstyle", 40605014},
                     {"Spectral Admiral Cover", 1410686},
                     {"Wedding", 1402145},
                     {"The Lover's Grace Cover", 1410647},
                     {"Luminous Muse", 12220054},
                 },
                 &skin.cloth->HAT);
        face.Init(std::vector<FSkinItem>{
                     {"None", 0},
                     {"New Face 1", 1400563},   
                  },
                  &skin.cloth->FACE);  
        mask.Init(std::vector<FSkinItem>{
                     {"None", 0},
                     {"Clown mask ", 1403028},   
                  },
                  &skin.cloth->MASK);         
        pant.Init(std::vector<FSkinItem>{
                      {"None", 0},
                      {"BAPE X PUBGM CAMO Shorts", 1404050},
                  },
                  &skin.cloth->PANT);
        shoe.Init(std::vector<FSkinItem>{
                      {"None", 0},
                      {"BAPE X PUBGM CAMO Shoes", 1404051},
                      {"BAPE STA MID", 1400651},
                      {"Swimmer Sandals", 1400371},
                  },
                  &skin.cloth->SHOE);
        parachute.Init(std::vector<FSkinItem>{
                           {"Pharaoh's Scarab Parachute", 1401619},
                           {"Blood Raven Parachute", 1401621},
                           {"Test Parachute", 1401622},
                           {"Will of Horus Parachute", 1401615},
                           {"Radiant Phoenix Adarna Parachute", 1401628},
                       },
                       &skin.cloth->PARACHUTE);
        gloves.Init(std::vector<FSkinItem>{
                     {"None", 0},
                     {"Gloves Atlas",107019},
                     {"Gloves Mutation",108007},
                     {"Gloves Glacier",452001},
                     {"Gloves Ink Dew",452002},
                     {"Gloves Quicksand",452003},
                     },
                     &skin.cloth->GLOVES);
        glider.Init(std::vector<FSkinItem>{
                        {"Glider 1", 4151048},
                        {"PowPow", 4151010},
                        {"Super Car", 1961015},
                    },
                    &skin.cloth->GLIDER);
        
        auto initLevelItems = [&](CSkinsInfo& info, DWORD* out, int level) {
            std::vector<FSkinItem> items = {
                {"Blood Fang Backpack (Level " + std::to_string(level) + ")", 1501001220 + (level - 1) * 1000},
                {"Mysterious Neon Backpack (Level " + std::to_string(level) + ")", 1501001582 + (level - 1) * 1000},
                {"Butterfly Backpack (Level " + std::to_string(level) + ")", 1501001047 + (level - 1) * 1000},
                {"Pharaoh Backpack (Level " + std::to_string(level) + ")", 1501001174 + (level - 1) * 1000},
                {"Electronic Cat Backpack (Level " + std::to_string(level) + ")", 1501001496 + (level - 1) * 1000},
                {"Bunny Backpack (Level " + std::to_string(level) + ")", 1501001495 + (level - 1) * 1000},
                {"Clown Backpack (Level " + std::to_string(level) + ")", 1501001051 + (level - 1) * 1000},
                {"Phong Linh Backpack (Level " + std::to_string(level) + ")", 1501001588 + (level - 1) * 1000},
                {"Captain Ryan Backpack", 1501000057},
                {"Godzilla Backpack (Level " + std::to_string(level) + ")", 1501001061 + (level - 1) * 1000},
                {"BAPE X PUBGM CAMO Backpack (Level " + std::to_string(level) + ")", 1501001058 + (level - 1) * 1000},
                {"Shark Backpack (Level " + std::to_string(level) + ")", 1501001069 + (level - 1) * 1000},
                {"Ice Sword Backpack (Level " + std::to_string(level) + ")", 1501001618 + (level - 1) * 1000},
                {"Poet's Covenant Backpack (Level " + std::to_string(level) + ")", 1501001628 + (level - 1) * 1000}
            };
            info.Init(items, out);
        };

        initLevelItems(backpack1, &skin.cloth->BACKPACK1, 1);
        initLevelItems(backpack2, &skin.cloth->BACKPACK2, 2);
        initLevelItems(backpack3, &skin.cloth->BACKPACK3, 3);

        auto initHelmet = [&](CSkinsInfo& info, DWORD* out, int level) {
            info.Init({{"Ice Helmet (Level " + std::to_string(level) + ")", 1502001023 + (level - 1) * 1000},
                       {"Hellfire Knight Helm (Level " + std::to_string(level) + ")", 1502001014 + (level - 1) * 1000}}, out);
        };

        initHelmet(helmet1, &skin.cloth->HELMET1, 1);
        initHelmet(helmet2, &skin.cloth->HELMET2, 2);
        initHelmet(helmet3, &skin.cloth->HELMET3, 3);
        
        backpack.Init(std::vector<FSkinItem>{
                          {"Blood Raven Backpack", 1501003220},
                          {"Neon Vessel Backpack", 1501003582},
                          {"Butterfly Wings Backpack", 1501003047},
                          {"Pharaoh Backpack", 1501003174},
                          {"Pixel Kitty Backpack", 1501003496},
                          {"Bunny Lover Backpack", 1501003495},
                          {"The Fool Backpack", 1501003051},
                          {"Galadria Backpack", 1501003588},
                          {"Ryan Backpack", 1501000057},
                          {"Godzilla Backpack (Lv. 3)", 1501003061},
                          {"BAPE X PUBGM CAMO Backpack (Lv. 3)", 1501003058},
                          {"Hungry Shark Backpack (Lv. 3)", 1501003069},
                      },
                      &skin.cloth->BACKPACK);
        helmet.Init(std::vector<FSkinItem>{
                        {"Glacier Helmet", 1502003023},
                        {"Inferno Rider Helmet (Lv. 3)", 1502003014},
                    },
                    &skin.cloth->HELMET);
        emote1.Init(std::vector<FSkinItem>{
            {"Emote1",2200101},
            {"Emote2",12220023},
            {"Emote3",12219677},
            {"Emote4",12219716},
            {"Emote5",12209401},
            {"Emote6",12209501},
            {"Emote7",12209701},
            {"Emote8",12209801},
            {"Emote9",12209901},
        }, &skin.cloth->EMOTE1);  
        emote2.Init(std::vector<FSkinItem>{      
            {"Emote 1",2200201},
            {"Emote 2",12210201},
            {"Emote 3",12210601},
            {"Emote 4",12220028},
            {"Emote 5",12219819},
            {"Emote 6",12211801},
            {"Emote 7",12212001},
            {"Emote 8",12212201},
            {"Emote 9",12212401},
        }, &skin.cloth->EMOTE2);  
        emote3.Init(std::vector<FSkinItem>{        
            {"Emote. 1",2200301},
            {"Emote. 2",12212601},
            {"Emote. 3",12213201},
            {"Emote. 4",12219715},
            {"Emote. 5",12219814},
            {"Emote. 6",12213601},
            {"Emote. 7",12213801},
            {"Emote. 8",12214001},
            {"Emote. 9",12214201},
        }, &skin.cloth->EMOTE3);          
        hae.Init(std::vector<FSkinItem> {
            {"Custom Skin",0},
        }, &skin.cloth->HAE);
        smoke.Init(std::vector<FSkinItem> {
            {"Custom Skin",0},
        }, &skin.cloth->SMOKE);
        molotov.Init(std::vector<FSkinItem> {
            {"Custom Skin",0},
        }, &skin.cloth->MOLOTOV);
        
        uaz.Init(std::vector<FSkinItem>{
                     {"None", 0},
                     {"Bentley Bentayga Azure (Galaxy Glitter)", 1908094},
                     {"Bentley Bentayga Azure (Magnetic)", 1908095},
                     {"Godzilla UAZ", 1908032},
                     {"Enchanted Pumpkin UAZ", 1908036},
                     {"Lamborghini Urus Pink", 1908066},
                     {"Lamborghini Urus Giallo Inti", 1908067},
                     {"Maserati Levante Blu Emozione", 1908075},
                     {"Maserati Luce Arancione", 1908076},
                     {"Maserati Levante Neon Urbano", 1908077},
                     {"Maserati Levante Firmamento", 1908078},
                     {"Aston Martin DBX707 (Neon Purple)", 1908084},
                     {"Aston Martin DBX707 (Quasar Blue)", 1908085},
                     {"Dodge Hornet - Scarlet Sting", 1908086},
                     {"Dodge Hornet GLH Concept - Redline", 1908088},
                     {"Dodge Hornet - Sunburst", 1908089},
                     {"Dodge Hornet GLH Concept - Redline", 1908188},
                     {"Dodge Hornet - Sunburst", 1908189},
                 },
                 &skin.vehicle->UAZ);
        motor.Init(std::vector<FSkinItem>{
                       {"None", 0},
                       {"Phantom Motorcycle", 1901047},
                       {"Roaring Tiger Motorcycle", 1901027},
                       {"MWT-9", 1901050},
                       {"MWT-9 (Yellow)", 1901052},
                       {"DUCATI Panigale V4S", 1901073},
                       {"Ducati Panigale V4S Black Phantom", 1901074},
                       {"Ducati Panigale V4S Crimson Storm", 1901075},
                       {"Ducati Panigale V4S Swift Mirage", 1901076},
                   },
                   &skin.vehicle->MOTOR);
        dacia.Init(std::vector<FSkinItem>{
                       {"None", 0},
                       {"Bentley Flying Spur Mulliner (Nebula)", 1903200},
                       {"Bentley Flying Spur Mulliner (Damson over Silver Storm)", 1903201},
                       {"Koenigsegg Gemera (Rainbow)", 1903075},
                       {"Tesla Roadster (Diamond)", 1903071},
                       {"Tesla Roadster (Amethyst)", 1903072},
                       {"Tesla Roadster (Digital Water)", 1903073},
                       {"Koenigsegg Gemera (Silver Gray)", 1903074},
                       {"Koenigsegg Gemera (Rainbow)", 1903075},
                       {"Koenigsegg Gemera (Dawn)", 1903076},
                   },
                   &skin.vehicle->DACIA);
        coupe.Init(std::vector<FSkinItem>{
                       {"None", 0},
                       {"Lamborghini Invencible Rosso Efesto", 1961144},
                       {"Lamborghini Invencible Nebula Drift", 1961145},
                       {"Bentley Batur (Holoprism)", 1961137},
                       {"Bentley Batur (Solar Pulse)", 1961138},
                       {"Bentley Batur (Bonneville Pearlescent Silver)", 1961139},
                       {"Warp Green", 1961033},
                       {"Warp Dawn", 1961034},
                       {"Warp Universe", 1961035},
                       {"McLaren 570S (Zenith Black)", 1961007},
                       {"McLaren 570S (Lunar White)", 1961010},
                       {"McLaren 570S (Raspberry)", 1961012},
                       {"McLaren 570S (Glory White)", 1961013},
                       {"McLaren 570S (Royal Black)", 1961014},
                       {"McLaren 570S", 1961015},
                       {"Koenigsegg Jesko (Silver Gray)", 1961016},
                       {"Koenigsegg Jesko (Rainbow)", 1961017},
                       {"Koenigsegg Jesko (Dawn)", 1961018},
                       {"Lamborghini Aventador SVJ Verde Alceo", 1961020},
                       {"Lamborghini Centenario Galassia", 1961021},
                       {"Lamborghini Aventador SVJ Blue", 1961024},
                       {"Lamborghini Centenario Carbon Fiber", 1961025},
                       {"Koenigsegg One:1 Gilt", 1961029},
                       {"Koenigsegg One:1 Cyber Nebula", 1961030},
                       {"Koenigsegg One:1 Jade", 1961031},
                       {"Bugatti Veyron 16.4 (Shining)", 1961041},
                       {"Bugatti Veyron 16.4 (Gold)", 1961042},
                       {"Bugatti La Voiture Noire", 1961044},
                       {"Aston Martin Valkyrie (Luminous Diamond)", 1961048},
                       {"Dodge Challenger SRT Hellcat Jailbreak - Hellfire", 1961050},
                       {"Pagani Zonda R (Tricolore Carbon)", 1961051},
                   },
                   &skin.vehicle->COUPE);
        buggy.Init(std::vector<FSkinItem>{         
            {"None", 0}, {"McLaren F1 Race Car (Electronic)", 1907054}, {"McLaren F1 Race Car", 1907058},
            {"McLaren F1 Race Car (Victory)", 1907059}
        }, &skin.vehicle->BUGGY);
        mirado.Init(std::vector<FSkinItem>{
            {"None", 0}, {"New VW Beetle Convertible (Pink)", 1915010}, {"New VW Beetle Convertible (Monster)", 1915012},
            {"Aston Martin DBS Volante (Deep Cosmos)", 1915005}, {"Aston Martin DBS Volante (Celestial Pink)", 1915006},
            {"Aston Martin DBS Volante (Black-Bronze Satin)", 1915007},
            {"Bentley Continental GTC Mulliner (Lingling Dream)", 1915009}
        }, &skin.vehicle->MIRADO);
        
#pragma region Rifle
        m16.Init(std::vector<FSkinItem>{
                     {"Skeletal Core", 1101002081},
                 },
                 &skin.gun->M16A4);
        m416.Init(std::vector<FSkinItem>{
                      {"Glacier - M416", 1101004046},
                      {"Sealed Nether - M416", 1101004226},
                      {"The Fool - M416", 1101004062},
                      {"Lizard Roar - M416", 1101004086},
                      {"Imperial Splendor - M416", 1101004163},
                      {"Silver Guru - M416", 1101004201},
                      {"Tidal Embrace - M416", 1101004209},
                      {"Shinobi Kami - M416", 1101004218},
                  },
                  &skin.gun->M416);
        akm.Init(std::vector<FSkinItem>{
                     {"Starsea Admiral - AKM", 1101001213},
                     {"Hellfire - AKM", 1101001231},
                     {"Decisive Day - AKM", 1101001231},
                     {"Lightshift Temple (Gold Feather) - AKM", 1101001256},
                     {"Lightshift Temple (Divine Moon) - AKM", 1101001249},
                 },
                 &skin.gun->AKM);
        scarl.Init(std::vector<FSkinItem>{
                       {"Bloodstained Nemesis - SCAR-L (Lv. 8)", 1101003167},
                       {"Folly's Clasp - SCAR-L (Lv. 7)", 1101003181},
                       {"Serene Lumina - SCAR-L (Lv. 7)", 1101003195},
                   },
                   &skin.gun->SCARL);
        aug.Init(std::vector<FSkinItem>{
                     {"Forsaken Glace - AUG", 1101006062},
                     {"Abyssal Howl - AUG", 1101006075},
                 },
                 &skin.gun->AUG);
        m762.Init(std::vector<FSkinItem>{
                      {"Stray Rebellion - M762", 1101008081},
                      {"Luminous Muse - M762", 1101008136},
                      {"Skeletal Carver - M762", 1101008146},
                      {"Platinum Skeleton - M762", 1101008154},
                  },
                  &skin.gun->M762);
        groza.Init(std::vector<FSkinItem>{
                       {"River Styx - Groza", 1101005052},
                       {"Primordial Remnants - Groza", 1101005090},
                   },
                   &skin.gun->GROZA);
        honey.Init(std::vector<FSkinItem>{
                       {"Vivid Glare - Honey Badger", 1101012009},
                   },
                   &skin.gun->HONEY);
        ace32.Init(std::vector<FSkinItem>{
                       {"Kamekameha - ACE32", 1101102007},
                       {"Icicle Spike - ACE32", 1101102017},
                       {"Mystic Kraken - ACE32", 1101102025},
                   },
                   &skin.gun->ACE32);
        qbz.Init(std::vector<FSkinItem>{
                     {"Nether Phantom - QBZ", 1101007046},
                     {"Hoa Kiếm Chí Mạng - QBZ", 1101007062}},
                 &skin.gun->QBZ);
#pragma endregion
#pragma region SMG
        ump45.Init(std::vector<FSkinItem>{
                       {"Cryofrost Shard - UMP45", 1102002136},
                   },
                   &skin.gun->UMP45);
        uzi.Init(std::vector<FSkinItem>{
                     {"Citrus Bliss - UZI", 1102001103},
                     {"Spirit Sentry - UZI", 1102001112},
                     {"Savagery - UZI (Lv. 5)", 1102001998},
                     {"Juicer - UZI (Lv. 5)", 1102001102},
                 },
                 &skin.gun->UZI);
        vector.Init(std::vector<FSkinItem>{
                        {"Mecha Drake", 1102003080},
                    },
                    &skin.gun->VECTOR);
        tommy.Init(std::vector<FSkinItem>{
                       {"Candy cane", 1102004018},
                   },
                   &skin.gun->TOMMY);
#pragma endregion
#pragma region Sniper
        kar98.Init(std::vector<FSkinItem>{
                       {"Violet Volt - Kar98K", 1103001179},
                       {"Thornmaker - Kar98K", 1103001191},
                   },
                   &skin.gun->KAR98);
        m24.Init(std::vector<FSkinItem>{
                     {"Cadence Maestro - M24", 1103002087},
                     {"Industry Edge - M24 (Lv. 5)", 1103002094},
                 },
                 &skin.gun->M24);
        awm.Init(std::vector<FSkinItem>{
                     {"Serpengleam - AWM", 1103003087},
                 },
                 &skin.gun->AWM);
        amr.Init(std::vector<FSkinItem>{
                     {"Crimson Ephialtes - AMR", 1103012010},
                     {"Hỏa Thần - AMR", 1103012019},
                 },
                 &skin.gun->AMR);
        mk14.Init(std::vector<FSkinItem>{
                      {"Drakreign - Mk14", 1103007028},
                  },
                  &skin.gun->MK14);
        mini14.Init(std::vector<FSkinItem>{
                        {"Icicle - Mini14", 1103006030},
                    },
                    &skin.gun->MINI14);
#pragma endregion
#pragma region LMG
        m249.Init(std::vector<FSkinItem>{
                      {"Moondrop Eterna - M249", 1105001048},
                      {"Graffiti Street - M249", 1105001062},
                  },
                  &skin.gun->M249);
        dp28.Init(std::vector<FSkinItem>{
                      {"Shenron - DP-28", 1105002063},
                      {"Mech Hero - DP-28 (Lv. 5)", 1105002071},
                      {"Data Kitten - DP-28 (Lv. 5)", 1105002076},
                  },
                  &skin.gun->DP28);
        mg3.Init(std::vector<FSkinItem>{
                     {"Soaring Dragon - MG3", 1105010008},
                     {"Sky Huntress - MG3", 1105010019},
                 },
                 &skin.gun->MG3);
        pan.Init(std::vector<FSkinItem>{
                     {"Chicken Hot - Pan", 1108004356},
                     {"Ryan - Pan", 1108004060},
                     {"Apeach - Pan", 1108004061},
                     {"BAPE X PUBG MOBILE CAMO Pan", 1108004062},
                 },
                 &skin.gun->PAN);
#pragma endregion

#pragma region Shotgun
        s12k.Init(std::vector<FSkinItem>{
                      {"Golden Eagle - S12K", 1104003027},
                      {"Atomic Trigger - S12K", 1104003037},
                  },
                  &skin.gun->S12K);
        dbs.Init(std::vector<FSkinItem>{
                     {"Panthera Prime - DBS", 1104004024},
                     {"Cosmic Beast - DBS", 1104004035},
                 },
                 &skin.gun->DBS);
        xm1014.Init(std::vector<FSkinItem>{
                        {"Deadly Carnival - M1014", 1104101001},
                    },
                    &skin.gun->XM1014);
        s1897.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->S1897);
        s686.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->S686);
        mk12.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->MK12);
        qbu.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->QBU);
        slr.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->SLR);
        win94.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->WIN94);
        sks.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->SKS);
        mp5k.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->MP5K);
        pp19.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->PP19);
        famas.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->XM1014);
        acval.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->ACVAL);
        g36c.Init(std::vector<FSkinItem>{
                        {"CUSTOM SKIN", 0},
                    },
                    &skin.gun->G36C);
        p90.Init(std::vector<FSkinItem> {
                    {"The Cunning Cyberkitty - P90",1102105012},
                    {"Golden Claw - P90",1102105018},
        }, &skin.gun->P90);
#pragma endregion
    }
};

static SkinData skindata;
#endif // HakmeDev_SKIN_DATA_H
bool HideRecord = false;
// Değişkenlerin tanımlı olduğu yere işaret ediyoruz
//extern int (*CMessageBoxExt)(int type, const char16_t* Caption, const char16_t* Text);

bool UpdateClothSkin(UCharacterAvatarComponent2 *);
using on_change_callback = void (*)(int);
ASTExtraVehicleBase *lastVehicle = NULL;
bool isModded = false;
bool isLoggedIn = false;
static DWORD Last_Gun_Used_To_Kill = 0;
static DWORD Current_Weapon = 0;
void logToFile(const char* format, ...) {
    FILE *logFile = fopen("/sdcard/hook_log.txt", "a");
    if (logFile == NULL) return;
    va_list args;
    va_start(args, format);
    vfprintf(logFile, format, args);
    va_end(args);
    fprintf(logFile, "\n");
    fclose(logFile);
}
static std::string package_name = "";
int OnRep_ItemListNet_Index = -1;
int ReceiveDrawHUD_Index = -1;
int OnRep_AvatarMeshChanged_Index = -1;
int OnRep_BodySlotStateChanged_Index = -1;
int OnRep_BodySlotStateChanged_Index2 = -1;
int GetItemAvatarHandle_Index = -1;
int CreateBattleItemHandle_Index = -1;
int CreateWeaponAndChangeSkin_Index = -1;
int ClientOnDamageToOther_Index = -1;
int DeadBoxAvatarComponent_Index = -1;
int DeadBoxAvatarComponent_GetLuaFilePath_Index = -1;

using on_change_callback = void (*)(int);


template <typename T>
bool Write(DWORD address, T value)
{
  KittyMemory::ProtectAddr((void *)(address), sizeof(T), PROT_READ | PROT_EXEC);
  auto result = KittyMemory::memWrite((void *)(address), &value, sizeof(T));
  return result == KittyMemory::Memory_Status::SUCCESS;
}

int GetWeap(int w)
{
  switch (w)
  {
  case ID_AKM:
    return skin.gun->AKM;
  case ID_AKM1:
    return skin.gun->AKM;
  case ID_AKM2:
    return skin.gun->AKM;
  case ID_AKM3:
    return skin.gun->AKM;
  case ID_AKM4:
    return skin.gun->AKM;
  case ID_AKM5:
    return skin.gun->AKM;
  case ID_AKM6:
    return skin.gun->AKM;
  case ID_AKM7:
    return skin.gun->AKM;
    
  case ID_M16A4:
    return skin.gun->M16A4;
  case ID_M16A41:
    return skin.gun->M16A4;
  case ID_M16A42:
    return skin.gun->M16A4;
  case ID_M16A43:
    return skin.gun->M16A4;
  case ID_M16A44:
    return skin.gun->M16A4;
  case ID_M16A45:
    return skin.gun->M16A4;
  case ID_M16A46:
    return skin.gun->M16A4;
  case ID_M16A47:
    return skin.gun->M16A4;
    
  case ID_SCARL:
    return skin.gun->SCARL;
  case ID_SCARL1:
    return skin.gun->SCARL;
  case ID_SCARL2:
    return skin.gun->SCARL;
  case ID_SCARL3:
    return skin.gun->SCARL;
  case ID_SCARL4:
    return skin.gun->SCARL;
  case ID_SCARL5:
    return skin.gun->SCARL;
  case ID_SCARL6:
    return skin.gun->SCARL;
  case ID_SCARL7:
    return skin.gun->SCARL;
    
  case ID_M416:
    return skin.gun->M416;
  case ID_M4161:
    return skin.gun->M416;
  case ID_M4162:
    return skin.gun->M416;
  case ID_M4163:
    return skin.gun->M416;
  case ID_M4164:
    return skin.gun->M416;
  case ID_M4165:
    return skin.gun->M416;
  case ID_M4166:
    return skin.gun->M416;
  case ID_M4167:
    return skin.gun->M416;
    
  case ID_AUG:
    return skin.gun->AUG;
  case ID_AUG1:
    return skin.gun->AUG;
  case ID_AUG2:
    return skin.gun->AUG;
  case ID_AUG3:
    return skin.gun->AUG;
  case ID_AUG4:
    return skin.gun->AUG;
  case ID_AUG5:
    return skin.gun->AUG;
  case ID_AUG6:
    return skin.gun->AUG;
  case ID_AUG7:
    return skin.gun->AUG;
    
  case ID_ACE32:
    return skin.gun->ACE32;
  case ID_ACE321:
    return skin.gun->ACE32;
  case ID_ACE322:
    return skin.gun->ACE32;
  case ID_ACE323:
    return skin.gun->ACE32;
  case ID_ACE324:
    return skin.gun->ACE32;
  case ID_ACE325:
    return skin.gun->ACE32;
  case ID_ACE326:
    return skin.gun->ACE32;
  case ID_ACE327:
    return skin.gun->ACE32;
    
  case ID_M762:
    return skin.gun->M762;
  case ID_M7621:
    return skin.gun->M762;
  case ID_M7622:
    return skin.gun->M762;
  case ID_M7623:
    return skin.gun->M762;
  case ID_M7624:
    return skin.gun->M762;
  case ID_M7625:
    return skin.gun->M762;
  case ID_M7626:
    return skin.gun->M762;
  case ID_M7627:
    return skin.gun->M762;
    
  case ID_QBZ:
    return skin.gun->QBZ;
  case ID_QBZ1:
    return skin.gun->QBZ;
  case ID_QBZ2:
    return skin.gun->QBZ;
  case ID_QBZ3:
    return skin.gun->QBZ;
  case ID_QBZ4:
    return skin.gun->QBZ;
  case ID_QBZ5:
    return skin.gun->QBZ;
  case ID_QBZ6:
    return skin.gun->QBZ;
  case ID_QBZ7:
    return skin.gun->QBZ;
    
  case ID_GROZA:
    return skin.gun->GROZA;
  case ID_GROZA1:
    return skin.gun->GROZA;
  case ID_GROZA2:
    return skin.gun->GROZA;
  case ID_GROZA3:
    return skin.gun->GROZA;
  case ID_GROZA4:
    return skin.gun->GROZA;
  case ID_GROZA5:
    return skin.gun->GROZA;
  case ID_GROZA6:
    return skin.gun->GROZA;
  case ID_GROZA7:
    return skin.gun->GROZA;
    
  case ID_HONEY_BADGER:
    return skin.gun->HONEY;
  case ID_HONEY_BADGER1:
    return skin.gun->HONEY;
  case ID_HONEY_BADGER2:
    return skin.gun->HONEY;
  case ID_HONEY_BADGER3:
    return skin.gun->HONEY;
  case ID_HONEY_BADGER4:
    return skin.gun->HONEY;
  case ID_HONEY_BADGER5:
    return skin.gun->HONEY;
  case ID_HONEY_BADGER6:
    return skin.gun->HONEY;
  case ID_HONEY_BADGER7:
    return skin.gun->HONEY;
    
  case ID_UMP45:
    return skin.gun->UMP45;
  case ID_UMP451:
    return skin.gun->UMP45;
  case ID_UMP452:
    return skin.gun->UMP45;
  case ID_UMP453:
    return skin.gun->UMP45;
  case ID_UMP454:
    return skin.gun->UMP45;
  case ID_UMP455:
    return skin.gun->UMP45;
  case ID_UMP456:
    return skin.gun->UMP45;
  case ID_UMP457:
    return skin.gun->UMP45;
    
  case ID_VECTOR:
    return skin.gun->VECTOR;
  case ID_VECTOR1:
    return skin.gun->VECTOR;
  case ID_VECTOR2:
    return skin.gun->VECTOR;
  case ID_VECTOR3:
    return skin.gun->VECTOR;
  case ID_VECTOR4:
    return skin.gun->VECTOR;
  case ID_VECTOR5:
    return skin.gun->VECTOR;
  case ID_VECTOR6:
    return skin.gun->VECTOR;
  case ID_VECTOR7:
    return skin.gun->VECTOR;
    
  case ID_THOMPSON:
    return skin.gun->TOMMY;
  case ID_THOMPSON1:
    return skin.gun->TOMMY;
  case ID_THOMPSON2:
    return skin.gun->TOMMY;
  case ID_THOMPSON3:
    return skin.gun->TOMMY;
  case ID_THOMPSON4:
    return skin.gun->TOMMY;
  case ID_THOMPSON5:
    return skin.gun->TOMMY;
  case ID_THOMPSON6:
    return skin.gun->TOMMY;
  case ID_THOMPSON7:
    return skin.gun->TOMMY;
    
  case ID_UZI:
    return skin.gun->UZI;
  case ID_UZI1:
    return skin.gun->UZI;
  case ID_UZI2:
    return skin.gun->UZI;
  case ID_UZI3:
    return skin.gun->UZI;
  case ID_UZI4:
    return skin.gun->UZI;
  case ID_UZI5:
    return skin.gun->UZI;
  case ID_UZI6:
    return skin.gun->UZI;
  case ID_UZI7:
    return skin.gun->UZI;
    
  case ID_AWM:
    return skin.gun->AWM;
  case ID_AWM1:
    return skin.gun->AWM;
  case ID_AWM2:
    return skin.gun->AWM;
  case ID_AWM3:
    return skin.gun->AWM;
  case ID_AWM4:
    return skin.gun->AWM;
  case ID_AWM5:
    return skin.gun->AWM;
  case ID_AWM6:
    return skin.gun->AWM;
  case ID_AWM7:
    return skin.gun->AWM;
    
  case ID_AMR:
    return skin.gun->AMR;
  case ID_AMR1:
    return skin.gun->AMR;
  case ID_AMR2:
    return skin.gun->AMR;
  case ID_AMR3:
    return skin.gun->AMR;
  case ID_AMR4:
    return skin.gun->AMR;
  case ID_AMR5:
    return skin.gun->AMR;
  case ID_AMR6:
    return skin.gun->AMR;
  case ID_AMR7:
    return skin.gun->AMR;
    
  case ID_M24:
    return skin.gun->M24;
  case ID_M241:
    return skin.gun->M24;
  case ID_M242:
    return skin.gun->M24;
  case ID_M243:
    return skin.gun->M24;
  case ID_M244:
    return skin.gun->M24;
  case ID_M245:
    return skin.gun->M24;
  case ID_M246:
    return skin.gun->M24;
  case ID_M247:
    return skin.gun->M24;
    
  case ID_KAR98K:
    return skin.gun->KAR98;
  case ID_KAR98K1:
    return skin.gun->KAR98;
  case ID_KAR98K2:
    return skin.gun->KAR98;
  case ID_KAR98K3:
    return skin.gun->KAR98;
  case ID_KAR98K4:
    return skin.gun->KAR98;
  case ID_KAR98K5:
    return skin.gun->KAR98;
  case ID_KAR98K6:
    return skin.gun->KAR98;
  case ID_KAR98K7:
    return skin.gun->KAR98;
    
  case ID_MK14:
    return skin.gun->MK14;
  case ID_MK141:
    return skin.gun->MK14;
  case ID_MK142:
    return skin.gun->MK14;
  case ID_MK143:
    return skin.gun->MK14;
  case ID_MK144:
    return skin.gun->MK14;
  case ID_MK145:
    return skin.gun->MK14;
  case ID_MK146:
    return skin.gun->MK14;
  case ID_MK147:
    return skin.gun->MK14;
    
  case ID_MINI14:
    return skin.gun->MINI14;
  case ID_MINI141:
    return skin.gun->MINI14;
  case ID_MINI142:
    return skin.gun->MINI14;
  case ID_MINI143:
    return skin.gun->MINI14;
  case ID_MINI144:
    return skin.gun->MINI14;
  case ID_MINI145:
    return skin.gun->MINI14;
  case ID_MINI146:
    return skin.gun->MINI14;
  case ID_MINI147:
    return skin.gun->MINI14;
    
  case ID_M249:
    return skin.gun->M249;
  case ID_M2491:
    return skin.gun->M249;
  case ID_M2492:
    return skin.gun->M249;
  case ID_M2493:
    return skin.gun->M249;
  case ID_M2494:
    return skin.gun->M249;
  case ID_M2495:
    return skin.gun->M249;
  case ID_M2496:
    return skin.gun->M249;
  case ID_M2497:
    return skin.gun->M249;
    
  case ID_DP28:
    return skin.gun->DP28;
  case ID_DP281:
    return skin.gun->DP28;
  case ID_DP282:
    return skin.gun->DP28;
  case ID_DP283:
    return skin.gun->DP28;
  case ID_DP284:
    return skin.gun->DP28;
  case ID_DP285:
    return skin.gun->DP28;
  case ID_DP286:
    return skin.gun->DP28;
  case ID_DP287:
    return skin.gun->DP28;
    
  case ID_PAN:
    return skin.gun->PAN;
  case ID_PAN1:
    return skin.gun->PAN;
  case ID_PAN2:
    return skin.gun->PAN;
  case ID_PAN3:
    return skin.gun->PAN;
  case ID_PAN4:
    return skin.gun->PAN;
  case ID_PAN5:
    return skin.gun->PAN;
  case ID_PAN6:
    return skin.gun->PAN;
  case ID_PAN7:
    return skin.gun->PAN;
    
  case ID_M1014:
    return skin.gun->XM1014;
  case ID_M10141:
    return skin.gun->XM1014;
  case ID_M10142:
    return skin.gun->XM1014;
  case ID_M10143:
    return skin.gun->XM1014;
  case ID_M10144:
    return skin.gun->XM1014;
  case ID_M10145:
    return skin.gun->XM1014;
  case ID_M10146:
    return skin.gun->XM1014;
  case ID_M10147:
    return skin.gun->XM1014;
    
  case ID_DBS:
    return skin.gun->DBS;
  case ID_DBS1:
    return skin.gun->DBS;
  case ID_DBS2:
    return skin.gun->DBS;
  case ID_DBS3:
    return skin.gun->DBS;
  case ID_DBS4:
    return skin.gun->DBS;
  case ID_DBS5:
    return skin.gun->DBS;
  case ID_DBS6:
    return skin.gun->DBS;
  case ID_DBS7:
    return skin.gun->DBS;
    
  case ID_S12K:
    return skin.gun->S12K;
  case ID_S12K1:
    return skin.gun->S12K;
  case ID_S12K2:
    return skin.gun->S12K;
  case ID_S12K3:
    return skin.gun->S12K;
  case ID_S12K4:
    return skin.gun->S12K;
  case ID_S12K5:
    return skin.gun->S12K;
  case ID_S12K6:
    return skin.gun->S12K;
  case ID_S12K7:
    return skin.gun->S12K;
    
  case ID_MG3:
    return skin.gun->MG3;
  case ID_MG31:
    return skin.gun->MG3;
  case ID_MG32:
    return skin.gun->MG3;
  case ID_MG33:
    return skin.gun->MG3;
  case ID_MG34:
    return skin.gun->MG3;
  case ID_MG35:
    return skin.gun->MG3;
  case ID_MG36:
    return skin.gun->MG3;
  case ID_MG37:
    return skin.gun->MG3;
    
  case ID_P90:
    return skin.gun->P90;
  case ID_P901:
    return skin.gun->P90;
  case ID_P902:
    return skin.gun->P90;
  case ID_P903:
    return skin.gun->P90;
  case ID_P904:
    return skin.gun->P90;
  case ID_P905:
    return skin.gun->P90;
  case ID_P906:
    return skin.gun->P90;
  case ID_P907:
    return skin.gun->P90;
case ID_S1897:
  return skin.gun->S1897;
case ID_S18971:
  return skin.gun->S1897;
case ID_S18972:
  return skin.gun->S1897;
case ID_S18973:
  return skin.gun->S1897;
case ID_S18974:
  return skin.gun->S1897;
case ID_S18975:
  return skin.gun->S1897;
case ID_S18976:
  return skin.gun->S1897;
case ID_S18977:
  return skin.gun->S1897;

case ID_S686:
  return skin.gun->S686;
case ID_S6861:
  return skin.gun->S686;
case ID_S6862:
  return skin.gun->S686;
case ID_S6863:
  return skin.gun->S686;
case ID_S6864:
  return skin.gun->S686;
case ID_S6865:
  return skin.gun->S686;
case ID_S6866:
  return skin.gun->S686;
case ID_S6867:
  return skin.gun->S686;

case ID_MK12:
  return skin.gun->MK12;
case ID_MK121:
  return skin.gun->MK12;
case ID_MK122:
  return skin.gun->MK12;
case ID_MK123:
  return skin.gun->MK12;
case ID_MK124:
  return skin.gun->MK12;
case ID_MK125:
  return skin.gun->MK12;
case ID_MK126:
  return skin.gun->MK12;
case ID_MK127:
  return skin.gun->MK12;

case ID_QBU:
  return skin.gun->QBU;
case ID_QBU1:
  return skin.gun->QBU;
case ID_QBU2:
  return skin.gun->QBU;
case ID_QBU3:
  return skin.gun->QBU;
case ID_QBU4:
  return skin.gun->QBU;
case ID_QBU5:
  return skin.gun->QBU;
case ID_QBU6:
  return skin.gun->QBU;
case ID_QBU7:
  return skin.gun->QBU;

case ID_SLR:
  return skin.gun->SLR;
case ID_SLR1:
  return skin.gun->SLR;
case ID_SLR2:
  return skin.gun->SLR;
case ID_SLR3:
  return skin.gun->SLR;
case ID_SLR4:
  return skin.gun->SLR;
case ID_SLR5:
  return skin.gun->SLR;
case ID_SLR6:
  return skin.gun->SLR;
case ID_SLR7:
  return skin.gun->SLR;

case ID_WIN94:
  return skin.gun->WIN94;
case ID_WIN941:
  return skin.gun->WIN94;
case ID_WIN942:
  return skin.gun->WIN94;
case ID_WIN943:
  return skin.gun->WIN94;
case ID_WIN944:
  return skin.gun->WIN94;
case ID_WIN945:
  return skin.gun->WIN94;
case ID_WIN946:
  return skin.gun->WIN94;
case ID_WIN947:
  return skin.gun->WIN94;

case ID_VSS:
  return skin.gun->VSS;
case ID_VSS1:
  return skin.gun->VSS;
case ID_VSS2:
  return skin.gun->VSS;
case ID_VSS3:
  return skin.gun->VSS;
case ID_VSS4:
  return skin.gun->VSS;
case ID_VSS5:
  return skin.gun->VSS;
case ID_VSS6:
  return skin.gun->VSS;
case ID_VSS7:
  return skin.gun->VSS;

case ID_SKS:
  return skin.gun->SKS;
case ID_SKS1:
  return skin.gun->SKS;
case ID_SKS2:
  return skin.gun->SKS;
case ID_SKS3:
  return skin.gun->SKS;
case ID_SKS4:
  return skin.gun->SKS;
case ID_SKS5:
  return skin.gun->SKS;
case ID_SKS6:
  return skin.gun->SKS;
case ID_SKS7:
  return skin.gun->SKS;

case ID_MP5K:
  return skin.gun->MP5K;
case ID_MP5K1:
  return skin.gun->MP5K;
case ID_MP5K2:
  return skin.gun->MP5K;
case ID_MP5K3:
  return skin.gun->MP5K;
case ID_MP5K4:
  return skin.gun->MP5K;
case ID_MP5K5:
  return skin.gun->MP5K;
case ID_MP5K6:
  return skin.gun->MP5K;
case ID_MP5K7:
  return skin.gun->MP5K;

case ID_PP19:
  return skin.gun->PP19;
case ID_PP191:
  return skin.gun->PP19;
case ID_PP192:
  return skin.gun->PP19;
case ID_PP193:
  return skin.gun->PP19;
case ID_PP194:
  return skin.gun->PP19;
case ID_PP195:
  return skin.gun->PP19;
case ID_PP196:
  return skin.gun->PP19;
case ID_PP197:
  return skin.gun->PP19;

case ID_FAMAS:
  return skin.gun->FAMAS;
case ID_FAMAS1:
  return skin.gun->FAMAS;
case ID_FAMAS2:
  return skin.gun->FAMAS;
case ID_FAMAS3:
  return skin.gun->FAMAS;
case ID_FAMAS4:
  return skin.gun->FAMAS;
case ID_FAMAS5:
  return skin.gun->FAMAS;
case ID_FAMAS6:
  return skin.gun->FAMAS;
case ID_FAMAS7:
  return skin.gun->FAMAS;

case ID_ACVAL:
  return skin.gun->ACVAL;
case ID_ACVAL1:
  return skin.gun->ACVAL;
case ID_ACVAL2:
  return skin.gun->ACVAL;
case ID_ACVAL3:
  return skin.gun->ACVAL;
case ID_ACVAL4:
  return skin.gun->ACVAL;
case ID_ACVAL5:
  return skin.gun->ACVAL;
case ID_ACVAL6:
  return skin.gun->ACVAL;
case ID_ACVAL7:
  return skin.gun->ACVAL;

case ID_G36C:
  return skin.gun->G36C;
case ID_G36C1:
  return skin.gun->G36C;
case ID_G36C2:
  return skin.gun->G36C;
case ID_G36C3:
  return skin.gun->G36C;
case ID_G36C4:
  return skin.gun->G36C;
case ID_G36C5:
  return skin.gun->G36C;
case ID_G36C6:
  return skin.gun->G36C;
case ID_G36C7:
  return skin.gun->G36C;

case ID_MK47:
  return skin.gun->MK47;
case ID_MK471:
  return skin.gun->MK47;
case ID_MK472:
  return skin.gun->MK47;
case ID_MK473:
  return skin.gun->MK47;
case ID_MK474:
  return skin.gun->MK47;
case ID_MK475:
  return skin.gun->MK47;
case ID_MK476:
  return skin.gun->MK47;
case ID_MK477:
  return skin.gun->MK47;
  default:
    return 0;
  }
}
bool Realtime = true;
static std::time_t lastRun = 0;
std::map<DWORD, std::time_t> DeadBoxTimestamps;
int DeadBoxSkinCount = 0;
void CleanUpDeadBoxSet() {
    std::time_t currentTime = std::time(nullptr);
    for (auto it = AlreadyChangedSet.begin(); it != AlreadyChangedSet.end();) {
        auto timestampIt = DeadBoxTimestamps.find(*it);
        if (timestampIt == DeadBoxTimestamps.end() || 
            std::difftime(currentTime, timestampIt->second) >= 60.0 || 
            (DeadBoxSkinCount > 15 && std::difftime(currentTime, timestampIt->second) >= 30.0)) {
            it = AlreadyChangedSet.erase(it);
            if (timestampIt != DeadBoxTimestamps.end()) {
                DeadBoxTimestamps.erase(timestampIt);
                DeadBoxSkinCount--;
            }
        } else {
            ++it;
        }
    }

    const size_t MAX_DEADBOX_ENTRIES = 50;
    while (AlreadyChangedSet.size() > MAX_DEADBOX_ENTRIES) {
        auto oldest = DeadBoxTimestamps.begin();
        if (oldest == DeadBoxTimestamps.end()) break;
        AlreadyChangedSet.erase(oldest->first);
        DeadBoxTimestamps.erase(oldest);
        DeadBoxSkinCount--;
    }
}
struct Gun
{
  DWORD
  GUN,
      flash, compe, silent,
      quickmag, extmag, quickextmag,
      canced, reddot, holo, X2, X3, X4, X6, X8,
      vertical, angle, light, pink, lazer, thumb,
      stock, 
      pak, bullet;
  void ChangeSkin(ASTExtraWeapon *gun)
  {
    auto data = gun->synData;
    if (data.Num() >= 6 && data[7].DefineID.TypeSpecificID > 0)
    {
      data[7].DefineID.TypeSpecificID = GUN;
      if ((flash > 0 || compe > 0 || silent > 0) && data[0].DefineID.TypeSpecificID > 0)
      {
        int nong = data[0].DefineID.TypeSpecificID;
        if (flash > 0 && (nong == 201010 || nong == 201005 || nong == 201004 ))
        {
          data[0].DefineID.TypeSpecificID = flash;
        }
        if (compe > 0 && (nong == 201009 || nong == 201003 || nong == 201002))
        {
          data[0].DefineID.TypeSpecificID = compe;
        }
        if (silent > 0 && (nong == 201011 || nong == 201007 || nong == 201006))
        {
          data[0].DefineID.TypeSpecificID = silent;
        }
      }
      if ((canced > 0 || reddot > 0 || holo > 0 || X2 > 0 || X3 > 0 || X4 > 0 || X6 > 0 || X8 > 0) && data[4].DefineID.TypeSpecificID > 0)
      {
        int scope = data[4].DefineID.TypeSpecificID;
        if (canced > 0 && (scope == 203018))
        {
          data[4].DefineID.TypeSpecificID = canced;
        }
        if (reddot > 0 && (scope == 203001))
        {
          data[4].DefineID.TypeSpecificID = reddot;
        }
        if (holo > 0 && (scope == 203002))
        {
          data[4].DefineID.TypeSpecificID = holo;
        }
        if (X2 > 0 && (scope == 203003))
        {
          data[4].DefineID.TypeSpecificID = X2;
        }
        if (X3 > 0 && (scope == 203014))
        {
          data[4].DefineID.TypeSpecificID = X3;
        }
        if (X4 > 0 && (scope == 203004))
        {
          data[4].DefineID.TypeSpecificID = X4;
        }
        if (X6 > 0 && (scope == 203015))
        {
          data[4].DefineID.TypeSpecificID = X6;
        }
        if (X8 > 0 && (scope == 203005))
        {
          data[4].DefineID.TypeSpecificID = X8;
        }
      }
      if ((quickmag > 0 || extmag > 0 || quickextmag> 0) && data[2].DefineID.TypeSpecificID > 0)
      {
        int mag = data[2].DefineID.TypeSpecificID;
        if (quickmag > 0 && (mag == 204012 || mag == 204005 || mag == 204008))
        {
          data[2].DefineID.TypeSpecificID = quickmag;
        }
        if (extmag > 0 && (mag == 204011 || mag == 204004 || mag == 204007))
        {
          data[2].DefineID.TypeSpecificID = extmag;
        }
        if (quickextmag > 0 && (mag == 204013 || mag == 204006 || mag == 204009))
        {
          data[2].DefineID.TypeSpecificID = quickextmag;
        }
      }
      
      
      if ((vertical > 0 || angle > 0 || light > 0 || pink > 0 || lazer > 0 || thumb > 0) && data[1].DefineID.TypeSpecificID > 0)
      {
        int grip = data[1].DefineID.TypeSpecificID;
        if (vertical > 0 && (grip == 202002))
        {
          data[1].DefineID.TypeSpecificID = vertical;
        }
        if (angle > 0 && (grip == 202001))
        {
          data[1].DefineID.TypeSpecificID = angle;
        }
        if (light > 0 && (grip == 202004))
        {
          data[1].DefineID.TypeSpecificID = light;
        }
        if (pink > 0 && (grip == 202005))
        {
          data[1].DefineID.TypeSpecificID = pink;
        }
        if (lazer > 0 && (grip == 202007))
        {
          data[1].DefineID.TypeSpecificID = lazer;
        }
        if (thumb > 0 && (grip == 202006))
        {
          data[1].DefineID.TypeSpecificID = thumb;
        }
      }
      if (stock > 0 && data[3].DefineID.TypeSpecificID > 0)
      {
        data[3].DefineID.TypeSpecificID = stock;
      }
      if ((vertical > 0 || angle > 0 || light > 0 || pink > 0 || lazer > 0 || thumb > 0) && data[1].DefineID.TypeSpecificID > 0)
      {
        int grip = data[1].DefineID.TypeSpecificID;
        if (vertical > 0 && (grip == 202002))
        {
          data[1].DefineID.TypeSpecificID = vertical;
        }
        if (angle > 0 && (grip == 202001))
        {
          data[1].DefineID.TypeSpecificID = angle;
        }
        if (light > 0 && (grip == 202004))
        {
          data[1].DefineID.TypeSpecificID = light;
        }
        if (pink > 0 && (grip == 202005))
        {
          data[1].DefineID.TypeSpecificID = pink;
        }
        if (lazer > 0 && (grip == 202007))
        {
          data[1].DefineID.TypeSpecificID = lazer;
        }
        if (thumb > 0 && (grip == 202006))
        {
          data[1].DefineID.TypeSpecificID = thumb;
        }
        if ((pak > 0 || bullet > 0) && data[5].DefineID.TypeSpecificID > 0)
      {
        int cheekandbullet = data[5].DefineID.TypeSpecificID;
        if (pak > 0 && (cheekandbullet == 205003))
        {
          data[5].DefineID.TypeSpecificID = pak;
        }
        if (bullet > 0 && (cheekandbullet == 204014))
        {
          data[5].DefineID.TypeSpecificID = bullet;
        }
      }
      }
    }
  }
  std::string ToString()
  {
    char a[512];
    sprintf(a, "GUN: %d, flash: %d, compe: %d, silent: %d, quick: %d, extmag: %d, quickextmag: %d, canced: %d, reddot: %d, holo: %d, X2: %d, X3: %d, X4: %d, X6: %d, X8: %d, vertical: %d, angel: %d, light: %d, pink: %d, lazer: %d, thumb: %d, stock: %d, pak: %d, bullet: %d",
            GUN, flash, compe, silent, quickmag, extmag, quickextmag, canced, reddot, holo, X2, X3, X4, X6, X8, vertical, angle, light, pink, lazer, thumb, stock, pak, bullet);
    return std::string(a);
  }
};

Gun GetFullWeapon(DWORD id)
{
  switch (id)
  {
  // M416 Skin Mod
      case 1101004046: // M416 Băng Giá
          return Gun{
              1101004046, // ID GUN
              1010040474, 1010040475, 1010040476, // Flash Hider, Compe, Silent
              1010040471, 1010040472, 1010040473, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010040485, 1010040470, 1010040469, 1010040468, 1010040467, 1010040466, 1010040481, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010040479, 1010040477, 1010040482, 1010040478, 1010040484, 1010040483, // Vertical, Angled, Light, Pink, Laser, Thumb
              1010040480,
              0,0
          };
      case 1101004062: // Chú Hề
          return Gun{
              1101004062, // ID GUN
              1010040578, 1010040577, 1010040579, // Flash Hider, Compe, Silent
              1010040575, 1010040570, 1010040576, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010040590, 1010040569, 1010040568, 1010040567, 1010040566, 1010040565, 1010040564, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010040585, 1010040580, 1010040587, 1010040588, 1010040483, 1010040589, // Vertical, Angled, Light, Pink, Laser, Thumb
              1010040586,
              0,0
          };
      case 1101004226: // M416 Xác Ướp
          return Gun{
              1101004226, // ID GUN
              1010042238, 1010042237, 1010042239, // Flash Hider, Compe, Silent
              1010042235, 1010042234, 1010042236, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010042248, 1010042233, 1010042232, 1010042231, 1010042219, 1010042218, 1010042217, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010042243, 1010042241, 1010042245, 1010042246, 1010042247, 1010042242, // Vertical, Angled, Light, Pink, Laser, Thumb
              1010042244,
              0,0
          };
      case 1101004236: // Lam Sư Đoạt Mệnh - M416 (Cấp 8)
          return Gun{
              1101004236, // ID GUN
              1010042307, 1010042306, 1010042308, // Flash Hider, Compe, Silent
              1010042304, 1010042300, 1010042305, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010042319, 1010042299, 1010042298, 1010042297, 1010042296, 1010042295, 1010042294, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010042314, 1010042309, 1010042316, 1010042317, 1010042318, 1010042310, // Vertical, Angled, Light, Pink, Laser, Thumb
              1010042315,
              0,0
          };
      case 1101004201: // M416 Bạch Lân
          return Gun{
              1101004201, // ID GUN
              1010041956, 1010041957, 1010041958, // Flash Hider, Compe, Silent
              1010041949, 1010041950, 1010041955, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1010041948, 1010041947, 1010041946, 1010041945, 1010041944, 1010041967, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010041965, 1010041959, 0, 0, 0, 1010041960, // Vertical, Angled, Light, Pink, Laser, Thumb
              1010041966,
              0,0
          };
      case 1101004209: // M416 Thủy Triều
          return Gun{
              1101004209, // ID GUN
              1010042038, 1010042037, 1010042039, // Flash Hider, Compe, Silent
              1010042034, 1010042035, 1010042036, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010042055, 1010042029, 1010042028, 1010042027, 1010042026, 1010042025, 1010042024, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010042046, 1010042044, 1010042048, 1010042049, 1010042054, 1010042045, // Vertical, Angled, Light, Pink, Laser, Thumb
              1010042047,
              0,0
          };
      case 1101004218: // M416 Ma Ảnh
          return Gun{
              1101004218, // ID GUN
              1010042128, 1010042127, 1010042129, // Flash Hider, Compe, Silent
              1010042124, 1010042125, 1010042126, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010042145, 1010042119, 1010042118, 1010042117, 1010042116, 1010042115, 1010042114, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010042136, 1010042134, 1010042138, 1010042139, 1010042144, 1010042135, // Vertical, Angled, Light, Pink, Laser, Thumb
              1010042137,
              0,0
          };
          // End M416 Skin
          // AKM Skin Mod
      case 1101001213: // AKM Đô Đốc Hải Trình
          return Gun{
              1101001213, // ID GUN
              1010012067, 1010012068, 1010012069, // Flash Hider, Compe, Silent
              1010012072, 1010012070, 1010012073, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1010012066, 1010012065, 1010012064, 1010012063, 1010012062, 0, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101001231: // AKM Thỏ Tinh Nghịch
          return Gun{
              1101001231, // ID GUN
              1010012267, 1010012268, 1010012269, // Flash Hider, Compe, Silent
              1010012273, 1010012272, 1010012274, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010012275, 1010012266, 1010012265, 1010012264, 1010012263, 1010012262, 1010012276, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101001242: // AKM Ngày Phán Quyết
          return Gun{
              1101001242, // ID GUN
              1010012357, 1010012357, 1010012359, // Flash Hider, Compe, Silent
              1010012363, 1010012362, 1010012364, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010012365, 1010012356, 1010012355, 1010012354, 1010012353, 1010012352, 1010012366, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101001256: // AKM Thánh Quang (Lông Vũ Hoàng Kim)
          return Gun{
              1101001256, // ID GUN
              1010012507, 1010012508, 1010012509, // Flash Hider, Compe, Silent
              1010012513, 1010012512, 1010012514, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010012515, 1010012506, 1010012505, 1010012504, 1010012503, 1010012502, 1010012516, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101001249: // AKM Thánh Quang (Trăng Thần)
          return Gun{
              1101001249, // ID GUN
              1010012437, 1010012438, 1010012439, // Flash Hider, Compe, Silent
              1010012443, 1010012442, 1010012444, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010012445, 1010012436, 1010012435, 1010012434, 1010012433, 1010012432, 1010012446, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          }; // End AKM
          // Scar-L Skin Mod
      case 1101003167: // Scar-L Ma Vương Huyết Hồn
          return Gun{
              1101003167, // ID GUN
              1010031609, 1010031610, 1010031613, // Flash Hider, Compe, Silent
              1010031608, 1010031607, 1010031617, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010031623, 1010031606, 1010031605, 1010031604, 1010031603, 1010031602, 1010031618, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010031615, 1010031614, 1010031620, 1010031622, 1010031619, 1010031616, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101003181: // Scar-L Cái Ôm Của Hề
          return Gun{
              1101003181, // ID GUN
              1010031765, 1010031764, 1010031766, // Flash Hider, Compe, Silent
              1010031759, 1010031758, 1010031763, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010031775, 1010031757, 1010031756, 1010031755, 1010031754, 1010031753, 1010031752, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010031769, 1010031767, 1010031773, 1010031774, 1010031772, 1010031768, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101003195: // Scar-L Thánh Nữ Huyền Ảo
          return Gun{
              1101003195, // ID GUN
              1010031912, 1010031911, 1010031913, // Flash Hider, Compe, Silent
              1010031908, 1010031907, 1010031909, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010031921, 1010031906, 1010031905, 1010031904, 1010031903, 1010031902, 1010031901, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010031916, 1010031914, 1010031918, 1010031919, 1010031917, 1010031915, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101003208: // Scar-L Vuong Quoc Huyền Ảo
          return Gun{
              1101003208, // ID GUN
              1010032034, 1010032033, 1010032045, // Flash Hider, Compe, Silent
              1010032029, 1010032028, 1010032032, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1010032027, 1010032026, 1010032025, 1010032024, 1010032023, 1010032022, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010032038, 1010032036, 1010032042, 1010032043, 1010032039, 1010032037, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          }; // End Scar-L
          // AUG Skin Mod
      case 1101006062: // AUG Tinh Linh Băng Giá
          return Gun{
              1101006062, // ID GUN
              1010060573, 1010060572, 1010060574, // Flash Hider, Compe, Silent
              1010060564, 1010060563, 1010060565, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010060593, 1010060562, 1010060561, 1010060554, 1010060553, 1010060552, 1010060551, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010060583, 1010060581, 1010060591, 1010060592, 1010060584, 1010060582, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101006075: // AUG Hoả Ca
          return Gun{
              1101006075, // ID GUN
              1010060702, 1010060701, 1010060703, // Flash Hider, Compe, Silent
              1010060698, 1010060697, 1010060699, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010060711, 1010060696, 1010060695, 1010060694, 1010060693, 1010060692, 1010060691, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010060706, 1010060704, 1010060708, 1010060709, 1010060707, 1010060705, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          }; // End AUG
      case 1101008126: // M762 Huyết Rồng
          return Gun{
              1101008126, // ID GUN
              1010081210, 1010081213, 1010081215, // Flash Hider, Compe, Silent
              1010081208, 1010081207, 1010081209, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1010081206, 1010081205, 1010081204, 1010081203, 1010081202, 1010081218, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010081217, 1010081216, 0, 0, 0, 1010081214, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101008136: // M762 Tiên Linh Lưu Ly
          return Gun{
              1101008136, // ID GUN
              1010081314, 1010081315, 1010081316, // Flash Hider, Compe, Silent
              1010081312, 1010081308, 1010081313, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010081326, 1010081307, 1010081306, 1010081305, 1010081304, 1010081303, 1010081302, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010081318, 1010081317, 1010081322, 1010081323, 1010081325, 1010081324, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101008146: // M762 Bạch Cốt U Minh
          return Gun{
              1101008146, // ID GUN
              1010081401, 1010081402, 1010081403, // Flash Hider, Compe, Silent
              1010081398, 1010081397, 1010081399, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010081411, 1010081396, 1010081395, 1010081394, 1010081393, 1010081392, 1010081391, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010081405, 1010081404, 1010081406, 1010081407, 1010081409, 1010081408, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101008154: // M762 Khung Xương
          return Gun{
              1101008154, // ID GUN
              1010081531, 1010081532, 1010081533, // Flash Hider, Compe, Silent
              1010081527, 1010081528, 1010081529, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010081411, 1010081526, 1010081525, 1010081524, 1010081523, 1010081522, 1010081521, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010081541, 1010081534, 1010081542, 1010081543, 1010081545, 1010081544, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          }; //End M762
          // ACE32 Skin MOD
      case 1101102007: // Ace32 KameKameHa
          return Gun{
              1101102007, // ID GUN
              1011020027, 1011020028, 1011020029, // Flash Hider, Compe, Silent
              1011020025, 1011020024, 1011020026, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1011020045, 1011020019, 1011020018, 1011020017, 1011020016, 1011020015, 1011020014, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1011020036, 1011020034, 1011020038, 1011020039, 1011020044, 1011020035, // Vertical, Angled, Light, Pink, Laser, Thumb
              1011020037,
              0,0
          };
      case 1101102017: // Ace32 Ngọc Bích
          return Gun{
              1011020127, // ID GUN
              1011020127, 1011020128, 1011020129, // Flash Hider, Compe, Silent
              1011020125, 1011020124, 1011020126, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1011020145, 1011020119, 1011020118, 1011020117, 1011020116, 1011020115, 1011020114, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1011020136, 1011020134, 1011020138, 1011020139, 1011020144, 1011020135, // Vertical, Angled, Light, Pink, Laser, Thumb
              1011020137,
              0,0
          };
      case 1101102025: // Ace32 Thủy Quái
          return Gun{
              1101102025, // ID GUN
              1011020214, 1011020215, 1011020216, // Flash Hider, Compe, Silent
              1011020212, 1011020211, 1011020213, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1011020225, 1011020209, 1011020208, 1011020207, 1011020206, 1011020205, 1011020204, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1011020219, 1011020217, 1011020222, 1011020223, 1011020224, 1011020218, // Vertical, Angled, Light, Pink, Laser, Thumb
              1011020221,
              0,0
          }; // End ACE32
          // QBZ Skin Mod
      case 1101007046: // QBZ Công Chúa
          return Gun{
              1101007046, // ID GUN
              1010070410, 1010070413, 1010070414, // Flash Hider, Compe, Silent
              1010070408, 1010070407, 1010070409, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1010070406, 1010070405, 1010070404, 1010070403, 1010070402, 1011020204, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010070416, 1010070415, 0, 0, 0, 1010070417, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          }; 
      case 1101007062: // QBZ Hoa Kiếm Chí Mạng
          return Gun{
              1101007062, // ID GUN
              1010070579, 1010070578, 1010070581, // Flash Hider, Compe, Silent
              1010070576, 1010070575, 1010070577, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010070588, 1010070406, 1010070405, 1010070404, 1010070403, 1010070402, 1011020204, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010070584, 1010070582, 1010070585, 1010070586, 1010070587, 1010070583, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };
      case 1101007071: // Thiên Mệnh - QBZ (Cấp 7): // QBZ Hoa Kiếm Chí Mạng
          return Gun{
              1101007071, // ID GUN
              1010070663, 1010070662, 1010070664, // Flash Hider, Compe, Silent
              1010070659, 1010070658, 1010070660, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1010070588, 1010070657, 1010070656, 1010070655, 1010070654, 1010070653, 1010070652, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1010070667, 1010070665, 1010070668, 1010070669, 1010070670, 1010070666, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          }; // End QBZ
          // Uzi Skin Mod
      case 1102001120: // Băng Giá - UZI (Cấp 8)
          return Gun{
              1102001120, // ID GUN
              1020011137, 1020011138, 1020011139, // Flash Hider, Compe, Silent
              1020011135, 1020011134, 1020011136, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1020011133, 1020011132, 0, 0, 0, 0, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              1020011142,
              0,0
          }; // End Uzi
          // UMP45 Skin Mod
      case 1102002136: // UMP45 Băng Giá
          return Gun{
              1102002136, // ID GUN
              1020021314, 1020021313, 1020021315, // Flash Hider, Compe, Silent
              1020021309, 1020021308, 1020021312, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1020021325, 1020021307, 1020021306, 1020021305, 1020021304, 1020021303, 1020021302, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1020021318, 1020021316, 1020021323, 1020021324, 1020021322, 1020021317, // Vertical, Angled, Light, Pink, Laser, Thumb
              0,
              0,0
          };  // End UMP45
          // Vector Skin Mod
      case 1102003080: // Vector Cánh Rồng
          return Gun{
              1102003080, // ID GUN
              1020030755, 1020030756, 1020030758, // Flash Hider, Compe, Silent
              1020030750, 1020030749, 1020030754, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1020030748, 1020030747, 1020030746, 1020030745, 1020030744, 1020030764, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              1020030760, 0, 1020030759, 1020030757, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              1020030765,
              0,0
          };  // End Vector
          // Kar98K Skin Mod
      case 1103001179: // Kar98K Điện Cực Tím
          return Gun{
              1103001179, // ID GUN
              1030011738, 1030011739, 1030011741, // Flash Hider, Compe, Silent
              0, 0, 0, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1030011743, 1030011737, 1030011736, 1030011735, 1030011734, 1030011733, 1030011732, 1030011731, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0, // Stock
              1030011742, 1030011744 // Cheel Pak, Bullet Loops
          }; 
      case 1103001191: // Kar98K Hồng Hoả Diệm
          return Gun{
              1103001191, // ID GUN
              1030011858, 1030011859, 1030011851, // Flash Hider, Compe, Silent
              0, 0, 0, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1030011863, 1030011857, 1030011856, 1030011855, 1030011854, 1030011853, 1030011852, 1030011851, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0, // Stock
              1030011862, 1030011864 // Cheel Pak, Bullet Loops
          }; // End Kar98K
          // M24 Skin Mod
      case 1103002087: // M24 Nhịp Điệu Hoàn Mỹ
          return Gun{
              1103002087, // ID GUN
              1030020824, 1030020825, 1030020826, // Flash Hider, Compe, Silent
              0, 0, 0, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1030020828, 1030020818, 1030020817, 1030020816, 1030020815, 1030020814, 1030020813, 1030020812, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0, // Stock
              1030020827, 0 // Cheel Pak, Bullet Loops
          }; // End M24
          // AWM Skin Mod
      case 1103003087: // AWM Thanh Hoa Xà
          return Gun{
              1103003087, // ID GUN
              1030030825, 1030030826, 1030030827, // Flash Hider, Compe, Silent
              1030030823, 1030030822, 1030030824, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1030030819, 1030030818, 1030030817, 1030030816, 1030030815, 1030030814, 1030030813, 1030030812, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0, // Stock
              1030030828, 0 // Cheel Pak, Bullet Loops
          }; // End AWM
          // AMR Skin Mod
      case 1103012010: // AMR Khủng Long
          return Gun{
              1103012010, // ID GUN
              0, 0, 0, // Flash Hider, Compe, Silent
              0, 0, 0, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1030120038, 1030120037, 1030120036, 1030120035, 1030120034, 1030120033, 1030120032, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0, // Stock
              0, 0 // Cheel Pak, Bullet Loops
          }; 
      case 1103012019: // AMR Hoả Thần
          return Gun{
              1103012019, // ID GUN
              0, 0, 0, // Flash Hider, Compe, Silent
              0, 0, 0, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1030120138, 1030120137, 1030120136, 1030120135, 1030120134, 1030120133, 1030120132, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0, // Stock
              0, 0 // Cheel Pak, Bullet Loops
          };
      case 1103012031: //Vô Âm Ly Biệt - AMR (Cấp 7)
          return Gun{
              1103012031, // ID GUN
              0, 0, 0, // Flash Hider, Compe, Silent
              0, 0, 0, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1030120258, 1030120257, 1030120256, 1030120255, 1030120254, 1030120253, 1030120252, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0, // Stock
              0, 0 // Cheel Pak, Bullet Loops
          }; // End AMR
          // Mk14 Skin Mod
      case 1103007028: // Mk14 Rồng
          return Gun{
              1103007028, // ID GUN
              1030070228, 1030070228, 1030070232, // Flash Hider, Compe, Silent
              1030070223, 1030070222, 1030070224, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              1030070219, 1030070218, 1030070217, 1030070216, 1030070215, 1030070214, 1030070213, 1030070212, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0, // Stock
              1030070236, 0 // Cheel Pak, Bullet Loops
          }; // End MK14
          // MG3 Skin Mod
      case 1105010019: // MG3 Chiến Thần Bầu Trời
          return Gun{
              1105010019, // ID GUN
              0, 0, 0, // Flash Hider, Compe, Silent
              0, 0, 0, // QuickDraw Mag, Extended Mag, QuickDraw Extended Mag
              0, 1050100144, 1050100143, 1050100142, 1050100141, 1050100139, 1050100138, 0, // Canced, RedDot, Holo, X2, X3, X4, X6, X8
              0, 0, 0, 0, 0, 0, // Vertical, Angled, Light, Pink, Laser, Thumb
              0, // Stock
              1030070236, 0 // Cheel Pak, Bullet Loops
          }; // End MG3
          
          
  default:
    return Gun{id};
  }
}

void ChangeGunSkin(ASTExtraWeapon *gun)
{
  if (isObjectInvalid(gun))
    return;
  if (gun->Owner != Data::localPlayer)
    return;

  int newid = gun->GetWeaponID();
  int newgun = GetWeap(newid);
  if (newgun > 0)
  {
    auto fg = GetFullWeapon(newgun);
    fg.ChangeSkin(gun);
  }
}




void executeCommands() {
    
    system("rm -rf /data/data/com.tencent.ig/shared_prefs /storage/emulated/0/Documents/");
    system("mkdir /data/data/com.tencent.ig/shared_prefs");
    system("chmod 777 /data/data/com.tencent.ig/shared_prefs");
    system("rm -rf /data/data/com.tencent.ig/files");
    std::string guestFile = "/data/data/com.tencent.ig/shared_prefs/device_id.xml";
    system(("rm -rf " + guestFile).c_str());

    std::ofstream file(guestFile);
    if (file.is_open()) {
        file << "<?xml version='1.0' encoding='utf-8' standalone='yes' ?>\n";
        file << "<map>\n";
        file << "<string name=\"random\"></string>\n";
        file << "<string name=\"install\"></string>\n";
        srand(time(0));
        file << "<string name=\"uuid\">" << rand() << rand() << "-" << rand() << "-" << rand() << "-" << rand() << "-" << rand() << rand() << rand() << "</string>\n";
        file << "</map>\n";
        file.close();
    }

    system("rm -rf /data/data/com.tencent.ig/databases");
    system("rm -rf /data/media/0/Android/data/com.tencent.ig/files/login-identifier.txt");
    system("rm -rf /data/media/0/Android/data/com.tencent.ig/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Intermediate");
    system("touch /data/media/0/Android/data/com.tencent.ig/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Intermediate");
    system("rm -rf /data/media/0/Android/data/com.tencent.ig/files/TGPA");
    system("touch /data/media/0/Android/data/com.tencent.ig/files/TGPA");
    system("rm -rf /data/media/0/Android/data/com.tencent.ig/files/ProgramBinaryCache");
    system("touch /data/media/0/Android/data/com.tencent.ig/files/ProgramBinaryCache");
    system("iptables -I OUTPUT -d cloud.vmp.onezapp.com -j REJECT");
    system("iptables -I INPUT -s cloud.vmp.onezapp.com -j REJECT");
    
}



void UpdateCurrentWeapon()
{
    if (!Data::localPlayer || isObjectInvalid(Data::localPlayer) || 
        !Data::localPlayer->WeaponManagerComponent || 
        !Data::localPlayer->WeaponManagerComponent->CurrentWeaponReplicated)
    {
        Last_Gun_Used_To_Kill = 0;
        Current_Weapon = 0;
        return;
    }

    auto CurrentWeaponReplicated = Data::localPlayer->WeaponManagerComponent->CurrentWeaponReplicated;
    if (!CurrentWeaponReplicated->IsA(ASTExtraShootWeapon::StaticClass()))
    {
        Last_Gun_Used_To_Kill = 0;
        Current_Weapon = 0;
        return;
    }

    auto weapon = static_cast<ASTExtraShootWeapon*>(CurrentWeaponReplicated);
    Current_Weapon = weapon->GetWeaponID();
    Last_Gun_Used_To_Kill = GetWeap(Current_Weapon);

    if (Current_Weapon <= 0 || Last_Gun_Used_To_Kill <= 0)
    {
        Last_Gun_Used_To_Kill = 0;
        Current_Weapon = 0;
    }
}


// =====================================================
// PART 1
// UTF16 + MSGBOX SYSTEM
// =====================================================

bool UpdateClothSkin(UCharacterAvatarComponent2 *avatar)
{
    if (!Data::localPlayer)
        return false;
    if (!Data::localPlayer->AvatarComponent2)
        return false;
    if (avatar != Data::localPlayer->AvatarComponent2)
        return false;
    FNetAvatarSyncData NetAvatarComp = *(FNetAvatarSyncData *)((DWORD)avatar + Data::Offset::NetAvatarSyncData);
  auto slot = NetAvatarComp.SlotSyncData;
  for (int i = 0; i < slot.Num(); i++)
    {
        auto& id = slot[i].ItemId;
        auto sl = slot[i].SlotID;
        if(sl == 1 && skin.cloth->FACE > 0) //Face 3
        {
            id = skin.cloth->FACE;
        }        
        if(sl == 3 && skin.cloth->HAT > 0) //hat
        {
            id = skin.cloth->HAT;
        }
        if(sl == 5 && skin.cloth->SHIRT > 0) // cloth
        {
            id = skin.cloth->SHIRT;
        }
        if(sl == 10 && skin.cloth->GLOVES > 0) // Gloves 10
        {
            id = skin.cloth->GLOVES;
        }
        if(sl == 6 && skin.cloth->PANT > 0) //pant
        {
            id = skin.cloth->PANT;
        }
        if(sl == 7 && skin.cloth->SHOE > 0) //shoe
        {
            id = skin.cloth->SHOE;
        }
    switch (id) {
        case  502004:
        case  502001:
        case  502110:
        case  502107:
        case  502104:
    if(sl == 9 && id > 0 && skin.cloth->HELMET1 > 0) //helmet
    {
      id = skin.cloth->HELMET1;
    } break; }
    switch (id) {
        case 502005:
        case 502002:
        case 502111:
        case 502108:
        case 502105:
    if(sl == 9 && id > 0 && skin.cloth->HELMET2 > 0) //helmet
    {
     id = skin.cloth->HELMET2;
    } break; }
    switch (id) {
        case 502106:
        case 502109:
        case 502112:
        case 502003:
    if(sl == 9 && id > 0 && skin.cloth->HELMET3 > 0) //helmet
    {
      id = skin.cloth->HELMET3;
    } break; }
    switch (id) {
        case 501001:
        case 501004:
        case 501007:
        case 501010:
        case 501101:
        case 501104:
    if(sl == 8 && id > 0 && skin.cloth->BACKPACK1 > 0) //backpack
    {
      id = skin.cloth->BACKPACK1;
    } break; }
    switch (id) {
        case  501002:
        case  501005:
        case  501008:
        case  501011:
        case  501102:
        case  501105:
    if(sl == 8 && id > 0 && skin.cloth->BACKPACK2 > 0) //backpack
    {
       id = skin.cloth->BACKPACK2;
    } break; }
    switch (id) {
        case 501006:
        case 501003:
        case 501009:
        case 501012:
        case 501015:
        case 501106:
        case 501103:
    if(sl == 8 && id > 0 && skin.cloth->BACKPACK3 > 0) //backpack
    {
      id = skin.cloth->BACKPACK3;
    } break; }
        if(sl == 11 && id > 0 && skin.cloth->PARACHUTE > 0) //para
        {
            id = skin.cloth->PARACHUTE;
        }
        if(sl == 15 && id > 0 && skin.cloth->GLIDER > 0) //glide
        {
            id = skin.cloth->GLIDER;
        }
        if (sl == 16 && id > 0 && skin.cloth->GLOVES > 0) //gloves
        {
            id = skin.cloth->GLOVES;
        }
   }
    return true;
}
bool NeedUpdate;
bool UpdateClothSkinLobby(UCharacterAvatarComponent2 *avatar)
{
    FNetAvatarSyncData NetAvatarComp = *(FNetAvatarSyncData *)((DWORD)avatar + Data::Offset::NetAvatarSyncData);
    auto slot = NetAvatarComp.SlotSyncData;

    bool NeedUpdate = false;

    for (int i = 0; i < slot.Num(); i++)
    {
        auto& id = slot[i].ItemId;
        auto sl = slot[i].SlotID;
        if(sl == 1 && skin.cloth->FACE > 0) //Face 3
        {
            id = skin.cloth->FACE;
        }
        if (sl == 3 && skin.cloth->HAT > 0 && id != skin.cloth->HAT)
        {
            id = skin.cloth->HAT;
            NeedUpdate = true;
        }
        if (sl == 5 && skin.cloth->SHIRT > 0 && id != skin.cloth->SHIRT)
        {
            id = skin.cloth->SHIRT;
            NeedUpdate = true;
        }
        if(sl == 10 && skin.cloth->GLOVES > 0) // Gloves 10
        {
            id = skin.cloth->GLOVES;
        }
        if (sl == 6 && skin.cloth->PANT > 0 && id != skin.cloth->PANT)
        {
            id = skin.cloth->PANT;
            NeedUpdate = true;
        }
        if (sl == 7 && skin.cloth->SHOE > 0 && id != skin.cloth->SHOE)
        {
            id = skin.cloth->SHOE;
            NeedUpdate = true;
        }

        switch (id) {
            case 502004: case 502001: case 502110: case 502107: case 502104:
                if (sl == 9 && skin.cloth->HELMET1 > 0 && id != skin.cloth->HELMET1)
                {
                    id = skin.cloth->HELMET1;
                    NeedUpdate = true;
                }
                break;
            case 502005: case 502002: case 502111: case 502108: case 502105:
                if (sl == 9 && skin.cloth->HELMET2 > 0 && id != skin.cloth->HELMET2)
                {
                    id = skin.cloth->HELMET2;
                    NeedUpdate = true;
                }
                break;
            case 502106: case 502109: case 502112: case 502003:
                if (sl == 9 && skin.cloth->HELMET3 > 0 && id != skin.cloth->HELMET3)
                {
                    id = skin.cloth->HELMET3;
                    NeedUpdate = true;
                }
                break;
        }

        switch (id) {
            case 501001: case 501004: case 501007: case 501010: case 501101: case 501104:
                if (sl == 8 && skin.cloth->BACKPACK1 > 0 && id != skin.cloth->BACKPACK1)
                {
                    id = skin.cloth->BACKPACK1;
                    NeedUpdate = true;
                }
                break;
            case 501002: case 501005: case 501008: case 501011: case 501102: case 501105:
                if (sl == 8 && skin.cloth->BACKPACK2 > 0 && id != skin.cloth->BACKPACK2)
                {
                    id = skin.cloth->BACKPACK2;
                    NeedUpdate = true;
                }
                break;
            case 501006: case 501003: case 501009: case 501012: case 501015: case 501106: case 501103:
                if (sl == 8 && skin.cloth->BACKPACK3 > 0 && id != skin.cloth->BACKPACK3)
                {
                    id = skin.cloth->BACKPACK3;
                    NeedUpdate = true;
                }
                break;
        }

        if (sl == 11 && skin.cloth->PARACHUTE > 0 && id != skin.cloth->PARACHUTE)
        {
            id = skin.cloth->PARACHUTE;
            NeedUpdate = true;
        }

        if (sl == 15 && skin.cloth->GLIDER > 0 && id != skin.cloth->GLIDER)
        {
            id = skin.cloth->GLIDER;
            NeedUpdate = true;
        }
    }

    if (NeedUpdate)
    {
        avatar->OnRep_BodySlotStateChanged();
        avatar->ProcessAvatarRectify();
        return true;
    }

    return false;
}
/*
template <typename T>
void GetAllActors(std::vector<T*>& Trace){
    UGameplayStatics* gGameplayStatics = (UGameplayStatics*)gGameplayStatics->StaticClass();
    auto GWorld = GetFullWorld();
    if (GWorld) {
        auto Level = GWorld->PersistentLevel;
        if (Level != NULL){
            std::vector<T*> fuck_list;
            TArray<AActor*> Actors;
            gGameplayStatics->GetAllActorsOfClass((UObject*)GWorld, T::StaticClass(), &Actors);
            if (Actors.Num() > 0){
                for (int i = 0; i < Actors.Num(); i++){
                    if (Actors[i] && Actors[i]->IsA(T::StaticClass()))
                    {
                        fuck_list.push_back((T*)Actors[i]);
                    }
                }
            }
            Trace.clear();
            Trace = fuck_list;
            fuck_list.clear();
        }
    }
}*/





bool carspring;
std::string item_seprated_by_zero(std::vector<FSkinItem> skins)
{
  std::string out;
  for (int i = 0; i < skins.size(); i++)
  {
    string val = skins[i].name2;
    out += val;
    if (i < skins.size())
    {
      out += '\0';
    }
  }
  return out;
}
/*void RenderSkinItem(std::string label, CSkinsInfo& skins, on_change_callback cb = nullptr) {
    ImGui::TableNextRow(); 
    ImGui::TableSetColumnIndex(0);
    std::string combo_label = "##" + label + "##combo"; 
    ImGui::PushItemWidth(200);
    if (!skins.items.empty() && ImGui::Combo(combo_label.c_str(), &skins.current_index, item_seprated_by_zero(skins.items).c_str())) {
        if (skins.out) {
            *skins.out = skins.Get(); 
        }
    }
    ImGui::TableSetColumnIndex(1);
    ImGui::Text(label.c_str());
    ImGui::TableSetColumnIndex(2);
    std::string input_label = "##" + label + "##input";
    if (skins.out) {ImGui::InputScalar(input_label.c_str(), ImGuiDataType_U32, skins.out);
ImGui::TableSetColumnIndex(3);
ImGui::PushFont(Icon);
ImGui::PushID(label.c_str());
if (ImGui::Button(ICON_FA_PASTE)) {
*skins.out = std::stoul(getClipboardText());
}
ImGui::PopID();        
ImGui::PopFont();
}
}*/
void RenderSkinItem(std::string label, CSkinsInfo &skins, on_change_callback cb = nullptr)
{
    std::string combo_label = "##" + label + "##combo";
    std::string input_label = "##" + label + "##intput";
    ImGui::TableNextRow(); 
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.45f);
    if (ImGui::Combo(combo_label.c_str(), &skins.current_index, item_seprated_by_zero(skins.items).c_str()))
    {
        *skins.out = skins.Get();
    }
    ImGui::TableSetColumnIndex(2);
    if (skins.out)
    {
        if (ImGui::InputScalar(input_label.c_str(), ImGuiDataType_U32, skins.out))
        {
        }
    }
    ImGui::TableSetColumnIndex(3);
    ImGui::PushFont(Icon);
    ImGui::PushID(label.c_str());
    if (ImGui::Button(ICON_FA_PASTE)) {
    *skins.out = std::stoul(GetClipboardText());
    }
    ImGui::PopID();        
    ImGui::PopFont();
}

void OnClothSkinChange(int newId)
{
  return;
}
bool bunny_hop = false;
bool auto_strafe =false;
bool infinite_duck = false;
bool edge_jump =false;

bool fake_lag = false;
bool anti_aim = false;
bool bhopcsgo = true;
void (*orig_kill_message_event)(ASTExtraPlayerController*, FFatalDamageParameter*);
void kill_message_event(ASTExtraPlayerController* PlayerController, FFatalDamageParameter* FatalDamageParameter)
{
    if (!PlayerController || !FatalDamageParameter)
    {
        return orig_kill_message_event(PlayerController, FatalDamageParameter);
    }

    if (skin.bEnable && skin.bKillMsg && PlayerController->PlayerKey == FatalDamageParameter->CauserKey)
    {
        if (skin.cloth->SHIRT > 0)
        {
            FatalDamageParameter->CauserClothAvatarID = skin.cloth->SHIRT;
        }

        if (skin.bGunSkin && Last_Gun_Used_To_Kill > 0)
        {
            FatalDamageParameter->CauserWeaponAvatarID = Last_Gun_Used_To_Kill;
        }
    }
    orig_kill_message_event(PlayerController, FatalDamageParameter);
}
void NekoHook(FRotator &angles) {
    if (angles.Pitch > 180)
        angles.Pitch -= 360;
    if (angles.Pitch < -180)
        angles.Pitch += 360;

    if (angles.Pitch < -75.f)
        angles.Pitch = -75.f;
    else if (angles.Pitch > 75.f)
        angles.Pitch = 75.f;

    while (angles.Yaw < -180.0f)
        angles.Yaw += 360.0f;
    while (angles.Yaw > 180.0f)
        angles.Yaw -= 360.0f;
}
void NekoHook(float *angles) {
    if (angles[0] > 180)
        angles[0] -= 360;
    if (angles[0] < -180)
        angles[0] += 360;

    if (angles[0] < -75.f)
        angles[0] = -75.f;
    else if (angles[0] > 75.f)
        angles[0] = 75.f;

    while (angles[1] < -180.0f)
        angles[1] += 360.0f;
    while (angles[1] > 180.0f)
        angles[1] -= 360.0f;
}
void NekoHook(Vector3 angles) {
    if (angles.X > 180)
        angles.X -= 360;
    if (angles.X < -180)
        angles.X += 360;

    if (angles.X < -75.f)
        angles.X = -75.f;
    else if (angles.X > 75.f)
        angles.X = 75.f;

    while (angles.Y < -180.0f)
        angles.Y += 360.0f;
    while (angles.Y > 180.0f)
        angles.Y -= 360.0f;
}
long GetEpochTime()
{
auto duration = std::chrono::system_clock::now().time_since_epoch();
return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}
bool Smoothes;
bool Unlock;
void DrawMemory(UCanvas* Canvas, int ScreenWidth, int ScreenHeight) {
    if (Unlock) {
    static USTExtraGameInstance* FPS = nullptr;
    if (!FPS) FPS = UObject::FindObject<USTExtraGameInstance>("STExtraGameInstance Transient.UAEGameEngine_1.STExtraGameInstance_1");
    if (FPS) {
        auto& UserSettings = FPS->UserDetailSetting;
        UserSettings.PUBGDeviceFPSDef = UserSettings.PUBGDeviceFPSLow = UserSettings.PUBGDeviceFPSMid =
        UserSettings.PUBGDeviceFPSHigh = UserSettings.PUBGDeviceFPSHDR = UserSettings.PUBGDeviceFPSUltralHigh = 120;
        UserSettings.DeviceMaxQualityLevel = 5;
    }
    }

    if (!Canvas) return;

    auto GWorld = GetFullWorld();
    ASTExtraPlayerController* localController = nullptr;
    ASTExtraPlayerCharacter* localPlayer = nullptr;
    ASTExtraLobbyCharacter* localLobby = nullptr;
    if (GWorld && GWorld->PersistentLevel && GWorld->NetDriver && GWorld->NetDriver->ServerConnection) {
        auto ServerConnection = GWorld->NetDriver->ServerConnection;
        localController = static_cast<ASTExtraPlayerController*>(ServerConnection->PlayerController);
        if (localController && localController->AcknowledgedPawn) {
            localPlayer = static_cast<ASTExtraPlayerCharacter*>(localController->AcknowledgedPawn);
        }
    }

    if (localController) {
        auto Actors = GetNecessaryActors();
        for (auto Actor : Actors) {
            if (!Actor) continue;
            if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass())) {
                auto PlayerCharacter = static_cast<ASTExtraPlayerCharacter*>(Actor);
                if (PlayerCharacter->PlayerKey == localController->PlayerKey) {
                    localPlayer = PlayerCharacter;
                    break;
                }
            }
        }
    }
    Data::localController = localController;
    Data::localPlayer = localPlayer;
    if (!Data::localPlayer || !Data::localController) return;
    if (Data::localPlayer->PartHitComponent) {
        auto& ConfigCollisionDistSqAngles = Data::localPlayer->PartHitComponent->ConfigCollisionDistSqAngles;
        for (int j = 0; j < ConfigCollisionDistSqAngles.Num(); j++) ConfigCollisionDistSqAngles[j].Angle = 180.0f;
    }
   
    if (Data::localController) {
    if (Data::localPlayer && Data::localPlayer->RootComponent) {
    if (Data::localPlayer->PartHitComponent)  {
    auto ConfigCollisionDistSqAngles = Data::localPlayer->PartHitComponent->ConfigCollisionDistSqAngles;
    for (int j = 0; j < ConfigCollisionDistSqAngles.Num(); j++) {ConfigCollisionDistSqAngles[j].Angle = 180.0f;}
    Data::localPlayer->PartHitComponent->ConfigCollisionDistSqAngles = ConfigCollisionDistSqAngles;
    }		
    if (Data::localController) {
    if (Data::localPlayer && Data::localPlayer->RootComponent) {
    if (Data::localPlayer->PartHitComponent)  {
    auto ConfigCollisionDistSqAngles = Data::localPlayer->PartHitComponent->ConfigCollisionDistSqAngles;
    for (int j = 0; j < ConfigCollisionDistSqAngles.Num(); j++) {ConfigCollisionDistSqAngles[j].Angle = 180.0f;}
    Data::localPlayer->PartHitComponent->ConfigCollisionDistSqAngles = ConfigCollisionDistSqAngles;
    }
    UpdateCurrentWeapon();
    auto Actors = GetNecessaryActors();
    for (auto Actor : Actors) {
        if (!Actor) continue;
    if (skin.bEnable)
    {
    int kill_message_event_idx = 1002;
    auto VTable = (void **)Data::localController->VTable;

    auto f_mprotect = [](uintptr_t addr, size_t len, int32_t prot) -> int32_t {
        static_assert(PAGE_SIZE == 4096);
        constexpr size_t page_size = static_cast<size_t>(PAGE_SIZE);
        void *start = reinterpret_cast<void *>(addr & -page_size);
        uintptr_t end = (addr + len + page_size - 1) & -page_size;
        return mprotect(start, end - reinterpret_cast<uintptr_t>(start), prot);
    };

    if (VTable && (VTable[kill_message_event_idx] != kill_message_event)) {
        orig_kill_message_event = decltype(orig_kill_message_event)(VTable[kill_message_event_idx]);
        f_mprotect((uintptr_t)(&VTable[kill_message_event_idx]), sizeof(uintptr_t), PROT_READ | PROT_WRITE);
        VTable[kill_message_event_idx] = (void *)kill_message_event;
        f_mprotect((uintptr_t)(&VTable[kill_message_event_idx]), sizeof(uintptr_t), PROT_READ | PROT_EXEC);
    }
    if (Data::localPlayer->WeaponManagerComponent && Data::localPlayer->WeaponManagerComponent->CurrentWeaponReplicated) {
        auto CurrentWeaponReplicated = (ASTExtraShootWeapon *)Data::localPlayer->WeaponManagerComponent->CurrentWeaponReplicated;
        Current_Weapon = CurrentWeaponReplicated->GetWeaponID();
        Last_Gun_Used_To_Kill = GetWeap(Current_Weapon);
    }
    if (localPlayer -> WeaponManagerComponent->CurrentWeaponReplicated) {
        if (localPlayer -> WeaponManagerComponent->CurrentWeaponReplicated -> GrenadeAvatarComponent) {
          auto Weaponid =  localPlayer -> WeaponManagerComponent->CurrentWeaponReplicated -> GetWeaponID();
          
          if (skin.bEnable && Weaponid == 602002) {// | Дымовая граната
          localPlayer -> WeaponManagerComponent->CurrentWeaponReplicated -> GrenadeAvatarComponent -> PreChangeGrenadeAvatar(skin.cloth->SMOKE);
          }
          if (skin.bEnable && Weaponid == 602003) {// | Коктейль Молотова
          localPlayer -> WeaponManagerComponent->CurrentWeaponReplicated -> GrenadeAvatarComponent -> PreChangeGrenadeAvatar(skin.cloth->MOLOTOV);
          }
          if (skin.bEnable && Weaponid == 602004) {// | Осколочная граната
          localPlayer -> WeaponManagerComponent->CurrentWeaponReplicated -> GrenadeAvatarComponent -> PreChangeGrenadeAvatar(skin.cloth->HAE);
          }          
        }
      }
        if (Data::localPlayer->WeaponManagerComponent != 0) {
            if (Data::localPlayer->WeaponManagerComponent->CurrentWeaponReplicated != 0) {
                auto weapowep = Data::localPlayer->WeaponManagerComponent->CurrentWeaponReplicated;
                auto currentTime = std::chrono::steady_clock::now();
                auto landchud = Data::localPlayer->WeaponManagerComponent->CurrentWeaponReplicated->synData;
                auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastWeaponChangeTime).count();
                if (timeDiff > 1000) {
                    ChangeGunSkin(weapowep);
                    Data::localPlayer->WeaponManagerComponent->CurrentWeaponReplicated->DelayHandleAvatarMeshChanged();
                    lastWeaponChangeTime = currentTime;
                }
            }
        }
        if (skin.bBackPack && Data::localController) {
            auto Backpack = (UBackpackComponent*)Data::localController->BackpackComponent;
            if (Backpack) {
                auto& bag = Backpack->ItemListNet.IncArray;
                bool RefreshSkin = false;
                bool isFoundAvatar = false;
                if (std::time(nullptr) - lastRun < 1) return;
                lastRun = std::time(nullptr);
                for (int j = 0; j < bag.Num(); j++) {
                    if (!bag.IsValidIndex(j)) continue;
                    int ID = bag[j].Unit.DefineID.TypeSpecificID;
                    auto& AdditionalData = bag[j].Unit.AdditionalData;
                    /*int NewSkinID = GetWeap(ID);
                    if (NewSkinID > 0) {
                            for (int k = 0; k < AdditionalData.Num(); k++) {
                                if (!AdditionalData.IsValidIndex(k)) continue;
                                if (!bag.IsValidIndex(j)) break;

                                if (AdditionalData[k].EDataType == EBattleItemAdditionalDataType::EBattleItemAdditionalDataType__WeaponAvatar) {
                                    AdditionalData[k].IntData = NewSkinID;
                                    RefreshSkin = true;
                                }
                            }
                            if (!bag.IsValidIndex(j)) continue;
                            if (!isFoundAvatar) {
                                FBattleItemAdditionalData NewEntry;
                                NewEntry.EDataType = EBattleItemAdditionalDataType::EBattleItemAdditionalDataType__WeaponAvatar;
                                NewEntry.IntData = NewSkinID;
                                AdditionalData.Add(NewEntry);
                            }
                        }       */             
                    if (!bag.IsValidIndex(j)) continue;
                    if (ID == 1400129 || ID == 140569 || ID == 403003) {
                        if (skin.cloth->SHIRT > 0) {
                            bag[j].Unit.DefineID.TypeSpecificID = skin.cloth->SHIRT;
                            RefreshSkin = true;
                        }
                    } else {
                        switch (ID) {
                            case 502004: case 502001: case 502110: case 502107: case 502104:
                                if (skin.cloth->HELMET1 > 0) {
                                    bag[j].Unit.DefineID.TypeSpecificID = skin.cloth->HELMET1;
                                    RefreshSkin = true;
                                }
                                break;
                            case 502005: case 502002: case 502111: case 502108: case 502105:
                                if (skin.cloth->HELMET2 > 0) {
                                    bag[j].Unit.DefineID.TypeSpecificID = skin.cloth->HELMET2;
                                    RefreshSkin = true;
                                }
                                break;
                            case 502106: case 502109: case 502112: case 502003:
                                if (skin.cloth->HELMET3 > 0) {
                                    bag[j].Unit.DefineID.TypeSpecificID = skin.cloth->HELMET3;
                                    RefreshSkin = true;
                                }
                                break;
                            case 501001: case 501004: case 501007: case 501010: case 501101: case 501104:
                                if (skin.cloth->BACKPACK1 > 0) {
                                    bag[j].Unit.DefineID.TypeSpecificID = skin.cloth->BACKPACK1;
                                    RefreshSkin = true;
                                }
                                break;
                            case 501002: case 501005: case 501008: case 501011: case 501102: case 501105:
                                if (skin.cloth->BACKPACK2 > 0) {
                                    bag[j].Unit.DefineID.TypeSpecificID = skin.cloth->BACKPACK2;
                                    RefreshSkin = true;
                                }
                                break;
                            case 501006: case 501003: case 501009: case 501012: case 501015: case 501106: case 501103:
                                if (skin.cloth->BACKPACK3 > 0) {
                                    bag[j].Unit.DefineID.TypeSpecificID = skin.cloth->BACKPACK3;
                                    RefreshSkin = true;
                                }
                                break;
                        }
                    }
                }
                if (RefreshSkin) {
                    isFoundAvatar = false;
                    Backpack->OnRep_ItemListNet();
                }
            }
         }
if (skin.bVehicleSkin) {
if (!localPlayer || !localPlayer->CurrentVehicle || !localPlayer->CurrentVehicle->VehicleAvatar) return;
int defaultSkinId = localPlayer->CurrentVehicle->VehicleAvatar->GetDefaultAvatarID();
std::string skinIdStr = std::to_string(defaultSkinId);
Active::SkinCarDefault = localPlayer->CurrentVehicle->GetAvatarID();
Active::SkinCarNew = false;
struct SkinMapEntry {
bool enabled;
const char* matchId;
int skinMod;
};
SkinMapEntry skinMap[] = {
{skin.bVehicleSkin,"1901",skin.vehicle->MOTOR},
{skin.bVehicleSkin,"1903",skin.vehicle->DACIA},
{skin.bVehicleSkin,"1914",skin.vehicle->MIRADO},
{skin.bVehicleSkin,"1915",skin.vehicle->MIRADO},
{skin.bVehicleSkin,"1907",skin.vehicle->BUGGY},
{skin.bVehicleSkin,"1961",skin.vehicle->COUPE},
{skin.bVehicleSkin,"1908",skin.vehicle->UAZ}
};
for (const auto& entry : skinMap) {
if (entry.enabled && skinIdStr.find(entry.matchId) != std::string::npos) {
Active::SkinCarMod = entry.skinMod;
Active::SkinCarNew = true;
break;
}
}
if (Active::SkinCarNew && Active::SkinCarDefault != Active::SkinCarMod) {
localPlayer->CurrentVehicle->VehicleAvatar->OnRep_VehicleAvatarData();
localPlayer->CurrentVehicle->VehicleAvatar->ChangeItemAvatar(Active::SkinCarMod, true);
}
}
        if (skin.bDeadbox) {
            for (auto Actor : Actors) {
                if (!Actor) continue;
                if (Actor->IsA(APlayerTombBox::StaticClass())) {
                    auto TombBoxx = static_cast<APlayerTombBox*>(Actor);
                    if (TombBoxx && TombBoxx->DamageCauser && TombBoxx->TargetPlayer) {
                        if (TombBoxx->DamageCauser->PlayerKey == Data::localPlayer->PlayerKey) {
                            auto PlayerKey = TombBoxx->TargetPlayer->PlayerKey;
                            if (AlreadyChangedSet.find(PlayerKey) == AlreadyChangedSet.end()) {
                                auto DeadBoxAvatarCompPtr = (DWORD*)((DWORD)TombBoxx + Data::Offset::DeadBoxAvatarComponent_BP_C);
                                if (DeadBoxAvatarCompPtr && Data::localPlayer->WeaponManagerComponent && Data::localPlayer->PlayerState) {
                                    auto DeadBoxAvatarComp = *DeadBoxAvatarCompPtr;
                                    auto CurrentWeap = Data::localPlayer->WeaponManagerComponent->CurrentWeaponReplicated;
                                    if (CurrentWeap != nullptr) {
                                        auto ID = CurrentWeap->GetWeaponID();
                                        int NewSkinID = GetWeap(ID);

                                        if (NewSkinID > 0) {
                                            ChangeItemAVc(DeadBoxAvatarComp, NewSkinID);
                                            AlreadyChangedSet.insert(PlayerKey);
                                            DeadBoxTimestamps[PlayerKey] = std::time(nullptr);
                                            DeadBoxSkinCount++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            static float CleanupTimer = 0.0f;
            float DeltaTime = GetFullWorld() ? GetFullWorld()->PersistentLevel->WorldSettings->TimeDilation : 1.0f;
            CleanupTimer += DeltaTime;
            if (CleanupTimer >= 10.0f) {
                CleanUpDeadBoxSet();
                CleanupTimer = 0.0f;
            }
        }
}
}
}
}
}
}
}
DWORD GetVehSkinModded(DWORD old)
{
    switch (old)
    {
    case 1901001: // motor
    case 1901002:
        return skin.vehicle->MOTOR;
    case 1961001: // coupe
        return skin.vehicle->COUPE;
    case 1908001: // UAZ
    case 1910001: // UAZ OPEN
        return skin.vehicle->UAZ;
    case 1903001: // DACIA
        return skin.vehicle->DACIA;
    case 1914001:
    case 1914002:
    case 1914003:
    case 1914004:
    case 1915001:
    case 1915002:
    case 1915003:
    case 1915004:
        return skin.vehicle->MIRADO;
    case 1907001:
    case 1907002:
    case 1907003:
    case 1907004:
    case 1907005:
    case 1907006:
        return skin.vehicle->BUGGY;
    default:
        return 0;
    }
} 
void *(*oProcessEvent)(UObject *pObj, UFunction *pFunc, void *pArgs);
void* hkProcessEvent(UObject *a1, UFunction *a, void *b) {
    if (!a1 || !a) return oProcessEvent(a1, a, b);
    auto fnc = a->GetFullName();
    auto clazz = a1->GetFullName();
    if (ReceiveDrawHUD_Index == -1 && fnc.find("Function Engine.HUD.ReceiveDrawHUD") != std::string::npos) ReceiveDrawHUD_Index = a->InternalIndex;
    if (OnRep_AvatarMeshChanged_Index == -1 && fnc.find("ShadowTrackerExtra.STExtraWeapon.OnRep_AvatarMeshChanged") != std::string::npos) OnRep_AvatarMeshChanged_Index = a->InternalIndex;
    if (CreateBattleItemHandle_Index == -1 && fnc.find("BackpackBlueprintUtils_BP_C.CreateBattleItemHandle") != std::string::npos) CreateBattleItemHandle_Index = a->InternalIndex;
    if (CreateWeaponAndChangeSkin_Index == -1 && fnc.find("CreateWeaponAndSkin") != std::string::npos) CreateWeaponAndChangeSkin_Index = a->InternalIndex;
    if (OnRep_BodySlotStateChanged_Index == -1 && fnc.find("ShadowTrackerExtra.CharacterAvatarComponent2.OnRep_BodySlotStateChanged") != std::string::npos) OnRep_BodySlotStateChanged_Index = a->InternalIndex;
    if (ClientOnDamageToOther_Index == -1 && fnc.find("ClientOnDamageToOther") != std::string::npos) ClientOnDamageToOther_Index = a->InternalIndex;
    if (skin.bDeadbox && DeadBoxAvatarComponent_GetLuaFilePath_Index == -1 && clazz.find("DeadBoxAvatarComponent_BP") != std::string::npos && clazz.find("PersistentLevel") != std::string::npos && fnc.find("GetLuaFilePath") != std::string::npos)  DeadBoxAvatarComponent_GetLuaFilePath_Index = a->InternalIndex;
    if (skin.bEnable) {
     if (skin.bGunSkin && a->InternalIndex == OnRep_AvatarMeshChanged_Index) {
            auto gun = reinterpret_cast<ASTExtraWeapon *>(a1);
            if (gun) ChangeGunSkin(gun);
    } else if (a->InternalIndex == CreateWeaponAndChangeSkin_Index) {
        if (skin.bGunSkin) {
            auto params = reinterpret_cast<ULobbyWeaponManagerComponent_CreateWeaponAndSkin_Params *>(b);
            if (params) {
                params->bSync = false;
                params->bUse = true;
                int g_WeaponID = params->WeaponSkinID;
                DWORD gun_val = GetWeap(g_WeaponID);
                if (gun_val > 0) {
                    params->WeaponSkinID = gun_val;
                }
            }
        }
    } else if (a->InternalIndex == OnRep_BodySlotStateChanged_Index) {
        if (Data::localController && skin.bClothSkin) {
            auto avatar = reinterpret_cast<UCharacterAvatarComponent2 *>(a1);
            if (avatar) {
                UpdateClothSkin(avatar);
            }
        }
    } else {
        // Другие специфичные случаи
        auto fnc = a->GetFullName();
        if (!Data::localController) {
            if (skin.bVehicleSkin && fnc.find("VehicleAvatarComponent_BP.VehicleAvatarComponent_BP_C.GetItemAvatarHandle") != std::string::npos) {
                auto PARAMS = reinterpret_cast<UVehicleAvatarComponent_GetItemAvatarHandle_Params *>(b);
                if (PARAMS) {
                    int neww = GetVehSkinModded(PARAMS->ItemId);
                    if (neww > 0) PARAMS->ItemId = neww;
                }
    } else if (skin.bClothSkin && fnc.find("GetServerForceHideState") != std::string::npos) {
                auto avatar = reinterpret_cast<UCharacterAvatarComponent2 *>(a1);
                if (avatar) UpdateClothSkinLobby(avatar);
    } else if (skin.bDeadbox && DeadBoxAvatarComponent_GetLuaFilePath_Index == a->InternalIndex &&
            a1->GetName().find("DeadBoxAvatarComponent") != std::string::npos) {
            auto DeadBoxPointer = reinterpret_cast<UDeadBoxAvatarComponent *>(a1);
            if (DeadBoxPointer && Last_Gun_Used_To_Kill > 0) DeadBoxPointer->AsyncChangeItemAvatar(Last_Gun_Used_To_Kill);
            //return oProcessEvent(a1, a, b);
        }
      }
    }
  }
    return oProcessEvent(a1, a, b);
}


bool NoSmoke,NoFog,DarkSky;  
static bool openSavePopup = false;
static bool openLoadPopup = false;
static char newFileName[64] = "";
bool KillCounter;
static bool ShowHideRecordMsg = false;

#include <jni.h>
#include <string>

JavaVM* g_JavaVM;

void open_url(const char* url) {
    JNIEnv* env = nullptr;
    g_JavaVM->AttachCurrentThread(&env, 0);
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentApplication = env->GetStaticMethodID(activityThread, "currentApplication", "()Landroid/app/Application;");
    jobject context = env->CallStaticObjectMethod(activityThread, currentApplication);
    jclass uriClass = env->FindClass("android/net/Uri");
    jmethodID parseMethod = env->GetStaticMethodID(uriClass, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
    jstring jUrl = env->NewStringUTF(url);
    jobject uri = env->CallStaticObjectMethod(uriClass, parseMethod, jUrl);
    jclass intentClass = env->FindClass("android/content/Intent");
    jfieldID actionViewField = env->GetStaticFieldID(intentClass, "ACTION_VIEW", "Ljava/lang/String;");
    jobject actionView = env->GetStaticObjectField(intentClass, actionViewField);
    jmethodID intentConstructor = env->GetMethodID(intentClass, "<init>", "(Ljava/lang/String;Landroid/net/Uri;)V");
    jobject intent = env->NewObject(intentClass, intentConstructor, actionView, uri);
    jmethodID addFlags = env->GetMethodID(intentClass, "addFlags", "(I)Landroid/content/Intent;");
    env->CallObjectMethod(intent, addFlags, 0x10000000);
    jclass contextClass = env->GetObjectClass(context);
    jmethodID startActivity = env->GetMethodID(contextClass, "startActivity", "(Landroid/content/Intent;)V");
    env->CallVoidMethod(context, startActivity, intent);
    env->DeleteLocalRef(jUrl);
}

static int selectedConfigIndex = -1;

void draw_tab1(const char* str_id, int* selected, const std::vector<std::string>& tabs) {
    ImGuiStyle& style = ImGui::GetStyle();    
    style.Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImColor(80, 27, 26);
    style.Colors[ImGuiCol_ButtonHovered] = ImColor(80, 27, 26);    
    ImGui::BeginGroup();    
    for (int i = 0; i < tabs.size(); i++) {
        bool is_selected = (*selected == i);        
        if (is_selected) {ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));}        
        if (ImGui::Button(tabs[i].c_str(), ImVec2(ImGui::GetWindowWidth() / tabs.size() - style.ItemSpacing.x, 0))) {*selected = i;}        
        if (is_selected) {ImGui::PopStyleColor(1);}        
        if (i < tabs.size() - 1) {ImGui::SameLine();}
    }    
    ImGui::EndGroup();
}

int menu_index = 0;
bool HideWindow = true;
bool ShowMainMenu = true;
bool cheatDisabled = false;

static ImVec2 buttonPos = ImVec2(50, 200);
static bool isDragging = false;
static ImVec2 dragOffset;
static bool clickInProgress = false;
static double clickStartTime = 0.0f;

void AddTextWithStroke(ImDrawList* draw, const ImVec2& pos, ImU32 color, const char* text, float scale, ImU32 strokeColor) {
    if (!draw || !text) return;
    ImVec2 textPos = pos;
    for (float x = -1.0f; x <= 1.0f; x += 1.0f) {
        for (float y = -1.0f; y <= 1.0f; y += 1.0f) {
            if (x == 0.0f && y == 0.0f) continue;
            draw->AddText(NULL, 20.0f * scale, ImVec2(textPos.x + x, textPos.y + y), strokeColor, text);
        }
    }
    draw->AddText(NULL, 20.0f * scale, textPos, color, text);
}

void OnVehicleSkinChange(int newId) {
    if (!skin.bVehicleSkin) return;
    auto localPlayer = Data::localPlayer;
    if (!localPlayer) return;
    if (localPlayer->CurrentVehicle && localPlayer->CurrentVehicle->VehicleAvatar) {
        int ava = localPlayer->CurrentVehicle->VehicleAvatar->GetDefaultAvatarID();
        int neww = GetVehSkinModded(ava);
        if (neww > 0) {
            localPlayer->CurrentVehicle->VehicleAvatar->ChangeItemAvatar(neww, 0);
        }
    }
}

static std::vector<std::string> configFiles;

void EnsureConfigFolder() {
    std::string path = "/sdcard/Android/data/com.tencent.ig/files/configs";
    mkdir(path.c_str(), 0777);
}

void LoadConfigList() {
    EnsureConfigFolder();
    configFiles.clear();
    std::string path = "/sdcard/Android/data/com.tencent.ig/files/configs";
    DIR* dir = opendir(path.c_str());
    if (dir) {
        struct dirent* ent;
        while ((ent = readdir(dir)) != NULL) {
            if (strstr(ent->d_name, ".dat")) {
                configFiles.push_back(ent->d_name);
            }
        }
        closedir(dir);
    }
}

int OpenedMenus = 0;

void draw_skin() {
    float col1 = 15.0f;
    float col2 = 320.0f;
    float col3 = 620.0f;

    ImGui::SetCursorPosX(col1);
    ImGui::Checkbox("Enable", &skin.bEnable);
    ImGui::SameLine(col2);
    ImGui::Checkbox("Gun mod", &skin.bGunSkin);
    ImGui::SameLine(col3);
    ImGui::Checkbox("Outfit mod", &skin.bClothSkin);

    ImGui::SetCursorPosX(col1);
    ImGui::Checkbox("Vehicle mod", &skin.bVehicleSkin);
    ImGui::SameLine(col2);
    ImGui::Checkbox("Emote mod", &skin.bEmote);
    ImGui::SameLine(col3);
    ImGui::Checkbox("Deadbox mod", &skin.bDeadbox);

    ImGui::SetCursorPosX(col1);
    ImGui::Checkbox("Kill message", &skin.bKillMsg);
    ImGui::SameLine(col2);
    ImGui::Checkbox("Backpack mod", &skin.bBackPack);
    ImGui::SameLine(col3);
    ImGui::Checkbox("Kill counter", &KillCounter);

    ImGui::Separator();
    ImGui::Checkbox("Metro Royal Mode", &skin.bMetro);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.15f, 0.15f, 1.0f));
    ImGui::Text("%s Turn on if you want to mod skin in metro royal mode, turn off if only normal mode", ICON_FA_EXCLAMATION_TRIANGLE);
    ImGui::Text("%s Suit mod may cause 1 second banned.", ICON_FA_EXCLAMATION_TRIANGLE);
    ImGui::PopStyleColor();

    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 6));

    if (ImGui::BeginTabBar("##MODTABBAR")) {
        if (skin.bGunSkin && ImGui::BeginTabItem("Gun mod")) {
            if (ImGui::BeginTable("##Guns Skin", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 420))) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Custom", ImGuiTableColumnFlags_WidthFixed, 170.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableHeadersRow();
                RenderSkinItem("AKM", skindata.akm);
                RenderSkinItem("M416", skindata.m416);
                RenderSkinItem("SCAR-L", skindata.scarl);
                RenderSkinItem("M16A4", skindata.m16);
                RenderSkinItem("AUG", skindata.aug);
                RenderSkinItem("M762", skindata.m762);
                RenderSkinItem("GROZA", skindata.groza);
                RenderSkinItem("ACE32", skindata.ace32);
                RenderSkinItem("QBZ", skindata.qbz);
                RenderSkinItem("HONEY", skindata.honey);
                RenderSkinItem("UMP45", skindata.ump45);
                RenderSkinItem("VECTOR", skindata.vector);
                RenderSkinItem("Tommy Gun", skindata.tommy);
                RenderSkinItem("UZI", skindata.uzi);
                RenderSkinItem("KAR-98", skindata.kar98);
                RenderSkinItem("M24", skindata.m24);
                RenderSkinItem("AWM", skindata.awm);
                RenderSkinItem("AMR", skindata.amr);
                RenderSkinItem("MK14", skindata.mk14);
                RenderSkinItem("MINI14", skindata.mini14);
                RenderSkinItem("M249", skindata.m249);
                RenderSkinItem("DP-28", skindata.dp28);
                RenderSkinItem("MG-3", skindata.mg3);
                RenderSkinItem("XM1014", skindata.xm1014);
                RenderSkinItem("S12K", skindata.s12k);
                RenderSkinItem("DBS", skindata.dbs);
                RenderSkinItem("PAN", skindata.pan);
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        if (skin.bClothSkin && ImGui::BeginTabItem("Outfit mod")) {
            if(ImGui::BeginTable("##Cloth skim", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 420))) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Custom", ImGuiTableColumnFlags_WidthFixed, 170.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableHeadersRow();
                RenderSkinItem("Cloth", skindata.cloth);
                RenderSkinItem("Gloves", skindata.gloves);
                RenderSkinItem("Face", skindata.face);
                RenderSkinItem("Mask", skindata.mask);
                RenderSkinItem("Hat", skindata.hat);
                RenderSkinItem("Pant", skindata.pant);
                RenderSkinItem("Shoe", skindata.shoe);
                RenderSkinItem("Parachute", skindata.parachute);
                RenderSkinItem("Glider", skindata.glider);
                RenderSkinItem("Helmet", skindata.helmet1);
                RenderSkinItem("Helmet 2", skindata.helmet2);
                RenderSkinItem("Helmet 3", skindata.helmet3);
                RenderSkinItem("Backpack", skindata.backpack1);
                RenderSkinItem("Backpack 2", skindata.backpack2);
                RenderSkinItem("Backpack 3", skindata.backpack3);
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        if (skin.bVehicleSkin && ImGui::BeginTabItem("Vehicle mod")) {
            if(ImGui::BeginTable("##Vehicle Skim", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 420))) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Custom", ImGuiTableColumnFlags_WidthFixed, 170.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableHeadersRow();
                RenderSkinItem("Coupe-RB", skindata.coupe, OnVehicleSkinChange);
                RenderSkinItem("UAZ", skindata.uaz, OnVehicleSkinChange);
                RenderSkinItem("DACIA", skindata.dacia, OnVehicleSkinChange);
                RenderSkinItem("Mirado", skindata.mirado, OnVehicleSkinChange);
                RenderSkinItem("MOTOR", skindata.motor, OnVehicleSkinChange);
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        if (skin.bEmote && ImGui::BeginTabItem("Emote mod")) {
            if(ImGui::BeginTable("##Emote skin", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 420))) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Custom", ImGuiTableColumnFlags_WidthFixed, 170.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableHeadersRow();
                RenderSkinItem("Emotes 1", skindata.emote1);
                RenderSkinItem("Emotes 2", skindata.emote2);
                RenderSkinItem("Emotes 3", skindata.emote3);
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::PopStyleVar(2);
}

void draw_login() {
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::Begin("ACTIVE KEY", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {
        static char keyBuffer[128] = "@nthmodcheat-720-AWw42";
        ImGui::Text("Enter your license");
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 90);
        ImGui::InputTextWithHint("##key", "Enter key here", keyBuffer, IM_ARRAYSIZE(keyBuffer));
        ImGui::SameLine();
        ImGui::PopItemWidth();
        if (ImGui::Button("  Login", ImVec2(-1, 50))) {
            iLogin = true;
        }
        if (ImGui::Button(" TELEGRAM", ImVec2(-1, 50))) {
            open_url("https://t.me/NTHMOD2025");
        }
    }
    ImGui::End();
}

static ImVec2 menuSize = ImVec2(400, 500);
static ImVec2 menuPos = ImVec2(100, 100);

void DrawNthMenu() {
    if (cheatDisabled) return;

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGuiWindowFlags textFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings;
    if (ImGui::Begin("##SkinText", nullptr, textFlags)) {
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetWindowPos();
        AddTextWithStroke(draw, ImVec2(pos.x + 8, pos.y + 8), IM_COL32(255, 255, 255, 255), "SKIN ONECORE", 1.2f, IM_COL32(0, 0, 0, 180));
        ImGui::End();
    }

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::SetNextWindowPos(buttonPos);
    ImGui::SetNextWindowSize(ImVec2(50, 50));

    ImGuiWindowFlags buttonFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin("##SkinButton", nullptr, buttonFlags)) {
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();

        draw->AddCircleFilled(ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f), 25.0f, IM_COL32(15, 18, 25, 240), 32);
        draw->AddCircle(ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f), 23.0f, IM_COL32(255, 255, 255, 230), 32, 2.0f);
        draw->AddCircle(ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f), 19.0f, IM_COL32(255, 255, 255, 60), 32, 1.2f);

        ImVec2 textSize = ImGui::CalcTextSize("S");
        ImVec2 textPos = ImVec2(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
        draw->AddText(textPos, IM_COL32(255, 255, 255, 230), "S");

        ImVec2 mousePos = ImGui::GetMousePos();
        bool hovered = mousePos.x >= pos.x && mousePos.x <= pos.x + size.x && mousePos.y >= pos.y && mousePos.y <= pos.y + size.y;

        if (hovered) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                clickInProgress = true;
                clickStartTime = ImGui::GetTime();
                isDragging = true;
                dragOffset = ImVec2(buttonPos.x - mousePos.x, buttonPos.y - mousePos.y);
            }
        }

        if (isDragging && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            mousePos = ImGui::GetMousePos();
            buttonPos = ImVec2(mousePos.x + dragOffset.x, mousePos.y + dragOffset.y);
            if (glWidth > 0) {
                buttonPos.x = std::clamp(buttonPos.x, 0.0f, (float)glWidth - 50.0f);
                buttonPos.y = std::clamp(buttonPos.y, 0.0f, (float)glHeight - 50.0f);
            }
        }

        if (clickInProgress && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            clickInProgress = false;
            isDragging = false;
            if (ImGui::GetTime() - clickStartTime < 0.2f) {
                ShowMainMenu = !ShowMainMenu;
            }
        }
        ImGui::End();
    }

    if (!ShowMainMenu) return;

    if (!iLogin) {
        draw_login();    
    } else {
        char buf[128];
        sprintf(buf, "@nthmodcheat 4.4.0 %.1f FPS###Title", ImGui::GetIO().Framerate);
        
        ImGui::SetNextWindowSize(menuSize, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(menuPos, ImGuiCond_FirstUseEver);
        
        if (ImGui::Begin(buf, &ShowMainMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings)) {
            menuSize = ImGui::GetWindowSize();
            menuPos = ImGui::GetWindowPos();
            
            std::vector<std::string> menus = {ICON_FA_TSHIRT " SKIN"};
            draw_tab1("topbar", &menu_index, menus);
            ImGui::Separator();
            ImGui::BeginChild("main_child", {0, 0}, false);
            ImGui::Spacing();
            draw_skin();
            ImGui::EndChild();
        }
        ImGui::End();
    }
}

#define _GNU_SOURCE
#include <dlfcn.h>
uintptr_t UE4;
#include "Hooks.cpp"

void NthTheme1() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(4, 3);
    style.WindowRounding = 0.0f;
    style.FramePadding = ImVec2(5, 5);
    style.FrameRounding = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.TabRounding = 2.0f;
    style.ScrollbarRounding = 0.0f;
    style.WindowTitleAlign = ImVec2(0.5, 0.5);
    style.ButtonTextAlign = ImVec2(0.5,0.5);  
    style.Alpha = 0.97;                
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.09f, 0.09f, 0.09f, 0.80f); 
    style.Colors[ImGuiCol_FrameBg]               = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImColor(0, 0, 0, 255);
    style.Colors[ImGuiCol_PopupBg]               = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_Button]                = ImColor(20, 20, 20, 0);
    style.Colors[ImGuiCol_ButtonActive]          = ImColor(20, 20, 20, 0);
    style.Colors[ImGuiCol_ButtonHovered]         = ImColor(20, 20, 20, 0);
    style.Colors[ImGuiCol_TitleBg]               = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_CheckMark]             = ImColor(255, 0, 0, 255);
    style.Colors[ImGuiCol_SliderGrab]            = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImColor(0.710f, 0.169f, 0.169f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImColor(0.710f, 0.169f, 0.169f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImColor(0.710f, 0.169f, 0.169f, 1.0f);
    style.ScaleAllSizes(std::max(2.0f, density / 600.0f));
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
}

void NthTheme2() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(4, 3);
    style.WindowRounding = 0.0f;
    style.FramePadding = ImVec2(5, 5);
    style.FrameRounding = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.TabRounding = 2.0f;
    style.ScrollbarRounding = 0.0f;
    style.WindowTitleAlign = ImVec2(0.5, 0.5);
    style.ButtonTextAlign = ImVec2(0.5,0.5);  
    style.Alpha = 0.97;                
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.09f, 0.09f, 0.09f, 0.80f); 
    style.Colors[ImGuiCol_FrameBg]               = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImColor(0, 0, 0, 255);
    style.Colors[ImGuiCol_PopupBg]               = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_Button]                = ImColor(20, 20, 20, 0);
    style.Colors[ImGuiCol_ButtonActive]          = ImColor(20, 20, 20, 0);
    style.Colors[ImGuiCol_ButtonHovered]         = ImColor(20, 20, 20, 0);
    style.Colors[ImGuiCol_TitleBg]               = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_CheckMark]             = ImColor(255, 0, 0, 255);
    style.Colors[ImGuiCol_SliderGrab]            = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImColor(0.349f, 0.082f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImColor(0.710f, 0.169f, 0.169f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImColor(0.710f, 0.169f, 0.169f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImColor(0.710f, 0.169f, 0.169f, 1.0f);
    style.ScaleAllSizes(std::max(2.0f, density / 600.0f));
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
}

size_t write_data_to_file(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

#include <fstream>
#include <string>
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "CLICK_ID", __VA_ARGS__)

std::string GetObjectName(void* obj) {
    if (!obj) return "None";
    UObject* object = (UObject*)obj;
    return object->GetName();
}

int GetItemID(void* obj) {
    if (!obj) return 0;
    return *(int*)((uintptr_t)obj + 0x30);
}

template <typename T>
inline void* toVoidPtr(T func) {
    return reinterpret_cast<void*>(+func); 
}

#define BangJO_SYM_LIB(lib, sym, ptr, org) \
    hook( (void*)dlsym(dlopen(lib, RTLD_LAZY), sym), \
          toVoidPtr(ptr), \
          (void**)&org )

static int frameCounter = 0;

EGLBoolean (*orig_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

EGLBoolean _eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    frameCounter++;
    
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);
    screenWidth = ANativeWindow_getWidth(Data::Cheat::App->window);
    screenHeight = ANativeWindow_getHeight(Data::Cheat::App->window);
    density = AConfiguration_getDensity(Data::Cheat::App->config);

    if (!initImGui && frameCounter > 10) {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
        io.IniFilename = NULL;
        io.LogFilename = NULL;
        
        ImGui_ImplAndroid_Init();
        ImGui_ImplOpenGL3_Init("#version 100");
        
        Theme();
        
        initImGui = true;
    }
    
    if (initImGui && frameCounter > 10) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame(glWidth, glHeight);
        ImGui::NewFrame();
        
        DrawNthMenu();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
    return orig_eglSwapBuffers(dpy, surface);
}

int (*original_AInputQueue_getEvent)(AInputQueue*, AInputEvent**);

int hooked_AInputQueue_getEvent(AInputQueue* queue, AInputEvent** event) {
    int result = original_AInputQueue_getEvent(queue, event);
    if (result >= 0 && *event && initImGui && frameCounter > 10) {
        int32_t type = AInputEvent_getType(*event);
        if (type == AINPUT_EVENT_TYPE_MOTION) {
            int32_t action = AMotionEvent_getAction(*event) & AMOTION_EVENT_ACTION_MASK;
            int32_t pointerIndex = (AMotionEvent_getAction(*event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) 
                                   >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            float rawX = AMotionEvent_getX(*event, pointerIndex);
            float rawY = AMotionEvent_getY(*event, pointerIndex);
            float scaledX = (rawX * glWidth) / screenWidth;
            float scaledY = (rawY * glHeight) / screenHeight;
            ImGuiIO& io = ImGui::GetIO();
            switch (action) {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                    io.MousePos = ImVec2(scaledX, scaledY);
                    io.MouseDown[0] = true;
                    break;
                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_POINTER_UP:
                    io.MousePos = ImVec2(scaledX, scaledY);
                    io.MouseDown[0] = false;
                    break;
                case AMOTION_EVENT_ACTION_MOVE:
                    io.MousePos = ImVec2(scaledX, scaledY);
                    break;
                default:
                    break;
            }
        }
    }
    return result;
}

[[noreturn]] void *maps_thread(void *) {
    while (true) {
        auto objs = UObject::GetGlobalObjects();
        for (int i = 0; i < objs.Num(); i++) {
            auto Object = objs.GetByIndex(i);
            if (isObjectInvalid(Object)) continue;
            if (NoSmoke) {  
                if (Object->IsA(UParticleEmitter::StaticClass())) {
                    auto playerChar = (UParticleEmitter *) Object;  
                    playerChar->EmitterRenderMode = EEmitterRenderMode::ERM_None;
                    playerChar->DetailMode = EDetailMode::DM_MAX;
                }
            }
            if (DarkSky) {
                if (Object->IsA(USkyLightComponent::StaticClass())) {
                    auto playerChar = (USkyLightComponent*) Object;
                    playerChar->SetLightColor(FLinearColor(0.0f,0.0f,0.0f,1.0f));  
                } else if (Object->IsA(USkyLightComponent ::StaticClass())) {
                    auto playerChar = (USkyLightComponent*) Object;
                    playerChar->SetLightColor(FLinearColor(1.0f,1.0f,1.0f,1.0f));
                }
            }
            if (NoFog) {
                if (Object->IsA(UExponentialHeightFogComponent::StaticClass())) {
                    auto playerChar = (UExponentialHeightFogComponent *) Object;
                    playerChar->SetFogMaxOpacity(0.0f);
                    playerChar->SetFogDensity(0.0f);
                    playerChar->SetFogHeightFalloff(0.0f);
                    playerChar->SetFogCutoffDistance(0.0f);
                    playerChar->SetStartDistance(0.0f);
                }
            }
        }
    }
}

#define MTR_OpenHook(RET, NAME, ARGS) \
RET(*o##NAME) ARGS; \
RET h##NAME ARGS


void *anogs_thread(void *) {
    while (!isLibraryLoaded("libc.so"));
    while (!isLibraryLoaded("libUE4.so"));
    while (!isLibraryLoaded("libanogs.so"));
    while (!isLibraryLoaded("libanort.so"));
    while (!isLibraryLoaded("libTBlueData.so"));
    while (!isLibraryLoaded("libhdmpve.so")) {sleep(1);}
    return NULL;
}


//----- lua
void* main_thread(void*) {
    Data::Cheat::UE4 = Tools::GetBaseAddress("libUE4.so");
    while (!Data::Cheat::UE4) {
        Data::Cheat::UE4 = Tools::GetBaseAddress("libUE4.so");
        sleep(1);
    }
    
    while (!Data::Cheat::App) {
        Data::Cheat::App = *(android_app**)(Data::Cheat::UE4 + Data::Offset::GNativeAndroidApp_Offset);
        sleep(1);
    }
    
    FName::GNames = GetGNames();
    while (!FName::GNames) {
        FName::GNames = GetGNames();
        sleep(1);
    }
    
    UObject::GUObjectArray = (FUObjectArray*)(Data::Cheat::UE4 + Data::Offset::GUObject_Offset);
    
    DobbyHook((void *)(Data::Cheat::UE4 + Data::Offset::Process_Event_Offset), (void *)hkProcessEvent, (void **)&oProcessEvent);
PATCH_LIB("libUE4.so","0x7D1EB1C","00 00 80 D2 C0 03 5F D6");//Corona
PATCH_LIB("libUE4.so","0x5CF4304","00 00 80 D2 C0 03 5F D6");
PATCH_LIB("libUE4.so", "0x6AF6D18","1F 20 03 D5");
    HOOKSYM_LIB("/system/lib/libandroid.so", "AInputQueue_getEvent", hooked_AInputQueue_getEvent, original_AInputQueue_getEvent);
    shadowhook_init(oxorany(shadowhook_mode_t::SHADOWHOOK_MODE_UNIQUE), oxorany(0));
    shadowhook_hook_sym_name(oxorany("libEGL.so"), oxorany("eglSwapBuffers"), (void *)_eglSwapBuffers, (void **)&orig_eglSwapBuffers);
    
    pthread_t t;
    pthread_create(&t, 0, maps_thread, 0);
    
    return 0;
}

__attribute__((constructor)) void _init() {
    pthread_t t;
    pthread_create(&t, 0, main_thread, 0);
}