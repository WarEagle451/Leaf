// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Logger.hpp>
#include <Leaf/Version.hpp>
#include <Leaf/Details/Registry.hpp>

#define LEAF_LOGGER

//! accomodate for this stuff
//! - Windows 10 Version 1511 update which unexpectedly implemented support for ANSI escape sequences
//! - This is not the default behavior and must be enabled programmatically with the Win32 API via SetConsoleMode(handle, ENABLE_VIRTUAL_TERMINAL_PROCESSING)
//! 
//! make trace, debug etc functions that can be used without creating a logger // will not include any sinks // only out to console
//! basically detach the submit funcs from the logger
//! 
//! Make a thing that will add the file name and and line // will be %F
//! 
//! TODO: improve the locking system, stuff that output to the console is #1 problem, for somereason it is slower than not multithreading