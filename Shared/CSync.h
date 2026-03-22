/*************************************************************
*
* Solution   : Mafia 2 Multiplayer
* Project    : Shared Library
* File       : CSync.h
* Developers : AaronLad <aaron@m2-multiplayer.com>
*
***************************************************************/

#pragma once

// NOTE(zaklaus): Even though it sounds forced, Linux.h contains condition for non-win32 system.
#include "CCommon.h"
#include "Linux.h"

#include "CColor.h"
#include "Math/CVector3.h"
#include "RakString.h"

// Sync data

struct OnFootSync
{
	CVector3		m_vecPosition;
	CVector3		m_vecRotation;
	CVector3		m_vecDirection;
	CVector3		m_vecVelocity;
	float			m_fHealth;
	DWORD			m_dwSelectedWeapon;
	int				m_iSelectedWeaponBullet;
	BYTE			m_bControlState;
	bool			m_bAiming;
	bool			m_bShooting;
	bool			m_bCrouching;
	unsigned int	m_uiModelIndex;
	CVector3		m_vecLookAt;
	int				m_iHandModel;
	int				m_iHand;
	int				m_iMoney;

	OnFootSync()
		: m_vecPosition()
		, m_vecRotation()
		, m_vecDirection()
		, m_vecVelocity()
		, m_fHealth(PLAYER_DEFAULT_HEALTH)
		, m_dwSelectedWeapon(1)
		, m_iSelectedWeaponBullet(0)
		, m_bControlState(0)
		, m_bAiming(false)
		, m_bShooting(false)
		, m_bCrouching(false)
		, m_uiModelIndex(0)
		, m_vecLookAt()
		, m_iHandModel(0)
		, m_iHand(0)
		, m_iMoney(50000)
	{
	}

	OnFootSync& operator=(const OnFootSync &rhs)
	{
		m_vecPosition = rhs.m_vecPosition;
		m_vecRotation = rhs.m_vecRotation;
		m_vecDirection = rhs.m_vecDirection;
		m_vecVelocity = rhs.m_vecVelocity;
		m_fHealth = rhs.m_fHealth;
		m_dwSelectedWeapon = rhs.m_dwSelectedWeapon;
		m_iSelectedWeaponBullet = rhs.m_iSelectedWeaponBullet;
		m_bControlState = rhs.m_bControlState;
		m_bAiming = rhs.m_bAiming;
		m_bShooting = rhs.m_bShooting;
		m_bCrouching = rhs.m_bCrouching;
		m_uiModelIndex = rhs.m_uiModelIndex;
		m_vecLookAt = rhs.m_vecLookAt;
		m_iHandModel = rhs.m_iHandModel;
		m_iHand = rhs.m_iHand;
		m_iMoney = rhs.m_iMoney;
		return *this;
	}
};

const size_t MAX_LICENSE_PLATE_LENGTH = 6;

struct InVehicleSync
{
	CVector3		m_vecPosition;
	CVector3		m_vecRotation;
	CVector3		m_vecVelocity;
	float			m_fDirtLevel;
	int				m_iTuningTable;
	bool			m_bHornState;
	bool			m_bSirenState;
	bool			m_bBeaconLightState;
	bool			m_bEngineState;
	float			m_fFuel;
	float			m_fTurnSpeed;
	float			m_fEngineDamage;
	char			m_szPlateText[MAX_LICENSE_PLATE_LENGTH + 1];
	int				m_model;
	CColor			m_primaryColour;
	CColor			m_secondaryColour;
	bool			m_bPower;
	bool			m_bBrake;
	BYTE			m_bWheelModels[3];
	bool			m_bHandbrake;
	bool			m_bLightState;
	bool			m_bPartState_Hood;
	bool			m_bPartState_Trunk;
	bool			m_bLeftIndicator;
	bool			m_bRightIndicator;
	bool			m_bTaxiLight;
};

struct InPassengerSync
{
	float			m_fHealth;
	DWORD			m_dwSelectedWeapon;
	int				m_iSelectedWeaponBullet;
};

enum ePlayerDamageSource
{
	PLAYER_DAMAGE_SOURCE_UNKNOWN = 0,
	PLAYER_DAMAGE_SOURCE_FIREARM,
	PLAYER_DAMAGE_SOURCE_GENERIC,
	PLAYER_DAMAGE_SOURCE_VEHICLE_IMPACT
};

struct PlayerDamageEvent
{
	float			m_fOldHealth;
	float			m_fNewHealth;
	EntityId		m_attackerId;
	DWORD			m_dwWeapon;
	int				m_iWeaponBullet;
	BYTE			m_byteDamageSource;

	PlayerDamageEvent()
		: m_fOldHealth(PLAYER_DEFAULT_HEALTH)
		, m_fNewHealth(PLAYER_DEFAULT_HEALTH)
		, m_attackerId(INVALID_ENTITY_ID)
		, m_dwWeapon(0)
		, m_iWeaponBullet(0)
		, m_byteDamageSource(PLAYER_DAMAGE_SOURCE_UNKNOWN)
	{
	}
};

struct PlayerHitEvent
{
	EntityId		m_targetId;
	DWORD			m_dwWeapon;
	int				m_iWeaponBullet;
	CVector3		m_vecShooterPosition;
	CVector3		m_vecLookAt;
	unsigned long	m_ulShotTime;

	PlayerHitEvent()
		: m_targetId(INVALID_ENTITY_ID)
		, m_dwWeapon(0)
		, m_iWeaponBullet(0)
		, m_vecShooterPosition()
		, m_vecLookAt()
		, m_ulShotTime(0)
	{
	}
};

static inline void SerializePlayerDamageEvent( RakNet::BitStream * pBitStream, const PlayerDamageEvent &damageEvent )
{
	pBitStream->Write( damageEvent.m_fOldHealth );
	pBitStream->Write( damageEvent.m_fNewHealth );
	pBitStream->Write( damageEvent.m_attackerId );
	pBitStream->Write( damageEvent.m_dwWeapon );
	pBitStream->Write( damageEvent.m_iWeaponBullet );
	pBitStream->Write( damageEvent.m_byteDamageSource );
}

static inline bool DeserializePlayerDamageEvent( RakNet::BitStream * pBitStream, PlayerDamageEvent * pDamageEvent )
{
	if( !pBitStream || !pDamageEvent )
		return false;

	if( pBitStream->GetNumberOfUnreadBits() < ((sizeof(float) * 2) + sizeof(EntityId) + sizeof(DWORD) + sizeof(int) + sizeof(BYTE)) * 8 )
		return false;

	return pBitStream->Read( pDamageEvent->m_fOldHealth ) &&
		pBitStream->Read( pDamageEvent->m_fNewHealth ) &&
		pBitStream->Read( pDamageEvent->m_attackerId ) &&
		pBitStream->Read( pDamageEvent->m_dwWeapon ) &&
		pBitStream->Read( pDamageEvent->m_iWeaponBullet ) &&
		pBitStream->Read( pDamageEvent->m_byteDamageSource );
}

static inline void SerializePlayerHitEvent( RakNet::BitStream * pBitStream, const PlayerHitEvent &hitEvent )
{
	pBitStream->Write( hitEvent.m_targetId );
	pBitStream->Write( hitEvent.m_dwWeapon );
	pBitStream->Write( hitEvent.m_iWeaponBullet );
	hitEvent.m_vecShooterPosition.Serialize( pBitStream );
	hitEvent.m_vecLookAt.Serialize( pBitStream );
	pBitStream->Write( hitEvent.m_ulShotTime );
}

static inline bool DeserializePlayerHitEvent( RakNet::BitStream * pBitStream, PlayerHitEvent * pHitEvent )
{
	if( !pBitStream || !pHitEvent )
		return false;

	if( pBitStream->GetNumberOfUnreadBits() < (sizeof(EntityId) + sizeof(DWORD) + sizeof(int) + (sizeof(float) * 6) + sizeof(unsigned long)) * 8 )
		return false;

	if( !pBitStream->Read( pHitEvent->m_targetId ) ||
		!pBitStream->Read( pHitEvent->m_dwWeapon ) ||
		!pBitStream->Read( pHitEvent->m_iWeaponBullet ) )
	{
		return false;
	}

	pHitEvent->m_vecShooterPosition.Deserialize( pBitStream );
	pHitEvent->m_vecLookAt.Deserialize( pBitStream );
	return pBitStream->Read( pHitEvent->m_ulShotTime );
}

struct UnoccupiedVehicleSync
{
	CVector3		m_vecPosition;
	CVector3		m_vecRotation;
};
