// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/MDWeaponBase.h"
#include "MDWeaponTwoHandedSword.generated.h"

/**
 * 
 */
UCLASS()
class MAKEDUNGEON_API UMDWeaponTwoHandedSword : public UMDWeaponBase
{
	GENERATED_BODY()
	
public:
	UMDWeaponTwoHandedSword();

	virtual void InitWeapon(AMDCharacterBase* InCharacter) override;
};
