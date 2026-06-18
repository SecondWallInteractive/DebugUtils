# DebugUtils

Unreal Engine runtime plugin: a unified debug-output helper that fans a single
call out to `UE_LOG`, the Visual Logger, and `DrawDebug*` shapes, gated per
log-tag via console variables (`Swi.LogTags.*`).

## Install

Vendor (copy) the plugin into a host project's `Plugins/` folder:

```
<Project>/Plugins/DebugUtils/
```

Then regenerate project files and rebuild. The module is `Runtime`, loaded at
`Default` phase. Add `"DebugUtils"` to your module's `*.Build.cs`
`PublicDependencyModuleNames` / `PrivateDependencyModuleNames` to use it.

## Usage

```cpp
#include "SwiDebugOutput.h"

// Define a tag (toggle at runtime via cvar Swi.LogTags.Movement 1)
namespace Swi::LogTags { SWI_DEFINE_LOG_TAG_INLINE(Movement); }

FSwiDebugOutput Debug;
Debug.SetUp(this);                     // Owner + World
Debug.Log<LogTemp>({ .LogTag = &Swi::LogTags::Movement.Name,
                     .LogStr = TEXT("hello") });
```

## Notes

- Active only when `SWI_ENABLE_DEBUG_OUTPUT` (`UE_ENABLE_DEBUG_DRAWING && ENABLE_VISUAL_LOG`) is true.
- `UE_EMPTY` / `TEXT_EMPTY` are defined here with `#ifndef` guards; a host project
  that already defines them keeps its own.

Copyright (c) 2025–present Second Wall Interactive. All Rights Reserved.
