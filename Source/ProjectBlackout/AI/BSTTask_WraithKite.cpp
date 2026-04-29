#include "BSTTask_WraithKite.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

EStateTreeRunStatus FBSTTask_WraithKite::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	if (!Data.Controller || !Data.TargetPawn)
	{
		return EStateTreeRunStatus::Failed;
	}
	if (!Data.Controller->HasAuthority())
	{
		return EStateTreeRunStatus::Failed;
	}
	
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FBSTTask_WraithKite::Tick(
	FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	if (!Data.Controller || !Data.TargetPawn)
	{
		return EStateTreeRunStatus::Failed;
	}
	APawn* Owner = Data.Controller->GetPawn();
	if (!Owner)
	{
		return EStateTreeRunStatus::Failed;
	}
	const FVector OwnerLocation = Owner->GetActorLocation();
	const FVector TargetLocation = Data.TargetPawn->GetActorLocation();
	const FVector ToTarget = TargetLocation - OwnerLocation;
	const float Distance = ToTarget.Size2D();
	
	if (Distance >= Data.MinKiteDistance && Distance <= Data.MaxKiteDistance)
	{
		Data.Controller->StopMovement();
		return EStateTreeRunStatus::Succeeded;
	}
	if (Distance> Data.MaxKiteDistance)
	{
		Data.Controller->MoveToActor(Data.TargetPawn, Data.MaxKiteDistance);
	}else
	{
		const FVector RetreatDirection = -ToTarget.GetSafeNormal2D();
		const FVector RetreatGoal = OwnerLocation + RetreatDirection * (Data.MinKiteDistance - Distance + 200.0f);
		Data.Controller->MoveToLocation(RetreatGoal);
	}
	
	return EStateTreeRunStatus::Running;
}

void FBSTTask_WraithKite::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	if (Data.Controller)
	{
		Data.Controller->StopMovement();
	}
}
