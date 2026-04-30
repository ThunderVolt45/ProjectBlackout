#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "Interfaces/BlackoutPoolable.h"
#include "BOProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

USTRUCT()
struct FBOProjectileLaunchState
{
	GENERATED_BODY()

	UPROPERTY()
	FVector_NetQuantize10 Location = FVector::ZeroVector;

	UPROPERTY()
	FVector_NetQuantizeNormal Direction = FVector::ForwardVector;

	UPROPERTY()
	float Speed = 0.0f;

	UPROPERTY()
	uint8 LaunchId = 0;
};

UCLASS()
class PROJECTBLACKOUT_API ABOProjectile : public AActor, public IBlackoutPoolableInterface
{
	GENERATED_BODY()
	
public:	
	ABOProjectile();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnSpawnFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;

	virtual void InitFromSpec(const FGameplayEffectSpecHandle& InDamageSpec, float Radius);
	virtual void Launch(const FVector& Direction);

	UFUNCTION(BlueprintCallable, Category = "Blackout|Combat")
	float GetInitialSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Blackout|Combat")
	float GetGravityScale() const;

	UFUNCTION(BlueprintCallable, Category = "Blackout|Combat")
	float GetCollisionRadius() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_LaunchState();

	UFUNCTION()
	void OnRep_IsActive();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ApplyLaunchState();
	void ApplyActiveState();
	void ReturnToPool();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackout|Combat")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackout|Combat")
	TObjectPtr<UProjectileMovementComponent> Movement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackout|Combat")
	float SplashRadius = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_LaunchState)
	FBOProjectileLaunchState ReplicatedLaunchState;

	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
	bool bReplicatedActive = false;

	FGameplayEffectSpecHandle DamageSpec;
};
