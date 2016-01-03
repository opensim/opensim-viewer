// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#include <windows.h>
#include <thread>
#define usleep(x) std::this_thread::sleep_for(std::chrono::microseconds(x));
#include "HideWindowsPlatformTypes.h"
#endif

