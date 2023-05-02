#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "RoomComponentBase.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FROSTBITE_API URoomComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	URoomComponentBase();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoomComponent")
	UBoxComponent* ParentBoxCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoomComponent")
	bool Enabled{true};

	/** Base events to be called to activate various functionality for room components.*/
	UFUNCTION(BlueprintNativeEvent, Category = Default, Meta = (DisplayName = "Trigger manual hit"))
	void OnActivate();
	UFUNCTION(BlueprintNativeEvent, Category = Default, Meta = (DisplayName = "Trigger manual hit"))
	void OnDeactivate();
	

private:
	UFUNCTION()
	void BindOverlapEvents();

	UFUNCTION()
	void UnbindOverlapEvents();

	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnOverlapEnds(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnActorSpawned(AActor* SpawnedActor);
	FDelegateHandle ActorSpawnedDelegateHandle;

	UPROPERTY()
	bool PlayerInsideCollider;
};