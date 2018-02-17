#pragma once

#define OEMRESOURCE

#include <atomic>

#include <ppl.h>

#include <gsl/gsl>

// Included before windows.h, because pfc.h includes winsock2.h
#include "../pfc/pfc.h"

#include <windows.h>
#include <SHLWAPI.H>

#include "../foobar2000/SDK/foobar2000.h"

#include "../mmh/stdafx.h"
#include "../ui_helpers/stdafx.h"

#include "config_var.h"
#include "config_object.h"
#include "console.h"
#include "fcl.h"
#include "info_box.h"
#include "initquit.h"
#include "library.h"
#include "low_level_hook.h"
#include "main_thread_callback.h"
#include "sort.h"
#include "stream.h"
