// Copyright Notice

#include "Stormwatch/Types/Public/BHHistory.h"

template <typename T>
TBHHistory<T>::TBHHistory() : TBHHistory(2) {}

template <typename T>
TBHHistory<T>::TBHHistory(uint32 InLength)
{
	check(InLength > 0);
	Length = InLength;
	History.Reserve(InLength);
}

template <typename T>
void TBHHistory<T>::Set(const T& Value)
{
    if (History.Num() >= Length)
    {
        History.RemoveAt(History.Num() - 1);
    }
    History.Insert(Value, 0);
}

template <typename T>
const T* TBHHistory<T>::GetValue(uint32 Index) const
{
	return History.IsValidIndex(Index) ? History[Index] : nullptr;
}

template <typename T>
const T* TBHHistory<T>::GetCurrentValue() const
{
	return History.IsValidIndex(0) ? History[0] : nullptr;
}

template <typename T>
const T* TBHHistory<T>::GetPreviousValue() const
{
	return History.IsValidIndex(1) ? History[1] : nullptr;
}

template <typename T>
bool TBHHistory<T>::IsSet(uint32 Index) const
{
	return History.IsValidIndex(Index);
}

template <typename T>
void TBHHistory<T>::SetLength(uint32 NewLength)
{
	ensureAlways(NewLength > 0);
	Length = NewLength;
	if (History.Num() > Length)
	{
		History.SetNum(Length);
	}
}

template <typename T>
TBHHistory<T>& TBHHistory<T>::operator=(const T& Value)
{
	Set(Value);
	return *this;
}
