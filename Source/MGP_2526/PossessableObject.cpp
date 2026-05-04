// Fill out your copyright notice in the Description page of Project Settings.


#include "PossessableObject.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APossessableObject::APossessableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ObjMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ObjMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SetRootComponent(ObjMesh);

	Tags.Add("Possessable");

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

}

// Called when the game starts or when spawned
void APossessableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APossessableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

