#include <iostream>
#include <chrono>
using namespace std;
#include <algorithm>
#include <cmath>
#define _USE_MATH_DEFINES

#include "olcConsoleGameEngine.h"

class OneLoneCode_UltimateFPS : public olcConsoleGameEngine {
public:
    OneLoneCode_UltimateFPS() {
        m_sAppName = L"Ultimate First Person Shooter";
    }

    virtual bool OnUserCreate() {
        map += L"################";
        map += L"#..............#";
        map += L"#..............#";
        map += L"#..............#";
        map += L"#..............#";
        map += L"#..............#";
        map += L"#.........######";
        map += L"#..............#";
        map += L"#..............#";
        map += L"#.........#....#";
        map += L"#.........#....#";
        map += L"#.........#....#";
        map += L"#.........#....#";
        map += L"#.........#....#";
        map += L"#.........#....#";
        map += L"################";

        spriteWall = new olcSprite(L"../SpriteEditor/fps_wall1.spr");
        spriteLamp = new olcSprite(L"../SpriteEditor/fps_lamp1.spr");
        spriteFireBall = new olcSprite(L"../SpriteEditor/fps_fireball1.spr");

        fDepthBuffer = new float[ScreenWidth()];

        listObjects = {
            {8.5f, 10.5f, 0.0f, 0.0f, false, spriteLamp},
            {7.5f, 11.5f, 0.0f, 0.0f, false, spriteLamp},
            {6.5f, 9.5f, 0.0f, 0.0f, false, spriteLamp},
        };

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) {
        if (m_keys[L'Q'].bHeld)
            fPlayerA -= (1.2f) * fElapsedTime;
        if (m_keys[L'E'].bHeld)
            fPlayerA += (1.2f) * fElapsedTime;
        if (m_keys[L'W'].bHeld) {
            fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            if (map.c_str()[(nMapHeight - (int)fPlayerY - 1) * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }
        if (m_keys[L'A'].bHeld) {
            fPlayerX -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY += sinf(fPlayerA) * 5.0f * fElapsedTime;
            if (map.c_str()[(nMapHeight - (int)fPlayerY - 1) * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX += cosf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }
        if (m_keys[L'S'].bHeld) {
            fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            if (map.c_str()[(nMapHeight - (int)fPlayerY - 1) * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }
        if (m_keys[L'D'].bHeld) {
            fPlayerX += cosf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            if (map.c_str()[(nMapHeight - (int)fPlayerY - 1) * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX -= cosf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY += sinf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }
        if (m_keys[VK_SPACE].bReleased) {
            sObject o;
            o.x = fPlayerX;
            o.y = fPlayerY;

            float fNoise = ((float)rand() / (float)RAND_MAX - 0.5f) * 0.1f;
            o.vx = sinf(fPlayerA + fNoise) * 8.0f;
            o.vy = cosf(fPlayerA + fNoise) * 8.0f;

            o.sprite = spriteFireBall;
            o.bRemove = false;
            listObjects.push_back(o);
        }

        for (int x = 0; x < ScreenWidth(); x++) {
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)ScreenWidth()) * fFOV;

            float fStepSize = 0.01f;
            float fDistanceToWall = 0;
            bool bHitWall = false;
            bool bBoundary = false;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            float fSampleX = 0.0f;

            while (!bHitWall && fDistanceToWall < fDepth) {
                fDistanceToWall += fStepSize;
                float fTestPointX = fPlayerX + fEyeX * fDistanceToWall;
                float fTestPointY = fPlayerY + fEyeY * fDistanceToWall;
                int nTestX = (int)fTestPointX;
                int nTestY = (int)fTestPointY;

                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else {
                    if (map.c_str()[(nMapHeight - nTestY - 1) * nMapWidth + nTestX] == '#') {
                        bHitWall = true;
                       
                        float fBlockMidX = (float)nTestX + 0.5f;
                        float fBlockMidY = (float)nTestY + 0.5f;

                        float fTestAngle = atan2f(fTestPointY - fBlockMidY, fTestPointX - fBlockMidX);

                        if (fTestAngle >= -M_PI_4 && fTestAngle < M_PI_4)
                            fSampleX = fTestPointY - (float)nTestY;
                        if (fTestAngle >= M_PI_4 && fTestAngle < M_PI * 0.75f)
                            fSampleX = fTestPointX - (float)nTestX;
                        if (fTestAngle < -M_PI_4 && fTestAngle >= -M_PI * 0.75f)
                            fSampleX = fTestPointX - (float)nTestX;
                        if (fTestAngle >= M_PI * 0.75f || fTestAngle < -M_PI * 0.75f)
                            fSampleX = fTestPointY - (float)nTestY;
                    }
                }
            }

            int nCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToWall);
            int nFloor = ScreenHeight() - nCeiling;

            fDepthBuffer[x] = fDistanceToWall;

            for (int y = 0; y < ScreenHeight(); y++) {
                if (y <= nCeiling)
                    Draw(x, y, L' ');
                else if (y > nCeiling && y <= nFloor) {
                    if (fDistanceToWall < fDepth) {
                        float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);
                        Draw(x, y, spriteWall->SampleGlyph(fSampleX, fSampleY), spriteWall->SampleColour(fSampleX, fSampleY));
                    }
                    else {
                        Draw(x, y, PIXEL_SOLID, 0);
                    }
                }
                else {
                    Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
                }
            }
        }

        for (auto& object : listObjects) {
            object.x += object.vx * fElapsedTime;
            object.y += object.vy * fElapsedTime;

            if (map.c_str()[(int)object.x * nMapWidth + (int)object.y] == '#')
                object.bRemove = true;

            float fVecX = object.x - fPlayerX;
            float fVecY = object.y - fPlayerY;
            float fDistanceFromPlayer = sqrtf(fVecX * fVecX + fVecY * fVecY);

            float fEyeX = sinf(fPlayerA);
            float fEyeY = cosf(fPlayerA);
            float fAngleDis = atan2f(fEyeY, fEyeX) - atan2f(fVecY, fVecX);

            if (fAngleDis > M_PI)
                fAngleDis = M_PI * 2 - fAngleDis;
            if (fAngleDis < -M_PI)
                fAngleDis = M_PI * 2 + fAngleDis;
            bool bInPlayerFOV = fabs(fAngleDis) < fFOV / 2.0f;

            if (bInPlayerFOV && fDistanceFromPlayer >= 0.5f && fDistanceFromPlayer < fDepth) {
                float fObjectCeiling = (float)(ScreenHeight() / 2.0f) - ScreenHeight() / (float)fDistanceFromPlayer;
                float fObjectFloor = ScreenHeight() - fObjectCeiling;
                float fObjectHeight = fObjectFloor - fObjectCeiling;
                float fObjectAspectRatio = (float)object.sprite->nWidth / (float)object.sprite->nHeight;
                float fObjectWidth = fObjectHeight * fObjectAspectRatio;
                float fMiddleObject = (0.5f * (fAngleDis / (fFOV / 2.0f)) + 0.5f) * (float)ScreenWidth();

                for (float lx = 0; lx < fObjectWidth; lx++) {
                    for (float ly = 0; ly < fObjectHeight; ly++) {
                        float fSampleX = lx / fObjectWidth;
                        float fSampleY = ly / fObjectHeight;
                        wchar_t c = object.sprite->SampleGlyph(fSampleX, fSampleY);
                        int nObjectColumn = (int)(fMiddleObject + lx - fObjectWidth / 2.0f);
                        if (nObjectColumn > 0 && nObjectColumn < ScreenWidth() && c != L' ' && fDepthBuffer[nObjectColumn] >= fDistanceFromPlayer) {
                            fDepthBuffer[nObjectColumn] = fDistanceFromPlayer;
                            Draw(nObjectColumn, fObjectCeiling + ly, c, object.sprite->SampleColour(fSampleX, fSampleY));
                        }
                    }
                }
            }
        }

        listObjects.remove_if([](sObject& o) {return o.bRemove; });

        for (int nx = 0; nx < nMapWidth; nx++) {
            for (int ny = 0; ny < nMapHeight; ny++) {
                Draw(nx, ny + 1, map.c_str()[ny * nMapWidth + nx]);
            }
        }
        Draw((int)fPlayerX, nMapHeight - (int)fPlayerY, L'P');

        

        return true;
    }
private:
    float fPlayerX = 8.0f;
    float fPlayerY = 8.0f;
    float fPlayerA = 0.0f;

    int nMapHeight = 16;
    int nMapWidth = 16;

    float fFOV = 3.14159f / 4.0f;
    float fDepth = 16.0f;

    wstring map;

    olcSprite* spriteWall;
    olcSprite* spriteLamp;
    olcSprite* spriteFireBall;

    float* fDepthBuffer = nullptr;

    struct sObject {
        float x;
        float y;
        float vx;
        float vy;
        bool bRemove;
        olcSprite* sprite;
    };

    list<sObject> listObjects;
};



int main()
{
    OneLoneCode_UltimateFPS game;
    game.ConstructConsole(320, 240, 4, 4);
    game.Start();
    return 0;
}