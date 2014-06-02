/**
 * @file shell.h
 *
 * This file provides functions to write strings to the serial console.
 * Also it has the shellTask() function which can be used for a very
 * basic shell.
 */

#pragma once

#define PRINT_HEADER "jefax> "

/**
 * This task can be scheduled.
 */
void shellTask();