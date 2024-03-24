// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"

template<typename T>
class TBHHistory
{
private:
	TArray<T> History;
	uint32 Length;

public:
	TBHHistory();
	explicit TBHHistory(uint32 InLength);
	
	const T* GetValue(uint32 Index) const;
	const T* GetCurrentValue() const;
	const T* GetPreviousValue() const;
	void Set(const T& Value);
	bool IsSet(uint32 Index = 0) const;
	void SetLength(uint32 NewLength);
	void Reset();
	bool IsEmpty() const;

	TBHHistory<T>& operator=(const T& Value);
};