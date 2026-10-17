// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogRed, Log, All);

//CUSTOM_LOG 라는 이름으로 커스텀 로그 출력
#define CUSTOM_LOG_5S(FMT, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT(FMT), ##__VA_ARGS__)); UE_LOG(LogRed, Warning, TEXT(FMT), ##__VA_ARGS__);

#define CUSTOM_LOG_0S(FMT, ...) GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT(FMT), ##__VA_ARGS__)); UE_LOG(LogRed, Warning, TEXT(FMT), ##__VA_ARGS__);

#define CUSTOM_LOG(TIME, FMT, ...) GEngine->AddOnScreenDebugMessage(-1, TIME, FColor::Green, FString::Printf(TEXT(FMT), ##__VA_ARGS__)); UE_LOG(LogRed, Warning, TEXT(FMT), ##__VA_ARGS__);

#define CUSTOM_LOG_FUNCNAME() \
if (GEngine) \
{ \
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString(__FUNCTION__)); \
} \
UE_LOG(LogTemp, Warning, TEXT("%hs"), __FUNCTION__);

#define CUSTOM_LOG_ROLE(ActorPtr, FMT, ...) \
if(ActorPtr) \
{ \
    FString RoleStr; \
    switch(ActorPtr->GetLocalRole()) \
    { \
        case ROLE_Authority: RoleStr = TEXT("Server"); break; \
        case ROLE_AutonomousProxy: RoleStr = TEXT("Client-Auto"); break; \
        case ROLE_SimulatedProxy: RoleStr = TEXT("Client-Sim"); break; \
        default: RoleStr = TEXT("Unknown"); break; \
    } \
    FString Msg = FString::Printf(TEXT("[%s] " FMT), *RoleStr, ##__VA_ARGS__); \
    if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, Msg); \
    UE_LOG(LogRed, Warning, TEXT("%s"), *Msg); \
}

#define CUSTOM_LOG_ROLE_FUNCNAME(ActorPtr) \
if(ActorPtr) \
{ \
    FString RoleStr; \
    switch(ActorPtr->GetLocalRole()) \
    { \
        case ROLE_Authority: RoleStr = TEXT("Server"); break; \
        case ROLE_AutonomousProxy: RoleStr = TEXT("Client-Auto"); break; \
        case ROLE_SimulatedProxy: RoleStr = TEXT("Client-Sim"); break; \
        default: RoleStr = TEXT("Unknown"); break; \
    } \
    if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString(__FUNCTION__)); \
    UE_LOG(LogRed, Warning, TEXT("%s"), *FString(__FUNCTION__)); \
}