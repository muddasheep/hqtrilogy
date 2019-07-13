/***
*	Headcrab Frenzy Public Source Code
*
*	Copyright (c) 2010-2015, Chain Studios. All rights reserved.
*	
****/
#ifndef STEAMWORKS_H
#define STEAMWORKS_H

#if !defined(UINT32)
typedef unsigned int UINT32;
#endif

enum {
	HQ_SOMOS1 = 0,
	HQ_SOMOS2 = 1,
	HQ_SOMOS3 = 2,
	HQ_DRAGON = 3,
	HQ_VICTIM = 4,
	HQ_NOTES = 5,
	HQ_PATIENCE = 6,
	HQ_SECRET = 7,
	HQ_MAZE = 8,
	HQ_RISE = 9,
	HQ_BUTTONS = 10,
	HQ_MUSIC = 11,
	HQ_COFFEE = 12,
	HQ_DEATH = 13,
	HQ_GRENADE = 14,
	HQ_JUMP = 15,
	HQ_DANCE = 16,
};

struct IHCFSteamWorks
{
	virtual ~IHCFSteamWorks() {}
	virtual void Initialize( void ) = 0;
	virtual void Shutdown( void ) = 0;
	virtual void Think( void ) = 0;
/*	virtual void BindStatData( HCFStatData_t *psd ) = 0;*/
	virtual void Load( void ) = 0;
	virtual void Save( void ) = 0;
	virtual bool SetAchievement( int id ) = 0;
	virtual bool SetAchievement( const char *name ) = 0;
	virtual void ReportAchievementProgress( int id, int curp, int maxp ) = 0;
	virtual void PrintAchievements( void ) = 0;
	virtual const char *GetSUID( void ) const = 0;
};

extern IHCFSteamWorks *g_pSteamWorks;
extern void HCFSteamWorks_Create( void );

#endif //STEAMWORKS_H
