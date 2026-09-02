#include <common.h>

#if defined(CTR_NATIVE)
// Keep the retail overlay data layout unchanged. Native uses these extended
// lists so OPTIONS and EXIT stay at the bottom whether or not Scrapbook is unlocked.
static struct MenuRow s_nativeRowsMainMenuBasic[] = {
	{LNG_ADVENTURE, 0, 1, 0, 0},
	{LNG_TIME_TRIAL, 0, 2, 1, 1},
	{LNG_ARCADE, 1, 3, 2, 2},
	{LNG_VS, 2, 4, 3, 3},
	{LNG_BATTLE, 3, 5, 4, 4},
	{LNG_HIGH_SCORE, 4, 6, 5, 5},
	{LNG_OPTIONS, 5, 7, 6, 6},
	{LNG_OPTIONS_EXIT, 6, 7, 7, 7},
	{RECTMENU_STRING_NONE, 0, 0, 0, 0},
};

static struct MenuRow s_nativeRowsMainMenuWithScrapbook[] = {
	{LNG_ADVENTURE, 0, 1, 0, 0},
	{LNG_TIME_TRIAL, 0, 2, 1, 1},
	{LNG_ARCADE, 1, 3, 2, 2},
	{LNG_VS, 2, 4, 3, 3},
	{LNG_BATTLE, 3, 5, 4, 4},
	{LNG_HIGH_SCORE, 4, 6, 5, 5},
	{LNG_SCRAPBOOK, 5, 7, 6, 6},
	{LNG_OPTIONS, 6, 8, 7, 7},
	{LNG_OPTIONS_EXIT, 7, 8, 8, 8},
	{RECTMENU_STRING_NONE, 0, 0, 0, 0},
};

enum NativeCheatRow
{
	NATIVE_CHEAT_MAX_WUMPA,
	NATIVE_CHEAT_INFINITE_MASKS,
	NATIVE_CHEAT_MAX_TURBOS,
	NATIVE_CHEAT_INVISIBILITY,
	NATIVE_CHEAT_MAX_ENGINE,
	NATIVE_CHEAT_MAX_BOMBS,
	NATIVE_CHEAT_ADV_DIFFICULTY,
	NATIVE_CHEAT_SUPER_HARD,
	NATIVE_CHEAT_ICY_TRACKS,
	NATIVE_CHEAT_SUPER_TURBO_PADS,
	NATIVE_CHEAT_ONE_LAP,
	NATIVE_CHEAT_TURBO_COUNTER,
	NATIVE_CHEAT_UNLOCK_CHARACTERS,
	NATIVE_CHEAT_UNLOCK_STAGES,
	NATIVE_CHEAT_UNLOCK_SCRAPBOOK,
	NATIVE_CHEAT_BACK,
	NATIVE_CHEAT_COUNT,
};

enum NativeVideoRow
{
	NATIVE_VIDEO_RENDER_RESOLUTION,
	NATIVE_VIDEO_FPS_MODE,
	NATIVE_VIDEO_DRAW_DISTANCE,
	NATIVE_VIDEO_BACK,
	NATIVE_VIDEO_ROW_COUNT,
};

struct NativeCheatEntry
{
	char *label;
	u32 gameModeFlag;
};

static struct NativeCheatEntry s_nativeCheatEntries[NATIVE_CHEAT_COUNT] = {
	{"MAX WUMPA FRUIT", CHEAT_WUMPA},
	{"INFINITE MASKS", CHEAT_MASK},
	{"MAX TURBOS", CHEAT_TURBO},
	{"INVISIBILITY", CHEAT_INVISIBLE},
	{"MAX ENGINE", CHEAT_ENGINE},
	{"MAX BOMBS", CHEAT_BOMBS},
	{"ADV DIFFICULTY", CHEAT_ADV},
	{"SUPER HARD", CHEAT_SUPERHARD},
	{"ICY TRACKS", CHEAT_ICY},
	{"SUPER TURBO PADS", CHEAT_TURBOPAD},
	{"ONE LAP RACES", CHEAT_ONELAP},
	{"TURBO COUNTER", CHEAT_TURBOCOUNT},
	{"ALL CHARACTERS", 0},
	{"TRACKS & ARENAS", 0},
	{"UNLOCK SCRAPBOOK", 0},
	{"BACK", 0},
};

static s16 s_nativeCheatSelected;
static s16 s_nativeCheatFirstVisible;
static s16 s_nativeVideoSelected;
static s16 s_nativeModsSelected;

static void MM_NativeOptions_MenuProc(struct RectMenu *menu);
static void MM_NativeCheats_MenuProc(struct RectMenu *menu);
static void MM_NativeVideo_MenuProc(struct RectMenu *menu);
static void MM_NativeMods_MenuProc(struct RectMenu *menu);

static struct MenuRow s_nativeRowsOptions[] = {
	{RECTMENU_STRING_NATIVE_VIDEO, 3, 1, 0, 0},
	{RECTMENU_STRING_NATIVE_CHEATS, 0, 2, 1, 1},
	{RECTMENU_STRING_NATIVE_MODS, 1, 3, 2, 2},
	{RECTMENU_STRING_NATIVE_BACK, 2, 0, 3, 3},
	{RECTMENU_STRING_NONE, 0, 0, 0, 0},
};

static struct RectMenu s_nativeOptionsMenu = {
	.stringIndexTitle = LNG_OPTIONS_TITLE,
	.posX_curr = 0x100,
	.posY_curr = 0xa0,
	.state = RECTMENU_STATE_SMALL_EXEC_CENTERED,
	.rows = s_nativeRowsOptions,
	.funcPtr = MM_NativeOptions_MenuProc,
};

static struct RectMenu s_nativeCheatsMenu = {
	.state = DISABLE_INPUT_ALLOW_FUNCPTRS,
	.funcPtr = MM_NativeCheats_MenuProc,
};

static struct RectMenu s_nativeVideoMenu = {
	.state = DISABLE_INPUT_ALLOW_FUNCPTRS,
	.funcPtr = MM_NativeVideo_MenuProc,
};

static struct RectMenu s_nativeModsMenu = {
	.state = DISABLE_INPUT_ALLOW_FUNCPTRS,
	.funcPtr = MM_NativeMods_MenuProc,
};

static void MM_NativeOptions_MenuProc(struct RectMenu *menu)
{
	if (menu->funcState != RECTMENU_FUNC_STATE_INPUT)
	{
		return;
	}

	if (menu->rowSelected == 0)
	{
		sdata->ptrDesiredMenu = &s_nativeVideoMenu;
	}
	else if (menu->rowSelected == 1)
	{
		sdata->ptrDesiredMenu = &s_nativeCheatsMenu;
	}
	else if (menu->rowSelected == 2)
	{
		sdata->ptrDesiredMenu = &s_nativeModsMenu;
	}
	else
	{
		// Native submenus keep the title scene active in the background, so they
		// must return directly.  Applying the retail return transition here would
		// animate Crash/trophy from an off-screen position it never moved to.
		sdata->ptrDesiredMenu = &D230.menuMainMenu;
		RECTMENU_ClearInput();
	}
}

static void MM_NativeMods_Draw(void)
{
	struct GameTracker *gGT = sdata->gGT;
	int centerX = gGT->pushBuffer_UI.rect.x + (gGT->pushBuffer_UI.rect.w / 2);
	RECT background = {centerX - 130, 65, 260, 90};

	CTR_Box_DrawClearBox(&background, &sdata->menuRowHighlight_Normal, TRANS_50_DECAL, gGT->pushBuffer_UI.ptrOT);
	DecalFont_DrawLine("MODS", centerX, 70, FONT_BIG, JUSTIFY_CENTER | ORANGE);
	for (s16 row = 0; row < 2; row++)
	{
		s16 y = 98 + row * 20;
		if (row == s_nativeModsSelected)
		{
			RECT highlight = {centerX - 124, y - 1, 248, 17};
			CTR_Box_DrawClearBox(&highlight, &sdata->menuRowHighlight_Normal, TRANS_50_DECAL, gGT->pushBuffer_UI.ptrOT);
		}
		DecalFont_DrawLine(row == 0 ? "SKIP ALL INTRO" : "BACK", centerX - 118, y, FONT_SMALL, WHITE);
		if (row == 0)
			DecalFont_DrawLine(Platform_GetSkipAllIntro() ? "ON" : "OFF", centerX + 118, y, FONT_SMALL, JUSTIFY_RIGHT | ORANGE);
	}
}

static void MM_NativeMods_MenuProc(struct RectMenu *menu)
{
	u32 buttons = sdata->buttonTapPerPlayer[0];
	(void)menu;

	if ((buttons & (BTN_UP | BTN_DOWN)) != 0)
	{
		s_nativeModsSelected ^= 1;
		OtherFX_Play(0, 1);
	}
	else if ((buttons & (BTN_CROSS_one | BTN_CIRCLE)) != 0)
	{
		if (s_nativeModsSelected == 0)
		{
			Platform_SetSkipAllIntro(!Platform_GetSkipAllIntro());
			OtherFX_Play(1, 1);
		}
		else
		{
			sdata->ptrDesiredMenu = &s_nativeOptionsMenu;
		}
	}
	else if ((buttons & (BTN_TRIANGLE | BTN_SQUARE_one)) != 0)
	{
		sdata->ptrDesiredMenu = &s_nativeOptionsMenu;
		OtherFX_Play(2, 1);
	}

	MM_NativeMods_Draw();
	RECTMENU_ClearInput();
}

static char *MM_NativeVideo_GetValue(s16 row)
{
	static char *resolutionLabels[] = {"480P", "720P", "1080P", "1440P", "4K"};
	static char *distanceLabels[] = {"1X", "2X", "5X", "10X", "20X", "100X"};
	int scale;

	switch (row)
	{
		case NATIVE_VIDEO_RENDER_RESOLUTION:
			return resolutionLabels[Platform_GetVideoResolutionPreset()];
		case NATIVE_VIDEO_FPS_MODE:
			return CTR_60HzMode_GetTargetFPS() == 30 ? "30 FPS" : (CTR_60HzMode_GetTargetFPS() == 60 ? "60 FPS" : "120 FPS");
		case NATIVE_VIDEO_DRAW_DISTANCE:
			scale = PushBuffer_GetTraversalScale();
			for (s16 i = 0; i < 6; i++)
			{
				if (scale == (int[]){1, 2, 5, 10, 20, 100}[i])
				{
					return distanceLabels[i];
				}
			}
			return "10X";
	}

	return "";
}

static void MM_NativeVideo_Adjust(s16 row, int direction)
{
	static int distanceScales[] = {1, 2, 5, 10, 20, 100};
	int value;

	if (row == NATIVE_VIDEO_RENDER_RESOLUTION)
	{
		value = Platform_GetVideoResolutionPreset() + direction;
		if (value < 0)
			value = 4;
		if (value > 4)
			value = 0;
		Platform_SetVideoResolutionPreset(value);
	}
	else if (row == NATIVE_VIDEO_FPS_MODE)
	{
		value = CTR_60HzMode_GetTargetFPS();
		value = value == 30 ? 60 : (value == 60 ? 120 : 30);
		CTR_60HzMode_SetTargetFPS(value);
		Platform_SetConfiguredFPS(value);
	}
	else if (row == NATIVE_VIDEO_DRAW_DISTANCE)
	{
		value = 0;
		for (s16 i = 0; i < 6; i++)
		{
			if (PushBuffer_GetTraversalScale() == distanceScales[i])
			{
				value = i;
				break;
			}
		}
		value = (value + direction + 6) % 6;
		PushBuffer_SetTraversalScale(distanceScales[value]);
		Platform_SetConfiguredDrawDistance(distanceScales[value]);
	}
}

static void MM_NativeVideo_Draw(void)
{
	static char *labels[NATIVE_VIDEO_ROW_COUNT] = {"RENDER RES", "FPS MODE", "DRAW DISTANCE", "BACK"};
	struct GameTracker *gGT = sdata->gGT;
	int centerX = gGT->pushBuffer_UI.rect.x + (gGT->pushBuffer_UI.rect.w / 2);
	const s16 firstY = 72;
	const s16 rowHeight = data.font_charPixHeight[FONT_SMALL] + 6;
	RECT background = {centerX - 120, 45, 240, 130};

	CTR_Box_DrawClearBox(&background, &sdata->menuRowHighlight_Normal, TRANS_50_DECAL, gGT->pushBuffer_UI.ptrOT);
	DecalFont_DrawLine("VIDEO", centerX, 50, FONT_BIG, JUSTIFY_CENTER | ORANGE);

	for (s16 row = 0; row < NATIVE_VIDEO_ROW_COUNT; row++)
	{
		s16 y = firstY + row * rowHeight;

		if (row == s_nativeVideoSelected)
		{
			RECT highlight = {centerX - 114, y - 1, 228, rowHeight};
			CTR_Box_DrawClearBox(&highlight, &sdata->menuRowHighlight_Normal, TRANS_50_DECAL, gGT->pushBuffer_UI.ptrOT);
		}

		DecalFont_DrawLine(labels[row], centerX - 108, y, FONT_SMALL, WHITE);
		if (row != NATIVE_VIDEO_BACK)
		{
			DecalFont_DrawLine(MM_NativeVideo_GetValue(row), centerX + 108, y, FONT_SMALL, JUSTIFY_RIGHT | ORANGE);
		}
	}

	DecalFont_DrawLine("LEFT/RIGHT: CHANGE", centerX, 155, FONT_SMALL, JUSTIFY_CENTER | WHITE);
}

static void MM_NativeVideo_MenuProc(struct RectMenu *menu)
{
	u32 buttons = sdata->buttonTapPerPlayer[0];
	(void)menu;

	if ((buttons & BTN_UP) != 0)
	{
		s_nativeVideoSelected = (s_nativeVideoSelected + NATIVE_VIDEO_ROW_COUNT - 1) % NATIVE_VIDEO_ROW_COUNT;
		OtherFX_Play(0, 1);
	}
	else if ((buttons & BTN_DOWN) != 0)
	{
		s_nativeVideoSelected = (s_nativeVideoSelected + 1) % NATIVE_VIDEO_ROW_COUNT;
		OtherFX_Play(0, 1);
	}
	else if ((buttons & BTN_LEFT) != 0 || (buttons & BTN_RIGHT) != 0)
	{
		if (s_nativeVideoSelected != NATIVE_VIDEO_BACK)
		{
			MM_NativeVideo_Adjust(s_nativeVideoSelected, (buttons & BTN_LEFT) != 0 ? -1 : 1);
			OtherFX_Play(0, 1);
		}
	}
	else if ((buttons & (BTN_CROSS_one | BTN_CIRCLE)) != 0)
	{
		if (s_nativeVideoSelected == NATIVE_VIDEO_BACK)
		{
			sdata->ptrDesiredMenu = &s_nativeOptionsMenu;
		}
		else
		{
			MM_NativeVideo_Adjust(s_nativeVideoSelected, 1);
			OtherFX_Play(1, 1);
		}
	}
	else if ((buttons & (BTN_TRIANGLE | BTN_SQUARE_one)) != 0)
	{
		sdata->ptrDesiredMenu = &s_nativeOptionsMenu;
		OtherFX_Play(2, 1);
	}

	MM_NativeVideo_Draw();
	RECTMENU_ClearInput();
}

static b32 MM_NativeCheats_IsEnabled(s16 row)
{
	struct GameTracker *gGT = sdata->gGT;
	const struct PlatformCheatConfig *config = Platform_GetCheatConfig();

	if (s_nativeCheatEntries[row].gameModeFlag != 0)
	{
		return (gGT->gameMode2 & s_nativeCheatEntries[row].gameModeFlag) != 0;
	}

	switch (row)
	{
		case NATIVE_CHEAT_UNLOCK_CHARACTERS:
			return config->unlockCharacters;
		case NATIVE_CHEAT_UNLOCK_STAGES:
			return config->unlockStages;
		case NATIVE_CHEAT_UNLOCK_SCRAPBOOK:
			return config->unlockScrapbook;
	}

	return 0;
}

static void MM_NativeCheats_Toggle(s16 row)
{
	struct GameTracker *gGT = sdata->gGT;
	struct PlatformCheatConfig config = *Platform_GetCheatConfig();

	if (s_nativeCheatEntries[row].gameModeFlag != 0)
	{
		gGT->gameMode2 ^= s_nativeCheatEntries[row].gameModeFlag;
		config.gameModeFlags = gGT->gameMode2 & CHEAT_ALL;
		Platform_SetCheatConfig(&config);
		return;
	}

	switch (row)
	{
		case NATIVE_CHEAT_UNLOCK_CHARACTERS:
			if (!config.unlockCharacters)
			{
				config.originalCharacters = sdata->gameProgress.unlockFlags & UNLOCK_CHARACTERS;
				config.originalOxideCharacter = CHECK_ADV_BIT(sdata->gameProgress.unlocks, GAME_UNLOCK_BIT_NITROS_OXIDE);
				sdata->gameProgress.unlockFlags |= UNLOCK_CHARACTERS;
				UNLOCK_MEMCARD_BIT(sdata->gameProgress.unlocks, GAME_UNLOCK_BIT_NITROS_OXIDE);
				config.unlockCharacters = 1;
			}
			else
			{
				sdata->gameProgress.unlockFlags = (sdata->gameProgress.unlockFlags & ~UNLOCK_CHARACTERS) | config.originalCharacters;
				if (!config.originalOxideCharacter)
				{
					sdata->gameProgress.unlocks[MEMCARD_BIT_WORD(GAME_UNLOCK_BIT_NITROS_OXIDE)] &= ~MEMCARD_BIT_MASK(GAME_UNLOCK_BIT_NITROS_OXIDE);
				}
				config.unlockCharacters = 0;
			}
			break;

		case NATIVE_CHEAT_UNLOCK_STAGES:
			if (!config.unlockStages)
			{
				config.originalStages = sdata->gameProgress.unlocks[0] & GAME_UNLOCK_TRACKS_MASK;
				sdata->gameProgress.unlocks[0] |= GAME_UNLOCK_TRACKS_MASK;
				config.unlockStages = 1;
			}
			else
			{
				sdata->gameProgress.unlocks[0] = (sdata->gameProgress.unlocks[0] & ~GAME_UNLOCK_TRACKS_MASK) | config.originalStages;
				config.unlockStages = 0;
			}
			break;

		case NATIVE_CHEAT_UNLOCK_SCRAPBOOK:
			if (!config.unlockScrapbook)
			{
				config.originalScrapbook = CHECK_ADV_BIT(sdata->gameProgress.unlocks, GAME_UNLOCK_BIT_SCRAPBOOK);
				UNLOCK_MEMCARD_BIT(sdata->gameProgress.unlocks, GAME_UNLOCK_BIT_SCRAPBOOK);
				config.unlockScrapbook = 1;
			}
			else
			{
				if (!config.originalScrapbook)
				{
					sdata->gameProgress.unlocks[MEMCARD_BIT_WORD(GAME_UNLOCK_BIT_SCRAPBOOK)] &= ~MEMCARD_BIT_MASK(GAME_UNLOCK_BIT_SCRAPBOOK);
				}
				config.unlockScrapbook = 0;
			}
			break;
	}

	Platform_SetCheatConfig(&config);
}

static void MM_NativeCheats_Draw(void)
{
	struct GameTracker *gGT = sdata->gGT;
	int centerX = gGT->pushBuffer_UI.rect.x + (gGT->pushBuffer_UI.rect.w / 2);
	const s16 firstY = 42;
	const s16 rowHeight = data.font_charPixHeight[FONT_SMALL] + 3;
	const s16 visibleRows = 11;
	RECT background = {centerX - 150, 20, 300, 202};

	CTR_Box_DrawClearBox(&background, &sdata->menuRowHighlight_Normal, TRANS_50_DECAL, gGT->pushBuffer_UI.ptrOT);
	DecalFont_DrawLine("CHEATS", centerX, 25, FONT_BIG, JUSTIFY_CENTER | ORANGE);

	for (s16 displayRow = 0; displayRow < visibleRows; displayRow++)
	{
		s16 row = s_nativeCheatFirstVisible + displayRow;
		s16 y = firstY + (displayRow * rowHeight);

		if (row >= NATIVE_CHEAT_COUNT)
		{
			break;
		}

		if (row == s_nativeCheatSelected)
		{
			RECT highlight = {centerX - 144, y - 1, 288, rowHeight};
			CTR_Box_DrawClearBox(&highlight, &sdata->menuRowHighlight_Normal, TRANS_50_DECAL, gGT->pushBuffer_UI.ptrOT);
		}

		DecalFont_DrawLine(s_nativeCheatEntries[row].label, centerX - 138, y, FONT_SMALL, WHITE);
		if (row != NATIVE_CHEAT_BACK)
		{
			DecalFont_DrawLine(MM_NativeCheats_IsEnabled(row) ? "ON" : "OFF", centerX + 132, y, FONT_SMALL, JUSTIFY_RIGHT | ORANGE);
		}
	}

	DecalFont_DrawLine("X: TOGGLE   TRIANGLE: BACK", centerX, 210, FONT_SMALL, JUSTIFY_CENTER | WHITE);
}

static void MM_NativeCheats_MenuProc(struct RectMenu *menu)
{
	u32 buttons = sdata->buttonTapPerPlayer[0];
	(void)menu;

	if ((buttons & BTN_UP) != 0)
	{
		s_nativeCheatSelected = (s_nativeCheatSelected + NATIVE_CHEAT_COUNT - 1) % NATIVE_CHEAT_COUNT;
		OtherFX_Play(0, 1);
	}
	else if ((buttons & BTN_DOWN) != 0)
	{
		s_nativeCheatSelected = (s_nativeCheatSelected + 1) % NATIVE_CHEAT_COUNT;
		OtherFX_Play(0, 1);
	}
	else if ((buttons & (BTN_CROSS_one | BTN_CIRCLE)) != 0)
	{
		if (s_nativeCheatSelected == NATIVE_CHEAT_BACK)
		{
			sdata->ptrDesiredMenu = &s_nativeOptionsMenu;
		}
		else
		{
			MM_NativeCheats_Toggle(s_nativeCheatSelected);
			OtherFX_Play(1, 1);
		}
	}
	else if ((buttons & (BTN_TRIANGLE | BTN_SQUARE_one)) != 0)
	{
		sdata->ptrDesiredMenu = &s_nativeOptionsMenu;
		OtherFX_Play(2, 1);
	}

	if (s_nativeCheatSelected < s_nativeCheatFirstVisible)
	{
		s_nativeCheatFirstVisible = s_nativeCheatSelected;
	}
	else if (s_nativeCheatSelected >= s_nativeCheatFirstVisible + 11)
	{
		s_nativeCheatFirstVisible = s_nativeCheatSelected - 10;
	}

	MM_NativeCheats_Draw();
	RECTMENU_ClearInput();
}
#endif

// NOTE(aalhendi): ASM-verified against retail 230 0x800abaf0-0x800abcac.
u8 MM_TransitionInOut(struct TransitionMeta *meta, int framesPassed, int numFrames)
{
	u8 allTransitionsDone = 1;
	int transitionIndex = 0;

	// last member of array is null-terminated with 0xFFFF
	for (/**/; meta->headStart > -1; meta++, transitionIndex++)
	{
		s16 start = meta->headStart;
		s16 framesLeft = ((s16)framesPassed - start);

		if ((framesLeft == MM_TRANSITION_SWISH_FRAME) && (transitionIndex == 0))
		{
			// Play "swoosh" sound for menu transition
			OtherFX_Play(MM_TRANSITION_SWISH_SFX, 0);
		}

		if (framesLeft < 1)
		{
			allTransitionsDone = 0;
			meta->currX = 0;
			meta->currY = 0;
			continue;
		}

		// else if
		if (framesLeft < (s16)numFrames)
		{
			allTransitionsDone = 0;
			meta->currX = framesLeft * meta->distX / (s16)numFrames;
			meta->currY = framesLeft * meta->distY / (s16)numFrames;
			continue;
		}

		// else
		meta->currX = meta->distX;
		meta->currY = meta->distY;
	}
	return allTransitionsDone;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800acff4-0x800ad448.
void MM_MenuProc_Main(struct RectMenu *mainMenu)
{
	struct GameTracker *gGT = sdata->gGT;

#if defined(CTR_NATIVE)
	mainMenu->rows = CHECK_ADV_BIT(sdata->gameProgress.unlocks, GAME_UNLOCK_BIT_SCRAPBOOK) ? s_nativeRowsMainMenuWithScrapbook : s_nativeRowsMainMenuBasic;
#else
	// if scrapbook is unlocked, change "rows" to extended array
	if (CHECK_ADV_BIT(sdata->gameProgress.unlocks, GAME_UNLOCK_BIT_SCRAPBOOK))
	{
		mainMenu->rows = &D230.rowsMainMenuWithScrapbook[0];
	}
#endif

	MM_ParseCheatCodes();
	MM_ToggleRows_Difficulty();
	MM_ToggleRows_PlayerCount();

	// If you are at the highest hierarchy level of main menu
	if (mainMenu->funcState == RECTMENU_FUNC_STATE_UPDATE)
	{
		MM_Title_MenuUpdate();

		if (
		    // main menu, "title" exists, and timer >= 230
		    (D230.titleMenuState == TITLE_MENU_STATE_IN_MENU) && (D230.titleObj != NULL) && (TITLE_INTRO_TM_DRAW_MIN_FRAME < D230.titleIntroFrame))
		{
			DecalFont_DrawLineOT(sdata->lngStrings[LNG_TM], MM_TITLE_TM_X, MM_TITLE_TM_Y, FONT_SMALL, ORANGE,
			                     &gGT->backBuffer->otMem.uiOT[MM_TITLE_TM_OT_INDEX]);
		}

		if ((D230.menuMainMenu.state & DRAW_NEXT_MENU_IN_HIERARCHY) == 0)
		{
			gGT->numPlyrNextGame = 1;

			// if no buttons pressed, check demo mode
			if (sdata->gGamepads->anyoneHeldCurr == 0)
			{
				gGT->demoCountdownTimer--;

				// If time runs out
				if (gGT->demoCountdownTimer < 1)
				{
					// Transition out of main menu
					D230.titleMenuState = TITLE_MENU_STATE_EXITING;

					// Go to a cutscene of some kind, either the Oxide intro
					// or a demo-mode race.
					D230.desiredMenuIndex = MM_EXIT_ROUTE_DEMO;
				}
			}

			// if button pressed, reset timer
			else
			{
				gGT->demoCountdownTimer = TITLE_DEMO_IDLE_FRAMES;
			}
		}
	}

	MM_Title_Init();

	// if drawing ptrNextBox_InHierarchy
	if ((mainMenu->state & DRAW_NEXT_MENU_IN_HIERARCHY) != 0)
	{
		D230.titleIntroFrame = TITLE_INTRO_SKIP_FRAME;
	}

	// if funcPtr is null
	if ((mainMenu->state & EXECUTE_FUNCPTR) == 0)
	{
		return;
	}

	struct Title *titleObj = D230.titleObj;

	// if "title" object exists
	if (titleObj != NULL)
	{
		// Character selection moves the title camera while its models are off
		// screen.  Restore the complete title-view state on every main-menu
		// update; resetting only X leaves stale Y/Z or perspective values after
		// returning from a submenu.
		titleObj->cameraPosOffset.x = 0;
		titleObj->cameraPosOffset.y = 0;
		titleObj->cameraPosOffset.z = 0;
		gGT->pushBuffer[0].distanceToScreen_PREV = TITLE_INTRO_DISTANCE_TO_SCREEN;
		gGT->pushBuffer[0].distanceToScreen_CURR = TITLE_INTRO_DISTANCE_TO_SCREEN;
	}

	// if you are at highest level of menu hierarchy
	if (mainMenu->funcState != RECTMENU_FUNC_STATE_INPUT)
	{
		// leave the function
		return;
	}

	// If you are here, then you must not be
	// at the highest level of menu hierarchy

	// if row is negative, do nothing
	if ((mainMenu->rowSelected) < 0)
	{
		return;
	}

	// get LNG index of row selected
	s16 choose = mainMenu->rows[mainMenu->rowSelected].stringIndex;

#if defined(CTR_NATIVE)
	if (choose == LNG_OPTIONS)
	{
		sdata->ptrDesiredMenu = &s_nativeOptionsMenu;
		RECTMENU_ClearInput();
		return;
	}

	if (choose == LNG_OPTIONS_EXIT)
	{
		Platform_RequestQuit();
		return;
	}
#endif

	// clear flags from game mode
	gGT->gameMode1 &= ~(BATTLE_MODE | ADVENTURE_MODE | TIME_TRIAL | ADVENTURE_ARENA | ARCADE_MODE | ADVENTURE_CUP);

	// clear more game mode flags
	gGT->gameMode2 &= ~(CUP_ANY_KIND);

	mainMenu->state |= ONLY_DRAW_TITLE;

	// Default to 3,
	// this intentionally disables the 1-lap cheat
	// in Time Trial and Adventure, DONT change it
	gGT->numLaps = MM_DEFAULT_LAP_COUNT;

	// Adventure Mode
	if (choose == LNG_ADVENTURE)
	{
		// Turn on Adventure Mode, turn off item cheats
		gGT->gameMode1 |= ADVENTURE_MODE;
		gGT->gameMode2 &= ~(CHEAT_WUMPA | CHEAT_MASK | CHEAT_TURBO | CHEAT_ENGINE | CHEAT_BOMBS);

		// menu for new/load
		mainMenu->ptrNextBox_InHierarchy = &D230.menuAdventure;
		mainMenu->state |= DRAW_NEXT_MENU_IN_HIERARCHY;
		return;
	}

	// Time Trial
	if (choose == LNG_TIME_TRIAL)
	{
		// Leave main menu hierarchy
		D230.titleMenuState = TITLE_MENU_STATE_EXITING;

		// Leave through the normal character-select flow.
		D230.desiredMenuIndex = MM_EXIT_ROUTE_CHARACTER_SELECT;

		// set game mode to Time Trial Mode
		gGT->numPlyrNextGame = 1;
		gGT->gameMode1 |= TIME_TRIAL;
		gGT->gameMode2 &= ~(CHEAT_WUMPA | CHEAT_MASK | CHEAT_TURBO | CHEAT_ENGINE | CHEAT_BOMBS);

		return;
	}

	// Arcade Mode
	if (choose == LNG_ARCADE)
	{
		// DONT change, should only work in Arcade, and VS
		if ((gGT->gameMode2 & CHEAT_ONELAP) != 0)
		{
			gGT->numLaps = MM_ONE_LAP_CHEAT_COUNT;
		}

		// set game mode to Arcade Mode
		gGT->gameMode1 |= ARCADE_MODE;

		// set next menu
		mainMenu->ptrNextBox_InHierarchy = &D230.menuRaceType;
		mainMenu->state |= DRAW_NEXT_MENU_IN_HIERARCHY;
		return;
	}

	// Versus
	if (choose == LNG_VS)
	{
		// DONT change, should only work in Arcade, and VS
		if ((gGT->gameMode2 & CHEAT_ONELAP) != 0)
		{
			gGT->numLaps = MM_ONE_LAP_CHEAT_COUNT;
		}

		// next menu is choosing single+cup
		mainMenu->ptrNextBox_InHierarchy = &D230.menuRaceType;
		mainMenu->state |= DRAW_NEXT_MENU_IN_HIERARCHY;
		return;
	}

	// Battle
	if (choose == LNG_BATTLE)
	{
		D230.characterSelectTransitionState = EXITING_MENU;

		// set game mode to Battle Mode
		gGT->gameMode1 |= BATTLE_MODE;

		// set next menu to 2P,3P,4P
		mainMenu->ptrNextBox_InHierarchy = &D230.menuPlayers2P3P4P;
		mainMenu->state |= DRAW_NEXT_MENU_IN_HIERARCHY;
		return;
	}

	// High Score
	if (choose == LNG_HIGH_SCORE)
	{
		// Set next stage to high score menu
		D230.desiredMenuIndex = MM_EXIT_ROUTE_HIGH_SCORE;

		// Leave main menu hierarchy
		D230.titleMenuState = TITLE_MENU_STATE_EXITING;

		return;
	}

	// Scrapbook
	if (choose == LNG_SCRAPBOOK)
	{
		// Set next stage to Scrapbook
		D230.desiredMenuIndex = MM_EXIT_ROUTE_SCRAPBOOK;

		// Leave main menu hierarchy
		D230.titleMenuState = TITLE_MENU_STATE_EXITING;

		return;
	}
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800ad448-0x800ad560.
void MM_ToggleRows_PlayerCount(void)
{
	for (s32 rowIndex = 0; rowIndex < MM_PLAYER_1P2P_SELECTABLE_ROWS; rowIndex++)
	{
		struct MenuRow *row = &D230.rowsPlayers1P2P[rowIndex];

		// unlock row
		row->stringIndex &= MENU_ROW_LNG_MASK;

		if (!MainFrame_HaveAllPads(rowIndex + 1))
		{
			// lock row
			row->stringIndex |= MENU_ROW_LOCKED;
		}
	}

	for (s32 rowIndex = 0; rowIndex < MM_PLAYER_2P3P4P_SELECTABLE_ROWS; rowIndex++)
	{
		struct MenuRow *row = &D230.rowsPlayers2P3P4P[rowIndex];

		// unlock row
		row->stringIndex &= MENU_ROW_LNG_MASK;

		if (!MainFrame_HaveAllPads(rowIndex + 2))
		{
			// lock row
			row->stringIndex |= MENU_ROW_LOCKED;
		}
	}
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800ad560-0x800ad5e8.
void MM_MenuProc_1p2p(struct RectMenu *menu)
{
	struct GameTracker *gGT = sdata->gGT;
	s16 row = menu->rowSelected;

	// if uninitialized
	if (row == -1)
	{
		menu->ptrPrevBox_InHierarchy->state &= ~(ONLY_DRAW_TITLE | DRAW_NEXT_MENU_IN_HIERARCHY);

		gGT->numPlyrNextGame = 1;

		D230.characterSelectTransitionState = ENTERING_MENU;
	}

	else
	{
		// if on row 0 or 1
		if ((row >= 0) && (row < MM_PLAYER_1P2P_SELECTABLE_ROWS))
		{
			// row 0 is 1P, row 1 is 2P
			gGT->numPlyrNextGame = menu->rowSelected + 1;

			// go to difficulty box
			menu->ptrNextBox_InHierarchy = &D230.menuDifficulty;

			menu->state |= ONLY_DRAW_TITLE | DRAW_NEXT_MENU_IN_HIERARCHY;
			return;
		}
	}
	return;
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800ad5e8-0x800ad678.
void MM_MenuProc_2p3p4p(struct RectMenu *menu)
{
	struct GameTracker *gGT = sdata->gGT;
	s16 row = menu->rowSelected;

	// if uninitialized
	if (row == -1)
	{
		menu->ptrPrevBox_InHierarchy->state &= ~(ONLY_DRAW_TITLE | DRAW_NEXT_MENU_IN_HIERARCHY);

		gGT->numPlyrNextGame = 1;

		D230.characterSelectTransitionState = ENTERING_MENU;
	}
	else
	{
		// row is 0, 1, 2
		if ((row >= 0) && (row < MM_PLAYER_2P3P4P_SELECTABLE_ROWS))
		{
			// row 0 is 2P, row 1 is 3P, row 2 is 4P
			gGT->numPlyrNextGame = menu->rowSelected + 2;

			D230.titleMenuState = TITLE_MENU_STATE_EXITING;
			D230.desiredMenuIndex = MM_EXIT_ROUTE_CHARACTER_SELECT;

			menu->state |= ONLY_DRAW_TITLE;
			return;
		}
	}
	return;
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800ad678-0x800ad7a4.
void MM_ToggleRows_Difficulty(void)
{
	struct GameTracker *gGT = sdata->gGT;

	// check 3 mods (easy, medium, hard)
	for (s32 difficultyIndex = 0; difficultyIndex < MM_DIFFICULTY_COUNT; difficultyIndex++)
	{
		s16 bitIndex = D230.cupDifficulty.firstUnlockBit[difficultyIndex];

		// if -1 (for EASY row), skip
		if (-1 == bitIndex)
		{
			continue;
		}

		// assume unlocked
		u32 isUnlocked = 1;

		// check 4 bits starting at bitIndex,
		// one for each track in cup
		for (s32 trackIndex = 0; trackIndex < MM_CUP_TRACK_COUNT; trackIndex++)
		{
			b32 shouldCheckNextTrack = (isUnlocked != 0);
			isUnlocked = 0;

			// if not determined locked
			if (shouldCheckNextTrack)
			{
				s32 unlockBit = (s32)bitIndex + trackIndex;

				// check what is unlocked
				isUnlocked = CHECK_ADV_BIT(sdata->gameProgress.unlocks, unlockBit);
			}
		}

		// get current value of lng index,
		// for easy, medium, hard
		u16 lngIndex = D230.cupDifficulty.stringIndex[difficultyIndex];

		if (
		    // if locked
		    (isUnlocked == 0) &&

		    // If you're in Arcade mode
		    ((gGT->gameMode1 & ARCADE_MODE) != 0) &&

		    // if you are in Arcade or VS cup
		    ((gGT->gameMode2 & CUP_ANY_KIND) != 0))
		{
			// use high bits for "LOCKED"
			lngIndex |= MENU_ROW_LOCKED;
		}

		// save new value
		D230.rowsDifficulty[difficultyIndex].stringIndex = lngIndex;
	}
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800ad7a4-0x800ad828.
void MM_MenuProc_Difficulty(struct RectMenu *menu)
{
	s16 row = menu->rowSelected;

	// if uninitialized
	if (row == -1)
	{
		menu->ptrPrevBox_InHierarchy->state &= ~(ONLY_DRAW_TITLE | DRAW_NEXT_MENU_IN_HIERARCHY);
	}

	else
	{
		// if you are on a valid row
		if ((row >= 0) && (row < MM_DIFFICULTY_COUNT))
		{
			// set difficulty to value, from array of fixed difficulty values
			sdata->gGT->arcadeDifficulty = D230.cupDifficulty.speed[row];

			D230.titleMenuState = TITLE_MENU_STATE_EXITING;
			D230.desiredMenuIndex = MM_EXIT_ROUTE_CHARACTER_SELECT;

			menu->state |= ONLY_DRAW_TITLE;
			return;
		}
	}
	return;
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800ad828-0x800ad8f0.
void MM_MenuProc_SingleCup(struct RectMenu *menu)
{
	struct GameTracker *gGT = sdata->gGT;
	s16 row = menu->rowSelected;

	if (row == -1)
	{
		menu->ptrPrevBox_InHierarchy->state &= ~(ONLY_DRAW_TITLE | DRAW_NEXT_MENU_IN_HIERARCHY);
		return;
	}

	if ((row >= 0) && (row < MM_RACE_TYPE_SELECTABLE_ROWS))
	{
		// disable Cup mode
		gGT->gameMode2 &= ~(CUP_ANY_KIND);

		// if you choose cup mode
		if (menu->rowSelected != 0)
		{
			// enable cup mode
			gGT->gameMode2 |= CUP_ANY_KIND;
		}

		menu->state |= ONLY_DRAW_TITLE | DRAW_NEXT_MENU_IN_HIERARCHY;

		// if mode is Arcade
		if ((gGT->gameMode1 & ARCADE_MODE) != 0)
		{
			// set next menu to 1P+2P select
			menu->ptrNextBox_InHierarchy = &D230.menuPlayers1P2P;
			D230.characterSelectTransitionState = IN_MENU;
			return;
		}

		// if mode is VS

		// set next menu to 2P+3P+4P (vs or battle)
		menu->ptrNextBox_InHierarchy = &D230.menuPlayers2P3P4P;
		D230.characterSelectTransitionState = EXITING_MENU;
	}
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800ad8f0-0x800ad980.
void MM_MenuProc_NewLoad(struct RectMenu *menu)
{
	// row number
	s16 row = menu->rowSelected;

	if (row == -1)
	{
		menu->ptrPrevBox_InHierarchy->state &= ~(ONLY_DRAW_TITLE | DRAW_NEXT_MENU_IN_HIERARCHY);
		return;
	}

	if ((row < 0) || (row >= MM_ADV_NEW_LOAD_ROUTE_COUNT))
	{
		return;
	}

	// if Load was chosen
	D230.desiredMenuIndex = row;

	// MM_Title transitioning out
	D230.titleMenuState = TITLE_MENU_STATE_EXITING;

	menu->state |= ONLY_DRAW_TITLE;
	return;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800ad980-0x800ad98c.
struct RectMenu *MM_AdvNewLoad_GetMenuPtr(void)
{
	// menu for new/load
	return &D230.menuAdventure;
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800b42b0-0x800b4334.
void MM_ResetAllMenus(void)
{
	for (s32 menuIndex = 0; menuIndex < MM_MENU_RESET_COUNT; menuIndex++)
	{
		struct RectMenu *menu = D230.arrayMenuPtrs[menuIndex];

// NOTE(aalhendi): Retail resets one menu per array slot; native walks chained
// menus because overlay 230 data is not reloaded.
#ifdef CTR_NATIVE
		do
		{
			struct RectMenu *next = menu->ptrNextBox_InHierarchy;
#endif

			// Close menu
			menu->state |= RECTMENU_CLOSE_TRANSIENT;
			menu->state &= ~(ONLY_DRAW_TITLE | DRAW_NEXT_MENU_IN_HIERARCHY);

			// Reset ptrNext and ptrPrev
			menu->ptrNextBox_InHierarchy = 0;
			menu->ptrPrevBox_InHierarchy = 0;

#ifdef CTR_NATIVE
			menu = next;
		} while (menu != 0);
#endif
	}

	// unused
	sdata->framesRemainingInMenu = MM_MENU_RESET_DONE_FRAMES;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b4334-0x800b4364.
void MM_JumpTo_Title_Returning(void)
{
	// return to main menu from another menu
	D230.titleMenuState = TITLE_MENU_STATE_RETURNING;

	// return to main menu
	sdata->ptrDesiredMenu = &D230.menuMainMenu;

	D230.titleMenuTransitionFrame = D230.titleMenuTransitionDurationFrames;
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800b4364-0x800b43f4.
void MM_JumpTo_Title_FirstTime(void)
{
	struct GameTracker *gGT = sdata->gGT;

	MM_ResetAllMenus();

	MainStats_ClearBattleVS();

#if BUILD == EurRetail
	// if you have not chose a language or skipped the language menu
	if (sdata->boolLangChosen == 0)
	{
		sdata->ptrActiveMenu = &D230.menuLngBoot;
		D230.langMenuTimer = MM_LANGUAGE_MENU_TIMEOUT_FRAMES;
	}
	else
	{
		// if not set to normal main menu
		sdata->ptrActiveMenu = &D230.menuMainMenu;
	}
#else
	// open Main Menu for the first time
	sdata->ptrActiveMenu = &D230.menuMainMenu;
#endif

	D230.titleIntroFrame = 0;

	// first time in main menu
	// (play crash trophy anim)
	D230.titleMenuState = TITLE_MENU_STATE_INTRO;

	// reset countdown clock for battle or crystal challenge
	gGT->originalEventTime = TITLE_INITIAL_EVENT_TIME;

	D230.menuMainMenu.state &= ~(EXECUTE_FUNCPTR | ONLY_DRAW_TITLE);
	D230.menuMainMenu.state |= DISABLE_INPUT_ALLOW_FUNCPTRS;

	// distance to screen (perspective)
	gGT->pushBuffer[0].distanceToScreen_PREV = TITLE_DEFAULT_DISTANCE_TO_SCREEN;
	gGT->pushBuffer[0].distanceToScreen_CURR = TITLE_DEFAULT_DISTANCE_TO_SCREEN;
	gGT->gameMode1 &= ~(TIME_TRIAL);
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b43f4-0x800b4430.
void MM_JumpTo_BattleSetup(void)
{
	// Go to battle setup
	sdata->ptrActiveMenu = &D230.menuBattleWeapons;

	D230.menuBattleWeapons.state &= ~(ONLY_DRAW_TITLE);

	MM_Battle_Init();
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b4430-0x800b446c.
void MM_JumpTo_TrackSelect(void)
{
	// return to track selection
	sdata->ptrActiveMenu = &D230.menuTrackSelect;

	D230.menuTrackSelect.state &= ~(ONLY_DRAW_TITLE);

	MM_TrackSelect_Init();
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b446c-0x800b44a8.
void MM_JumpTo_Characters(void)
{
	// return to character selection
	sdata->ptrActiveMenu = &D230.menuCharacterSelect;

	D230.menuCharacterSelect.state &= ~(ONLY_DRAW_TITLE);

	MM_Characters_RestoreIDs();
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800b44a8-0x800b44e4.
void MM_JumpTo_Scrapbook(void)
{
	// go to scrapbook
	sdata->ptrActiveMenu = &D230.menuScrapbook;

	D230.menuScrapbook.state &= ~(ONLY_DRAW_TITLE);

	MM_Scrapbook_Init();
}
