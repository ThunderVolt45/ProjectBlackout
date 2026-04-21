#include "AI/BTService_LineOfSightCheck.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"

UBTService_LineOfSightCheck::UBTService_LineOfSightCheck()
{
	NodeName = "Line of Sight Check";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_LineOfSightCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BlackboardComp)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
	
	bool bHasLoS = false;
	if (TargetActor)
	{
		bHasLoS = AIController->LineOfSightTo(TargetActor);
	}

	BlackboardComp->SetValueAsBool(OutHasLineOfSightKey.SelectedKeyName, bHasLoS);
}
