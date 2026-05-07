// Copyright Epic Games, Inc. All Rights Reserved.

#include "MGP_2526Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PossessableObject.h"
#include "MGP_2526.h"

AMGP_2526Character::AMGP_2526Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMGP_2526Character::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add("Player"); //Mark player as player
	Tags.Add("Possessable"); //Mark player as possessable

	PossessedObj = this;
}

void AMGP_2526Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMGP_2526Character::Look);

		// Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMGP_2526Character::StartAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMGP_2526Character::StopAim);

		// Possess
		EnhancedInputComponent->BindAction(PossessAction, ETriggerEvent::Ongoing, this, &AMGP_2526Character::Possess);
		EnhancedInputComponent->BindAction(PossessAction, ETriggerEvent::Completed, this, &AMGP_2526Character::PossessResult);
		EnhancedInputComponent->BindAction(PossessAction, ETriggerEvent::Canceled, this, &AMGP_2526Character::PossessResult);
	}
	else
	{
		UE_LOG(LogMGP_2526, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMGP_2526Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMGP_2526Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMGP_2526Character::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMGP_2526Character::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);

		AActor* ViewTarget = Controller->GetViewTarget();
		if (!ViewTarget) return;

		USpringArmComponent* Arm = ViewTarget->FindComponentByClass<USpringArmComponent>();

		if (Arm)
		{
			FRotator Rot = Arm->GetRelativeRotation();

			Rot.Yaw += Yaw;

			if (PossessedObj != this)
			{
				Rot.Pitch = FMath::Clamp(Rot.Pitch - Pitch, -80.f, 80.f);
			}
			else
			{
				Rot.Pitch = FMath::Clamp(Rot.Pitch + Pitch, -80.f, 80.f);
			}

			Arm->SetRelativeRotation(Rot);
		}
	}
}

void AMGP_2526Character::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AMGP_2526Character::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AMGP_2526Character::Transition(AActor* OldTarget)
{
	APlayerController* PController = Cast<APlayerController>(Controller);

	PController->SetViewTarget(OldTarget);
	PController->SetViewTargetWithBlend(this,0.2);
}

void AMGP_2526Character::StartAim()
{
	UE_LOG(LogTemp, Warning, TEXT("Started aiming"));
	/*Goes first person*/
	IsAiming = true;

	APossessableObject* CharObj = ObjOrPlr(PossessedObj); //Stores what object is being possessed

	USpringArmComponent* ActiveCam;

	if(CharObj)
	{
		ActiveCam = CharObj->CameraBoom;
		CharObj->ObjMesh->SetVisibility(false);
		CharObj->ObjMesh->SetHiddenInGame(true);
		BaseArmLength = ActiveCam->TargetArmLength;
		BaseSocketOffset = ActiveCam->SocketOffset;
		ActiveCam->SocketOffset = FVector(0.f, 0.f, -50.f);
	}
	else
	{
		ActiveCam = this->CameraBoom;
		ActiveCam->SocketOffset = FVector(0.f, 0.f, 70.f);
		BaseArmLength = ActiveCam->TargetArmLength;
		BaseSocketOffset = ActiveCam->SocketOffset;
		GetMesh()->SetVisibility(false);
		GetMesh()->SetHiddenInGame(true);
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
	
	ActiveCam->TargetArmLength = 0.0f;
}

void AMGP_2526Character::StopAim()
{
	UE_LOG(LogTemp, Warning, TEXT("Stopped aiming"));
	/*Return to third person*/
	IsAiming = false;

	APossessableObject* CharObj = ObjOrPlr(PossessedObj); //Stores what object is being possessed

	USpringArmComponent* ActiveCam;

	if (CharObj)
	{
		ActiveCam = CharObj->CameraBoom;
		CharObj->ObjMesh->SetVisibility(true);
		CharObj->ObjMesh->SetHiddenInGame(false);
	}
	else
	{
		ActiveCam = this->CameraBoom;
		GetMesh()->SetVisibility(true);
		GetMesh()->SetHiddenInGame(false);
	}

	ActiveCam->TargetArmLength = BaseArmLength;
	ActiveCam->SocketOffset = BaseSocketOffset;

	//GetMesh()->SetOwnerNoSee(false);
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AMGP_2526Character::Possess()
{
	UE_LOG(LogTemp, Warning, TEXT("Progress: %f"),PossessionProgress);
	PossessionProgress++;
}

bool AMGP_2526Character::HasTarget(APlayerController* PController)
{
	if (!PController) return false; //Denies line trace if Controller isn't passed

	FVector CameraLocation;
	FRotator CameraRotation;

	PController->GetPlayerViewPoint(CameraLocation, CameraRotation); //Sets CameraLocation and CameraRotation based on the player's camera
	FVector Start = CameraLocation; //Start the line at the player's camera location
	FVector End = Start + CameraRotation.Vector() * 10000.f; //Aim the line toward where the player is facing, with a distance of 10000

	FHitResult Hit;
	FCollisionQueryParams FilterList;
	FilterList.AddIgnoredActor(PossessedObj); //Prevents currently possessed object (or player) from being counted as a hit for the linetrace
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, FilterList); //Line trace based on the parameters

	if (!bHit) return false; //A valid target wasn't hit

	AActor* HitActor = Hit.GetActor();

	UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *HitActor->GetName());


	if (!HitActor->Tags.Contains("Possessable")) //If the hit actor isn't possessable
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid target!"))
		return false; //A valid target wasn't hit
	}

	UE_LOG(LogTemp, Warning, TEXT("Valid target!"))
	CurrentTarget = HitActor; //Sets the possession target to the valid target
	return true;
}

APossessableObject* AMGP_2526Character::ObjOrPlr(AActor* Actor)
{
	APossessableObject* ObjActor = Cast<APossessableObject>(Actor); //Stores what object is being possessed
	if (ObjActor)
	{
		return ObjActor;
	}
	return nullptr;
}

void AMGP_2526Character::PossessResult()
{
	if (PossessionProgress > MaxPossession) //Checks if possession progress met the goal or if it was cancelled
	{
		UE_LOG(LogTemp, Warning, TEXT("Succeeded!"));
		StopAim();
		APossessableObject* ObjCandidate = Cast<APossessableObject>(CurrentTarget);
		AMGP_2526Character* PlrCandidate = Cast<AMGP_2526Character>(CurrentTarget);

		APlayerController* PController = Cast<APlayerController>(Controller);
		if (PlrCandidate) //If is a player
		{
			if (PlrCandidate == this) //If PlrCandidate is the current char, move camera rather than ->Possess
			{
				PController->SetViewTargetWithBlend(PlrCandidate, 0.2);
			}
			else
			{
				PController->Possess(PlrCandidate); //New character, so ->Possess
				PlrCandidate->Transition(PossessedObj);
			}
			PossessedObj = this; //Set PossessedObj to this: If it's the current char, it's currently possessing this, if it's not then this is default anyways
		}
		else if (ObjCandidate) //If is a possessable object
		{
			PossessedObj = ObjCandidate;
			PController->SetViewTargetWithBlend(ObjCandidate, 0.2);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Target!"));
		}
		CurrentTarget = nullptr; //Resets what is currently being targetted
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Canceled!"));
	}
	PossessionProgress = 0;
}
