// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/MDGA_Bullet_Normal.h"
#include "Character/MDCharacterBase.h"
#include "Character/MDProjectile.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Game/ObjectPoolWorldSubsystem.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Tags/MDGameplayTag.h"
#include "../MakeDungeon.h"

UMDGA_Bullet_Normal::UMDGA_Bullet_Normal()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	Range = 1000.f;
	SocketName = FName("weapon_r");
}

void UMDGA_Bullet_Normal::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo);
	
	AMDCharacterBase* SpawnInstigator = Cast<AMDCharacterBase>(GetAvatarActorFromActorInfo());

	FRotator Direction = SpawnInstigator->GetAttackDirection();
	Direction.Pitch += 5.f;
	DirectionYaw = TriggerEventData->EventMagnitude;
	Direction.Yaw += DirectionYaw;

	AMDProjectile* SpawnProjectile = nullptr;
	SpawnProjectile = AMDProjectile::ShootProjectile(GetWorld(), ProjectileClass, GetOwningActorFromActorInfo(),
		SpawnInstigator, SpawnInstigator->GetActorLocation(), Direction, 3000.f, ProjectileType);

	if (SpawnProjectile)
	{
		SpawnProjectile->GetCollisionComp()->OnComponentBeginOverlap.AddDynamic(this, &UMDGA_Bullet_Normal::OnBeginOverlap);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
void UMDGA_Bullet_Normal::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != GetOwningActorFromActorInfo()))
	{
		AMDCharacterBase* MDCharacter = Cast<AMDCharacterBase>(OtherActor);
		if (MDCharacter)
		{
			UAbilitySystemComponent* TargetASC = MDCharacter->GetAbilitySystemComponent();
			if (!TargetASC)
			{
				MD_LOG(LogMD, Error, TEXT("ASC not found!"));
				return;
			}

			FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(SweepResult);
			FGameplayAbilityTargetDataHandle TargetDataHandle;
			TargetDataHandle.Add(TargetData);

			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);
			if (EffectSpecHandle.IsValid())
			{ 
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

				FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
				CueContextHandle.AddHitResult(SweepResult);
				FGameplayCueParameters CueParam;
				CueParam.EffectContext = CueContextHandle;

				TargetASC->ExecuteGameplayCue(HitEffectTag, CueParam);

				MD_LOG(LogMD, Warning, TEXT("Hit!"));
			}

			for (auto TargetDebuffEffect : TargetDebuffEffects)
			{
				FGameplayEffectSpecHandle DebuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(TargetDebuffEffect);
				if (DebuffEffectSpecHandle.IsValid())
				{
					ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, DebuffEffectSpecHandle, TargetDataHandle);
				}
			}
		}
	}
}