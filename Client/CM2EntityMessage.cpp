/*************************************************************
*
* Solution   : Mafia 2 Multiplayer
* Project    : Client
* File       : CM2EntityMessage.cpp
* Developers : AaronLad <aaron@m2-multiplayer.com>
*
***************************************************************/

#include "BaseInc.h"

#include "CCore.h"

#include "CM2EntityMessage.h"
#include "CM2Enums.h"

#include "engine/CM2Entity.h"
#include "engine/CM2Ped.h"

#include "CNetworkPlayer.h"
#include "CRemotePlayer.h"
#include "CLocalPlayer.h"
#include "CPlayerManager.h"

#include "SharedUtility.h"
#include "CLogFile.h"

bool CM2EntityMessage::HandleEntityEvent( M2EntityMessage * pMessage )
{
	if( !pMessage )
		return false;

	if( !CCore::Instance()->GetPlayerManager() || !CCore::Instance()->GetPlayerManager()->GetLocalPlayer() )
		return false;

	CLocalPlayer * pLocalPlayer = CCore::Instance()->GetPlayerManager()->GetLocalPlayer();
	CM2Ped * pLocalPed = pLocalPlayer->GetPlayerPed();
	DWORD dwLocalGuid = pLocalPed->GetGUID();

	if( pMessage->m_dwMessage == M2Enums::ON_SHOT_HIT_ENTITY )
	{
		bool bLocalShooter = (pMessage->m_dwSenderGUID == dwLocalGuid);
		if( !bLocalShooter && pMessage->m_dwReceiveGUID == dwLocalGuid && pMessage->m_dwSenderGUID == 0 )
			bLocalShooter = true;

		if( bLocalShooter )
		{
			DWORD dwCandidateGuids[2] = { pMessage->m_dwReceiveGUID, pMessage->M2DamageMessage__dwEnemyGUID };
			EntityId targetId = INVALID_ENTITY_ID;
			DWORD dwTargetGuid = 0;

			for( int i = 0; i < 2; ++i )
			{
				if( dwCandidateGuids[i] == 0 || dwCandidateGuids[i] == dwLocalGuid )
					continue;

				EntityId candidateId = CCore::Instance()->GetPlayerManager()->GetIdFromGameGUID( dwCandidateGuids[i] );
				if( candidateId != INVALID_ENTITY_ID )
				{
					targetId = candidateId;
					dwTargetGuid = dwCandidateGuids[i];
					break;
				}
			}

			CLogFile::Printf( "[damage-debug][client-shot-event] senderGuid=%u receiveGuid=%u enemyGuid=%u resolvedTargetGuid=%u target=%u",
				pMessage->m_dwSenderGUID,
				pMessage->m_dwReceiveGUID,
				pMessage->M2DamageMessage__dwEnemyGUID,
				dwTargetGuid,
				targetId );

			if( targetId != INVALID_ENTITY_ID )
				pLocalPlayer->ReportShotHit( targetId );
		}

		return true;
	}

	if( pMessage->m_dwReceiveGUID == dwLocalGuid )
	{
		switch( pMessage->m_dwMessage )
		{

		case M2Enums::ON_SHOOT:
			{
				int selectedWeapon = CCore::Instance()->GetPlayerManager()->GetLocalPlayer()->GetSelectedWeapon();
				if (selectedWeapon != 0 && selectedWeapon != 1)
				{
					CCore::Instance()->GetPlayerManager()->GetLocalPlayer()->SetTimeStartShooting( SharedUtility::GetTime() );
					CCore::Instance()->GetPlayerManager()->GetLocalPlayer()->SetShooting( true );
				}
				break;
			}

		case M2Enums::ON_AIM_ENTER:
			{
				int selectedWeapon = CCore::Instance()->GetPlayerManager()->GetLocalPlayer()->GetSelectedWeapon();
				if (selectedWeapon != 0 && selectedWeapon != 1)
				{
					CCore::Instance()->GetPlayerManager()->GetLocalPlayer()->SetAiming(true);
				}
				break;
			}

		case M2Enums::ON_AIM_LEAVE:
			{
				int selectedWeapon = CCore::Instance()->GetPlayerManager()->GetLocalPlayer()->GetSelectedWeapon();
				if (selectedWeapon != 0 && selectedWeapon != 1)
				{
					CCore::Instance()->GetPlayerManager()->GetLocalPlayer()->SetAiming(false);
				}
				break;
			}

		case M2Enums::ON_VEHICLE_START_ENTER:
			{
				pLocalPlayer->OnEnterVehicle();
				break;
			}

		case M2Enums::ON_VEHICLE_ENTER:
			{
				pLocalPlayer->OnEnterVehicleDone();
				break;
			}

		case M2Enums::ON_VEHICLE_EXIT:
			{
				pLocalPlayer->OnLeaveVehicle();
				break;
			}

		case M2Enums::ON_DAMAGE:
			{
				EntityId attackerId = INVALID_ENTITY_ID;

				if( pMessage->M2DamageMessage__dwEnemyGUID != 0 )
					attackerId = CCore::Instance()->GetPlayerManager()->GetIdFromGameGUID( pMessage->M2DamageMessage__dwEnemyGUID );

				if( attackerId == INVALID_ENTITY_ID && pMessage->m_dwSenderGUID != 0 )
					attackerId = CCore::Instance()->GetPlayerManager()->GetIdFromGameGUID( pMessage->m_dwSenderGUID );

				DWORD dwWeapon = 0;
				int iWeaponBullet = 0;
				BYTE byteDamageSource = PLAYER_DAMAGE_SOURCE_GENERIC;

				if( attackerId != INVALID_ENTITY_ID )
				{
					CNetworkPlayer * pAttacker = CCore::Instance()->GetPlayerManager()->Get( attackerId );

					if( pAttacker )
					{
						dwWeapon = pAttacker->GetSelectedWeapon();
						iWeaponBullet = pAttacker->GetSelectedWeaponBullet();

						if( dwWeapon > 1 )
							byteDamageSource = PLAYER_DAMAGE_SOURCE_FIREARM;
					}
				}

				CLogFile::Printf( "[damage-debug][client-context] attacker=%u weapon=%u bullet=%d source=%d enemyGuid=%u senderGuid=%u",
					attackerId, dwWeapon, iWeaponBullet, (int)byteDamageSource, pMessage->M2DamageMessage__dwEnemyGUID, pMessage->m_dwSenderGUID );

				pLocalPlayer->RegisterDamageContext( attackerId, dwWeapon, iWeaponBullet, byteDamageSource );
				break;
			}

		case M2Enums::ON_DEATH:
			{
				CNetworkPlayer * pKiller = CCore::Instance()->GetPlayerManager()->GetFromGameGUID( pMessage->M2HumanDeathMessage__dwKillerGUID );
				pLocalPlayer->OnDeath( pKiller );
				break;
			}
		}
		return true;
	}

	return false;
}
