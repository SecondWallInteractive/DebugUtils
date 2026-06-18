// Copyright (c) 2025–present Second Wall Interactive. All Rights Reserved.

#include "SwiDebugOutput.h"

#define SWI_ENSURE_VALID()\
	if (!ensure(IsValid())) \
	{ \
		return; \
	}


FSwiDebugOutput::FSwiDebugOutput ()
{
	RegisteredTags.Add(Swi::LogTags::Default);
}

bool FSwiDebugOutput::IsValid () const
{
	return Owner.IsValid() && World.IsValid();
}

void FSwiDebugOutput::SetUp (const UObject* InOwner, const UWorld* InWorld)
{
	Owner = InOwner;
	World = InWorld;
	if (!World.IsValid())
	{
		World = Owner.IsValid() ? Owner->GetWorld() : nullptr;
	}

	SWI_ENSURE_VALID();
}

bool FSwiDebugOutput::IsLogTagEnabled (const FString& LogTagName) const
{
	return !!Algo::FindByPredicate(
		RegisteredTags,
		[LogTagName] (const Swi::LogTags::FSwiLogTag& Tag) -> bool
		{
			return Tag.Name == LogTagName && Tag.bEnabled;
		});
}

void FSwiDebugOutput::DrawLine (const FVector& Start, const FVector& End, const FString* LogTag) const
{
	SWI_ENSURE_VALID();

	SWI_CHECK_LOG_TAG_WITH_RETURN(LogTag);

	DrawDebugLine(World.Get(), Start, End, ColorDefault, false, DefaultDuration);
}

#undef SWI_ENSURE_VALID
