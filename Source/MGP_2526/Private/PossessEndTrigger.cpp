// Fill out your copyright notice in the Description page of Project Settings.


#include "PossessEndTrigger.h"
#include "MGP_2526Character.h"
#include "EnhancedPlayerInput.h"
#include "GameFramework/PlayerController.h"

ETriggerState UPossessEndTrigger::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime)
{
	if (!PlayerInput) return ETriggerState::None;
    if (APlayerController* PC = Cast<APlayerController>(PlayerInput->GetOuter()))
    {
        if (AMGP_2526Character* Char = Cast<AMGP_2526Character>(PC->GetPawn()))
        {
            if (Char->IsAiming && Char->HasTarget())
            {
                return ETriggerState::Triggered;
            }
        }
    }

    return ETriggerState::None;
}