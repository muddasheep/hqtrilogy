/***
*
*	Copyright (c) 1996-2004, Shambler Team. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Shambler Team.  All other use, distribution, or modification is prohibited
*   without written permission from Shambler Team.
*
****/
/*
====== weapon_generic.cpp ========================================================
*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

enum generic_e 
{
	GENERIC_IDLE1 = 0,
	GENERIC_IDLE2,
	GENERIC_IDLE3,
	GENERIC_SHOOT,
	GENERIC_SHOOT2,
	GENERIC_RELOAD,
	GENERIC_RELOAD2,
	GENERIC_DRAW,
	GENERIC_HOLSTER,
	GENERIC_SPECIAL
};

LINK_ENTITY_TO_CLASS( weapon_generic, CWpnGeneric );

void CBasePlayerWeapon::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);

		sprintf(string, "models/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iszClip"))
	{
	          m_iClipSize = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CWpnGeneric::Spawn( void )
{
	Precache( );
	m_iId = WEAPON_GENERIC;
	m_iDefaultAmmo = m_iClipSize;
	
	SET_MODEL( ENT(pev), STRING(w_model) );
	FallInit();// get ready to fall down.
}

int CWpnGeneric::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

void CWpnGeneric::Precache( void )
{
#ifndef CLIENT_DLL //Shut down mirror cl_messages about this. G-Cont.
	if (!FStringNull (v_model) )//Shut down first call, from "UTIL_PrecacheOtherWeapon"
	{
		ALERT (at_debug, "v_model is %s \n",STRING( v_model));
		ALERT (at_debug, "p_model is %s \n",STRING( p_model));
		ALERT (at_debug, "w_model is %s \n",STRING( w_model));
		ALERT (at_debug, "clip size is %i \n", m_iClipSize);
	}
#endif
	if (!FStringNull (v_model) )
	{
		PRECACHE_MODEL( (char *)STRING(v_model) );//HACKHACK. this required "UTIL_PrecacheOtherWeapon" for 
		PRECACHE_MODEL( (char *)STRING(p_model) );//right adding into HUD.ammo registry. don't worry about 
		PRECACHE_MODEL( (char *)STRING(w_model) );//this ;) G-Cont.

		m_usFireGen1 = PRECACHE_EVENT ( 1, "events/generic1.sc" );//Customization of shooting
		m_usFireGen2 = PRECACHE_EVENT ( 1, "events/generic2.sc" );
		m_usFireGen3 = PRECACHE_EVENT ( 1, "events/generic3.sc" );
	}

#ifndef CLIENT_DLL
	if (!FStringNull (m_iClipSize) )
		UTIL_PrecacheOtherWeapon ("weapon_generic");
#endif
}

int CWpnGeneric::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = 250;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 10;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId  = WEAPON_GENERIC;
	p->iFlags = 0;
	p->iWeight = 10;

	return 1;
}

BOOL CWpnGeneric::Deploy( void )
{
#ifndef CLIENT_DLL
	m_pPlayer->TabulateAmmo();
#endif	
	m_pPlayer->pev->viewmodel = v_model;
	m_pPlayer->pev->weaponmodel = p_model;

	strcpy( m_pPlayer->m_szAnimExtention, "onehanded" );
	SendWeaponAnim( GENERIC_DRAW, 1, 0 );

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;

	return TRUE;
}

void CWpnGeneric::Holster( int skiplocal )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	SendWeaponAnim( GENERIC_HOLSTER );
}

void CWpnGeneric::PrimaryAttack()
{
	m_iClip--;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	int flags;

#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif
	ALERT ( at_debug, "Ba-Bax! :-)\n" );

	m_flNextPrimaryAttack = gpGlobals->time + 0.3;
}


void CWpnGeneric::SecondaryAttack( void )
{
	ALERT ( at_debug, "Boom! :-)\n" );

	m_flNextSecondaryAttack = gpGlobals->time + 0.3;
}

void CWpnGeneric::Reload( void )
{
	if ( m_pPlayer->ammo_9mm <= 0 )
		 return;

	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload( 17, GENERIC_RELOAD, 1.5 );
	else
		iResult = DefaultReload( 17, GENERIC_RELOAD2, 1.5 );

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	}
}

void CWpnGeneric::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	// only idle if the slid isn't back
	if (m_iClip != 0)
	{
		int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.0, 1.0 );

		if (flRand <= 0.3 + 0 * 0.75)
		{
			iAnim = GENERIC_IDLE3;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 49.0 / 16;
		}
		else if (flRand <= 0.6 + 0 * 0.875)
		{
			iAnim = GENERIC_IDLE1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0 / 16.0;
		}
		else
		{
			iAnim = GENERIC_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
		}
		SendWeaponAnim( iAnim, 1 );
	}
}
