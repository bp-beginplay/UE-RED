// Copyright Epic Games, Inc. All Rights Reserved.

#include "RED.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, RED, "RED");

//.cpp
DEFINE_LOG_CATEGORY(LogRed)

// 블루프린트에서 읽기만 가능
//UPROPERTY(BlueprintReadOnly)

// 블루프린트에서 읽기/쓰기 가능
//UPROPERTY(BlueprintReadWrite)

// 블루프린트에서 Getter/Setter 사용
//UPROPERTY(BlueprintGetter = GetHealth, BlueprintSetter = SetHealth)

// 블루프린트에서 아예 접근 불가
//UPROPERTY(BlueprintPrivate)

// 에디터에서만 수정 가능 (인스턴스)
//UPROPERTY(EditInstanceOnly)

// 에디터에서만 수정 가능 (클래스 기본값)
//UPROPERTY(EditDefaultsOnly)

// 어디서든 수정 가능
//UPROPERTY(EditAnywhere)

// 에디터에서는 읽기만 가능
//UPROPERTY(VisibleAnywhere)
//UPROPERTY(VisibleInstanceOnly)
//UPROPERTY(VisibleDefaultsOnly)