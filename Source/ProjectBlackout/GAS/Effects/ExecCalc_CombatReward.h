#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_CombatReward.generated.h"

class UAbilitySystemComponent;

/**
 * 전투 보상 실행 계산기 (TDD v5 §5.1)
 * 클래스 및 킬 조건에 따라 월드 드롭 아이템을 스폰합니다.
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTBLACKOUT_API UExecCalc_CombatReward : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_CombatReward();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	/** 서버 사망 확정 직후 마지막 타격 Spec을 기준으로 보상 GE를 타겟에게 적용합니다. */
	static bool ApplyConfiguredRewardEffect(
		const FGameplayEffectSpecHandle& DamageSpecHandle,
		UAbilitySystemComponent* TargetASC);

protected:
	/** 스폰할 통합 드롭 아이템 블루프린트 클래스입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackout|Reward")
	TSubclassOf<class ABlackoutDropItem> DropItemClass;

private:
	static bool TrySpawnRewardDropInternal(
		const FGameplayEffectSpec& RewardSpec,
		UAbilitySystemComponent* SourceASC,
		UAbilitySystemComponent* TargetASC,
		TSubclassOf<class ABlackoutDropItem> InDropItemClass);
};
