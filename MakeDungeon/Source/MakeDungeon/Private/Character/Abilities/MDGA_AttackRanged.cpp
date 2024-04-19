// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/MDGA_AttackRanged.h"
#include "Character/MDCharacterBase.h"
//#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/KismetMathLibrary.h"
//#include "Tags/MDGameplayTag.h"
#include "Game/ObjectPoolWorldSubsystem.h"

UMDGA_AttackRanged::UMDGA_AttackRanged()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	Range = 1000.f;
	SocketName = FName("Weapon_R");
}

void UMDGA_AttackRanged::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	/*UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TriggerEventData->EventTag);
	Task->EventReceived.AddDynamic(this, &UMDGA_AttackRanged::ShootBullet);
	Task->ReadyForActivation();*/

	ShootBullet(CurrentEventData);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UMDGA_AttackRanged::ShootBullet(FGameplayEventData EventData)
{
	AMDCharacterBase* MDCharacter = Cast<AMDCharacterBase>(GetAvatarActorFromActorInfo());
	if (!MDCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	FVector Start = MDCharacter->GetMesh()->GetSocketLocation(SocketName);
	FVector End = MDCharacter->GetActorLocation() + MDCharacter->GetActorForwardVector() * Range;
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

	FTransform MuzzleTransform = MDCharacter->GetMesh()->GetSocketTransform(SocketName);
	MuzzleTransform.SetRotation(Rotation.Quaternion());
	MuzzleTransform.SetScale3D(FVector(1.f));

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UObjectPoolWorldSubsystem* ObjectPool = UWorld::GetSubsystem<UObjectPoolWorldSubsystem>(GetWorld());

	//AMDProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMDProjectile>(ProjectileClass, MuzzleTransform, GetOwningActorFromActorInfo(), MDCharacter, SpawnParameters.SpawnCollisionHandlingOverride);
	//AMDProjectile* Projectile = GetWorld()->SpawnActor<AMDProjectile>(ProjectileClass, MuzzleTransform, SpawnParameters);
	AMDProjectile* Projectile = ObjectPool->ReuseObject(AMDProjectile::StaticClass(), Start, Rotation);
	//AMDProjectile* Projectile = ObjectPool->ReuseObject(ProjectileClass, MuzzleTransform);
	if(Projectile)
	{
		Projectile->SetOwner(GetOwningActorFromActorInfo());
		Projectile->SetInstigator(MDCharacter);
		Projectile->Range = Range;
		Projectile->Restart(MDCharacter->GetActorForwardVector());
	}
	//Projectile->FinishSpawning(MuzzleTransform);
}
