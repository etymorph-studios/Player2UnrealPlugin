/// Copyright (c) 2025 Etymorph Studios, LLC
/// This software is licensed under the MIT License

#pragma once

#include "UObject/NoExportTypes.h"


DECLARE_LOG_CATEGORY_EXTERN(P2_Logger, Log, All);

#define PLAYER2_LOG(Verbosity, Format, ...) UE_LOG( P2_Logger, Verbosity, Format, ##__VA_ARGS__)

#define P2_LOG_VV(Format, ...) PLAYER2_LOG(VeryVerbose, Format, ##__VA_ARGS__);
#define P2_LOG_V(Format, ...) PLAYER2_LOG(Verbose, Format, ##__VA_ARGS__);
#define P2_LOG_I(Format, ...) PLAYER2_LOG(Log, Format, ##__VA_ARGS__);
#define P2_LOG_W(Format, ...) PLAYER2_LOG(Warning, Format, ##__VA_ARGS__);
#define P2_LOG_E(Format, ...) PLAYER2_LOG(Error, Format, ##__VA_ARGS__);
