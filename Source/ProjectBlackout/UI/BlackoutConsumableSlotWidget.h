#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlackoutConsumableSlotWidget.generated.h"

class UTextBlock;

UCLASS(BlueprintType, Blueprintable)
class PROJECTBLACKOUT_API UBlackoutConsumableSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Blackout|HUD|Consumable")
	void SetConsumableCount(int32 NewCount);

	UFUNCTION(BlueprintPure, Category = "Blackout|HUD|Consumable")
	int32 GetConsumableCount() const { return ConsumableCount; }

	UFUNCTION(BlueprintPure, Category = "Blackout|HUD|Consumable")
	bool IsConsumableAvailable() const { return bIsConsumableAvailable; }

protected:
	virtual void NativePreConstruct() override;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blackout|HUD|Consumable")
	int32 ConsumableCount = 0;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blackout|HUD|Consumable")
	bool bIsConsumableAvailable = false;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Blackout|HUD|Consumable")
	TObjectPtr<UTextBlock> CountText;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Consumable Count Changed"), Category = "Blackout|HUD|Consumable")
	void ReceiveConsumableCountChanged(int32 NewCount);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Consumable Availability Changed"), Category = "Blackout|HUD|Consumable")
	void ReceiveConsumableAvailabilityChanged(bool bNewIsAvailable);
};
