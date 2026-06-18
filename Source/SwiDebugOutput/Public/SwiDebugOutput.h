// Copyright (c) 2025–present Second Wall Interactive. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"
#include "VisualLogger/VisualLogger.h"
#include "Algo/Find.h"

#include "SwiDebugOutput.generated.h"

// These were provided by the originating project's shared PCH. Guarded so a host
// project that already defines them keeps its own definition.
#ifndef UE_EMPTY
#define UE_EMPTY
#endif

#ifndef TEXT_EMPTY
#define TEXT_EMPTY TEXT("")
#endif

#define SWI_ENABLE_DEBUG_OUTPUT UE_ENABLE_DEBUG_DRAWING && ENABLE_VISUAL_LOG


namespace Swi::LogTags
{
struct FSwiLogTag
{
	FString& Name;
	bool& bEnabled;
};


FORCEINLINE bool operator== (const FSwiLogTag& A, const FSwiLogTag& B)
{
	return A.Name == B.Name;
}

FORCEINLINE uint32 GetTypeHash (const FSwiLogTag& LogTag)
{
	return GetTypeHash(LogTag.Name);
}
}


#define PRIVATE_SWI_DEFINE_LOG_TAG(TagName, INLINE_SPECIFIER)\
namespace Private\
{\
INLINE_SPECIFIER static FString TagName##Name = TEXT(#TagName);\
INLINE_SPECIFIER static FString TagName##Name##CVar = TEXT("Swi.LogTags.")TEXT(#TagName);\
INLINE_SPECIFIER static bool TagName##Enabled = false;\
INLINE_SPECIFIER static FAutoConsoleVariableRef CVar##TagName(*TagName##Name##CVar, TagName##Enabled, TEXT_EMPTY);\
}\
INLINE_SPECIFIER static Swi::LogTags::FSwiLogTag TagName\
{\
	.Name = Private::TagName##Name,\
	.bEnabled = Private::TagName##Enabled\
};

#define PRIVATE_SWI_DEFINE_LOG_TAG_WITH_CATEGORY(TagCategory, TagName, INLINE_SPECIFIER)\
namespace Private\
{\
INLINE_SPECIFIER static FString TagCategory##TagName##Name = TEXT(#TagCategory)TEXT(".")TEXT(#TagName);\
INLINE_SPECIFIER static FString TagCategory##TagName##Name##CVar = TEXT("Swi.LogTags.")TEXT(#TagCategory)TEXT(".")TEXT(#TagName);\
INLINE_SPECIFIER static bool TagCategory##TagName##Enabled = false;\
INLINE_SPECIFIER static FAutoConsoleVariableRef CVar##TagCategory##TagName(*TagCategory##TagName##Name##CVar, TagCategory##TagName##Enabled, TEXT_EMPTY);\
}\
INLINE_SPECIFIER static Swi::LogTags::FSwiLogTag TagCategory##TagName\
{\
	.Name = Private::TagCategory##TagName##Name,\
	.bEnabled = Private::TagCategory##TagName##Enabled\
};

#define SWI_DEFINE_LOG_TAG(TagName)\
	PRIVATE_SWI_DEFINE_LOG_TAG(TagName, UE_EMPTY)

#define SWI_DEFINE_LOG_TAG_INLINE(TagName)\
	PRIVATE_SWI_DEFINE_LOG_TAG(TagName, inline)

#define SWI_DEFINE_LOG_TAG_WITH_CATEGORY(TagCategory, TagName)\
	PRIVATE_SWI_DEFINE_LOG_TAG_WITH_CATEGORY(TagCategory, TagName, UE_EMPTY)

#define SWI_DEFINE_LOG_TAG_WITH_CATEGORY_INLINE(TagCategory, TagName)\
	PRIVATE_SWI_DEFINE_LOG_TAG_WITH_CATEGORY(TagCategory, TagName, inline)

#define SWI_CHECK_LOG_TAG_WITH_RETURN(LogTag)\
	if (!LogTag)\
	{\
		LogTag = &DefaultTagName;\
	}\
	\
	if (!IsLogTagEnabled(*LogTag))\
	{\
		return;\
	}


namespace Swi::LogTags
{
SWI_DEFINE_LOG_TAG_INLINE(Default);
}


/**
 * It is recommended to use this with designated initializer syntax for better readability.
 */
struct FSwiCommonDebugParams
{
	const FString* LogTag = nullptr;
	ELogVerbosity::Type Verbosity = ELogVerbosity::Log;
	FString LogStr = {};

	TOptional<FColor> Color;
	TOptional<float> Duration;
	// If passed to Log or Draw function, will call V- (VLOG) version of it, and vice versa.
	TOptional<bool> bAlsoLogOrVLog;
};


/**
 * @TODO: blueprints support maybe?
 * @TODO: implement Draw* methods
 */
USTRUCT()
struct FSwiDebugOutput
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TWeakObjectPtr<const UObject> Owner;

	UPROPERTY()
	TWeakObjectPtr<const UWorld> World;

	// might be useful to create color palettes for advanced debugging
	FColor ColorDefault = FColor::Green;
	FColor ColorTraceResultNoHit = FColor::Green;
	FColor ColorTraceResultHit = FColor::Red;

	float DefaultDuration = 10.f;

	bool bDoubleLogByDefault = true; // UE_LOG + UE_VLOG

	TSet<Swi::LogTags::FSwiLogTag> RegisteredTags;
	FString DefaultTagName = Swi::LogTags::Default.Name;

public:
	FSwiDebugOutput ();

	bool IsValid () const;
	void SetUp (const UObject* InOwner, const UWorld* InWorld = nullptr);

	bool IsLogTagEnabled (const FString& LogTagName) const;

#if SWI_ENABLE_DEBUG_OUTPUT
	template <auto& LogCategory>
	void Log (FSwiCommonDebugParams Params = {}) const;

	template <auto& LogCategory>
	void VLog (FSwiCommonDebugParams Params = {}) const;

	// @TODO: implement
	void DrawLine (const FVector& Start, const FVector& End, const FString* LogTag = nullptr) const;
	void DrawSphere () const {};
	void DrawCapsule () const {};
	void DrawSolidBox () const {};

	template <auto& LogCategory>
	void DrawArrow (
		const FVector& Start,
		const FVector& End,
		float ArrowSize,
		float Thickness,
		FSwiCommonDebugParams Params = {});

	template <auto& LogCategory>
	void VDrawArrow (
		const FVector& Start,
		const FVector& End,
		float ArrowSize,
		float Thickness,
		FSwiCommonDebugParams Params = {});


	template <auto& LogCategory>
	void DrawPrimitiveComponent (
		UPrimitiveComponent* PrimitiveComponent,
		bool bWireframe,
		FSwiCommonDebugParams Params = {});

	// @TODO: add versions that take Location only
	template <auto& LogCategory>
	void DrawCollisionShape (
		const FTransform& Transform,
		const FCollisionShape& Shape,
		bool bWireframe,
		FSwiCommonDebugParams Params = {});

	template <auto& LogCategory>
	void VDrawPrimitiveComponent (
		UPrimitiveComponent* PrimitiveComponent,
		bool bWireframe,
		FSwiCommonDebugParams Params = {});

	/** Expanded and merged version of UE_VLOG_CAPSULE, UE_VLOG_SPHERE, UE_VLOG_BOX */
	template <auto& LogCategory>
	void VDrawCollisionShape (
		const FTransform& Transform,
		const FCollisionShape& Shape,
		bool bWireframe,
		FSwiCommonDebugParams Params = {});

	void DrawTraceLine () const {};
	void DrawTraceSweep () const {};
#else
	void Dummy () const {}
#endif

private:
};

#if SWI_ENABLE_DEBUG_OUTPUT

#define SWI_ENSURE_VALID()\
if (!ensure(IsValid())) \
{ \
return; \
}

#define SWI_LOGTAG_APPEND_LOGSTR(LogTag, LogStr)\
	FString LogStrWithTag = LogStr;\
	LogStrWithTag.InsertAt(0, FString::Printf(TEXT("[%s] "), **LogTag));

template <auto& LogCategory>
void FSwiDebugOutput::Log (FSwiCommonDebugParams Params) const
{
	SWI_ENSURE_VALID();
	SWI_CHECK_LOG_TAG_WITH_RETURN(Params.LogTag);
	SWI_LOGTAG_APPEND_LOGSTR(Params.LogTag, Params.LogStr);

	const bool bDoubleLog = Params.bAlsoLogOrVLog.Get(bDoubleLogByDefault);
	Params.bAlsoLogOrVLog = false;

	switch (Params.Verbosity)
	{
		case ELogVerbosity::Error:
			UE_LOG(LogCategory, Error, TEXT("%s"), *LogStrWithTag);
			break;
		case ELogVerbosity::Warning:
			UE_LOG(LogCategory, Warning, TEXT("%s"), *LogStrWithTag);
			break;
		case ELogVerbosity::Display:
			UE_LOG(LogCategory, Display, TEXT("%s"), *LogStrWithTag);
			break;
		case ELogVerbosity::Log:
			UE_LOG(LogCategory, Log, TEXT("%s"), *LogStrWithTag);
			break;
		case ELogVerbosity::Verbose:
			UE_LOG(LogCategory, Verbose, TEXT("%s"), *LogStrWithTag);
			break;
		case ELogVerbosity::VeryVerbose:
			UE_LOG(LogCategory, VeryVerbose, TEXT("%s"), *LogStrWithTag);
			break;
		default: break;
	}

	if (bDoubleLog)
	{
		VLog<LogCategory>(Params);
	}
}

template <auto& LogCategory>
void FSwiDebugOutput::VLog (FSwiCommonDebugParams Params) const
{
	SWI_ENSURE_VALID();
	SWI_CHECK_LOG_TAG_WITH_RETURN(Params.LogTag);
	SWI_LOGTAG_APPEND_LOGSTR(Params.LogTag, Params.LogStr);

	const bool bDoubleLog = Params.bAlsoLogOrVLog.Get(bDoubleLogByDefault);
	Params.bAlsoLogOrVLog = false;

	if (FVisualLogger::IsRecording())
	{
		FVisualLogger::CategorizedLogf(Owner.Get(), LogCategory, Params.Verbosity, TEXT("%s"), *LogStrWithTag);
	}

	if (bDoubleLog)
	{
		Log<LogCategory>(Params);
	}
}

template <auto& LogCategory>
void FSwiDebugOutput::DrawArrow (
	const FVector& Start,
	const FVector& End,
	float ArrowSize,
	float Thickness,
	FSwiCommonDebugParams Params)
{
	SWI_ENSURE_VALID();
	SWI_CHECK_LOG_TAG_WITH_RETURN(Params.LogTag);
	SWI_LOGTAG_APPEND_LOGSTR(Params.LogTag, Params.LogStr);

	const bool bDoubleLog = Params.bAlsoLogOrVLog.Get(bDoubleLogByDefault);
	Params.bAlsoLogOrVLog = false;

	DrawDebugDirectionalArrow(
		World.Get(),
		Start, End,
		ArrowSize,
		Params.Color.Get(ColorDefault),
		false,
		Params.Duration.Get(DefaultDuration),
		/*DepthPriority*/0,
		Thickness);

	Log<LogCategory>(Params);

	if (bDoubleLog)
	{
		VDrawArrow<LogCategory>(Start, End, ArrowSize, Thickness, Params);
	}
}

template <auto& LogCategory>
void FSwiDebugOutput::VDrawArrow (
	const FVector& Start,
	const FVector& End,
	float ArrowSize,
	float Thickness,
	FSwiCommonDebugParams Params)
{
	SWI_ENSURE_VALID();
	SWI_CHECK_LOG_TAG_WITH_RETURN(Params.LogTag);
	SWI_LOGTAG_APPEND_LOGSTR(Params.LogTag, Params.LogStr);

	const bool bDoubleLog = Params.bAlsoLogOrVLog.Get(bDoubleLogByDefault);
	Params.bAlsoLogOrVLog = false;

	FVisualLogger::ArrowLineLogf(
		Owner.Get(), LogCategory, Params.Verbosity,
		Start, End, Params.Color.Get(ColorDefault),
		ArrowSize,
		TEXT("%s"), *LogStrWithTag);

	VLog<LogCategory>(Params);

	if (bDoubleLog)
	{
		DrawArrow<LogCategory>(
			Start, End, ArrowSize, Thickness, Params);
	}
}

template <auto& LogCategory>
void FSwiDebugOutput::DrawPrimitiveComponent (
	UPrimitiveComponent* PrimitiveComponent,
	bool bWireframe,
	FSwiCommonDebugParams Params)
{
	SWI_ENSURE_VALID();
	SWI_CHECK_LOG_TAG_WITH_RETURN(Params.LogTag);
	SWI_LOGTAG_APPEND_LOGSTR(Params.LogTag, Params.LogStr);

	const bool bDoubleLog = Params.bAlsoLogOrVLog.Get(bDoubleLogByDefault);
	Params.bAlsoLogOrVLog = false;

	if (!::IsValid(PrimitiveComponent))
	{
		return;
	}

	DrawCollisionShape<LogCategory>(
		PrimitiveComponent->GetComponentTransform(),
		PrimitiveComponent->GetCollisionShape(),
		bWireframe, Params);

	if (bDoubleLog)
	{
		VDrawPrimitiveComponent<LogCategory>(PrimitiveComponent, bWireframe, Params);
	}
}

template <auto& LogCategory>
void FSwiDebugOutput::DrawCollisionShape (
	const FTransform& Transform,
	const FCollisionShape& Shape,
	bool bWireframe,
	FSwiCommonDebugParams Params)
{
	SWI_ENSURE_VALID();
	SWI_CHECK_LOG_TAG_WITH_RETURN(Params.LogTag);
	SWI_LOGTAG_APPEND_LOGSTR(Params.LogTag, Params.LogStr);

	const bool bDoubleLog = Params.bAlsoLogOrVLog.Get(bDoubleLogByDefault);
	Params.bAlsoLogOrVLog = false;

	switch (Shape.ShapeType)
	{
		case ECollisionShape::Line: break; // Line in FCollisionShape basically means invalid
		case ECollisionShape::Box:
		{
			if (bWireframe)
			{
				DrawDebugBox(
					World.Get(), Transform.GetLocation(), Shape.GetExtent(),
					Transform.GetRotation(), Params.Color.Get(ColorDefault), false,
					Params.Duration.Get(DefaultDuration));
			}
			else
			{
				DrawDebugSolidBox(
					World.Get(),
					FBox::BuildAABB(FVector::ZeroVector, Shape.GetBox()),
					Params.Color.Get(ColorDefault), Transform, false, Params.Duration.Get(DefaultDuration));
			}
			break;
		}

		case ECollisionShape::Sphere:
		{
			DrawDebugSphere(
				World.Get(), Transform.GetLocation(), Shape.GetSphereRadius(),
				12, Params.Color.Get(ColorDefault), false, Params.Duration.Get(DefaultDuration));
			break;
		}
		case ECollisionShape::Capsule:
		{
			DrawDebugCapsule(
				World.Get(), Transform.GetLocation(),
				Shape.GetCapsuleHalfHeight(), Shape.GetCapsuleRadius(),
				Transform.GetRotation(), Params.Color.Get(ColorDefault), false, Params.Duration.Get(DefaultDuration));
			break;
		}
	}

	if (bDoubleLog)
	{
		VDrawCollisionShape<LogCategory>(Transform, Shape, bWireframe, Params);
	}
}

template <auto& LogCategory>
void FSwiDebugOutput::VDrawPrimitiveComponent (
	UPrimitiveComponent* PrimitiveComponent,
	bool bWireframe,
	FSwiCommonDebugParams Params)
{
	SWI_ENSURE_VALID();
	SWI_CHECK_LOG_TAG_WITH_RETURN(Params.LogTag);
	SWI_LOGTAG_APPEND_LOGSTR(Params.LogTag, Params.LogStr);

	const bool bDoubleLog = Params.bAlsoLogOrVLog.Get(bDoubleLogByDefault);
	Params.bAlsoLogOrVLog = false;

	if (!::IsValid(PrimitiveComponent))
	{
		return;
	}

	DrawCollisionShape<LogCategory>(
		PrimitiveComponent->GetComponentTransform(),
		PrimitiveComponent->GetCollisionShape(),
		bWireframe, Params);

	if (bDoubleLog)
	{
		DrawPrimitiveComponent<LogCategory>(PrimitiveComponent, bWireframe, Params);
	}
}

template <auto& LogCategory>
void FSwiDebugOutput::VDrawCollisionShape (
	const FTransform& Transform,
	const FCollisionShape& Shape,
	bool bWireframe,
	FSwiCommonDebugParams Params)
{
	SWI_ENSURE_VALID();
	SWI_CHECK_LOG_TAG_WITH_RETURN(Params.LogTag);
	SWI_LOGTAG_APPEND_LOGSTR(Params.LogTag, Params.LogStr);

	const bool bDoubleLog = Params.bAlsoLogOrVLog.Get(bDoubleLogByDefault);
	Params.bAlsoLogOrVLog = false;

	if (FVisualLogger::IsRecording())
	{
		switch (Shape.ShapeType)
		{
			case ECollisionShape::Line: break;
			case ECollisionShape::Box:
			{
				FVisualLogger::BoxLogf(
					Owner.Get(), LogCategory, Params.Verbosity,
					FBox::BuildAABB(FVector::ZeroVector, Shape.GetBox()), Transform.ToMatrixWithScale(),
					Params.Color.Get(ColorDefault), bWireframe, TEXT("%s"), *Params.LogStr);
				if (!Params.LogStr.IsEmpty())
				{
					VLog<LogCategory>(Params);
				}
				break;
			}
			case ECollisionShape::Sphere:
			{
				FVisualLogger::SphereLogf(
					Owner.Get(), LogCategory, Params.Verbosity,
					Transform.GetLocation(), Shape.GetSphereRadius(),
					Params.Color.Get(ColorDefault), bWireframe, TEXT("%s"), *Params.LogStr);
				if (!Params.LogStr.IsEmpty())
				{
					VLog<LogCategory>(Params);
				}
				break;
			}
			case ECollisionShape::Capsule:
			{
				FVisualLogger::CapsuleLogf(
					Owner.Get(), LogCategory, Params.Verbosity,
					Transform.TransformPosition(FVector::DownVector * Shape.GetCapsuleHalfHeight()),
					Shape.GetCapsuleHalfHeight(), Shape.GetCapsuleRadius(),
					FQuat(Transform.GetRotation()),
					Params.Color.Get(ColorDefault), bWireframe, TEXT("%s"), *Params.LogStr);
				if (!Params.LogStr.IsEmpty())
				{
					VLog<LogCategory>(Params);
				}
				break;
			}
		}
	}

	if (bDoubleLog)
	{
		DrawCollisionShape<LogCategory>(Transform, Shape, bWireframe, Params);
	}
}

#else // @TODO: properly implement dummy methods
#define Log(...) Dummy();
#define VLog(...) Dummy();
#define DrawLine(...) Dummy();
#define DrawSphere(...) Dummy();
#define DrawCapsule(...) Dummy();
#define DrawSolidBox(...) Dummy();
#define DrawCollisionShape(...) Dummy();
#define DrawPrimitiveComponent(...) Dummy();
#define DrawTraceLine(...) Dummy();
#define DrawTraceSweep(...) Dummy();
#endif


#define SWI_SCOPE_OVERRIDE_DEBUG_COLOR(ColorType)\
struct FSwiScopeOverrideColor##ColorType\
{\
	FSwiDebugOutput& Debug;\
	const FColor SavedColor;\
\
	FSwiScopeOverrideColor##ColorType(FSwiDebugOutput& InDebug, const FColor& NewColor)\
		: Debug(InDebug)\
		, SavedColor(Debug.Color##ColorType)\
	{\
		Debug.Color##ColorType = NewColor;\
	}\
	~FSwiScopeOverrideColor##ColorType()\
	{\
		Debug.Color##ColorType = SavedColor;\
	}\
};

SWI_SCOPE_OVERRIDE_DEBUG_COLOR(Default);


SWI_SCOPE_OVERRIDE_DEBUG_COLOR(TraceResultNoHit);


SWI_SCOPE_OVERRIDE_DEBUG_COLOR(TraceResultHit);

#undef SWI_ENSURE_VALID
#undef SWI_LOGTAG_APPEND_LOGSTR
