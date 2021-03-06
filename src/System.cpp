#include "stdafx.h"
#include "System.h"
#include "GraphicsDevice.h"
#include "RenderingManager.h"
#include "MemoryManager.h"
#include "CarnageGame.h"
#include "ImGuiManager.h"
#include "TimeManager.h"

//////////////////////////////////////////////////////////////////////////

static const char* SysConfigPath = "config/sys_config.json";

//////////////////////////////////////////////////////////////////////////

const int DefaultScreenResolutionX = 1024;
const int DefaultScreenResolutionY = 768;
const float DefaultPhysicsFramerate = 60.0f;

SystemConfig::SystemConfig()
{
    SetToDefaults();
}

void SystemConfig::SetToDefaults()
{
    mEnableFrameHeapAllocator = true;
    mShowImguiDemoWindow = false;
    mEnableVSync = false;
    mFullscreen = false;
    mScreenSizex = DefaultScreenResolutionX;
    mScreenSizey = DefaultScreenResolutionY;
    mPhysicsFramerate = DefaultPhysicsFramerate;
}

void SystemConfig::InitFromJsonDocument(const cxx::json_document& sourceDocument)
{
    cxx::json_document_node configRootNode = sourceDocument.get_root_node();

    if (cxx::json_document_node screenConfig = configRootNode["screen"])
    {
        if (cxx::json_document_node screenResolution = screenConfig["resolution"])
        {
            cxx::json_get_attribute(screenResolution, 0, mScreenSizex);
            cxx::json_get_attribute(screenResolution, 1, mScreenSizey);
        }

        cxx::json_get_attribute(screenConfig, "fullscreen", mFullscreen);
        cxx::json_get_attribute(screenConfig, "vsync", mEnableVSync);
    }

    // memory
    if (cxx::json_document_node memConfig = configRootNode["memory"])
    {
        cxx::json_get_attribute(memConfig, "enable_frame_heap_allocator", mEnableFrameHeapAllocator);
    }

    // debug
    if (cxx::json_document_node memConfig = configRootNode["debug"])
    {
        cxx::json_get_attribute(memConfig, "show_imgui_demo_window", mShowImguiDemoWindow);
    }
}

void SystemConfig::ExportToJsonDocument(cxx::json_document& sourceDocument)
{
    // todo
}

//////////////////////////////////////////////////////////////////////////

bool SystemStartupParams::ParseStartupParams(int argc, char *argv[])
{
    ClearParams();

    for (int iarg = 0; iarg < argc; )
    {
        if (cxx_stricmp(argv[iarg], "-mapname") == 0 && (argc > iarg + 1))
        {
            mDebugMapName.assign(argv[iarg + 1]);
            iarg += 2;
            continue;
        }
        if (cxx_stricmp(argv[iarg], "-gtadata") == 0 && (argc > iarg + 1))
        {
            mGtaDataLocation.assign(argv[iarg + 1]);
            iarg += 2;
            continue;
        }
        if (cxx_stricmp(argv[iarg], "-numplayers") == 0 && (argc > iarg + 1))
        {
            ::sscanf(argv[iarg + 1], "%d", &mPlayersCount);
            iarg += 2;
            continue;
        }
        ++iarg;
    }

    return true;
}

void SystemStartupParams::ClearParams()
{
    mDebugMapName.clear();
    mGtaDataLocation.clear();
    mPlayersCount = 0;
}

//////////////////////////////////////////////////////////////////////////

System gSystem;

void System::Initialize(int argc, char *argv[])
{
    if (!gConsole.Initialize())
    {
        debug_assert(false);
    }

    gConsole.LogMessage(eLogMessage_Info, GAME_TITLE);
    gConsole.LogMessage(eLogMessage_Info, "System initialize");
    
    if (!mStartupParams.ParseStartupParams(argc, argv))
    {
        debug_assert(false);
    }

    if (!gFiles.Initialize())
    {
        gConsole.LogMessage(eLogMessage_Error, "Cannot initialize filesystem");
        Terminate();
    }

    LoadConfiguration();

    if (!gFiles.SetupGtaDataLocation())
    {
        gConsole.LogMessage(eLogMessage_Error, "Set valid gta gamedata location via sys config param 'gta_gamedata_location'");
        Terminate();
    }

    if (!gMemoryManager.Initialize())
    {
        gConsole.LogMessage(eLogMessage_Error, "Cannot initialize system memory manager");
        Terminate();
    }

    if (!gGraphicsDevice.Initialize())
    {
        gConsole.LogMessage(eLogMessage_Error, "Cannot initialize graphics device");
        Terminate();
    }

    if (!gImGuiManager.Initialize())
    {
        gConsole.LogMessage(eLogMessage_Warning, "Cannot initialize debug ui system");
        // ignore failure
    }

    if (!gRenderManager.Initialize())
    {
        gConsole.LogMessage(eLogMessage_Error, "Cannot initialize render system");
        Terminate();
    }

    if (!gGuiManager.Initialize())
    {
        gConsole.LogMessage(eLogMessage_Error, "Cannot initialize gui system");
        Terminate();
    }

    if (!gCarnageGame.Initialize())
    {
        gConsole.LogMessage(eLogMessage_Error, "Cannot initialize game");
        Terminate();
    }

    gTimeManager.Initialize();

    mQuitRequested = false;
}

void System::Deinit()
{
    gConsole.LogMessage(eLogMessage_Info, "System shutdown");

    gTimeManager.Deinit();
    gCarnageGame.Deinit();
    gImGuiManager.Deinit();
    gGuiManager.Deinit();
    gRenderManager.Deinit();
    gGraphicsDevice.Deinit();
    gMemoryManager.Deinit();
    gFiles.Deinit();
    gConsole.Deinit();
}

void System::Execute()
{
    // main loop
    for (; !mQuitRequested; )
    {
        gInputs.UpdateFrame();
        gTimeManager.UpdateFrame();
        gMemoryManager.FlushFrameHeapMemory();
        gImGuiManager.UpdateFrame();
        gGuiManager.UpdateFrame();
        gCarnageGame.UpdateFrame();
        gRenderManager.RenderFrame();
    }
}

void System::Terminate()
{    
    Deinit(); // leave gracefully
    exit(EXIT_FAILURE);
}

void System::QuitRequest()
{
    mQuitRequested = true;
}

bool System::LoadConfiguration()
{
    mConfig.SetToDefaults();

    cxx::json_document configDocument;
    if (!gFiles.ReadConfig(SysConfigPath, configDocument))
    {
        gConsole.LogMessage(eLogMessage_Warning, "Cannot load config '%s'", SysConfigPath);
        return false;
    }

    cxx::json_document_node configRootNode = configDocument.get_root_node();
    mConfig.InitFromJsonDocument(configDocument);

    // gta1 data files location
    cxx::json_get_attribute(configRootNode, "gta_gamedata_location", gFiles.mGTADataDirectoryPath);
    return true;
}

bool System::SaveConfiguration()
{
    // todo
    return true;
}

double System::GetSystemSeconds() const
{
    double currentTime = ::glfwGetTime();
    return currentTime;
}
