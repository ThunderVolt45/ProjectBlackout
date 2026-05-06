#include "UI/BlackoutConsumableSlotWidget.h"

#include "Components/TextBlock.h"

void UBlackoutConsumableSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetConsumableCount(ConsumableCount);
}

void UBlackoutConsumableSlotWidget::SetConsumableCount(int32 NewCount)
{
	const bool bWasAvailable = bIsConsumableAvailable;

	ConsumableCount = FMath::Max(0, NewCount);
	bIsConsumableAvailable = ConsumableCount > 0;

	if (CountText)
	{
		CountText->SetText(FText::AsNumber(ConsumableCount));
	}

	ReceiveConsumableCountChanged(ConsumableCount);

	if (bWasAvailable != bIsConsumableAvailable)
	{
		ReceiveConsumableAvailabilityChanged(bIsConsumableAvailable);
	}
}
