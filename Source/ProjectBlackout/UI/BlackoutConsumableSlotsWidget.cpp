#include "UI/BlackoutConsumableSlotsWidget.h"

#include "UI/BlackoutConsumableSlotWidget.h"

void UBlackoutConsumableSlotsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetConsumableCounts(BloodRootCount, GulSerumCount);
}

void UBlackoutConsumableSlotsWidget::SetConsumableCounts(int32 NewBloodRootCount, int32 NewGulSerumCount)
{
	BloodRootCount = FMath::Max(0, NewBloodRootCount);
	GulSerumCount = FMath::Max(0, NewGulSerumCount);

	if (BloodRootSlotWidget)
	{
		BloodRootSlotWidget->SetConsumableCount(BloodRootCount);
	}

	if (GulSerumSlotWidget)
	{
		GulSerumSlotWidget->SetConsumableCount(GulSerumCount);
	}

	ReceiveConsumableCountsChanged(BloodRootCount, GulSerumCount);
}
