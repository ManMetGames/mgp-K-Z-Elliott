// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "PossessEndTrigger.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Target Required"))
class MGP_2526_API UPossessEndTrigger : public UInputTrigger
{
	GENERATED_BODY()
protected:
	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override;
};
