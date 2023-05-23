// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin.

#include "AmbiverseLayer.h"

#include "AmbiverseParameterManager.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseLayer, LogAmbienceLayer);

bool UAmbiverseLayer::GetEntryWithLowestTime(FAmbiverseLayerQueueEntry& OutEntry)
{
	if (SoundQueue.Num() == 0)
	{
		return false; 
	}

	float LowestTime {TNumericLimits<float>::Max()};
	const FAmbiverseLayerQueueEntry* LowestEntry {nullptr};

	for (const FAmbiverseLayerQueueEntry& Entry : SoundQueue)
	{
		if (Entry.Time < LowestTime)
		{
			LowestTime = Entry.Time;
			LowestEntry = &Entry;
		}
	}

	if (LowestEntry != nullptr)
	{
		OutEntry = *LowestEntry;
		return true;
	}

	return false;
}

void UAmbiverseLayer::SubtractTimeFromQueue(const float TimeToSubtract)
{
	for (FAmbiverseLayerQueueEntry& Entry : SoundQueue)
	{
		Entry.Time -= TimeToSubtract;
	}
}

void UAmbiverseLayer::SortQueueDataByTime()
{
	{
		Algo::Sort(SoundQueue, [](const FAmbiverseLayerQueueEntry& A, const FAmbiverseLayerQueueEntry& B)
		{
			return A.Time < B.Time;
		});
	}
}

void UAmbiverseLayer::InitializeSoundQueue()
{
	for (FAmbiverseProceduralSoundData& SoundData : ProceduralSounds)
	{
		FAmbiverseLayerQueueEntry QueueEntry {};
		QueueEntry.SoundData = SoundData;
		QueueEntry.ReferenceTime = FMath::RandRange(SoundData.DelayMin, SoundData.DelayMax);
		QueueEntry.Time = QueueEntry.ReferenceTime;

		SoundQueue.Add(QueueEntry);
	}
}

