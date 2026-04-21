#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTTask_ActivateBossAbility.generated.h"

/**
 * 하위 BT에서 Boss의 Ability(GA)를 트리거하기 위한 Task 노드.
 * Blackboard의 현재 액터를 타겟으로 설정하고, GameplayTag를 이용해 GA를 실행.
 */
UCLASS()
class PROJECTBLACKOUT_API UBTTask_ActivateBossAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ActivateBossAbility();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackout|Ability")
	FGameplayTag AbilityTag;

	/** GA 종료를 기다릴지 여부 */
	UPROPERTY(EditAnywhere, Category = "Blackout|Ability")
	bool bWaitForEnd = true;
};
