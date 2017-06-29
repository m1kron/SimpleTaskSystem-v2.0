#pragma once

// Prints to given buffer.
void PrintToBuffer( char* buffer, size_t buffer_size, const char* format, ... );

// Prints to debug output.
void PrintToDebugOutput( const char* format, ... );