/***************************************************************************//**
 * @file
 * @brief Processes commands coming from the serial port.
 * See @ref commands for documentation.
 *
 * @deprecated command-interpreter is deprecated and will be removed in a future
 * release.  Use command-interpreter2 instead.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SILABS_COMMAND_INTERPRETER_H
#define SILABS_COMMAND_INTERPRETER_H

/** @addtogroup commands
 * Interpret serial port commands. See command-interpreter.h for source code.
 *
 * See the following application usage example followed by a brief explanation.
 * @code
 * static EmberCommandEntry commands[] = {
 *   { "integers",   integersCommand,   "u1u2s1s2", "integers"},
 *   { "intlist",    intListCommand,    "u1*",      "intlist"},
 *   { "stringlist", stringListCommand, "bb*",      "stringlist"},
 *   { NULL, NULL, NULL, NULL}}; // NULL action makes this a terminator
 *
 *
 * static void commandErrorHandler(EmberCommandState *state)
 * {
 *   // ...
 * }
 *
 *
 * ArgTypeU arguments[4];
 *
 * EmberCommandState testState =
 *   {
 *     commands,
 *     commandErrorHandler,
 *     arguments,
 *     4,                    // maximum command arguments
 *     10                    // integer base
 *   };
 *
 *
 *  emberCommandReaderInit(&testState);
 * @endcode
 *
 * -# Applications specify the commands that can be interpreted
 *    by defining a commands table of type ::EmberCommandEntry.
 *    The table includes the following information for each command:
 *   -# The full command name.
 *   -# Your application's function name that implements the command.
 *   -# An ::EmberCommandEntry::argumentTypes string specifies the number and types of arguments
 *      the command accepts.  See ::argumentTypes for details.
 *   -# A description string explains the command.
 *   .
 * -# Applications also need to define a command error handler to deal with
 *    incorrect command input.
 * -# The command table, the error handler, and other command interpreter
 *    state information are bundled into an ::EmberCommandState structure.
 *    Using a structure allows commands to be read from multiple sources.
 * -# The application provides an instance of the ::EmberCommandState structure to
 *     the interpreter by calling ::emberCommandReaderInit().
 *@{
 */

/** @name Command Table Settings
 *@{
 */
#ifndef EMBER_COMMENT_CHARACTER
/** The command table's comment indicator.
 *  A comment consists of a '#' followed by any characters up to an end-of-line.
 */
#define EMBER_COMMENT_CHARACTER     '#'
#endif

#ifndef EMBER_MAX_COMMAND_LENGTH
/** The maximum number of characters in a command.
 */
#define EMBER_MAX_COMMAND_LENGTH    12
#endif

/** @} END name group
 */

//Since the Cortex-M3 platform is native 32bit, we enable 32bit unsigned
//arguments in the command interpreter.  This is useful since any addresses or
//similar arguments are best supplied as 32bits.  All other platforms are both
//not native 32bit and even more resource constrained, so only use 16bit args.
#ifdef CORTEXM3
typedef uint32_t ArgTypeU;
  #define TEMP_GREATER_THAN_MAX_ARG_SIZE (0)
  #define PERFORM_BOUNDS_CHECKING (!((type == 'u') && (size == '4')))
#else //everything else needs to save space
typedef uint16_t ArgTypeU;
  #define TEMP_GREATER_THAN_MAX_ARG_SIZE (0xFFFF < temp)
  #define PERFORM_BOUNDS_CHECKING (1)
#endif

struct EmberCommandStateS;      // To allow forward use of this type.

typedef void (*CommandAction)(struct EmberCommandStateS *state);

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**@brief Command entry for a command table.
 */
typedef struct {
#else
typedef const struct {
#endif
  /** Use letters, digits, and underscores, '_', for the long command name.
   * Command names are case-sensitive.
   */
  const char * longName;
  /** A reference to a function in the application that implements the
   *  command.
   */
  CommandAction action;
  /** String that specifies the number and types of arguments the
   * command accepts.  The argument specifiers are:
   *  - b:   buffer
   *  - u1:  one-byte unsigned
   *  - u2:  two-byte unsigned
   *  - u4:  four-byte unsigned  (32 bit platforms only)
   *  - s1:  one-byte signed
   *  - s2:  two-byte signed
   *  - *:   zero or more of the previous type.
   *         If used, this must be the last specifier.
   *
   *  Buffer arguments are given as a string "...".  There is currently
   *  no way for including quote characters ',"",' in buffer arguments.  Buffer arguments
   * can also be given as a hex digit string using curly braces: {1A2B3C}.
   * The hex digits are converted to bytes in pairs.
   *
   *  Integer arguments can be either decimal or hexidecimal.  If the
   *  integerBase field of the command state is 16 then all integers are
   *  read as hexidecimal.  If integerBase is 10 then a decimal is the
   *  default and a 0x prefix indicates a hexidecimal integer.
   */
  const char * argumentTypes;
  /** A description of the command.
   */
  const char * description;
} EmberCommandEntry;

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief Command error states.
 *
 * If you change this list, ensure you also change the strings that describe
 * these errors in the array emberCommandErrorNames[] in command-interpreter.c.
 */
enum EmberCommandStatus
#else
typedef uint8_t EmberCommandStatus;
enum
#endif
{
  EMBER_CMD_SUCCESS,                          /*!< no error  */
  EMBER_CMD_ERR_PORT_PROBLEM,                 /*!< serial port error */
  EMBER_CMD_ERR_NO_BUFFER_AVAILABLE,          /*!< command processor busy */
  EMBER_CMD_ERR_NO_SUCH_COMMAND,              /*!< no such command */
  EMBER_CMD_ERR_WRONG_COMMAND_ARGUMENTS,      /*!< incorrect command arguments */
  EMBER_CMD_ERR_TOO_MANY_COMMAND_ARGUMENTS,   /*!< too many command arguments */
  EMBER_CMD_ERR_ARGUMENT_OUT_OF_RANGE,        /*!< integer argument out of range */
  EMBER_CMD_ERR_ARGUMENT_SYNTAX_ERROR,        /*!< argument syntax error */
  EMBER_CMD_ERR_STRING_TOO_LONG,              /*!< message too long */
  EMBER_CMD_ERR_NO_BINARY_STRING_TERMINATOR   /*!< missing binary message terminator */
};

extern const char * emberCommandErrorNames[];

/** @brief The command state structure.
 * Using a structure allows commands to be read from multiple sources.
 */
typedef struct EmberCommandStateS {
  /** The table of commands created in the application.
   */
  EmberCommandEntry *commands;
  void (* errorHandler)(struct EmberCommandStateS *state);
  /** Arguments filled in by the interpreter. */
  ArgTypeU *arguments;

  uint8_t maximumArguments;
  /** Determines whether integers are parsed using base 10 or base 16.
   *  Valid values are only 10 or 16.
   */
  uint8_t integerBase;
  /** Set by the interpreter for use by the command function. */
  uint8_t argumentCount;

  // The rest are internal settings
  //
  /** Finite-state machine's current state. */
  uint8_t state;
  /** First error found in this command. */
  uint8_t error;
  /** For reading in and remembering the current command.
   *  The "+ 1" is for the trailing null.
   */
  uint8_t command[EMBER_MAX_COMMAND_LENGTH + 1];

  uint16_t commandIndex;
  /** Stepped forward as arguments are read.  */
  const char * argumentSpecs;
  /** Bytes read in so far. */
  uint8_t stringLength;
  /** Remaining bytes in the binary block. */
  uint8_t binaryLength;

  EmberMessageBuffer stringBuffer;

  ArgTypeU integerValue;
  /** Valid values are '+' or '-'. */
  uint8_t integerSign;

  uint8_t currentIntegerBase;

  /** Used for pointing to a table of subcommands. */
  EmberCommandEntry *currentCommands;
} EmberCommandState;

/** @name Macros to Retrieve Arguments
 * Use the following macros in your functions that process commands to
 * retrieve arguments from the command interpreter state object.
 * These macros pull out unsigned integers, signed integers, and buffers.
 *@{
 */
#define emberUnsignedCommandArgument(state, index) \
  ((state)->arguments[index])

#define emberSignedCommandArgument(state, index) \
  ((int16_t) ((state)->arguments[index]))

#define emberBufferCommandArgument(state, index) \
  (LOW_BYTE((state)->arguments[index]))
/** @} END name group
 */

/** @brief Initialize a command state.
 * @param state The command state structure.
 */
void emberCommandReaderInit(EmberCommandState *state);

/** @brief Parses 'size' characters from 'input'.
 * @return true if an end of line character was read.
 * If the application uses a command line prompt,
 * this indeicates it is time to print the prompt.
 */
bool emberProcessCommandString(EmberCommandState *state,
                               uint8_t *input,
                               uint8_t size);

/** @brief A macro that parses any input coming in on the serial port,
 * which returns when no more input is available.
 * This is a wrapper for ::emberProcessCommandString() with a null
 * string and the port passed in as the string size.
 * @code
 * void emberProcessCommandInput(EmberCommandState *state, uint8_t port);
 * @endcode
 */
#define emberProcessCommandInput(state, port) \
  emberProcessCommandString(state, NULL, port)

/** @brief Return a pointer to the command name being executed
 * @param state The command state structure.
 */
#define emberCommandName(state) (state->command)

/** @} END addtogroup
 */

#endif //__COMMAND_INTERPRETER_H__
