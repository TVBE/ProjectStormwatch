// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#define  \
protected: \
        class ABHPlayerCharacter* GetPlayerCharacter() const; \
        class ABHPlayerCharacterController* GetPlayerCharacterController() const; \
private: // Ensures members after this macro are private by default. \

#define  \
    ABHPlayerCharacter* PlayerComponent::GetPlayerCharacter() const \
    { \
        return static_cast<ABHPlayerCharacter*>(this->GetOwner()); \
    } \
    ABHPlayerCharacterController* PlayerComponent::GetPlayerCharacterController() const \
    { \
        ABHPlayerCharacter* PlayerCharacter = this->GetPlayerCharacter(); \
        if (PlayerCharacter) \
        { \
            return static_cast<ABHPlayerCharacterController*>(PlayerCharacter->GetController()); \
        } \
        return nullptr; \
    }


