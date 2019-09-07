#include "stdafx.h"
#include "GameCheatsWindow.h"
#include "imgui.h"
#include "RenderingManager.h"

GameCheatsWindow gGameCheatsWindow;

GameCheatsWindow::GameCheatsWindow()
    : DebugWindow("Game Cheats")
{
    for (int ilayer = 0; ilayer < MAP_LAYERS_COUNT; ++ilayer)
    {
        mDrawMapLayers[ilayer] = true;
    }
}

void GameCheatsWindow::DoUI(Timespan deltaTime)
{
    ImGuiWindowFlags wndFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | 
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize;

    ImGuiIO& io = ImGui::GetIO();
    if (!ImGui::Begin(mWindowName, &mWindowShown, wndFlags))
    {
        ImGui::End();
        return;
    }
    
    // pedestrian stats
    if (Pedestrian* pedestrian = gCarnageGame.mPlayerPedestrian)
    {
        ImGui::Separator();
        ImGui::Text("pos: %f, %f, %f", pedestrian->mPosition.x, pedestrian->mPosition.y, pedestrian->mPosition.z);
        ImGui::Text("heading: %f", pedestrian->mHeading);
        ImGui::SliderFloat("pos z", &pedestrian->mPosition.z, 0.0f, 5.0f, "%.2f");
        ImGui::Separator();

        MapCoord curr_block = pedestrian->mPosition;

        BlockStyleData* currBlock = gGameMap.GetBlockClamp(curr_block);
        debug_assert(currBlock);

        ImGui::Text("b ground: %s", cxx::enum_to_string(currBlock->mGroundType));
        ImGui::Text("b slope: %d", currBlock->mSlopeType);
        ImGui::Text("b directions: %d, %d, %d, %d", currBlock->mUpDirection, currBlock->mRightDirection, 
            currBlock->mDownDirection, currBlock->mLeftDirection);
        ImGui::Text("b flat: %d", currBlock->mIsFlat);

        // draw debug block

        glm::vec3 cube_center { 
            MAP_BLOCK_LENGTH * curr_block.x + MAP_BLOCK_LENGTH * 0.5f,
            MAP_BLOCK_LENGTH * curr_block.z + MAP_BLOCK_LENGTH * 0.5f + 0.05f,
            MAP_BLOCK_LENGTH * curr_block.y + MAP_BLOCK_LENGTH * 0.5f,
        };
        glm::vec3 cube_dims {
            MAP_BLOCK_LENGTH - 0.1f,
            MAP_BLOCK_LENGTH - 0.1f,
            MAP_BLOCK_LENGTH - 0.1f
        };  

        gRenderManager.mDebugRenderer.DrawCube(cube_center, cube_dims, COLOR_GREEN);
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Map layers"))
    {
        for (int ilayer = 0; ilayer < MAP_LAYERS_COUNT; ++ilayer)
        {
            cxx::string_buffer_16 cbtext;
            cbtext.printf("layer %d", ilayer);
            ImGui::Checkbox(cbtext.c_str(), &mDrawMapLayers[ilayer]);
        }
    }

    if (ImGui::CollapsingHeader("Ped"))
    {
        ImGui::SliderFloat("Turn speed", &gGameRules.mPedestrianTurnSpeed, 10.0f, 640.0f, "%.2f");
        ImGui::SliderFloat("Run speed", &gGameRules.mPedestrianRunSpeed, 0.1f, 16.0f, "%.2f");
        ImGui::SliderFloat("Walk speed", &gGameRules.mPedestrianWalkSpeed, 0.1f, 16.0f, "%.2f");
    }

    ImGui::End();
}