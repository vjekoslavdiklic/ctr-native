#ifndef PLATFORM_H
#define PLATFORM_H

struct PlatformMempackArena
{
	void *base;
	void *start;
	void *endOfMemory;
	int size;
	int backingSize;
};

// Native-only settings stored beside the executable.  Unlock originals are
// retained so turning an unlock cheat back off restores real game progress.
struct PlatformCheatConfig
{
	unsigned int gameModeFlags;
	unsigned int originalCharacters;
	unsigned int originalStages;
	int originalScrapbook;
	int originalOxideCharacter;
	int unlockCharacters;
	int unlockStages;
	int unlockScrapbook;
};

void Platform_Init(const char *title, int width, int height);
void Platform_Shutdown(void);
void Platform_InitScratchpad(void);
const struct PlatformMempackArena *Platform_InitMempackArena(void);
const struct PlatformMempackArena *Platform_GetMempackArena(void);
void Platform_BeginFrame(void);
int Platform_BeginScene(void);
void Platform_EndScene(void);
void Platform_EndFrame(void);
void Platform_PresentVRAMDisplay(void);
void Platform_PinVRAMDisplayFrames(int frameCount);
void Platform_PinVRAMDisplayRect(int x, int y, int w, int h, int frameCount);
int Platform_GetVBlankCount(void);
void Platform_WaitUntilVBlank(int targetVBlank);
void Platform_PollHostEvents(void);
int Platform_PollInput(void);

#if defined(CTR_NATIVE)
void Platform_RequestQuit(void);
int NikoGetEnterKey(void);
int Platform_GetVideoResolutionPreset(void);
void Platform_SetVideoResolutionPreset(int preset);
int Platform_GetConfiguredFPS(void);
void Platform_SetConfiguredFPS(int fps);
int Platform_GetConfiguredDrawDistance(void);
void Platform_SetConfiguredDrawDistance(int scale);
int Platform_GetSkipAllIntro(void);
void Platform_SetSkipAllIntro(int enabled);
const struct PlatformCheatConfig *Platform_GetCheatConfig(void);
void Platform_SetCheatConfig(const struct PlatformCheatConfig *config);
#endif

#endif
