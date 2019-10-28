/*This file is part of FFB Arcade Plugin.
FFB Arcade Plugin is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
FFB Arcade Plugin is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with FFB Arcade Plugin.If not, see < https://www.gnu.org/licenses/>.
*/

#include <string>
#include "GRID.h"
#include "SDL.h"
static EffectTriggers* myTriggers;
static EffectConstants* myConstants;
static Helpers* myHelpers;

static wchar_t* settingsFilename = TEXT(".\\FFBPlugin.ini");
static int SpringStrength = GetPrivateProfileInt(TEXT("Settings"), TEXT("SpringStrength"), 0, settingsFilename);
static int GearChangeStrength = GetPrivateProfileInt(TEXT("Settings"), TEXT("GearChangeStrength"), 20, settingsFilename);
static int GearChangeLength = GetPrivateProfileInt(TEXT("Settings"), TEXT("GearChangeLength"), 200, settingsFilename);

static int GearChangeThread(void* ptr)
{
	myHelpers->log("gear change");
	double percentForce = GearChangeStrength / 100.0;
	myTriggers->Sine(GearChangeLength, 0, percentForce);
	myTriggers->LeftRight(0, percentForce, 150);
	return 0;
}

void GRID::FFBLoop(EffectConstants* constants, Helpers* helpers, EffectTriggers* triggers) {

	INT_PTR WallBase = helpers->ReadIntPtr(0xB1B7F0, true);
	INT_PTR WallBase1 = helpers->ReadIntPtr(WallBase + 0x730, false);
	INT_PTR WallBase2 = helpers->ReadIntPtr(WallBase1 + 0x4, false);
	INT_PTR WallBase3 = helpers->ReadIntPtr(WallBase2 + 0x4, false);
	float WallBase4 = helpers->ReadFloat32(WallBase3 + 0x118, false);
	INT_PTR PanelBase = helpers->ReadIntPtr(0xA3FA34, true);
	INT_PTR PanelBase1 = helpers->ReadIntPtr(PanelBase + 0x678, false);
	INT_PTR PanelBase2 = helpers->ReadIntPtr(PanelBase1 + 0x14, false);
	INT_PTR PanelBase3 = helpers->ReadIntPtr(PanelBase2 + 0x30, false);
	UINT8 PanelBase4 = helpers->ReadByte(PanelBase3 + 0x2C, false);
	UINT8 Wheels = helpers->ReadByte(PanelBase3 + 0xB4, false);
	UINT8 gear = helpers->ReadByte(0x346A5C6C, false);
	INT_PTR speedoBase = helpers->ReadIntPtr(0x28C008, true);
	INT_PTR speedoBase1 = helpers->ReadIntPtr(speedoBase + 0xD0, false);
	INT_PTR speedoBase2 = helpers->ReadIntPtr(speedoBase1 + 0x460, false);
	INT_PTR speedoBase3 = helpers->ReadIntPtr(speedoBase2 + 0x184, false);
	float speedo = helpers->ReadFloat32(speedoBase3 + 0x4F4, false);

	UINT8 static oldgear = 0;
	UINT8 newgear = gear;

	{
		triggers->Springi(SpringStrength / 100.0);
	}

	if ((oldgear != newgear) && (speedo > 0))
	{
		SDL_Thread* gearChangeThread = SDL_CreateThread(GearChangeThread, "GearChangeThread", (void*)NULL);
	}

	if (Wheels > 0)
	{
		double percentForce = Wheels / 37.0;
		double percentLength = 100;
		triggers->LeftRight(percentForce, percentForce, percentLength);
		triggers->Sine(80, 80, percentForce);
	}

	if ((WallBase4 > 0) && (PanelBase4 > 0))
	{
		double percentForce = (PanelBase4) / 8.0;
		double percentLength = 100;
		triggers->LeftRight(percentForce, 0, percentLength);
		triggers->Constant(constants->DIRECTION_FROM_RIGHT, percentForce);
	}
	else if ((WallBase4 < 0) && (PanelBase4 > 0))
	{
		double percentForce = (PanelBase4) / 8.0;
		double percentLength = 100;
		triggers->LeftRight(0, percentForce, percentLength);
		triggers->Constant(constants->DIRECTION_FROM_LEFT, percentForce);
	}
	oldgear = newgear;
	myTriggers = triggers;
	myConstants = constants;
	myHelpers = helpers;
}