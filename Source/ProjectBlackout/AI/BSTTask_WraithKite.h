#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "BSTTask_WraithKite.generated.h"

class AAIController;

USTRUCT()
struct PROJECTBLACKOUT_API FBSTTask_WraithKiteInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Context")
	TObjectPtr<AAIController> Controller;
	
	UPROPERTY(EditAnywhere, Category="Context")
	TObjectPtr<APawn> TargetPawn;
	
	/** 사거리 하한 - 이보다 가까우면 후퇴 */
	UPROPERTY(EditAnywhere,Category="Parameter", meta=(ClampMin=0.0f))
	float MinKiteDistance = 1000.0f;
	
	/** 사거리 상한 - 이보다 멀면 접근 */
	UPROPERTY(EditAnywhere, Category="Parameter", meta=(ClampMin=0.0f))
	float MaxKiteDistance = 1500.0f;
	
};

/**
 *  Wraith 사거리 추적 TASK
 *  사거리 안 ( MIN ~ MAX ) 도달 시 Succeeded -> Fire
 *  멀면 MoveToActor, 너무 가까우면 반대방향으로
 */
USTRUCT(meta=(DisplayName = "Wraith Kite", Category="Blackout|Wraith"))
struct PROJECTBLACKOUT_API FBSTTask_WraithKite : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FBSTTask_WraithKiteInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {return FInstanceDataType::StaticStruct(); }
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
