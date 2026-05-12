// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackoutDedicatedSessionSubsystem.h"

#include "Core/BlackoutLog.h"
#include "Core/BlackoutLogCategories.h"

#include "BlackoutNetworkSettings.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "HAL/PlatformMisc.h"

void UBlackoutDedicatedSessionSubsystem::Initialize(
	FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	BO_LOG_NET(Log, "DedicatedSessionSubsystem Initialize - NetMode=%d",
	           static_cast<int32>(GetWorld()? GetWorld()->GetNetMode() :
		           NM_Standalone));
}

void UBlackoutDedicatedSessionSubsystem::Deinitialize()
{
	CurrentSessionId.Empty();
	Super::Deinitialize();
}

void UBlackoutDedicatedSessionSubsystem::SetSessionId(
	const FString& InSessionId)
{
	CurrentSessionId = InSessionId;
	BO_LOG_NET(Log,"DedicatedSession SessionId 보관 - %s",*CurrentSessionId);
}

void UBlackoutDedicatedSessionSubsystem::ReportFinishToMatchmakingServer()
{
	// 데디 전용 가드 - PIE / Standalone / Listen Server 에서 EndMatch 호출 시 차단
	const UWorld* World = GetWorld();
	
	if (World && World->GetNetMode()!=NM_DedicatedServer)
	{
		BO_LOG_NET(Warning,"ReportFinish - 데디 환경 아님 (NetMode=%d), skip",static_cast<int32>(World->GetNetMode()));
		return;
	}
	
	if (CurrentSessionId.IsEmpty())
	{
		BO_LOG_NET(Warning,"ReportFinish -SessionId 없음, skip");
		return;
	}
	
	// API 키 - 환경변수만
	const FString ApiKey =FPlatformMisc::GetEnvironmentVariable(TEXT("BLACKOUT_API_KEY"));
	if (ApiKey.IsEmpty())
	{
		BO_LOG_NET(Error, "ReportFinish - BLACKOUT_API_KEY 환경변수 없음, skip");
		return;
	}
	
	// 매칭서버 URL
	const UBlackoutNetworkSettings* Settings = GetDefault<UBlackoutNetworkSettings>();
	if (!Settings || Settings->ApiBaseUrl.IsEmpty())
	{
		BO_LOG_NET(Error, "ReportFinish - ApiBaseUrl 미설정");
		return;
	}
	
	const FString Url = FString::Printf(TEXT("%s/sessions/%s/finish"),*Settings->ApiBaseUrl,*CurrentSessionId);
	
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("x-server-api-key"), ApiKey);
	Request->OnProcessRequestComplete().BindUObject(this , &UBlackoutDedicatedSessionSubsystem::OnFinishResponse);
	
	Request->ProcessRequest();
	BO_LOG_NET(Log, "ReportFinish 요청 - %s",*Url);
}

void UBlackoutDedicatedSessionSubsystem::OnFinishResponse(
	FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded || !Response.IsValid())
	{
		BO_LOG_NET(Error, "ReportFinish 응답 실패 (네트워크 / 타임아웃)");
		return;
	}
	
	const int32 Status = Response->GetResponseCode();
	const FString Body = Response->GetContentAsString();
	
	if (Status >=200 && Status <300)
	{
		BO_LOG_NET(Log,"ReportFinishg 성공 - %d %s",Status, *Body);
		CurrentSessionId.Empty(); // 다음 매치
	}
	else
	{
		BO_LOG_NET(Error,"ReportFinish 실패 - %d %s",Status, *Body);
	}
	
}
