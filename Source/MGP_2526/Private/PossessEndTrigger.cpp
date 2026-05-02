// Fill out your copyright notice in the Description page of Project Settings.


#include "PossessEndTrigger.h"
#include "MGP_2526Character.h"
#include "EnhancedPlayerInput.h"
#include "GameFramework/PlayerController.h"

ETriggerState UPossessEndTrigger::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime)
{
	if (!PlayerInput) return ETriggerState::None; //Denies trigger if there isn't a PlayerInput system
    if (APlayerController* Controller = Cast<APlayerController>(PlayerInput->GetOuter())) //Finds the PlayerController by getting the parent of the PlayerInput system
    {
        if (AMGP_2526Character* Char = Cast<AMGP_2526Character>(Controller->GetPawn())) //Gets the actual Character from the Controller
        {
            if (Char->IsAiming && Char->HasTarget(Controller)) //If a player has a target and is aiming (thus, is aiming at a target) keep allowing possession
            {
                return ETriggerState::Ongoing;
            }
        }
    }

    return ETriggerState::None; //End possession when no longer aiming at a target
}