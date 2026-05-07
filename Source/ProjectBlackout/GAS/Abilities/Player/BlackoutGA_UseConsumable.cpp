#include "GAS/Abilities/Player/BlackoutGA_UseConsumable.h"

#include "AbilitySystemComponent.h"
#include "Core/BlackoutLog.h"
#include "Data/BOConsumableData.h"
#include "Framework/BlackoutPlayerState.h"
#include "GAS/Attributes/BlackoutBaseAttributeSet.h"
#include "GAS/Attributes/BlackoutPlayerAttributeSet.h"
#include "GAS/BlackoutAbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameplayTags/BlackoutGameplayTags.h"

UBlackoutGA_UseConsumable::UBlackoutGA_UseConsumable()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	ActivationOwnedTags.AddTag(BlackoutGameplayTags::State_UseConsumable);
	ActivationBlockedTags.AddTag(BlackoutGameplayTags::State_Downed);
	ActivationBlockedTags.AddTag(BlackoutGameplayTags::State_Locked);
}

void UBlackoutGA_UseConsumable::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	const UBOConsumableData* ResolvedConsumableData = ResolveConsumableData();
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	ABlackoutPlayerState* BlackoutPlayerState = ActorInfo ? Cast<ABlackoutPlayerState>(ActorInfo->OwnerActor.Get()) : nullptr;

	if (!ResolvedConsumableData || !ResolvedConsumableData->ConsumableTag.IsValid() || !AbilitySystemComponent || !BlackoutPlayerState)
	{
		BO_LOG_GAS(Warning, "소모품 사용 실패: Data, Tag, ASC 또는 PlayerState가 유효하지 않습니다. Data=%s ASC=%s PS=%s",
			*GetNameSafe(ResolvedConsumableData),
			*GetNameSafe(AbilitySystemComponent),
			*GetNameSafe(BlackoutPlayerState));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!IsConsumableCooldownReady(ResolvedConsumableData))
	{
		BO_LOG_GAS(Warning, "소모품 사용 실패: 아직 쿨다운 중입니다. Player=%s Tag=%s",
			*BlackoutPlayerState->GetPlayerName(),
			*ResolvedConsumableData->ConsumableTag.ToString());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (BlackoutPlayerState->GetConsumableCount(ResolvedConsumableData->ConsumableTag) < ConsumeAmount)
	{
		BO_LOG_GAS(Warning, "소모품 사용 실패: 수량이 부족합니다. Player=%s Tag=%s Current=%d Need=%d",
			*BlackoutPlayerState->GetPlayerName(),
			*ResolvedConsumableData->ConsumableTag.ToString(),
			BlackoutPlayerState->GetConsumableCount(ResolvedConsumableData->ConsumableTag),
			ConsumeAmount);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		BO_LOG_GAS(Warning, "소모품 사용 실패: CommitAbility 실패. Player=%s Tag=%s",
			*BlackoutPlayerState->GetPlayerName(),
			*ResolvedConsumableData->ConsumableTag.ToString());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!BlackoutPlayerState->ConsumeConsumable(ResolvedConsumableData->ConsumableTag, ConsumeAmount))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ApplyBuiltInConsumableEffect(ResolvedConsumableData);
	ApplyConfiguredGameplayEffect(ResolvedConsumableData);
	StartConsumableCooldown(ResolvedConsumableData);

	ReceiveConsumableUsed(ResolvedConsumableData);
	BO_LOG_GAS(Log, "소모품 사용 완료: Player=%s Data=%s Tag=%s",
		*BlackoutPlayerState->GetPlayerName(),
		*GetNameSafe(ResolvedConsumableData),
		*ResolvedConsumableData->ConsumableTag.ToString());

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

const UBOConsumableData* UBlackoutGA_UseConsumable::ResolveConsumableData()
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent ? AbilitySystemComponent->FindAbilitySpecFromHandle(CurrentSpecHandle) : nullptr;
	if (AbilitySpec)
	{
		if (const UBOConsumableData* SourceConsumableData = Cast<UBOConsumableData>(AbilitySpec->SourceObject.Get()))
		{
			return SourceConsumableData;
		}
	}

	return ConsumableData;
}

void UBlackoutGA_UseConsumable::ApplyBuiltInConsumableEffect(const UBOConsumableData* UsedConsumableData)
{
	if (!UsedConsumableData)
	{
		return;
	}

	if (UsedConsumableData->ConsumableTag.MatchesTagExact(BlackoutGameplayTags::Item_Consumable_BloodRoot))
	{
		StartBloodRootHealOverTime(UsedConsumableData);
		return;
	}

	if (UsedConsumableData->ConsumableTag.MatchesTagExact(BlackoutGameplayTags::Item_Consumable_GulSerum))
	{
		ApplyGulSerumBuff(UsedConsumableData);
	}
}

void UBlackoutGA_UseConsumable::ApplyConfiguredGameplayEffect(const UBOConsumableData* UsedConsumableData)
{
	if (!UsedConsumableData || !UsedConsumableData->GameplayEffect)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystemComponent)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(UsedConsumableData->GameplayEffect, GetAbilityLevel());
	if (!SpecHandle.IsValid())
	{
		BO_LOG_GAS(Warning, "소모품 GE 생성 실패: Data=%s Effect=%s",
			*GetNameSafe(UsedConsumableData),
			*GetNameSafe(UsedConsumableData->GameplayEffect.Get()));
		return;
	}

	SpecHandle.Data->AddDynamicAssetTag(UsedConsumableData->ConsumableTag);
	SpecHandle.Data->SetSetByCallerMagnitude(BlackoutGameplayTags::Data_Consumable_HealAmount, UsedConsumableData->HealAmount);
	SpecHandle.Data->SetSetByCallerMagnitude(BlackoutGameplayTags::Data_Consumable_Duration, UsedConsumableData->Duration);
	SpecHandle.Data->SetSetByCallerMagnitude(BlackoutGameplayTags::Data_Consumable_Cooldown, UsedConsumableData->Cooldown);

	for (const TPair<FGameplayTag, float>& EffectMagnitude : UsedConsumableData->EffectMagnitudes)
	{
		if (EffectMagnitude.Key.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(EffectMagnitude.Key, EffectMagnitude.Value);
		}
	}

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UBlackoutGA_UseConsumable::StartBloodRootHealOverTime(const UBOConsumableData* UsedConsumableData)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (!UsedConsumableData || !AbilitySystemComponent)
	{
		return;
	}

	const float TotalHealAmount = UsedConsumableData->HealAmount;
	if (TotalHealAmount <= 0.0f)
	{
		BO_LOG_GAS(Warning, "블러드 루트 회복 실패: HealAmount가 0 이하입니다. Data=%s", *GetNameSafe(UsedConsumableData));
		return;
	}

	const float Duration = UsedConsumableData->Duration;
	if (Duration <= 0.0f)
	{
		RemainingBloodRootHealAmount = TotalHealAmount;
		BloodRootHealAmountPerTick = TotalHealAmount;
		HandleBloodRootHealTick();
		return;
	}

	const int32 TickCount = FMath::Max(1, FMath::CeilToInt(Duration / FMath::Max(0.1f, BloodRootHealTickInterval)));
	RemainingBloodRootHealAmount = TotalHealAmount;
	BloodRootHealAmountPerTick = TotalHealAmount / static_cast<float>(TickCount);

	if (UWorld* World = AbilitySystemComponent->GetWorld())
	{
		World->GetTimerManager().ClearTimer(BloodRootHealTimerHandle);
		World->GetTimerManager().SetTimer(
			BloodRootHealTimerHandle,
			this,
			&UBlackoutGA_UseConsumable::HandleBloodRootHealTick,
			FMath::Max(0.1f, BloodRootHealTickInterval),
			true,
			0.0f);
	}
}

void UBlackoutGA_UseConsumable::HandleBloodRootHealTick()
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystemComponent || RemainingBloodRootHealAmount <= 0.0f)
	{
		if (AbilitySystemComponent)
		{
			if (UWorld* World = AbilitySystemComponent->GetWorld())
			{
				World->GetTimerManager().ClearTimer(BloodRootHealTimerHandle);
			}
		}
		return;
	}

	const float CurrentHealth = AbilitySystemComponent->GetNumericAttribute(UBlackoutBaseAttributeSet::GetHealthAttribute());
	const float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UBlackoutBaseAttributeSet::GetMaxHealthAttribute());
	if (MaxHealth <= 0.0f || CurrentHealth >= MaxHealth)
	{
		if (UWorld* World = AbilitySystemComponent->GetWorld())
		{
			World->GetTimerManager().ClearTimer(BloodRootHealTimerHandle);
		}
		RemainingBloodRootHealAmount = 0.0f;
		return;
	}

	const float HealingEffectivenessAttribute = AbilitySystemComponent->GetNumericAttribute(UBlackoutPlayerAttributeSet::GetHealingEffectivenessAttribute());
	const float HealingEffectiveness = HealingEffectivenessAttribute > 0.0f ? HealingEffectivenessAttribute : 1.0f;
	const float RawHealAmount = FMath::Min(BloodRootHealAmountPerTick, RemainingBloodRootHealAmount);
	const float EffectiveHealAmount = FMath::Min(RawHealAmount * HealingEffectiveness, MaxHealth - CurrentHealth);

	if (EffectiveHealAmount > 0.0f)
	{
		AbilitySystemComponent->ApplyModToAttribute(UBlackoutBaseAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, EffectiveHealAmount);
	}

	RemainingBloodRootHealAmount -= RawHealAmount;
	if (RemainingBloodRootHealAmount <= KINDA_SMALL_NUMBER)
	{
		if (UWorld* World = AbilitySystemComponent->GetWorld())
		{
			World->GetTimerManager().ClearTimer(BloodRootHealTimerHandle);
		}
		RemainingBloodRootHealAmount = 0.0f;
	}
}

void UBlackoutGA_UseConsumable::ApplyGulSerumBuff(const UBOConsumableData* UsedConsumableData)
{
	UBlackoutAbilitySystemComponent* BlackoutAbilitySystemComponent = Cast<UBlackoutAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!UsedConsumableData || !BlackoutAbilitySystemComponent)
	{
		return;
	}

	const float StaminaCostMultiplier = GetEffectMagnitudeOrDefault(
		UsedConsumableData,
		BlackoutGameplayTags::Data_Consumable_StaminaCostMultiplier,
		0.5f);
	const float Duration = UsedConsumableData->Duration > 0.0f ? UsedConsumableData->Duration : 60.0f;

	BlackoutAbilitySystemComponent->ApplyTemporaryStaminaCostMultiplier(StaminaCostMultiplier, Duration);
}

float UBlackoutGA_UseConsumable::GetEffectMagnitudeOrDefault(const UBOConsumableData* UsedConsumableData, FGameplayTag MagnitudeTag, float DefaultValue) const
{
	if (!UsedConsumableData || !MagnitudeTag.IsValid())
	{
		return DefaultValue;
	}

	if (const float* FoundMagnitude = UsedConsumableData->EffectMagnitudes.Find(MagnitudeTag))
	{
		return *FoundMagnitude;
	}

	return DefaultValue;
}

bool UBlackoutGA_UseConsumable::IsConsumableCooldownReady(const UBOConsumableData* UsedConsumableData) const
{
	if (!UsedConsumableData || UsedConsumableData->Cooldown <= 0.0f)
	{
		return true;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return true;
	}

	return World->GetTimeSeconds() >= ConsumableCooldownEndTime;
}

void UBlackoutGA_UseConsumable::StartConsumableCooldown(const UBOConsumableData* UsedConsumableData)
{
	if (!UsedConsumableData || UsedConsumableData->Cooldown <= 0.0f)
	{
		return;
	}

	if (const UWorld* World = GetWorld())
	{
		ConsumableCooldownEndTime = World->GetTimeSeconds() + UsedConsumableData->Cooldown;
	}
}
