#include "AI/BSTTask_RunSubBehaviorTree.h"
#include "StateTreeExecutionContext.h"
#include "AI/BlackoutBossAIController.h"

EStateTreeRunStatus FBSTTask_RunSubBehaviorTree::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.Controller && InstanceData.SubTreeAsset)
	{
		InstanceData.Controller->RunSubBehaviorTree(InstanceData.SubTreeAsset);
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

void FBSTTask_RunSubBehaviorTree::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.Controller)
	{
		InstanceData.Controller->StopSubBehaviorTree();
	}
}
