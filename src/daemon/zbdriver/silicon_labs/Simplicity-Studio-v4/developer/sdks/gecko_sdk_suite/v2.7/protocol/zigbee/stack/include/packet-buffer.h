/***************************************************************************//**
 * @file
 * @brief Packet buffer allocation and management routines
 * See @ref packet_buffer for documentation.
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

// The old overview was for the wrong audience. A new overview should be written.

#ifndef SILABS_PACKET_BUFFER_H
#define SILABS_PACKET_BUFFER_H

/**
 * @addtogroup packet_buffer
 *
 * These functions implement a fixed-block-size memory management scheme
 * to store and manipulate EmberZNet packets. Buffers are identified to
 * clients with a 1-byte ID.
 *
 * Buffers can be linked together to create longer packets.
 * The utility procedures allow you to treat a linked chain
 * of buffers as a single buffer.
 *
 * Freeing a buffer automatically decrements the reference count of
 * any following buffer, possibly freeing the following buffer as well.
 *
 * Packet buffers may be allocated, held, and released.
 *
 * See packet-buffer.h for source code.
 * @{
 */

/** @brief Buffers hold 32 bytes. Defined as the log to ensure it
 * is a power of 2.
 */
#define LOG_PACKET_BUFFER_SIZE 5

/** @brief Buffers hold 32 bytes.
 */
#define PACKET_BUFFER_SIZE (1 << LOG_PACKET_BUFFER_SIZE)

/** @brief Provides the message buffer equivalent of NULL.*/
#define EMBER_NULL_MESSAGE_BUFFER 0xFF

/**
 * @brief Gets a pointer to a buffer's contents.  This pointer can be
 * used to access only the first ::PACKET_BUFFER_SIZE bytes in the buffer.
 * To read a message composed of multiple buffers, use
 * ::emberCopyFromLinkedBuffers().
 *
 * @param buffer  The buffer whose contents are desired.
 *
 * @return Returns a pointer to the contents of \c buffer.
 */
uint8_t *emberMessageBufferContents(EmberMessageBuffer buffer);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// An old name.
#define emberLinkedBufferContents(buffer) emberMessageBufferContents(buffer)
#endif

/** @brief Returns the length of a buffer. Implemented as a macro
 * for improved efficiency.
 *
 * @param buffer  A buffer.
 *
 * @return The buffer length.
 */
#define emberMessageBufferLength(buffer)   (emMessageBufferLengths[buffer])

/** @brief Sets the length of a buffer.
 *
 * When asserts are enabled,
 * attempting to set a length greater than the size of the buffer triggers
 * an assert.
 *
 * @param buffer     A buffer
 *
 * @param newLength  The length to set the buffer to.
 */
void emberSetMessageBufferLength(EmberMessageBuffer buffer, uint8_t newLength);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifdef EM_DEBUG_BUFFER_USE
#define EM_BUFFER_DEBUG(X) X
#define EM_BUFFER_FILE_LOC , __FILE__, __LINE__
#define EM_BUFFER_FILE_DECL , char *file, int line
#define EM_BUFFER_FILE_VALUE , file, line
#else
#define EM_BUFFER_DEBUG(X)
#define EM_BUFFER_FILE_LOC
#define EM_BUFFER_FILE_DECL
#define EM_BUFFER_FILE_VALUE
#endif

#ifdef EMBER_TEST
#define EM_ASSERT_IS_NOT_FREE(buffer) \
  assert(emMessageBufferReferenceCounts[(buffer)] > 0)
#define EM_ASSERT_IS_VALID_BUFFER(buffer) \
  assert(buffer < emPacketBufferCount)
#else
#define EM_ASSERT_IS_NOT_FREE(buffer)
#define EM_ASSERT_IS_VALID_BUFFER(buffer)
#endif  // EMBER_TEST
#endif  // DOXYGEN_SHOULD_SKIP_THIS

/** @brief Holds a message buffer by incrementing its reference count.
 * Implemented as a macro for improved efficiency.
 *
 * @param buffer  A buffer.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
void emberHoldMessageBuffer(EmberMessageBuffer buffer);
#else

#ifdef EMBER_TEST
#define emberHoldMessageBuffer(buffer)                               \
  do {                                                               \
    EmberMessageBuffer EM_HOLD_BUFFER_TEMP_XXX = (buffer);           \
    EM_ASSERT_IS_NOT_FREE(EM_HOLD_BUFFER_TEMP_XXX);                  \
    EM_ASSERT_IS_VALID_BUFFER(EM_HOLD_BUFFER_TEMP_XXX);              \
    emHoldMessageBuffer(EM_HOLD_BUFFER_TEMP_XXX EM_BUFFER_FILE_LOC); \
  } while (false)

#else
#define emberHoldMessageBuffer(buffer) emHoldMessageBuffer(buffer)
#endif // EMBER_TEST

void emHoldMessageBuffer(EmberMessageBuffer buffer EM_BUFFER_FILE_DECL);
#endif

/** @brief Releases a message buffer by decrementing its reference
 * count. Implemented as a macro for improved efficiency.
 *
 * @param buffer  A buffer.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
void emberReleaseMessageBuffer(EmberMessageBuffer buffer);
#else

#if defined(EMBER_TEST)
#define emberReleaseMessageBuffer(buffer)                                  \
  do {                                                                     \
    EmberMessageBuffer EM_RELEASE_BUFFER_TEMP_XXX = (buffer);              \
    EM_ASSERT_IS_NOT_FREE(EM_RELEASE_BUFFER_TEMP_XXX);                     \
    EM_ASSERT_IS_VALID_BUFFER(EM_RELEASE_BUFFER_TEMP_XXX);                 \
    emReleaseMessageBuffer(EM_RELEASE_BUFFER_TEMP_XXX EM_BUFFER_FILE_LOC); \
  } while (false)
#else
#define emberReleaseMessageBuffer(buffer) emReleaseMessageBuffer(buffer)
#endif // EMBER_TEST

void emReleaseMessageBuffer(EmberMessageBuffer buffer EM_BUFFER_FILE_DECL);
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifndef DOXYGEN_SHOULD_SKIP_THIS
extern uint8_t emPacketBufferCount;
extern uint8_t *emPacketBufferData;
extern uint8_t *emMessageBufferLengths;
extern uint8_t *emMessageBufferReferenceCounts;
extern EmberMessageBuffer *emPacketBufferLinks;
extern EmberMessageBuffer *emPacketBufferQueueLinks;
#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @name Buffer Functions
 */
//@{

/**
 * @brief Returns the buffer that follows this one in the message.
 *  ::EMBER_NULL_MESSAGE_BUFFER is returned if there is no following buffer.
 *
 * @param buffer  The buffer whose following buffer is desired.
 *
 * @return Returns the buffer that follows \c buffer in the message.
 *  ::EMBER_NULL_MESSAGE_BUFFER is returned if there is no following buffer.
 */
#define emberStackBufferLink(buffer) \
  (emPacketBufferLinks[(buffer)])

/**
 * @brief Sets the buffer following this one in the message. The
 *  final buffer in the message has ::EMBER_NULL_MESSAGE_BUFFER as its link.
 *
 * @param buffer   The buffer whose link is to be set.
 *
 * @param newLink  The buffer that is to follow \c buffer.
 */
#define emberSetStackBufferLink(buffer, newLink) \
  (emPacketBufferLinks[(buffer)] = (newLink))

/**
 * @brief Allocates a stack buffer.
 *
 * @return A newly allocated buffer, or ::EMBER_NULL_MESSAGE_BUFFER if no buffer
 * is available.
 */
#define emberAllocateStackBuffer() (emberAllocateLinkedBuffers(1))

/**
 * @brief Allocates one or more linked buffers.
 *
 * @param count  The number of buffers to allocate.
 *
 * @return The first buffer in the newly allocated chain of buffers, or
 * ::EMBER_NULL_MESSAGE_BUFFER if insufficient buffers are available.
 */
EmberMessageBuffer emberAllocateLinkedBuffers(uint8_t count);

/**
 * @brief Allocates a stack buffer and fills the buffer with data
 * passed in the function call.
 *
 * @param count  The buffer length.
 *
 * @param ...   \c count bytes, which will be placed in the buffer.
 *
 * @return A newly allocated buffer, or ::EMBER_NULL_MESSAGE_BUFFER if no buffer
 * is available.
 */
EmberMessageBuffer emberFillStackBuffer(unsigned int count, ...);
//@} //END BUFFER UTILITIES

/**
 * @name Linked Buffer Utilities
 * The plural "buffers" in the names of these procedures is a reminder that
 * they deal with linked chains of buffers.
 */
//@{

/**
 * @brief Allocates a chain of stack buffers sufficient to hold
 * \c length bytes of data and fills the buffers with the data in contents.
 * If the value of contents is NULL, the buffers are allocated but
 * not filled.
 *
 * @param contents  A pointer to data to place in the allocated buffers.
 *
 * @param length    The buffer length.
 *
 * @return The first buffer in a series of linked stack buffers, or
 * ::EMBER_NULL_MESSAGE_BUFFER if insufficient buffers are available.
 */
EmberMessageBuffer emberFillLinkedBuffers(uint8_t *contents, uint8_t length);

/**
 * @brief Copies a specified number of bytes of data into a buffer,
 * starting at a specified index in the buffer array. Buffer links are
 * followed as required. No buffers are allocated or released.
 *
 * @param contents    A pointer to data to copy into the buffer.
 *
 * @param buffer      The buffer to copy data into.
 *
 * @param startIndex  The buffer index at which copying should start.
 *
 * @param length      The number of bytes of data to copy.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
void
emberCopyToLinkedBuffers(const uint8_t *contents,
                         EmberMessageBuffer buffer,
                         uint8_t startIndex,
                         uint8_t length);
#else
#define emberCopyToLinkedBuffers(contents, buffer, startIndex, length) \
  emReallyCopyToLinkedBuffers( (const uint8_t *)(contents), (buffer), (startIndex), (length), 1)

void
emReallyCopyToLinkedBuffers(const uint8_t * contents,
                            EmberMessageBuffer buffer,
                            uint8_t startIndex,
                            uint8_t length,
                            uint8_t direction);
#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @brief Copies \c length bytes of data from a buffer
 * to \c contents, starting at a specified index in the buffer array.
 * Buffer links are followed as required.
 *
 * @param buffer      The buffer to copy data from.
 *
 * @param startIndex  The buffer index at which copying should start.
 *
 * @param contents    A pointer to data to copy from the buffer.
 *
 * @param length      The number of bytes of data to copy.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
// To save flash, this shares an implementation with emberCopyToLinkedBuffers().
void
emberCopyFromLinkedBuffers(EmberMessageBuffer buffer,
                           uint8_t startIndex,
                           uint8_t *contents,
                           uint8_t length);
#else
#define emberCopyFromLinkedBuffers(buffer, startIndex, contents, length) \
  emReallyCopyToLinkedBuffers( (const uint8_t *)(contents), (buffer), (startIndex), (length), 0)
#endif

/**
 * @brief Copies a specified number of bytes of data from one buffer into
 * another. Buffer links are followed as required. No buffers are allocated or
 * released.
 *
 * @param to         The buffer to copy data into.
 * @param toIndex    The position in the destination buffer at which copying
 *                   should start.
 * @param from       The buffer to copy data from.
 * @param fromIndex  The position in the source buffer at which copying should
 *                   start.
 * @param count      The number of bytes of data to copy.
 */
void emberCopyBufferBytes(EmberMessageBuffer to,
                          uint16_t toIndex,
                          EmberMessageBuffer from,
                          uint16_t fromIndex,
                          uint16_t count);

void emberMoveBufferBytes(EmberMessageBuffer to,
                          uint16_t toIndex,
                          EmberMessageBuffer from,
                          uint16_t fromIndex,
                          uint16_t count);

/**
 * @brief Appends \c length bytes from \c contents onto a buffer.
 * Combines the functionality of ::setPacketBuffersLength() and
 * ::copyToPacketBuffers().
 *
 * @param buffer    The buffer to append data to.
 *
 * @param contents  A pointer to data to append.
 *
 * @param length    The number of bytes of data to append.
 *
 * @return ::EMBER_SUCCESS if sufficient buffers are available, and
 *  ::EMBER_NO_BUFFERS if not.
 */
EmberStatus emberAppendToLinkedBuffers(EmberMessageBuffer buffer,
                                       uint8_t *contents,
                                       uint8_t length);

/**
 * @brief Appends a string from program space (flash) to a buffer.
 *
 * @param buffer  The buffer to append data to.
 *
 * @param suffix  The string in program space to append.
 *
 * @return ::EMBER_SUCCESS if sufficient buffers are available, and
 *  ::EMBER_NO_BUFFERS if not.
 */
EmberStatus emberAppendPgmStringToLinkedBuffers(EmberMessageBuffer buffer, const char * suffix);

/**
 * @brief Sets the length of a chain of buffers, adding or removing
 * trailing buffers as needed.
 *
 * @param buffer  The buffer whose length is to be set.
 *
 * @param length  The length to set.
 *
 * @return ::EMBER_SUCCESS if changing \c buffer's length by \c length bytes
 *  does not require additional buffers or if sufficient buffers are available,
 *  and ::EMBER_NO_BUFFERS if not.
 */
EmberStatus emberSetLinkedBuffersLength(EmberMessageBuffer buffer, uint8_t length);

/**
 * @brief Gets a pointer to a specified byte in a linked list of buffers.
 *
 * @param buffer  The buffer that the requested byte must come from.
 *
 * @param index   The index of the requested byte.
 *
 * @return A pointer to the requested byte.
 */
uint8_t *emberGetLinkedBuffersPointer(EmberMessageBuffer buffer, uint8_t index);

/**
 * @brief Gets a specified byte in a linked list of buffers.
 *
 * @param buffer  The buffer that the requested byte must come from.
 *
 * @param index   The index of the requested byte.
 *
 * @return A byte.
 */
uint8_t emberGetLinkedBuffersByte(EmberMessageBuffer buffer, uint8_t index);

/**
 * @brief Sets the indexed byte in a linked list of buffers.
 *
 * @param buffer  The buffer holding the byte to be set.
 *
 * @param index   The index of the byte to set.

 * @param byte    The value to set the byte to.
 */
void emberSetLinkedBuffersByte(EmberMessageBuffer buffer, uint8_t index, uint8_t byte);

/**
 * @brief Gets a little-endian 2-byte value from a linked list of buffers.
 *
 * @param buffer  The buffer containing the 2-byte value.
 *
 * @param index   The index of the low byte.
 *
 * @return The 2-byte value.
 */
uint16_t emberGetLinkedBuffersLowHighInt16u(EmberMessageBuffer buffer,
                                            uint8_t index);

/**
 * @brief Sets a little-endian 2-byte value in a linked list of buffers.
 *
 * @param buffer  The buffer to set the 2-byte value in.
 *
 * @param index   The index of the low byte.

 * @param value   The 2-byte value to set.
 */
void emberSetLinkedBuffersLowHighInt16u(EmberMessageBuffer buffer,
                                        uint8_t index,
                                        uint16_t value);

/**
 * @brief Gets a little-endian 4-byte value from a linked list of buffers.
 *
 * @param buffer  The buffer containing the 4-byte value.
 *
 * @param index   The index of the low byte.
 *
 * @return The 4-byte value.
 */
uint32_t emberGetLinkedBuffersLowHighInt32u(EmberMessageBuffer buffer,
                                            uint8_t index);

/**
 * @brief Sets a little-endian 4-byte value in a linked list of buffers.
 *
 * @param buffer  The buffer to set the 2-byte value in.
 *
 * @param index   The index of the low byte.
 *
 * @param value   The 4-byte value to set.
 */
void emberSetLinkedBuffersLowHighInt32u(EmberMessageBuffer buffer,
                                        uint8_t index,
                                        uint32_t value);

/**
 * @brief Copies a chain of linked buffers.
 *
 * @param buffer  The first buffer in the chain to copy.
 *
 * @return A newly created copy of the \c buffer chain.
 */
EmberMessageBuffer emberCopyLinkedBuffers(EmberMessageBuffer buffer);

/**
 * @brief Creates a new, unshared copy of a specified buffer, if that
 * buffer is shared.  If it isn't shared, increments the reference count by
 * 1 so that the user of the returned buffer can release it in either case.
 *
 * @param buffer    The buffer to copy.
 *
 * @param isShared  A flag indicating whether the buffer is shared.
 *
 * @return A fresh copy of \c buffer if \c isShared is true, and \c buffer
 *  if \c isShared is not true.
 */
EmberMessageBuffer emberMakeUnsharedLinkedBuffer(EmberMessageBuffer buffer, bool isShared);

//@} //END OF LINKED BUFFERS UTILITIES

/**
 * @brief Retrieves the current number of free packet buffers.
 *
 * @return The number of free packet buffers.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
extern uint8_t emPacketBufferFreeCount;
#define emberPacketBufferFreeCount() (emPacketBufferFreeCount)
#else
uint8_t emberPacketBufferFreeCount(void);
#endif

/** @} END addtogroup */

#endif  // SILABS_PACKET_BUFFER_H
