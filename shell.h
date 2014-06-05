/**
 * @file shell.h
 *
 * This file provides the shellTask() function which can be used for a very
 * basic shell.
 */

#pragma once

#define PRINT_HEADER "jefax> "
#define HEADER_LIMIT 0x38 // ASCII: Column '8'

/**
 * This task can be scheduled.
 */
void shellTask();