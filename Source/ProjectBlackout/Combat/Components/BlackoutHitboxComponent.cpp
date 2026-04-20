#include "Combat/Components/BlackoutHitboxComponent.h"
#include "Interfaces/BlackoutDamageable.h"

UBlackoutHitboxComponent::UBlackoutHitboxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	UBoxComponent::SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	// 트레이스 채널 블록, 오버랩 무시 등 설정...
}

void UBlackoutHitboxComponent::ReceiveDamageSpec(const FGameplayEffectSpecHandle& SpecHandle)
{
	if (AActor* Owner = GetOwner())
	{
		if (IBlackoutDamageable* Damageable = Cast<IBlackoutDamageable>(Owner))
		{
			// 필요한 경우 SpecHandle에 PartTag 및 Multiplier 주입
			Damageable->ReceiveDamageFromHitbox(SpecHandle, AttachedBoneName);
		}
	}
}
