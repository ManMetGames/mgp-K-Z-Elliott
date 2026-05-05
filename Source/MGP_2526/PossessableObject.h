// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessableObject.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class MGP_2526_API APossessableObject : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	APossessableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/*Public variables for the possessable object*/
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ObjMesh;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
