// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTagContainer.h"
#include "Data/MDWeaponAttackData.h"
#include "MDWeaponBase.generated.h"

class UGameplayAbility;
class AMDCharacterBase;
class UMDWeaponInputData;

enum class EMDSkillMontage : uint8
{
	Skill_01, Skill_02, Skill_03, Skill_04
};

/**
 * 
 */
UCLASS()
class MAKEDUNGEON_API UMDWeaponBase : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UMDWeaponBase();

	FORCEINLINE UAnimMontage* GetAttackMontage() const { return WeaponAttackData->AttackMontage; }
	FORCEINLINE UMDWeaponAttackData* GetWeaponAttackData() const { return WeaponAttackData; }
	FORCEINLINE UAnimMontage* GetSkillMontage(EMDSkillMontage MontageIndex = EMDSkillMontage::Skill_01) const { return WeaponAttackData->SkillMontage[static_cast<uint8>(MontageIndex)]; }
	void SetWeaponAttackData(AMDCharacterBase* InCharacter, UMDWeaponAttackData* WeaponData);

	void EquipWeapon(AMDCharacterBase* InCharacter);
	void UnequipWeapon(AMDCharacterBase* InCharacter);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UMDWeaponAttackData> WeaponAttackData;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> WeaponInputAbilities;

private:
	UPROPERTY(EditAnywhere, Category = "Data")
	TObjectPtr<UMDWeaponInputData> InputData;
};
