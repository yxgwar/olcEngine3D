#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#define _USE_MATH_DEFINES
using namespace std;

#include "olcConsoleGameEngine.h"

class cPhysicsObject {
public:
	float px = 0.0f;
	float py = 0.0f;
	float vx = 0.0f;
	float vy = 0.0f;
	float ax = 0.0f;
	float ay = 0.0f;

	float radius = 4.0f;
	bool bStable = false;
	float fFriction = 0.8f;

	int nBounceBeforeDeath = -1;
	bool bDead = false;

	cPhysicsObject(float x = 0.0f, float y = 0.0f) {
		px = x;
		py = y;
	}

	virtual void Draw(olcConsoleGameEngine* engine, float fOffestX, float fOffestY) = 0;
};

class cDummy :public cPhysicsObject {
public:
	cDummy(float x = 0.0f, float y = 0.0f) :cPhysicsObject(x, y) {

	}

	virtual void Draw(olcConsoleGameEngine* engine, float fOffestX, float fOffestY) {
		engine->DrawWireFrameModel(vecModel, px - fOffestX, py - fOffestY, atan2f(vy, vx), radius, FG_WHITE);
	}
private:
	static vector<pair<float, float>> vecModel;
};

vector<pair<float, float>> DefineDummy() {
	vector<pair<float, float>> vecModel;
	vecModel.push_back({ 0.0f, 0.0f });
	for (int i = 0; i < 10; i++) {
		vecModel.push_back({ cosf(i / 9.0f * 2.0f * M_PI), sinf(i / 9.0f * 2.0f * M_PI) });
	}
	return vecModel;
}

vector<pair<float, float>> cDummy::vecModel = DefineDummy();

class cDebris: public cPhysicsObject {
public:
	cDebris(float x = 0.0f, float y = 0.0f): cPhysicsObject(x, y) {
		vx = 10.0f * cosf((float)rand() / (float)RAND_MAX * 2.0f * M_PI);
		vy = 10.0f * sinf((float)rand() / (float)RAND_MAX * 2.0f * M_PI);
		radius = 1.0f;
		fFriction = 0.8f;
		nBounceBeforeDeath = 5;
	}

	virtual void Draw(olcConsoleGameEngine* engine, float fOffestX, float fOffestY) {
		engine->DrawWireFrameModel(vecModel, px - fOffestX, py - fOffestY, atan2f(vy, vx), radius, FG_DARK_GREEN);
	}

private:
	static vector<pair<float, float>> vecModel;
};

vector<pair<float, float>> DefineDebris() {
	vector<pair<float, float>> vecModel;
	vecModel.push_back({ 0.0f, 0.0f });
	vecModel.push_back({ 1.0f, 0.0f });
	vecModel.push_back({ 0.0f, 1.0f });
	vecModel.push_back({ 1.0f, 1.0f });
	return vecModel;
}
vector<pair<float, float>> cDebris::vecModel = DefineDebris();

class OneLongCoder_Worms : public olcConsoleGameEngine {
public:
	OneLongCoder_Worms() {
		m_sAppName = L"Worms";
	}
private:
	int nMapWidth = 1024;
	int nMapHeight = 512;
	unsigned char* map = nullptr;

	float fCameraPosX = 0.0f;
	float fCameraPosY = 0.0f;

	list<unique_ptr<cPhysicsObject>> listObjects;

	virtual bool OnUserCreate() {
		map = new unsigned char[nMapWidth * nMapHeight];
		memset(map, 0, nMapWidth * nMapHeight * sizeof(unsigned char));
		CreateMap();
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		if (m_keys[L'M'].bReleased) {
			CreateMap();
		}

		if (m_mouse[0].bReleased) {
			for (int i = 0; i < 20; i++) {
				listObjects.push_back(unique_ptr<cDebris>(new cDebris(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY)));
			}
		}

		if (m_mouse[2].bReleased) {
			cDummy* p = new cDummy(m_mousePosX + fCameraPosX, m_mousePosY + fCameraPosY);
			listObjects.push_back(unique_ptr<cDummy>(p));
		}

		float fMapScrollSpeed = 400.0f;
		if (m_mousePosX < 5)
			fCameraPosX -= fMapScrollSpeed * fElapsedTime;
		if(m_mousePosX > ScreenWidth() - 5)
			fCameraPosX += fMapScrollSpeed * fElapsedTime;
		if (m_mousePosY < 5)
			fCameraPosY -= fMapScrollSpeed * fElapsedTime;
		if (m_mousePosY > ScreenHeight() - 5)
			fCameraPosY += fMapScrollSpeed * fElapsedTime;

		if (fCameraPosX < 0) fCameraPosX = 0;
		if (fCameraPosX >= nMapWidth - ScreenWidth()) fCameraPosX = nMapWidth - ScreenWidth();
		if (fCameraPosY < 0) fCameraPosY = 0;
		if (fCameraPosY >= nMapHeight - ScreenHeight()) fCameraPosY = nMapHeight - ScreenHeight();

		for (int z = 0; z < 10; z++) {
			for (auto& p : listObjects) {
				p->ay += 2.0f;
				p->vx += p->ax * fElapsedTime;
				p->vy += p->ay * fElapsedTime;

				float fPotentialX = p->px + p->vx * fElapsedTime;
				float fPotentialY = p->py + p->vy * fElapsedTime;

				p->ax = 0.0f;
				p->ay = 0.0f;
				p->bStable = false;

				float fAngle = atan2f(p->vy, p->vx);
				float fResponseX = 0;
				float fResponseY = 0;
				bool bCollision = false;

				for (float r = fAngle - M_PI_2; r < fAngle + M_PI_2; r += M_PI / 8.0f) {
					float fTestPosX = (p->radius) * cosf(r) + fPotentialX;
					float fTestPosY = (p->radius) * sinf(r) + fPotentialY;

					if (fTestPosX >= nMapWidth) fTestPosX = nMapWidth - 1;
					if (fTestPosY >= nMapHeight) fTestPosY = nMapHeight - 1;
					if (fTestPosX < 0) fTestPosX = 0;
					if (fTestPosY < 0) fTestPosY = 0;

					if (map[(int)fTestPosY * nMapWidth + (int)fTestPosX] != 0) {
						fResponseX += fPotentialX - fTestPosX;
						fResponseY += fPotentialY - fTestPosY;
						bCollision = true;
					}
				}
				float fMagVelocity = sqrtf(p->vx * p->vx + p->vy * p->vy);
				float fMagResponse = sqrtf(fResponseX * fResponseX + fResponseY * fResponseY);

				if (bCollision) {
					p->bStable = true;

					float dot = p->vx * (fResponseX / fMagResponse) + p->vy * (fResponseY / fMagResponse);
					p->vx = p->fFriction * (-2.0f * dot * (fResponseX / fMagResponse) + p->vx);
					p->vy = p->fFriction * (-2.0f * dot * (fResponseY / fMagResponse) + p->vy);

					if (p->nBounceBeforeDeath > 0) {
						p->nBounceBeforeDeath--;
						p->bDead = p->nBounceBeforeDeath == 0;
					}
				}
				else {
					p->px = fPotentialX;
					p->py = fPotentialY;
				}

				if (fMagVelocity < 0.1f) p->bStable = true;
			}

			listObjects.remove_if([](unique_ptr<cPhysicsObject> &o) {return o->bDead; });
		}

		

		for (int x = 0; x < ScreenWidth(); x++) {
			for (int y = 0; y < ScreenHeight(); y++) {
				switch (map[(y + (int)fCameraPosY) * nMapWidth + x + (int)fCameraPosX]) {
				case 0:
					Draw(x, y, PIXEL_SOLID, FG_CYAN);
					break;
				case 1:
					Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
					break;
				}
			}
		}

		for (auto& p : listObjects) {
			p->Draw(this, fCameraPosX, fCameraPosY);
		}

		return true;
	}

	void CreateMap() {
		float* fSurface = new float[nMapWidth];
		float* fNoiseSpeed = new float[nMapWidth];

		for (int i = 0; i < nMapWidth; i++) {
			fNoiseSpeed[i] = (float)rand() / RAND_MAX;
		}

		fNoiseSpeed[0] = 0.5f;
		PerlinNoise1D(nMapWidth, fNoiseSpeed, 8, 2.0f, fSurface);

		for (int x = 0; x < nMapWidth; x++) {
			for (int y = 0; y < nMapHeight; y++) {
				if (y >= fSurface[x] * nMapHeight)
					map[y * nMapWidth + x] = 1;
				else
					map[y * nMapWidth + x] = 0;
			}
		}

		delete[] fSurface;
		delete[] fNoiseSpeed;
	}

	void PerlinNoise1D(int nCount, float* fSeed, int nOctaves, float fBias, float* fOutput)
	{
		// Used 1D Perlin Noise
		for (int x = 0; x < nCount; x++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nCount >> o;
				int nSample1 = (x / nPitch) * nPitch;
				int nSample2 = (nSample1 + nPitch) % nCount;
				float fBlend = (float)(x - nSample1) / (float)nPitch;
				float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];
				fScaleAcc += fScale;
				fNoise += fSample * fScale;
				fScale = fScale / fBias;
			}

			// Scale to seed range
			fOutput[x] = fNoise / fScaleAcc;
		}
	}
};

int main() {
	OneLongCoder_Worms game;
	game.ConstructConsole(256, 160, 6, 6);
	game.Start();
	return 0;
}