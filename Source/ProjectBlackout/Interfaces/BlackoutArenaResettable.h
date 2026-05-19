#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BlackoutArenaResettable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBlackoutArenaResettableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 보스 아레나(구역)가 자기 상태를 결정적으로 초기화하는 단위로서 구현하는 인터페이스.
 * ABlackoutBattleGameMode 가 파티 전멸 / 체크포인트 복귀 시 CurrentArena->ResetArena() 호출.
 * 파괴물(기둥) 복원은 TDD §8 Fast-Retry 수용기준에 따라 구현자가 ResetArena 내에서 충족
 * (외부 §8 / 레벨 담당 트랙 — 본 인터페이스는 흐름 권위와 아레나 상태를 분리하는 seam).
 */
class PROJECTBLACKOUT_API IBlackoutArenaResettableInterface
{
	GENERATED_BODY()

public:
	/** 아레나를 결정적 초기 상태로 복원. 보스 / 미니언 풀 / 타이머 / 인플라이트 / GAS + (파괴물 = §8 계약). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Blackout|Arena")
	void ResetArena();
	virtual void ResetArena_Implementation() {}

	/** 리셋 결정성 자기보고(스파이크 / 계약 진단용). 파괴물 보유 아레나는 §8 충족 전까지 false 권장. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Blackout|Arena")
	bool IsResetDeterministic() const;
	virtual bool IsResetDeterministic_Implementation() const { return true; }
};
