// Copyright (c) Richard Rohac, 2014, All rights reserved.

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>

int CHudTokens::Init(void)
{
	m_iFlags = HUD_ACTIVE;
	gHUD.AddHudElem(this);

	return 1;
};

int CHudTokens::VidInit(void)
{
	m_SpriteHandle_t = 0;

	return 1;
};

int CHudTokens::Draw(float fTime)
{
	float flMaxTokens = CVAR_GET_FLOAT("sv_maxtokens");
	if((flMaxTokens <= 0.0f) || gEngfuncs.pfnGetCvarFloat("r_hidetokenhud"))
	{
		return 0;
	}

	if ( !m_SpriteHandle_t )
		m_SpriteHandle_t = LoadSprite("sprites/scramhud1.spr");

	// draw the base
	float x, y;
	SPR_Set(m_SpriteHandle_t, 255,255, 255);

	y = (0.20*ScreenHeight);
	x = ScreenWidth - (SPR_Width(m_SpriteHandle_t,0)*1.35);

	SPR_DrawAdditive( 0,  x, y, NULL);

	DrawNumbers(CVAR_GET_FLOAT("sv_numtokens"), x + 145, y+4);
	DrawNumbers(flMaxTokens, x + 145, y+48);

	return 1;
}

void CHudTokens::DrawNumbers(int iValue, int x, int y)
{
	int r, g, b;
	UnpackRGB(r,g,b, RGB_YELLOWISH);
	gHUD.DrawHudNumber(x, y, DHN_3DIGITS | DHN_DRAWZERO, iValue, r, g, b);
}