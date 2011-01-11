
#include "3dc.h"
#include "module.h"
#include "inline.h"
#include "stratdef.h"
#include "gamedef.h"
#include "gameplat.h"
#include "bh_types.h"
#include "usr_io.h"
#include "font.h"
#include <mmsystem.h>
#include "renderer.h"
#include "comp_shp.h"
#include "chnkload.hpp"
#include "npcsetup.h" /* JH 30/4/97 */
#include "pldnet.h"
#include "avpview.h"
#include "vision.h"
#include "avp_menus.h"
//#define UseLocalAssert TRUE
#include "ourasert.h" 
#include "ffstdio.h" // fast file stdio
#include "davehook.h"
#include "showcmds.h"
#include "consbind.hpp"
#include "AvpReg.hpp"
#include "mempool.h"
#include "GammaControl.h"
#include "avp_intro.h"
#include "CDTrackSelection.h"
#include "CD_Player.h"
#include "psndplat.h"
#include "AvP_UserProfile.h"
#include "avp_menus.h"
#include "configFile.h"
#include "vorbisPlayer.h"
#include "networking.h"
#include "avpview.h"
#include "renderer.h"

#if debug
#define MainTextPrint 1
extern int alloc_cnt, deall_cnt;
extern int ItemCount;
int DebugFontLoaded = 0;
#else
#define MainTextPrint 0
#endif

extern int PrintDebuggingText(const char* t, ...);
extern int WindowRequestMode;
extern int FrameRate;
extern BOOL ForceLoad_Alien;
extern BOOL ForceLoad_Marine;
extern BOOL ForceLoad_Predator;
extern BOOL ForceLoad_Hugger;
extern BOOL ForceLoad_Queen;
extern BOOL ForceLoad_Civvie;
extern BOOL ForceLoad_PredAlien;
extern BOOL ForceLoad_Xenoborg;
extern BOOL ForceLoad_Pretorian;
extern BOOL ForceLoad_SentryGun;

BOOL UseMouseCentreing = FALSE;
BOOL KeepMainRifFile = FALSE;

char LevelName[] = {"predbit6\0QuiteALongNameActually"};

int VideoModeNotAvailable = 0;
int QuickStartMultiplayer = 1;

extern HWND hWndMain;
extern int WindowMode;
extern int DebuggingCommandsActive;
extern void dx_log_close();
extern void TimeStampedMessage(char *stringPtr);
extern void ThisFramesRenderingHasBegun(void);
extern void ThisFramesRenderingHasFinished(void);
extern void ScanImagesForFMVs();
extern void RestartLevel();
extern void ResetEaxEnvironment(void);
extern void ReleaseAllFMVTextures();
extern void MinimalNetCollectMessages(void);
extern void InitCentreMouseThread();
extern void IngameKeyboardInput_ClearBuffer(void);
extern void Game_Has_Loaded();
extern void FinishCentreMouseThread();
extern void DoCompletedLevelStatisticsScreen(void);
extern void DeInitialisePlayer();
extern void BuildMultiplayerLevelNameArray();
extern void EmptyUserProfilesList(void);
extern char CommandLineIPAddressString[];
extern int AvP_MainMenus(void);
extern int AvP_InGameMenus(void);
extern int InGameMenusAreRunning(void);
extern void InitFmvCutscenes();

extern struct DEBUGGINGTEXTOPTIONS ShowDebuggingText;

extern bool bRunning;

bool unlimitedSaves = false;

void exit_break_point_fucntion()
{
	#if debug
	if (WindowMode == WindowModeSubWindow)
	{
		__debugbreak();
	}
	#endif
}

extern void LoadKeyConfiguration();

// so we can disable/enable stickey keys
STICKYKEYS startupStickyKeys = {sizeof(STICKYKEYS), 0};
STICKYKEYS skOff;
 
// entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	I_AVP_ENVIRONMENTS level_to_load = I_Num_Environments;
	char *command_line = lpCmdLine;
	char *instr = 0;

	skOff = startupStickyKeys;

	if ((skOff.dwFlags & SKF_STICKYKEYSON) == 0)
	{
		// Disable the hotkey and the confirmation
		skOff.dwFlags &= ~SKF_HOTKEYACTIVE;
		skOff.dwFlags &= ~SKF_CONFIRMHOTKEY;

		SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &skOff, 0);
	}

	InitFmvCutscenes();

	// load AliensVsPredator.cfg
	Config_Load();

	LoadCDTrackList(); //load list of cd tracks assigned to levels , from a text file
	LoadVorbisTrackList(); // do the same for any user ogg vorbis music files

	SetFastRandom();

	//see if any extra npc rif files should be loaded
	char* strpos = strstr(command_line, "-l");
	if (strpos)
	{
		while (strpos)
		{
			strpos += 2;
			if (*strpos >= 'a' && *strpos <= 'z')
			{
				while (*strpos >= 'a' && *strpos <= 'z')
				{
					switch (*strpos)
					{
						case 'a':
							ForceLoad_Alien = TRUE;
							break;
						case 'm':
							ForceLoad_Marine = TRUE;
							break;
						case 'p':
							ForceLoad_Predator = TRUE;
							break;
						case 'h':
							ForceLoad_Hugger = TRUE;
							break;
						case 'q':
							ForceLoad_Queen = TRUE;
							break;
						case 'c':
							ForceLoad_Civvie = TRUE;
							break;
						case 'x':
							ForceLoad_Xenoborg = TRUE;
							break;
						case 't':
							ForceLoad_Pretorian = TRUE;
							break;
						case 'r':
							ForceLoad_PredAlien = TRUE;
							break;
						case 's':
							ForceLoad_SentryGun = TRUE;
							break;
					}
					strpos++;
				}
			}
			else
			{
				ForceLoad_Alien = TRUE;
			}
			strpos = strstr(strpos, "-l");
		}
	}

//	#ifdef AVP_DEBUG_VERSION
	if (strstr(command_line, "-intro"))	
		WeWantAnIntro();

	if (strstr(command_line, "-qm"))
	{
		QuickStartMultiplayer = 1;
	}
	else if (strstr(command_line, "-qa"))
	{
		QuickStartMultiplayer = 2;
	}
	else if (strstr(command_line, "-qp"))
	{
		QuickStartMultiplayer = 3;
	}
	else
	{
		QuickStartMultiplayer = 0;
	}

	if (strstr(command_line, "-keeprif"))
	{
		KeepMainRifFile = TRUE;			
	}

	if (strstr(command_line, "-m"))
	{
		UseMouseCentreing = TRUE;
	}

	UseMouseCentreing = TRUE;

	// windowed mode?
	if (strstr(command_line, "-w"))
	{
		WindowRequestMode = WindowModeSubWindow;

		// will stop mouse cursor moving outside game window
		//UseMouseCentreing = TRUE;
	}

	if (strstr(command_line, "-dontgrabmouse"))
	{
		// if this was previously set due to -m or -w, disable it
		UseMouseCentreing = FALSE;
	}

	if (UseMouseCentreing)
	{
		InitCentreMouseThread();
	}

//	#endif //AVP_DEBUG_VERSION

	if (strstr(command_line, "-server"))
	{
		//game has been launched by mplayer , we best humour it
		LobbiedGame = LobbiedGame_Server;
		if (!Net_InitLobbiedGame())
		{
			exit(0x6364);
		}
	}
	else if (strstr(command_line, "-client"))
	{
		//ditto
		LobbiedGame = LobbiedGame_Client;
		if (!Net_InitLobbiedGame())
		{
			exit(0x6364);
		}
	}
	else if (strstr(command_line, "-debug"))
	{
		DebuggingCommandsActive = 1;
	}

	if (instr = strstr(command_line, "-ip"))
	{
		char buffer[100];
		sscanf(instr, "-ip %s", &buffer);
		strncpy(CommandLineIPAddressString,buffer,15);
		CommandLineIPAddressString[15] = 0;
	}
 
 	#if PLAY_INTRO//(MARINE_DEMO||ALIEN_DEMO||PREDATOR_DEMO)
  	if (!LobbiedGame)  // Edmond
	 	WeWantAnIntro();
	#endif
	GetPathFromRegistry();

	/* JH 28/5/97 */
	/* Initialise 'fast' file system */
	#if MARINE_DEMO
	ffInit("fastfile\\mffinfo.txt","fastfile\\");
	#elif ALIEN_DEMO
	ffInit("alienfastfile\\ffinfo.txt","alienfastfile\\");
	#else
	ffInit("fastfile\\ffinfo.txt","fastfile\\");
	#endif

	InitGame();

	/****** Put in by John to sort out easy sub window mode ******/
	/****** REMOVE FOR GAME!!!!! ******/

	#if debug && 1//!PREDATOR_DEMO

	if (instr = strstr(command_line, "-s"))
		sscanf(instr, "-s%d", &level_to_load);

	#endif

	Env_List[0]->main = LevelName;

	// as per linux port
	AvP.CurrentEnv = AvP.StartingEnv = I_Gen1;

	/******* System initialisation **********/

	timeBeginPeriod(1);

	InitialiseSystem(hInstance, nCmdShow);
	InitialiseRenderer();

	if (!InitialiseDirect3D())
	{
		MessageBox(hWndMain, "Couldn't create a Direct3D device. See avp_log.txt for details", "Couldn't create render device!", MB_OK | MB_ICONSTOP);
		ReleaseDirect3D();
		exit(-1);
	}

	LoadKeyConfiguration();

	/*-------------------Patrick 2/6/97-----------------------
	Start the sound system
	----------------------------------------------------------*/
	SoundSys_Start();
	CDDA_Start();

	// get rid of the mouse cursor
	SetCursor(NULL);

	// load language file and setup text string access
	InitTextStrings();

	BuildMultiplayerLevelNameArray(); //sort out multiplayer level names

	AvP.LevelCompleted = 0;
	LoadSounds("PLAYER"); 

	#if PREDATOR_DEMO||MARINE_DEMO||ALIEN_DEMO
	if(AvP_MainMenus())
	#else

	// support removing limit on number of game saves
	unlimitedSaves = Config_GetBool("[Misc]", "UnlimitedSaves", false);

	while (AvP_MainMenus() && bRunning)
	#endif
	{
		// start of level load
		BOOL menusActive = FALSE;
		int thisLevelHasBeenCompleted = 0;

		mainMenu = FALSE;

		#if !(PREDATOR_DEMO||MARINE_DEMO||ALIEN_DEMO)
		if (instr = strstr(command_line, "-n"))
		{
			sscanf(instr, "-n %s", &LevelName);
		}
		#endif

		// turn off any special effects
		d3d_light_ctrl.ctrl = LCCM_NORMAL;

		/* Check Gamma Settings are correct after video mode change */
//bjd		InitialiseGammaSettings(RequestedGammaSetting);

		// Load precompiled shapes 
	    start_of_loaded_shapes = load_precompiled_shapes();

		/***********  Load up the character stuff *******/
		InitCharacter();

		LoadRifFile(); /* sets up a map*/
		#if debug
		DebugFontLoaded = 1;
		#endif

		/*********** Process the data ************/
		AssignAllSBNames();
		StartGame();

		/* JH 28/5/97 */
		/* remove resident loaded 'fast' files */
		ffcloseall();
		/*********** Play the game ***************/

		/* KJL 15:43:25 03/11/97 - run until this boolean is set to 0 */
		AvP.MainLoopRunning = 1;

		ScanImagesForFMVs();

		ResetFrameCounter();
		Game_Has_Loaded();
		ResetFrameCounter();

		if (AvP.Network != I_No_Network)
		{
			/*Need to choose a starting position for the player , but first we must look
			through the network messages to find out which generator spots are currently clear*/
			netGameData.myGameState = NGS_Playing;
			MinimalNetCollectMessages();
			TeleportNetPlayerToAStartingPosition(Player->ObStrategyBlock,1);
		}

		IngameKeyboardInput_ClearBuffer();

		while (AvP.MainLoopRunning && bRunning) 
		{
			CheckForWindowsMessages();
			CursorHome();

			#if debug
			if (memoryInitialisationFailure)
			{
				OutputDebugString("Initialisation not completed - out of memory!\n");
				textprint("Initialisation not completed - out of memory!\n");
				GLOBALASSERT(1 == 0);
			}
			#endif

			switch (AvP.GameMode)
			{
				case I_GM_Playing:
				{
					if ((!menusActive || (AvP.Network!=I_No_Network && !netGameData.skirmishMode)) && !AvP.LevelCompleted)
					{
						//#if MainTextPrint 		/* debugging stuff */
						{
							if (ShowDebuggingText.FPS) ReleasePrintDebuggingText("FrameRate = %d fps\n",FrameRate);
							if (ShowDebuggingText.Environment) ReleasePrintDebuggingText("Environment %s\n", Env_List[AvP.CurrentEnv]->main);
							if (ShowDebuggingText.Coords) ReleasePrintDebuggingText("Player World Coords: %d,%d,%d\n",Player->ObWorld.vx,Player->ObWorld.vy,Player->ObWorld.vz);
							{
								PLAYER_STATUS *playerStatusPtr = (PLAYER_STATUS *)(Player->ObStrategyBlock->SBdataptr);
								PLAYER_WEAPON_DATA *weaponPtr = &(playerStatusPtr->WeaponSlot[playerStatusPtr->SelectedWeaponSlot]);
								TEMPLATE_WEAPON_DATA *twPtr = &TemplateWeapon[weaponPtr->WeaponIDNumber];
								if (ShowDebuggingText.GunPos)
								{
									PrintDebuggingText("Gun Position x:%d,y:%d,z:%d\n",twPtr->RestPosition.vx,twPtr->RestPosition.vy,twPtr->RestPosition.vz);
								}
							}
						}
						//#endif  /* MainTextPrint */

						ThisFramesRenderingHasBegun();
						
						DoAllShapeAnimations();

						UpdateGame();

						AvpShowViews();

						//Do screen shot here so that text and  hud graphics aren't shown
						MaintainHUD();

						//#if debug
						FlushTextprintBuffer();
						//#endif

						//check cd status
						CheckCDAndChooseTrackIfNeeded();
		
						// check to see if we're pausing the game;
						// if so kill off any sound effects
						if (InGameMenusAreRunning() && ((AvP.Network!=I_No_Network && netGameData.skirmishMode) || (AvP.Network==I_No_Network)))
							SoundSys_StopAll();
					}
					else
					{
						ReadUserInput();
						SoundSys_Management();

						ThisFramesRenderingHasBegun();
					}

					{
						menusActive = AvP_InGameMenus();
						if (AvP.RestartLevel) menusActive = FALSE;
					}
					if (AvP.LevelCompleted)
					{
						SoundSys_FadeOutFast();
						DoCompletedLevelStatisticsScreen();
						thisLevelHasBeenCompleted = 1;
					}

					{
						/* after this call, no more graphics can be drawn until the next frame */
						//extern void ThisFramesRend eringHasFinished(void);
						ThisFramesRenderingHasFinished();
					}

					FlipBuffers();

					FrameCounterHandler();
					{
						PLAYER_STATUS *playerStatusPtr= (PLAYER_STATUS *) (Player->ObStrategyBlock->SBdataptr);

						if (!menusActive && playerStatusPtr->IsAlive && !AvP.LevelCompleted)
						{
							DealWithElapsedTime();
						}
					}
					break;
				}
				case I_GM_Menus:
				{
					AvP.GameMode = I_GM_Playing;
					//StartGameMenus();
					LOCALASSERT(AvP.Network == I_No_Network);
					//AccessDatabase(0);
					break;
				}

				default:
				{
					GLOBALASSERT(2<1);
					break;
				}
			}

			if (AvP.RestartLevel)
			{
				AvP.RestartLevel = 0;
				AvP.LevelCompleted = 0;
				FixCheatModesInUserProfile(UserProfilePtr);
				RestartLevel();
			}
		}// end of main game loop

		AvP.LevelCompleted = thisLevelHasBeenCompleted;
		mainMenu = TRUE;

		FixCheatModesInUserProfile(UserProfilePtr);

		#if !(PREDATOR_DEMO||MARINE_DEMO||ALIEN_DEMO)
		TimeStampedMessage("We're out of the main loop");

		ReleaseAllFMVTextures();

		/* DHM 8/4/98 */
		CONSBIND_WriteKeyBindingsToConfigFile();

		/* CDF 2/10/97 */
		DeInitialisePlayer();

		DeallocatePlayersMirrorImage();

		// bjd - here temporarily as a test
		SoundSys_StopAll();

		Destroy_CurrentEnvironment();

		DeallocateAllImages();

		EndNPCs(); /* JH 30/4/97 - unload npc rifs */
		ExitGame();

		#endif
		/* Patrick 26/6/97
		Stop and remove all game sounds here, since we are returning to the menus */
//		SoundSys_StopAll();
		ResetEaxEnvironment();
		//make sure the volume gets reset for the menus
		SoundSys_ResetFadeLevel();

		CDDA_Stop();
		Vorbis_CloseSystem(); // stop ogg vorbis player

		// netgame support
		if (AvP.Network != I_No_Network)
		{
			/* we cleanup and reset our game mode here, at the end of the game loop, as other 
			clean-up functions need to know if we've just exited a netgame */
			EndAVPNetGame();
			//EndOfNetworkGameScreen();
		}

		ClearMemoryPool();

#if 0 //bjd - FIXME
		if(LobbiedGame)
		{
			/*
			We have been playing a lobbied game , and have now diconnected.
			Since we can't start a new multiplayer game , exit to avoid confusion
			*/
			break;
		}
#endif
	}
	#if !(PREDATOR_DEMO||MARINE_DEMO||ALIEN_DEMO)
	TimeStampedMessage("After Menus");

	/* Added 28/1/98 by DHM: hook for my code on program shutdown */
	DAVEHOOK_UnInit();

	/*-------------------Patrick 2/6/97-----------------------
	End the sound system
	----------------------------------------------------------*/

	SoundSys_StopAll();
	SoundSys_RemoveAll(); 

	/* bjd - delete some profile data that was showing up as memory leaks */
	EmptyUserProfilesList();

	#else
//	QuickSplashScreens();
	#endif
	#if !(PREDATOR_DEMO||MARINE_DEMO||ALIEN_DEMO)

	ExitSystem();

	#else
	SoundSys_End();
	ReleaseDirect3D();
	//TimeStampedMessage("after ReleaseDirect3D");

	/* Kill windows procedures */
//	ExitWindowsSystem();
	//TimeStampedMessage("after ExitWindowsSystem");

	#endif

	Config_Save();

	// close dx logfile if open (has to be called after all calls to TimeStampedMessage()
#if debug
	dx_log_close();
#endif

	CDDA_End();
	ClearMemoryPool();

	// restore stickey keys setting
	SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &startupStickyKeys, 0);

	// 'shutdown' timer
	timeEndPeriod(1);

	if (UseMouseCentreing)
	{
		FinishCentreMouseThread();
	}

	return 0;
}
