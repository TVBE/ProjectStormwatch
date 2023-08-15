#include "ReacousticStaticMeshDetailCustomization.h"
#include "ReacousticSoundUserData.h"

void ReacousticStaticMeshDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	UE_LOG(LogTemp, Log, TEXT("Starting CustomizeDetails..."));

	IDetailCategoryBuilder& ReacousticCategory= DetailBuilder.EditCategory("Reacoustic");
	
	/** Get the selected objects. */
	TArray<TWeakObjectPtr<UObject>> SelectedObjects = DetailBuilder.GetSelectedObjects();
	UE_LOG(LogTemp, Log, TEXT("Number of Selected Objects: %d"), SelectedObjects.Num());

	/** Create a list of normal pointers to add to external object property. */
	TArray<UObject*> SelectedUObjects;
	for (TWeakObjectPtr<UObject> Object : SelectedObjects)
	{
		if (Object.IsValid())
		{
			SelectedUObjects.Add(Object.Get());

			UStaticMesh* Mesh = Cast<UStaticMesh>(Object.Get());
			if (Mesh)
			{
				UE_LOG(LogTemp, Log, TEXT("Found a valid UStaticMesh."));

				/** Fetch or create the user data for the sound. */
				UReacousticSoundUserData* SoundData = Cast<UReacousticSoundUserData>(Mesh->GetAssetUserDataOfClass(UReacousticSoundUserData::StaticClass()));
				if (!SoundData)
				{
					UE_LOG(LogTemp, Log, TEXT("No existing ReacousticSoundUserData found. Creating a new one."));
					SoundData = NewObject<UReacousticSoundUserData>(Mesh);
					Mesh->AddAssetUserData(SoundData);
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Existing ReacousticSoundUserData found."));
				}
			}
		}
	}

	/** Create a property handle for the sound. */
	ReacousticCategory.AddExternalObjectProperty(SelectedUObjects, GET_MEMBER_NAME_CHECKED(UReacousticSoundUserData, ReacousticSound));

	UE_LOG(LogTemp, Log, TEXT("Finished CustomizeDetails."));
}

TSharedRef<IDetailCustomization,ESPMode::ThreadSafe> ReacousticStaticMeshDetailCustomization::MakeInstance()
{
	UE_LOG(LogTemp, Log, TEXT("Creating ReacousticStaticMeshDetailCustomization instance."));
	return MakeShared<ReacousticStaticMeshDetailCustomization>();
}
