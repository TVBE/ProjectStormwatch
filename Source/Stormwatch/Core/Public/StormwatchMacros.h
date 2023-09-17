// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#define  \
protected: \
        class APlayerCharacter* GetPlayerCharacter() const; \
        class APlayerCharacterController* GetPlayerCharacterController() const; \
private: // Ensures members after this macro are private by default. \

#define  \
    APlayerCharacter* PlayerComponent::GetPlayerCharacter() const \
    { \
        return static_cast<APlayerCharacter*>(this->GetOwner()); \
    } \
    APlayerCharacterController* PlayerComponent::GetPlayerCharacterController() const \
    { \
        APlayerCharacter* PlayerCharacter = this->GetPlayerCharacter(); \
        if (PlayerCharacter) \
        { \
            return static_cast<APlayerCharacterController*>(PlayerCharacter->GetController()); \
        } \
        return nullptr; \
    }


