/**
 * @file shell.h
 *
 * This file provides functions to write strings to the serial console.
 * Also it has the shellTask() function which can be used for a very
 * basic shell.
 */

#pragma once

#define PRINT_HEADER "jefax> "

void shellTask();

/**
 * Prints the given (null terminated) string to the serial console.
 */
void print(char *string);

char *read();