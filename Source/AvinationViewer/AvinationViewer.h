// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#include <windows.h>
#define AVNSleep(x) Sleep(x);
#include "HideWindowsPlatformTypes.h"
#elif
#define AVNSleep(x) usleep(x);
#endif

