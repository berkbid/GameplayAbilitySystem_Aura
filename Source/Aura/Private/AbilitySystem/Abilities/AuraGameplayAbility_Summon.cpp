// Copyright Berkeley Bidwell

#include "AbilitySystem/Abilities/AuraGameplayAbility_Summon.h"
#include "Character/AuraEnemy.h"

#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraGameplayAbility_Summon)

UAuraGameplayAbility_Summon::UAuraGameplayAbility_Summon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TArray<FVector> UAuraGameplayAbility_Summon::GetSpawnLocations()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || NumMinions <= 0)
	{
		return TArray<FVector>();
	}
	
	const FVector Forward = AvatarActor->GetActorForwardVector();
	const FVector Location = AvatarActor->GetActorLocation();
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	const float DeltaSpread = SpawnSpread / NumMinions;
	
	TArray<FVector> SpawnLocations;
	for (int32 i = 0; i < NumMinions; i++)
	{
		// Creating random spawn angle within spread left
		const float RandSpawnSpread = FMath::FRandRange(0.f, DeltaSpread);
		const FVector Direction = LeftOfSpread.RotateAngleAxis(RandSpawnSpread * (i + 1), FVector::UpVector);
		//const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		
		FVector SpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		// Make sure spawn locations are on the ground, if ground is not flat
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit,
			SpawnLocation + FVector(0.f, 0.f, 400.f),
			SpawnLocation - FVector(0.f, 0.f, 400.f),
			ECC_Visibility);

		if (Hit.bBlockingHit)
		{
			SpawnLocation = Hit.ImpactPoint;
		}
		
		//DrawDebugSphere(GetWorld(), SpawnLocation, 18.f, 12, FColor::Cyan, false, 3.f);
		SpawnLocations.Add(SpawnLocation);
		
		// UKismetSystemLibrary::DrawDebugArrow(AvatarActor, Location, Location + Direction * MaxSpawnDistance,
		// 	4.f, FLinearColor::Green, 3.f);
		// DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.f, 12, FColor::Red, false, 3.f);
		// DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.f, 12, FColor::Red, false, 3.f);
	}
	
	return SpawnLocations;
}

TSubclassOf<AAuraEnemy> UAuraGameplayAbility_Summon::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses.IsValidIndex(Selection) ? MinionClasses[Selection] : nullptr;
}

AAuraEnemy* UAuraGameplayAbility_Summon::GetRandomMinionClassCDO()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses.IsValidIndex(Selection) ? MinionClasses[Selection]->GetDefaultObject<AAuraEnemy>() : nullptr;
}
