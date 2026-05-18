// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BORavagerData.generated.h"

/**
 * 
 */
class ABOEnemyProjectile; 

USTRUCT()
struct FBossMeleeSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> Effect;
	
	UPROPERTY(EditAnywhere)
	TArray<FName> HitboxComponentNames;
	
	UPROPERTY(EditAnywhere)
	float DamageMagnitude = 10.f;
	
	bool IsValid() const { return Effect && DamageMagnitude > 0.f && HitboxComponentNames.Num() > 0; }
};

USTRUCT()
struct FProjectileSpawnParams
{
	GENERATED_BODY()
	 
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> Effect;
	
	UPROPERTY(EditAnywhere)
	float AbilityLevel = 1.f;
	
	UPROPERTY(EditAnywhere)
	float LifeSpan = 5.f;
	
	UPROPERTY(EditAnywhere)
	float Speed = 1300.f;
	
	UPROPERTY(EditAnywhere)
	float DamageMagnitude = 10.f;
};

USTRUCT()
struct FBossProjectileSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABOEnemyProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere)
	FName SocketName = NAME_None;
	
	UPROPERTY(EditAnywhere)
	FProjectileSpawnParams ProjectileSpawnParams;
	
	bool IsValid() const;
};

UCLASS()
class PROJECTBLACKOUT_API UBORavagerData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Blackout|Ability")
	TSubclassOf<UGameplayAbility> GrantedAbility;
	
	UPROPERTY(EditAnywhere, Category = "Blackout|Anim",meta = (Categories = "Ability"))
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> Montages;
	
	UPROPERTY(EditAnywhere, Category = "Blackout|Melee")
	FBossMeleeSettings MeleeSettings;
	
	UPROPERTY(EditAnywhere, Category = "Blackout|Projectile")
	FBossProjectileSettings ProjectileSettings;
	
};
