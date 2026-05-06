#include "BlackoutPlayerState.h"
#include "BlackoutAbilitySystemComponent.h"
#include "BlackoutAmmoAttributeSet.h"
#include "Attributes/BlackoutBaseAttributeSet.h"
#include "Data/BOCharacterData.h"
#include "Net/UnrealNetwork.h"
#include "BlackoutLog.h"
#include "BlackoutPlayerAttributeSet.h"

ABlackoutPlayerState::ABlackoutPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UBlackoutAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	BaseAttributeSet = CreateDefaultSubobject<UBlackoutBaseAttributeSet>(TEXT("BaseAttributeSet"));
	PlayerAttributeSet = CreateDefaultSubobject<UBlackoutPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
	AmmoAttributeSet = CreateDefaultSubobject<UBlackoutAmmoAttributeSet>(TEXT("AmmoAttributeSet"));

	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* ABlackoutPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABlackoutPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlackoutPlayerState, SelectedClassTag);
	DOREPLIFETIME(ABlackoutPlayerState, BloodRootCount);
	DOREPLIFETIME(ABlackoutPlayerState, GulSerumCount);
	DOREPLIFETIME(ABlackoutPlayerState, bIsReady);
}

void ABlackoutPlayerState::ApplyBattleTransitionPolicy(EBattleTransitionType TransitionType)
{
	switch (TransitionType)
	{
	case EBattleTransitionType::LobbyToBattle:
		// 전투 시작: 소모품 초기값 유지, 능력 부여는 PlayerCharacter::PossessedBy에서 처리
		BO_LOG_CORE(Log, "ApplyBattleTransitionPolicy: LobbyToBattle for %s", *GetPlayerName());
		break;

	case EBattleTransitionType::CheckpointRest:
		// 체크포인트 휴식: HP 회복 등은 GE로 처리, 여기서는 상태 플래그만
		BO_LOG_CORE(Log, "ApplyBattleTransitionPolicy: CheckpointRest for %s", *GetPlayerName());
		break;

	case EBattleTransitionType::PartyWipeRestart:
		// 전멸 재시작: 소모품 초기값 복구
		SetConsumableCounts(1, 1);
		BO_LOG_CORE(Log, "ApplyBattleTransitionPolicy: PartyWipeRestart for %s", *GetPlayerName());
		break;
	}
}

void ABlackoutPlayerState::SetConsumableCounts(int32 NewBloodRootCount, int32 NewGulSerumCount)
{
	BloodRootCount = FMath::Max(0, NewBloodRootCount);
	GulSerumCount = FMath::Max(0, NewGulSerumCount);

	BroadcastConsumableCounts();
}

void ABlackoutPlayerState::InitializeConsumablesFromCharacterData(const UBOCharacterData* CharacterData)
{
	if (!CharacterData)
	{
		BO_LOG_CORE(Warning, "소모품 초기화 실패: CharacterData가 유효하지 않습니다. Player=%s", *GetPlayerName());
		return;
	}

	SetConsumableCounts(CharacterData->InitialBloodRoot, CharacterData->InitialGulSerum);
	BO_LOG_CORE(Log,
		"소모품 초기화: Player=%s BloodRoot=%d GulSerum=%d",
		*GetPlayerName(),
		BloodRootCount,
		GulSerumCount);
}

void ABlackoutPlayerState::OnRep_BloodRootCount()
{
	BroadcastConsumableCounts();
}

void ABlackoutPlayerState::OnRep_GulSerumCount()
{
	BroadcastConsumableCounts();
}

void ABlackoutPlayerState::BroadcastConsumableCounts()
{
	OnConsumableCountsChanged.Broadcast(BloodRootCount, GulSerumCount);
}
