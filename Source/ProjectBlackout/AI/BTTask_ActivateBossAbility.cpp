#include "AI/BTTask_ActivateBossAbility.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GAS/BlackoutAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UBTTask_ActivateBossAbility::UBTTask_ActivateBossAbility()
{
	NodeName = "Activate Boss Ability";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ActivateBossAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn() || !AbilityTag.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AIController->GetPawn());
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	// Ability 실행 로직
	bool bActivated = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag));

	if (bActivated)
	{
		return bWaitForEnd ? EBTNodeResult::InProgress : EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

FString UBTTask_ActivateBossAbility::GetStaticDescription() const
{
	return FString::Printf(TEXT("Activate GA: %s"), *AbilityTag.ToString());
}
