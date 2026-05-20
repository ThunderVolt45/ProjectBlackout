#include "BlackoutCheckpoint.h"

#include "BlackoutPlayerController.h"
#include "GameFramework/Pawn.h"

ABlackoutCheckpoint::ABlackoutCheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	InteractionPrompt = FText::FromString(TEXT("캐릭터 변경"));
}

bool ABlackoutCheckpoint::CanInteract_Implementation(AActor* Interactor) const
{
	return Interactor != nullptr;
}

void ABlackoutCheckpoint::OnInteract_Implementation(AActor* Interactor)
{
	if (!HasAuthority())
	{
		return;
	}
	
	APawn* InteractorPawn = Cast<APawn>(Interactor);
	if (!InteractorPawn)
	{
		return;
	}
	
	// 캐릭터 변경 UI 호출 
	if (ABlackoutPlayerController* PC = Cast<ABlackoutPlayerController>(InteractorPawn->GetController()))
	{
		PC->Client_OpenClassSelectUI();
	}
}

FText ABlackoutCheckpoint::GetInteractionPrompt_Implementation() const
{
	return InteractionPrompt;
}
