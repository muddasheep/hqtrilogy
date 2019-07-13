/***
*	Headcrab Frenzy Public Source Code
*
*	Copyright (c) 2010-2015, Chain Studios. All rights reserved.
*	
****/
#include "extdll.h"
#include "util.h"
#include "gamerules.h"
#include "steamworks.h"
#include "../steam/steam_api.h"

#define HCF_STEAM_APPID						644320
#define HCF_STEAM_APPID_FILENAME			"steam_appid.txt"

#define HCF_STAT_VERSION_NUMBER				1
#define HCF_ACHIEVEMENT_VERSION_NUMBER		1

typedef struct HCFSteamAchievement_s {
	const char	*m_name;
	int			m_id;
	bool		m_achieved;
} HCFACH;

#define HCF_STAT( id, name )	{ name, id, -1 }
#define HCF_ACH( id, name )		{ name, id, false }

//////////////////////////////////////////////////////////////////////////

enum {
	HCF_STAT_VERSION = 1,
	HCF_ACHIEVEMENT_VERSION,
};

static HCFACH g_Ach[] = {
	HCF_ACH(HQ_SOMOS1, "HQ_SOMOS1"),
	HCF_ACH(HQ_SOMOS2, "HQ_SOMOS2"),
	HCF_ACH(HQ_SOMOS3, "HQ_SOMOS3"),
	HCF_ACH(HQ_DRAGON, "HQ_DRAGON"),
	HCF_ACH(HQ_VICTIM, "HQ_VICTIM"),
	HCF_ACH(HQ_NOTES, "HQ_NOTES"),
	HCF_ACH(HQ_PATIENCE, "HQ_PATIENCE"),
	HCF_ACH(HQ_SECRET, "HQ_SECRET"),
	HCF_ACH(HQ_MAZE, "HQ_MAZE"),
	HCF_ACH(HQ_RISE, "HQ_RISE"),
	HCF_ACH(HQ_BUTTONS, "HQ_BUTTONS"),
	HCF_ACH(HQ_MUSIC, "HQ_MUSIC"),
	HCF_ACH(HQ_COFFEE, "HQ_COFFEE"),
	HCF_ACH(HQ_DEATH, "HQ_DEATH"),
	HCF_ACH(HQ_GRENADE, "HQ_GRENADE"),
	HCF_ACH(HQ_JUMP, "HQ_JUMP"),
	HCF_ACH(HQ_DANCE, "HQ_DANCE"),
};

//////////////////////////////////////////////////////////////////////////

IHCFSteamWorks *g_pSteamWorks;

class CHCFSteamWorks : public IHCFSteamWorks
{
public:
	CHCFSteamWorks();
	virtual ~CHCFSteamWorks() {}

	virtual void Initialize( void );
	virtual void Shutdown( void );
	virtual void Think( void );
/*	virtual void BindStatData( HCFStatData_t *psd );*/
	virtual void Load( void );
	virtual void Save( void );
	virtual bool SetAchievement( int id );
	virtual bool SetAchievement( const char *name );
	virtual void ReportAchievementProgress( int id, int curp, int maxp );
	virtual void PrintAchievements( void );
	virtual const char *GetSUID( void ) const;

protected:
	bool IsLoggedOn( void );
	void MaintainAppid( void );
	
	HCFACH *GetAchievementPointer( int id );
	HCFACH *GetAchievementPointer( const char *name );

	STEAM_CALLBACK( CHCFSteamWorks, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored );

private:
	BOOL	m_fInitialized;
	UINT32	m_uiAppID;
	char	m_szSUID[128];
	HCFACH	*m_pAchievements;
	size_t	m_uiAchievementCount;
	size_t	m_uiStatCount;
	/*HCFStatData_t *m_pExternalStat;*/
};

void HCFSteamWorks_Create( void )
{
	if ( g_pSteamWorks )
		delete g_pSteamWorks;
	g_pSteamWorks = new CHCFSteamWorks();
}

CHCFSteamWorks :: CHCFSteamWorks() :
	m_CallbackAchievementStored( this, &CHCFSteamWorks::OnAchievementStored )
{
	m_fInitialized = FALSE;
	m_uiAppID = 0;
	memset( m_szSUID, 0, sizeof(m_szSUID) );

	m_pAchievements = g_Ach;
	m_uiAchievementCount = sizeof(g_Ach) / sizeof(g_Ach[0]);
}

void CHCFSteamWorks :: Initialize( void )
{
	ASSERT( m_fInitialized == FALSE );

	ALERT( at_console, "Initializing SteamWorks...\n" );

	// make sure appid is valid
	MaintainAppid();

	if ( !SteamAPI_Init() ) {
		ALERT( at_console, "SteamAPI_Init failed.\n" );
		return;
	}

	ISteamUtils *utils = SteamUtils();
	ISteamUser *user = SteamUser();
	ISteamUserStats *stats = SteamUserStats();
	if ( !user || !utils || !stats ) {
		ALERT( at_console, "SteamAPI interfaces missing.\n" );
		return;
	}

	// cache appid
	m_uiAppID = utils->GetAppID();
	ALERT( at_console, "Steam AppID = %u\n", m_uiAppID );

	// cache steam userid (SUID)
	CSteamID sid = user->GetSteamID();
	sprintf( m_szSUID, "%u_%u_%u", sid.GetEUniverse(), sid.GetAccountID(), sid.GetUnAccountInstance() );
	ALERT( at_aiconsole, "Steam UserID = %s\n", m_szSUID );

	m_fInitialized = TRUE;
	ALERT( at_console, "SteamWorks initialized.\n" );
}

void CHCFSteamWorks :: Shutdown( void )
{
	ALERT( at_console, "SteamWorks shutdown.\n" );
	SteamAPI_Shutdown();
}

void CHCFSteamWorks :: Think( void )
{
	SteamAPI_RunCallbacks();
}

void CHCFSteamWorks :: MaintainAppid( void )
{
	char buffer[32];
	bool update = true;

	FILE *fp = fopen( HCF_STEAM_APPID_FILENAME, "r" );
	if ( fp ) {
		fgets( buffer, sizeof(buffer)-1, fp );
		unsigned int check_appid = (UINT32)atoi( buffer );
		if ( check_appid == HCF_STEAM_APPID )
			update = false;
		fclose( fp );
	}

	if ( !update )
		return;

	ALERT( at_console, "Updating \"%s\"...\n", HCF_STEAM_APPID_FILENAME );
	fp = fopen( HCF_STEAM_APPID_FILENAME, "w" );
	fprintf( fp, "%u\n", HCF_STEAM_APPID );
	fclose( fp );
	SteamAPI_Shutdown();
}

void CHCFSteamWorks :: Load( void )
{
}

void CHCFSteamWorks :: Save( void )
{
}

const char *CHCFSteamWorks :: GetSUID( void ) const
{
	return m_szSUID[0] ? m_szSUID : "common";
}

bool CHCFSteamWorks :: IsLoggedOn( void )
{
	if ( !m_fInitialized )
		return false;
	return SteamUser()->BLoggedOn();
}

//////////////////////////////////////////////////////////////////////////
// STEAM ACHIEVEMENTS
//////////////////////////////////////////////////////////////////////////
HCFACH *CHCFSteamWorks :: GetAchievementPointer( int id )
{
	HCFACH *p = m_pAchievements;
	for ( size_t i = 0; i < m_uiAchievementCount; ++i, ++p )
		if ( p->m_id == id )
			return p;
	return NULL;
}

HCFACH *CHCFSteamWorks :: GetAchievementPointer( const char *name )
{
	HCFACH *p = m_pAchievements;
	for ( size_t i = 0; i < m_uiAchievementCount; ++i, ++p )
		if ( !_stricmp( name, p->m_name ) )
			return p;
	return NULL;
}

void CHCFSteamWorks :: OnAchievementStored( UserAchievementStored_t *pCallback )
{
	ALERT(at_console, "SteamWorks: got a callback... \n");

	// we may get callbacks for other games' stats arriving, ignore them
	if ( m_uiAppID != pCallback->m_nGameID )
		return;

	ALERT( at_console, "SteamWorks: stored achievement for Steam\n" );
}

bool CHCFSteamWorks :: SetAchievement( int id )
{
	ALERT(at_console, "is logged on?\n");

	// check if we're initialized and logged on
	if ( !IsLoggedOn() )
		return false;

	ALERT(at_console, "does achievement exist?\n");

	HCFACH *p = GetAchievementPointer( id );
	if ( !p || p->m_achieved ) {
		if ( !p )
			ALERT( at_console, "SteamWorks: no such achievement id %i\n", id );
		else
			ALERT( at_console, "SteamWorks: achievement \"%s\" already set\n", p->m_name ); //!TODO: ai
		return false;
	}

	ALERT( at_console, "SteamWorks: setting achievement \"%s\"!\n", p->m_name );

	SteamUserStats()->SetAchievement( p->m_name );
	p->m_achieved = true;

	return true;
}

bool CHCFSteamWorks :: SetAchievement( const char *name )
{
	// sanity check
	if ( !name || !*name )
		return false;

	// check if we're initialized and logged on
	if ( !IsLoggedOn() )
		return false;

	HCFACH *p = GetAchievementPointer( name );
	if ( !p || p->m_achieved ) {
		if ( !p )
			ALERT( at_console, "SteamWorks: no such achievement name \"%s\"\n", name );
		else
			ALERT( at_console, "SteamWorks: achievement \"%s\" already set\n", p->m_name );
		return false;
	}

	ALERT( at_aiconsole, "SteamWorks: setting achievement \"%s\"!\n", p->m_name );

	SteamUserStats()->SetAchievement( p->m_name );
	p->m_achieved = true;

	return true;
}

void CHCFSteamWorks :: ReportAchievementProgress( int id, int curp, int maxp )
{
	// check if we're initialized and logged on
	if ( !IsLoggedOn() )
		return;

	HCFACH *p = GetAchievementPointer( id );
	if ( !p || p->m_achieved ) {
		if ( !p )
			ALERT( at_console, "SteamWorks: no such achievement id %i\n", id );
		return;
	}

	ALERT( at_aiconsole, "SteamWorks: reporting achievement progress \"%s\": %i/%i!\n", p->m_name, curp, maxp );
	SteamUserStats()->IndicateAchievementProgress( p->m_name, curp, maxp );
}

void CHCFSteamWorks :: PrintAchievements( void )
{
	HCFACH *p = m_pAchievements;
	for ( size_t i = 0; i < m_uiAchievementCount; ++i, ++p )
		ALERT( at_console, "%3i : \"%s\" : %s\n", (int)( i + 1 ), p->m_name, p->m_achieved ? "YES" : "NO" );
}

