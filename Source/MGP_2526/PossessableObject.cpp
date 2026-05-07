// Fill out your copyright notice in the Description page of Project Settings.


#include "PossessableObject.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APossessableObject::APossessableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ObjMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh")); //Creates mesh component to be assigned in editor
	SetRootComponent(ObjMesh);

	Tags.Add("Possessable"); //Mark object as possessable

	/*Camera Setup for possession*/
	//Setup in a similar way to the one in Character for more seamless transition
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(50.f, 50.f, 80.f)); //Hard coded re-position, dynamic is out of scope for this mechanic showcase 
	CameraBoom->TargetArmLength = 400.0f; //Same TargetArmLength as player
	CameraBoom->bUsePawnControlRotation = true;

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

