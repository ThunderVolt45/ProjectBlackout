#include "Combat/Weapons/BOProjectile.h"
#include "Combat/Components/BlackoutHitboxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/BlackoutDamageable.h"
#include "Net/UnrealNetwork.h"
#include "Pool/BlackoutPoolSubsystem.h"

ABOProjectile::ABOProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(false);
	SetNetDormancy(DORM_Awake);
	SetNetUpdateFrequency(60.f);
	SetMinNetUpdateFrequency(30.f);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
	Collision->OnComponentHit.AddDynamic(this, &ABOProjectile::OnHit);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->bAutoActivate = false;
}

void ABOProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABOProjectile, ReplicatedLaunchState);
	DOREPLIFETIME(ABOProjectile, bReplicatedActive);
}

void ABOProjectile::OnSpawnFromPool_Implementation()
{
	if (HasAuthority())
	{
		SetNetDormancy(DORM_Awake);
		FlushNetDormancy();
		bReplicatedActive = true;
	}

	ApplyActiveState();
	Movement->Velocity = FVector::ZeroVector;

	if (HasAuthority())
	{
		ForceNetUpdate();
	}
}

void ABOProjectile::BeginPlay()
{
	Super::BeginPlay();

	ApplyActiveState();
}

void ABOProjectile::OnReturnToPool_Implementation()
{
	SetActorHiddenInGame(true);
	if (HasAuthority())
	{
		bReplicatedActive = false;
	}

	ApplyActiveState();
	DamageSpec = FGameplayEffectSpecHandle();

	if (HasAuthority())
	{
		ForceNetUpdate();
		SetNetDormancy(DORM_DormantAll);
	}
}

void ABOProjectile::InitFromSpec(const FGameplayEffectSpecHandle& InDamageSpec, float Radius)
{
	DamageSpec = InDamageSpec;
	SplashRadius = Radius;
}

void ABOProjectile::Launch(const FVector& Direction)
{
	if (!Movement)
	{
		return;
	}

	Movement->Velocity = Direction.GetSafeNormal() * Movement->InitialSpeed;
	Movement->SetActive(true, true);

	if (HasAuthority())
	{
		SetNetDormancy(DORM_Awake);
		FlushNetDormancy();
		bReplicatedActive = true;
		ApplyActiveState();
		ReplicatedLaunchState.Location = GetActorLocation();
		ReplicatedLaunchState.Direction = Direction.GetSafeNormal();
		ReplicatedLaunchState.Speed = Movement->InitialSpeed;
		++ReplicatedLaunchState.LaunchId;
		ForceNetUpdate();
	}
}

float ABOProjectile::GetInitialSpeed() const
{
	return Movement ? Movement->InitialSpeed : 0.0f;
}

float ABOProjectile::GetGravityScale() const
{
	return Movement ? Movement->ProjectileGravityScale : 1.0f;
}

float ABOProjectile::GetCollisionRadius() const
{
	return Collision ? Collision->GetScaledSphereRadius() : 0.0f;
}

void ABOProjectile::OnRep_LaunchState()
{
	ApplyLaunchState();
}

void ABOProjectile::OnRep_IsActive()
{
	ApplyActiveState();
}

void ABOProjectile::ApplyLaunchState()
{
	if (HasAuthority() || !Movement || ReplicatedLaunchState.Speed <= 0.0f)
	{
		return;
	}

	SetActorLocationAndRotation(
		ReplicatedLaunchState.Location,
		ReplicatedLaunchState.Direction.Rotation(),
		false,
		nullptr,
		ETeleportType::TeleportPhysics);
	Movement->Velocity = FVector(ReplicatedLaunchState.Direction) * ReplicatedLaunchState.Speed;
	Movement->SetActive(true, true);
}

void ABOProjectile::ApplyActiveState()
{
	SetActorHiddenInGame(!bReplicatedActive);

	if (Collision)
	{
		Collision->SetCollisionEnabled(bReplicatedActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}

	if (!bReplicatedActive && Movement)
	{
		Movement->Deactivate();
		Movement->Velocity = FVector::ZeroVector;
	}
}

void ABOProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority() && DamageSpec.IsValid())
	{
		if (UBlackoutHitboxComponent* HitboxComponent = Cast<UBlackoutHitboxComponent>(OtherComp))
		{
			HitboxComponent->ReceiveDamageSpec(DamageSpec);
		}
		else if (IBlackoutDamageable* Damageable = Cast<IBlackoutDamageable>(OtherActor))
		{
			Damageable->ReceiveDamageFromHitbox(DamageSpec, Hit.BoneName);
		}
	}

	ReturnToPool();
}

void ABOProjectile::ReturnToPool()
{
	if (!HasAuthority())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (UBlackoutPoolSubsystem* Pool = World->GetSubsystem<UBlackoutPoolSubsystem>())
		{
			Pool->ReturnToPool(this);
		}
	}
}
