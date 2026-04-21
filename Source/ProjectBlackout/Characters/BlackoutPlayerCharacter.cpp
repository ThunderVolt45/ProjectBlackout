#include "BlackoutPlayerCharacter.h"
#include "BlackoutAbilitySystemComponent.h"
#include "Data/BOCharacterData.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BlackoutLog.h"

ABlackoutPlayerCharacter::ABlackoutPlayerCharacter()
{
	// TPS 카메라 셋업
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 350.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// TPS: 컨트롤러 회전은 카메라에만 적용, 캐릭터는 이동 방향으로 자동 회전
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ABlackoutPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server: InitAbilityActorInfo
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetPlayerState()))
	{
		AbilitySystemComponent = Cast<UBlackoutAbilitySystemComponent>(
			ASCInterface->GetAbilitySystemComponent());

		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState(), this);

			if (CharacterData)
			{
				AbilitySystemComponent->GiveDefaultAbilities(CharacterData->GrantedAbilities);
				BO_LOG_GAS(Log, "Abilities granted to %s", *GetName());
			}
		}
	}
}

void ABlackoutPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client: InitAbilityActorInfo
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetPlayerState()))
	{
		AbilitySystemComponent = Cast<UBlackoutAbilitySystemComponent>(
			ASCInterface->GetAbilitySystemComponent());

		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->InitAbilityActorInfo(GetPlayerState(), this);
		}
	}
}
