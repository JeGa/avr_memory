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

/**
 * Prints the given (null terminated) string to the serial console.
 * The string data is copied.
 */
void print(char *string);

/**
 * Reads a line from the serial console (more precise, from the buffer).
 */
char read();

//int byteToString(uint8_t byte, char *string);