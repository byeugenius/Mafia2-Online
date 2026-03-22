/*************************************************************
*
* Solution   : Mafia 2 Multiplayer
* Project    : Server
* File       : CNetworkPlayer.h
* Developers : AaronLad <aaron@m2-multiplayer.com>
*
***************************************************************/

#pragma once

#include "CSync.h"

#include "Interfaces/CMultiplayerVehicle.h"
#include "Interfaces/CMultiplayerPlayer.h"

class CNetworkVehicle;

#include "CNetworkVehicle.h"

class CNetworkPlayer : public CMultiplayerPlayer
{

private:

	EntityId													m_playerId;
	String														m_strNick;
	unsigned int												m_uiColour;
	String														m_strSerial;
	String														m_strIP;
	bool														m_bIsDead;
	ePlayerState												m_State;

	OnFootSync													m_onFootSync;
	InVehicleSync												m_inVehicleSync;
	InPassengerSync												m_passengerSync;
	float														m_fHealth;
	EntityId													m_lastDamageAttackerId;
	DWORD														m_dwLastDamageWeapon;
	unsigned long												m_ulLastDamageTime;
	struct sPendingShotHit
	{
		EntityId												m_shooterId;
		DWORD													m_dwWeapon;
		int														m_iWeaponBullet;
		CVector3												m_vecShooterPosition;
		CVector3												m_vecLookAt;
		unsigned long											m_ulReceivedAt;

		sPendingShotHit()
			: m_shooterId( INVALID_ENTITY_ID )
			, m_dwWeapon( 0 )
			, m_iWeaponBullet( 0 )
			, m_vecShooterPosition()
			, m_vecLookAt()
			, m_ulReceivedAt( 0 )
		{
		}
	};
	std::list< sPendingShotHit >								m_pendingShotHits;

	bool														m_bAiming;
	bool														m_bCrouching;
	bool														m_bShooting;

	CNetworkVehicle												* m_pVehicle;
	EntityId													m_iSeat;

	std::list< CNetworkVehicle* >								m_syncingVehicles;
	unsigned long												m_ulLastPingTime;

	void							SyncAuthoritativeHealth		( void );
	void							CallHealthChangeEvent		( float fNewHealth, float fOldHealth );
	void							PrunePendingShotHits		( unsigned long ulCurrentTime );
	void							ClearPendingShotHits		( void );
	void							QueuePendingShotHit			( const sPendingShotHit &pendingShotHit );
	bool							ConsumePendingShotHit		( EntityId preferredAttackerId, sPendingShotHit * pPendingShotHit );
	bool							ApplyFallbackSyncDamage		( float fIncomingHealth, BYTE byteDamageSource );

public:

									CNetworkPlayer				( void );
									~CNetworkPlayer				( void );

	void							SetId						( EntityId playerId );
	EntityId						GetId						( void ) { return m_playerId; }

	void							SetNick						( const char * szNick ) { m_strNick.Set( szNick ); }
	void							ChangeNick					( const char * szNick );
	void							ChangeNickPlayer			( const char * szNick );
	const char						* GetNick					( void ) { return m_strNick.Get(); }

	void							SetColour					( unsigned int uiColour );
	unsigned int					GetColour					( void ) { return m_uiColour; }

	void							SetSerial					( const char * szSerial ) { m_strSerial.Set( szSerial ); }
	const char						* GetSerial					( void ) { return m_strSerial.Get(); }

	void							SetIP						( const char * szIP ) { m_strIP.Set( szIP ); }
	const char						* GetIP						( void ) { return m_strIP.Get(); }

	unsigned short					GetPing						( void );

	void							SetDead						( bool bDead ) { m_bIsDead = bDead; }
	bool							IsDead						( void ) { return m_bIsDead; }

	void							SetState					( ePlayerState state ) { m_State = state; }
	ePlayerState					GetState					( void ) { return m_State; }

	void							SetModel					( unsigned int iModel );
	unsigned int					GetModel					( void );

	void							SetHealth					( float fHealth );
	float							GetHealth					( void );
	bool							HandleShotHitEvent			( const PlayerHitEvent &hitEvent );
	bool							HandleDamageEvent			( const PlayerDamageEvent &damageEvent );
	EntityId						GetLastDamageAttacker		( void );

	void							GiveWeapon					( int iWeapon, int iAmmo );
	void							RemoveWeapon				( int iWeapon, int iAmmo );
	int								GetWeapon					( void );
	int								GetWeaponBullet				(void);

	void							SetPosition					( CVector3 vecPosition );
	void							GetPosition					( CVector3 * vecPosition );

	void							SetRotation					( CVector3 vecRotation );
	void							GetRotation					( CVector3 * vecRotation );

	void							AddForPlayer				( EntityId playerId );
	void							AddForWorld					( void );

	void							RemoveForPlayer				( EntityId playerId );
	void							RemoveForWorld				( void );

	void							KillForPlayer				( EntityId playerId );
	void							KillForWorld				( void );

	void							SpawnForPlayer				( EntityId playerId );
	void							SpawnForWorld				( void );

	void							StoreOnFootSync				( const OnFootSync &onFootSync );
	void							StoreInVehicleSync			( const InVehicleSync &inVehicleSync );
	void							StorePassengerSync			( const InPassengerSync &passengerSync );

	void							SendOnFootSync				( void );
	void							SendInVehicleSync			( void );
	void							SendPassengerSync			( void );

	void							Pulse						( void );

	void							Kick						( void );
	void							Ping						( void );

	void							SetVehicle					( CNetworkVehicle * pVehicle ) { m_pVehicle = pVehicle; }
	CNetworkVehicle					* GetVehicle(void)			{ return m_pVehicle; }
	bool							IsInVehicle					( void ) { return (m_pVehicle != NULL); }

	void							SetSeat						( EntityId iSeat ) { m_iSeat = iSeat; }
	EntityId						GetSeat						( void ) { return m_iSeat; }

	void							HandleVehicleEnter			( EntityId vehicleId, EntityId iSeat );
	void							HandleVehicleExit			( EntityId vehicleId, EntityId iSeat, bool bQuickly = false );
	void							HandleVehicleEnterDone		( void );

	void							StartSyncVehicle			( CNetworkVehicle * pNetworkVehicle );
	void							StopSyncVehicle				( CNetworkVehicle * pNetworkVehicle );
	bool							IsSyncingVehicle			( CNetworkVehicle * pNetworkVehicle );

	void							HandlePlayerQuit			( void );

	bool							PutInVehicle				( CNetworkVehicle * pVehicle, EntityId seatId );
	void							RemoveFromVehicle			( void );

	void							GiveMoney					( int iMoney );
	void							TakeMoney					( int iMoney );
	int								GetMoney					( void );

	void							SetAnimStyle				(const char *directory, const char *style);
	void							SetHandModel				(int iHand, int iModel);

};
