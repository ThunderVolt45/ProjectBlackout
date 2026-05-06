#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlackoutConsumableSlotsWidget.generated.h"

class UBlackoutConsumableSlotWidget;

UCLASS(BlueprintType, Blueprintable)
class PROJECTBLACKOUT_API UBlackoutConsumableSlotsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Blackout|HUD|Consumable")
	void SetConsumableCounts(int32 NewBloodRootCount, int32 NewGulSerumCount);

	UFUNCTION(BlueprintPure, Category = "Blackout|HUD|Consumable")
	int32 GetBloodRootCount() const { return BloodRootCount; }

	UFUNCTION(BlueprintPure, Category = "Blackout|HUD|Consumable")
	int32 GetGulSerumCount() const { return GulSerumCount; }

protected:
	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Blackout|HUD|Consumable")
	TObjectPtr<UBlackoutConsumableSlotWidget> BloodRootSlotWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Blackout|HUD|Consumable")
	TObjectPtr<UBlackoutConsumableSlotWidget> GulSerumSlotWidget;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blackout|HUD|Consumable")
	int32 BloodRootCount = 0;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blackout|HUD|Consumable")
	int32 GulSerumCount = 0;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Consumable Counts Changed"), Category = "Blackout|HUD|Consumable")
	void ReceiveConsumableCountsChanged(int32 NewBloodRootCount, int32 NewGulSerumCount);
};
