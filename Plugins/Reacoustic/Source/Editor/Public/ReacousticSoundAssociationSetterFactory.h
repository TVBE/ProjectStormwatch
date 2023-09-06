#pragma once

#include "ActorFactories/ActorFactory.h"
#include "ReacousticSoundAssociationSetterFactory.generated.h"

/**
 * Note: This factory does NOT actually create any actors. It exists solely for the purpose of enabling drag-and-drop functionality
 * for the Reacoustic Sound Asset within the Unreal Editor. When such an asset is dragged and dropped onto an actor,
 * the OnApplyObjectToActor delegate is triggered, allowing custom logic to be executed.
 * This approach is used because Unreal Engine tightly couples drag-and-drop functionality with asset factories.
 */

class UReacousticComponent;

UCLASS()
class REACOUSTICEDITOR_API UReacousticSoundAssociationSetterFactory : public UActorFactory
{
	GENERATED_BODY()

public:

protected:
	bool ISReaousticSoundvalid;
private:

	/** The reaoustic sound asset to apply.*/
	UObject* ObjectToApply{nullptr};
	
	/** The actor to apply the reacoustic sound asset to by creating a reacoustic component and applying the asset there.*/
	AActor* ActorToApplyTo{nullptr};

	
public:

	UFUNCTION()
	void TryApplyToActor(UObject* Object, AActor* Actor);
	UReacousticSoundAssociationSetterFactory(const FObjectInitializer& ObjectInitializer);

	/** UActorFactory interface */
	virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
};
