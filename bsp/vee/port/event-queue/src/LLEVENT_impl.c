/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include <LLEVENT_impl.h>
#include <event_configuration.h>
#include <osal.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define EVENT_QUEUE_NAME ((uint8_t *)"event-queue")
#define EVENT_QUEUE_MUTEX_NAME ((uint8_t *)"event-queue-mutex")

// Private functions
static jbyte read_one_byte(void);
static jshort read_two_bytes(void);
static jint read_four_bytes(void);
static jlong read_eight_bytes(void);

// Declare the Message Queue.
OSAL_queue_declare(event_queue, EVENT_QUEUE_SIZE);
static OSAL_queue_handle_t event_queue_handle;

// Initialize the mutex used when sending data into event_queue.
static OSAL_mutex_handle_t mutex_send_event;

static int32_t waiting_receive_java_thread_id = SNI_ERROR;

/**
 * True if the event queue was already initialized, false otherwise.
 */
static bool event_queue_initialized = false;

/**
 * Management of the extended event reading:
 * 	- data_length_extended_data = the number of bytes of the extended event.
 * 	- offset_extended_data_read = the number of bytes read by the Java listener.
 */
static uint32_t data_length_extended_data;
static uint32_t offset_extended_data_read;

/**
 * Management of the bytes remaining after getting an uint32_t (4 bytes buffer) from the queue:
 * 	- buffer_extended_data = the bytes remaining from the 4 bytes buffer read in the queue.
 * 	- offset_buffer_extended_data = The number of bytes already read from buffer_extended_data.
 * 									-1 if the buffer_extended_data is empty.
 */
static uint32_t buffer_extended_data;
static int8_t offset_buffer_extended_data;

/**
 * Long value is 64 bits and is build from 2 uint32_t. (@see LLEVENT_IMPL_readLong)
 *  - first_long_value = store the first uint32_t read from the event_queue.
 */
static uint32_t first_long_value;

/**
 * A counter to remember how many bytes have been skipped. (@see LLEVENT_IMPL_skipBytes)
 */
static uint32_t skip_bytes_counter;

/**
 * A static value to know if it is 4th or 8 bytes aligned.
 * If 0 -> 4 bytes aligned.
 * If 1 -> 8 bytes aligned.
 */
static uint8_t data_alignment;

/**
 * Starts the event pump.
 */
void LLEVENT_IMPL_initialize(void) {
	OSAL_status_t res = OSAL_queue_create(EVENT_QUEUE_NAME, &event_queue_handle, event_queue);
	if (res != OSAL_OK) {
		LLEVENT_ERROR_TRACE("ERROR during EventQueue.getInstance(): Cannot create queue (%d)\n", res);
		if (SNI_throwNativeIOException(EVENT_NOK, "Cannot create queue.") == SNI_ERROR) {
			LLEVENT_ERROR_TRACE(
				"This function is not called within the virtual machine task or if the current thread is suspended.\n");
		}
	}

	res = OSAL_mutex_create(EVENT_QUEUE_MUTEX_NAME, &mutex_send_event);
	if (res != OSAL_OK) {
		LLEVENT_ERROR_TRACE("ERROR during EventQueue.getInstance(): Cannot create mutex (%d)\n", res);
		if (SNI_throwNativeIOException(EVENT_NOK, "Cannot create mutex.") == SNI_ERROR) {
			LLEVENT_ERROR_TRACE(
				"This function is not called within the virtual machine task or if the current thread is suspended.\n");
		}
	}

	event_queue_initialized = true;
	data_length_extended_data = 0;
	offset_extended_data_read = 0;

	buffer_extended_data = (uint32_t)NULL;
	offset_buffer_extended_data = -1;

	first_long_value = 0;

	skip_bytes_counter = 0;
}

/**
 * Offers an event to the queue.
 *
 * @param type the type of the event.
 * @param data the data of the event.
 * @return true if the message has been sent, false otherwise.
 */
bool LLEVENT_IMPL_offer_event(uint32_t type, uint32_t data) {
	// The boolean to return at the end of the method.
	jboolean offer_status = JTRUE;

	// Don't send the event if the event queue has not been initialized by Managed World (mutex/queue not initialized).
	if (event_queue_initialized) {
		// Create the message from the type and the data.
		// Make sure that the first bit is 0 because it is not an extended event.
		uint32_t event_message = ((type << (uint32_t)24) | data) & ((uint32_t)0x7FFFFFFF);

		// Take the mutex before sending the event.
		OSAL_mutex_take(&mutex_send_event, OSAL_INFINITE_TIME);

		// Send the event into the queue.
		OSAL_status_t res = OSAL_queue_post(&event_queue_handle, (void *)event_message);
		if (res != OSAL_OK) {
			LLEVENT_ERROR_TRACE("ERROR while trying to send an event in the EventQueue. (errno = %s)\n",
			                    strerror(errno));
			offer_status = JFALSE;
		}

		// Free the mutex after sending the event.
		OSAL_mutex_give(&mutex_send_event);

		// If a Java thread is waiting to read an event, notify it.
		if (SNI_ERROR != waiting_receive_java_thread_id) {
			if (SNI_ERROR == SNI_resumeJavaThread(waiting_receive_java_thread_id)) {
				// Java thread ID is invalid.
				LLEVENT_ERROR_TRACE(
					"ERROR while trying to resume the EventQueue waiting thread: The Java Thread ID is invalid, can't resume the Event Queue waiting thread.\n");
			}
		} else {
			LLEVENT_DEBUG_TRACE("no thread waiting for events\n");
		}
	}

	LLEVENT_DEBUG_TRACE("Return status %d\n", offer_status);
	return offer_status;
}

/**
 * Offers an extended event to the queue.
 *
 * @param type the type of the event.
 * @param data the data of the event.
 * @return true if the message has been sent, false otherwise.
 */
bool LLEVENT_IMPL_offer_extended_event(uint32_t type, const void *data, uint32_t data_length) {
	// The boolean to return at the end of the method.
	jboolean offer_status = JTRUE;

	// Don't send the event if the event queue has not been initialized by Managed World (mutex/queue not initialized).
	if (event_queue_initialized) {
		// Create the first uint32_t of the extended event that contain the type and the data length (number of bytes).
		uint32_t event_message = ((uint32_t)0x1 << (uint32_t)31) | (type << (uint32_t)24) | data_length;

		// Convert the data.
		const uint32_t *event_data = (const uint32_t *)data;

		// Get the number of data messages to send in the event queue -> Number of bytes / sizeof(int) rounded up.
		int data_length_int = (int)ceil((float)(data_length / (float)sizeof(int)));

		// Take the mutex before sending the extended event.
		OSAL_mutex_take(&mutex_send_event, OSAL_INFINITE_TIME);

		// Send the first part of the event in the queue.
		if (offer_status == (jboolean)JTRUE) {
			OSAL_status_t res = OSAL_queue_post(&event_queue_handle, (void *)event_message);
			if (res != OSAL_OK) {
				LLEVENT_ERROR_TRACE("ERROR while trying to send an event in the EventQueue. (errno = %s)\n",
				                    strerror(errno));
				offer_status = JFALSE;
			}
		}

		// Send the data of the extended event through the queue.
		if (offer_status == (jboolean)JTRUE) {
			for (int i = 0; i < data_length_int; i++) {
				OSAL_status_t res = OSAL_queue_post(&event_queue_handle, (void *)event_data[i]);
				if (res != OSAL_OK) {
					LLEVENT_ERROR_TRACE(
						"ERROR while trying to send an extended event in the EventQueue. (errno = %s)\n",
						strerror(errno));
					offer_status = JFALSE;
				}
			}
		}

		// Free the mutex after sending the extended event.
		OSAL_mutex_give(&mutex_send_event);

		// If a Java thread is waiting to read an event, notify it.
		if (waiting_receive_java_thread_id != SNI_ERROR) {
			if (SNI_resumeJavaThread(waiting_receive_java_thread_id) == SNI_ERROR) {
				// Java thread ID is invalid.
				LLEVENT_ERROR_TRACE(
					"ERROR while trying to resume the EventQueue waiting thread: The Java Thread ID is invalid, can't resume the Event Queue waiting thread.\n");
			}
		} else {
			LLEVENT_DEBUG_TRACE("no thread waiting for events\n");
		}
	}

	return offer_status;
}

/**
 * Waits for an event from the queue.
 *
 * If an event is available, this function return the event.
 *
 * @return the event
 */
uint32_t LLEVENT_IMPL_wait_event(void) {
	// Get the thread Id in case the thread is suspended.
	waiting_receive_java_thread_id = SNI_getCurrentJavaThreadID();

	LLEVENT_DEBUG_TRACE("thread id %u waiting for events\n", waiting_receive_java_thread_id);

	jboolean wait_status = JFALSE;
	void *event_message = NULL;

	OSAL_status_t res = OSAL_queue_fetch(&event_queue_handle, (void **)&event_message, 0);
	if (res != OSAL_OK) {
		// OSAL will return OSAL_ERROR when the queue is empty
		LLEVENT_INFO_TRACE("event queue is empty\n");
	} else {
		wait_status = JTRUE;
	}

	if (!wait_status) {
		if (SNI_OK != SNI_suspendCurrentJavaThreadWithCallback(0, (SNI_callback)LLEVENT_IMPL_wait_event, NULL)) {
			// This function is not called within the virtual machine task or an exception is pending.
			LLEVENT_ERROR_TRACE("Error while trying to suspend EventQueue thread. (errno = %s)\n", strerror(errno));
			LLEVENT_ERROR_TRACE(
				"The Event Queue is not called within the virtual machine task or an exception is pending.\n");
			LLEVENT_ERROR_TRACE("An event has been returned with id 0 and data 0.\n");
			waiting_receive_java_thread_id = SNI_ERROR;
			// Set the returned event to 0.
			event_message = 0;
		}
	} else {
		waiting_receive_java_thread_id = SNI_ERROR;
	}

	LLEVENT_DEBUG_TRACE("Return event message 0x%08x\n", (uint32_t)event_message);
	return (uint32_t)event_message;
}

/**
 * Starts to read an extended data. Set the data_length_extended_data to the data_length and reset the
 * offset_extended_data_read.
 * At this point, the data is 8 bytes aligned.
 */
void LLEVENT_IMPL_start_read_extended_data(uint32_t data_length) {
	data_length_extended_data = data_length;
	offset_extended_data_read = 0;

	data_alignment = 1;
}

/**
 * The Java listener finished to read the data from the event queue.
 * If there is any left data left in the queue, purge it.
 */
void LLEVENT_IMPL_end_read_extended_data(void) {
	// If there is still extended data inside event_queue, purge it.
	if (LLEVENT_IMPL_skip_bytes(data_length_extended_data - offset_extended_data_read) == -1) {
		LLEVENT_INFO_TRACE("The Java listener did not finished properly. The message queue may not be purged.\n");
	}

	// reset the data length and the offset.
	data_length_extended_data = 0;
	offset_extended_data_read = 0;
	// reset the buffer used to read bytes of an extended event.
	buffer_extended_data = (uint32_t)NULL;
	offset_buffer_extended_data = -1;
}

/**
 * Returns the next boolean of data.
 * Throws IOException if there is no boolean remaining in the extended data.
 */
jboolean LLEVENT_IMPL_read_boolean(void) {
	return (jboolean)read_one_byte();
}

/**
 * Returns the next byte of data.
 * Throws IOException if there is no byte remaining in the extended data.
 */
jbyte LLEVENT_IMPL_read_byte(void) {
	return read_one_byte();
}

/**
 * Returns the next char of data.
 * Throws IOException if there is no char remaining in the extended data.
 */
jchar LLEVENT_IMPL_read_char(void) {
	return (jchar)read_two_bytes();
}

/**
 * Returns the next double of data.
 * Throws IOException if there is no double remaining in the extended data.
 */
jdouble LLEVENT_IMPL_read_double(void) {
	// Union that converts an long to a double.
	union {
		jlong long_value;
		jdouble double_value;
	} event_double;

	event_double.long_value = read_eight_bytes();

	return event_double.double_value;
}

/**
 * Returns the next float of data.
 * Throws IOException if there is no float remaining in the extended data.
 */
jfloat LLEVENT_IMPL_read_float(void) {
	// Union that converts an int to a float.
	union {
		jint int_value;
		jfloat float_value;
	} event_float;

	event_float.int_value = read_four_bytes();

	return event_float.float_value;
}

/**
 * Reads len bytes of data from the event queue and store it into the buffer at the offset off.
 * Throws IOException if there are not enough bytes available or if the buffer is too small.
 */
jint LLEVENT_IMPL_read(uint8_t *b, uint32_t off, uint32_t len) {
	// Status of the method.
	jboolean read_status = JTRUE;

	uint32_t remaining_bytes = LLEVENT_IMPL_available();
	jint byte_read = 0;

	// If not enough bytes available, throw a native IOException.
	if (remaining_bytes < len) {
		if (SNI_throwNativeIOException(EVENT_NOK, "No bytes remaining in the extended event.") == SNI_ERROR) {
			LLEVENT_ERROR_TRACE("ERROR during EventDataReader.read: No bytes remaining in the extended event.\n");
			LLEVENT_ERROR_TRACE(
				"This function is not called within the virtual machine task or if the current thread is suspended.\n");
		}
		read_status = JFALSE;
	}

	// If the buffer is too small to store the len bytes of data, throw a native IOException.
	if (off + len > SNI_getArrayLength(b) && read_status == (jboolean)JTRUE) {
		if (SNI_throwNativeIOException(EVENT_NOK, "The buffer is too small to store the event data.") == SNI_ERROR) {
			LLEVENT_ERROR_TRACE(
				"ERROR during EventDataReader.read: The buffer is too small to store the event data.\n");
			LLEVENT_ERROR_TRACE(
				"This function is not called within the virtual machine task or if the current thread is suspended.\n");
		}
		read_status = JFALSE;
	}

	// Read the len bytes from the event queue and store them in the buffer at the offset off.
	if (read_status == (jboolean)JTRUE) {
		for (uint32_t i = 0; i < len; i++) {
			jbyte read_byte = read_one_byte();
			// If an SNI exception occurs during reading, stop reading and return.
			if (SNI_isExceptionPending()) {
				break;
			}
			b[off + i] = read_byte;
			byte_read++;
		}
	}

	return byte_read;
}

/**
 * Returns the next integer of data.
 * Throws IOException if there is no integer remaining in the extended data.
 */
jint LLEVENT_IMPL_read_int(void) {
	return read_four_bytes();
}

/**
 * Returns the next long of data.
 * Throws IOException if there is no long remaining in the extended data.
 */
jlong LLEVENT_IMPL_read_long(void) {
	return read_eight_bytes();
}

/**
 * Returns the next short of data.
 * Throws IOException if there is no short remaining in the extended data.
 */
jshort LLEVENT_IMPL_read_short(void) {
	return read_two_bytes();
}

/**
 * Returns the next unsigned byte of data.
 * Throws IOException if there is no unsigned byte remaining in the extended data.
 */
jboolean LLEVENT_IMPL_read_unsigned_byte(void) {
	return (jboolean)read_one_byte();
}

/**
 * Returns the next unsigned short of data.
 * Throws IOException if there is no unsigned short remaining in the extended data.
 */
jchar LLEVENT_IMPL_read_unsigned_short(void) {
	return (jchar)read_two_bytes();
}

/**
 * Skips n bytes.
 * Returns -1 if it did not work.
 */
jint LLEVENT_IMPL_skip_bytes(uint32_t n) {
	// Status of the method.
	jint skip_status = EVENT_OK;

	// Number of byte skipped.
	skip_bytes_counter = 0;

	// If less than n bytes available, return -1.
	if (LLEVENT_IMPL_available() < n) {
		skip_status = EVENT_NOK;
	}

	// Read n bytes from the event queue.
	if (skip_status != EVENT_NOK) {
		while (skip_bytes_counter < n) {
			// Unused variable skipped_byte because the value returned by a function having non-void return value
			// shall be used according to rule misra-c2012-17.7.
			jbyte skipped_byte = read_one_byte();
			// If an SNI exception occurs during reading, stop skipping and return -1.
			if (SNI_isExceptionPending()) {
				if (SNI_clearPendingException() == SNI_ERROR) {
					LLEVENT_ERROR_TRACE("ERROR while clearing a pending exception during EventDataReader.skipBytes\n");
					LLEVENT_ERROR_TRACE("The function is not called within the virtual machine task.\n");
				}
				skip_status = EVENT_NOK;
				break;
			}
			skip_bytes_counter++;
		}
	}

	return skip_status;
}

/**
 * Gets the number of available data bytes.
 * Returns the number of available data bytes.
 */
uint32_t LLEVENT_IMPL_available(void) {
	return data_length_extended_data - offset_extended_data_read;
}

jbyte read_one_byte(void) {
	jbyte read_byte = JTRUE;
	// If less than one byte available, throw a native IOException.
	if (LLEVENT_IMPL_available() < sizeof(jbyte)) {
		if (SNI_throwNativeIOException(EVENT_NOK, "No byte remaining in the extended data.") == SNI_ERROR) {
			LLEVENT_ERROR_TRACE("ERROR during EventDataReader reading: No byte remaining in the extended data.\n");
			LLEVENT_ERROR_TRACE(
				"This function is not called within the virtual machine task or if the current thread is suspended.\n");
		}
		read_byte = JFALSE;
	}

	if (read_byte == JTRUE) {
		struct int_to_byte_t {
			jbyte first_byte;
			jbyte second_byte;
			jbyte third_byte;
			jbyte fourth_byte;
		};

		// If the static buffer is empty (offset == -1) or has been fully read (offset >= 4), read the next uint32_t of
		// data from the event_queue.
		if (offset_buffer_extended_data == (int8_t)-1 || offset_buffer_extended_data >= (int8_t)4) {
			// Fetch a message from the OSAL queue and store it in the static buffer. Suspend the thread if no message
			// available.
			OSAL_status_t res = OSAL_queue_fetch(&event_queue_handle, (void **)&buffer_extended_data, 0);
			if (res != OSAL_OK) {
				if (SNI_throwNativeIOException(EVENT_NOK, "No more data on the message queue.") == SNI_ERROR) {
					LLEVENT_ERROR_TRACE("ERROR during EventDataReader reading: No more data on the message queue.\n");
					LLEVENT_ERROR_TRACE(
						"This function is not called within the virtual machine task or if the current thread is suspended.\n");
				}
				read_byte = JFALSE;
			} else {
				// Set the offset of the buffer to 0 -> 0 byte has been read.
				offset_buffer_extended_data = 0;
				// Switch the alignment
				data_alignment = data_alignment == (uint8_t)0 ? 1 : 0;
			}
		}

		// Process the data if there is no error during SNI_suspendCurrentJavaThreadWithCallback
		if (read_byte == JTRUE) {
			// Get the next byte of data from the static buffer.
			struct int_to_byte_t *int_to_byte_ptr = (struct int_to_byte_t *)&buffer_extended_data;
			if (offset_buffer_extended_data == (int8_t)0) {
				read_byte = int_to_byte_ptr->first_byte;
			} else if (offset_buffer_extended_data == (int8_t)1) {
				read_byte = int_to_byte_ptr->second_byte;
			} else if (offset_buffer_extended_data == (int8_t)2) {
				read_byte = int_to_byte_ptr->third_byte;
			} else {
				read_byte = int_to_byte_ptr->fourth_byte;
			}
			// Increment the offset of the static buffer.
			offset_buffer_extended_data++;
			// Increment the offset of the data read.
			offset_extended_data_read++;
		}
	}
	LLEVENT_INFO_TRACE("read %d\n", read_byte);
	return read_byte;
}

jshort read_two_bytes(void) {
	jshort event_short = JTRUE;
	// If less than 2 bytes available, throw a native IOException.
	if (LLEVENT_IMPL_available() < sizeof(jshort)) {
		if (SNI_throwNativeIOException(EVENT_NOK, "Less than two bytes remaining in the extended data.") == SNI_ERROR) {
			LLEVENT_ERROR_TRACE(
				"ERROR during EventDataReader reading: Less than two bytes remaining in the extended data.\n");
			LLEVENT_ERROR_TRACE(
				"This function is not called within the virtual machine task or if the current thread is suspended.\n");
		}
		event_short = JFALSE;
	}

	if (event_short == JTRUE) {
		struct int_to_short_t {
			jshort first_short;
			jshort second_short;
		};

		// If the static buffer is empty (offset == -1) or has no char remaining (offset >= 3), read the next uint32_t
		// of data from the event_queue.
		if (offset_buffer_extended_data == (int8_t)-1 || offset_buffer_extended_data >= (int8_t)3) {
			// If offset equals 3, an alignment is done -> increase the offset_extended_data_read.
			if (offset_buffer_extended_data == (int8_t)3) {
				offset_extended_data_read++;
			}

			// Fetch a message from the OSAL queue and store it in the static buffer. Suspend the thread if no message
			// available.
			OSAL_status_t res = OSAL_queue_fetch(&event_queue_handle, (void **)&buffer_extended_data, 0);
			if (res != OSAL_OK) {
				if (SNI_throwNativeIOException(EVENT_NOK, "No more data on the message queue.") == SNI_ERROR) {
					LLEVENT_ERROR_TRACE("ERROR during EventDataReader reading: No more data on the message queue.\n");
					LLEVENT_ERROR_TRACE(
						"This function is not called within the virtual machine task or if the current thread is suspended.\n");
				}
				event_short = JFALSE;
			} else {
				// Set the offset of the buffer to 0 -> 0 byte has been read.
				offset_buffer_extended_data = 0;
				// Switch the alignment
				data_alignment = data_alignment == (uint8_t)0 ? 1 : 0;
			}
		}

		// Process the data if there is no error during SNI_suspendCurrentJavaThreadWithCallback
		if (event_short == JTRUE) {
			// Check if the buffer is 2 byte aligned. If the offset is equal to 1 -> aligned to 2.
			// The other possible values are 0 or 2 (if 3 a new uint32_t is read from the queue), in those cases the
			// buffer is aligned.
			// If an alignment is done, increase the offset_extended_data_read.
			if (offset_buffer_extended_data == (int8_t)1) {
				offset_buffer_extended_data = 2;
				offset_extended_data_read++;
			}
			// Get the two next bytes of data from the static buffer.
			struct int_to_short_t *int_to_short_ptr = (struct int_to_short_t *)&buffer_extended_data;
			if (offset_buffer_extended_data == (int8_t)0) {
				event_short = int_to_short_ptr->first_short;
			} else {
				event_short = int_to_short_ptr->second_short;
			}
			// Increment the offset of the static buffer.
			offset_extended_data_read += sizeof(jshort);
			//Increment the offset of the data read.
			offset_buffer_extended_data += (int8_t)sizeof(jshort);
		}
	}
	return event_short;
}

jint read_four_bytes(void) {
	jint event_int = JTRUE;
	// If less than 4 bytes available, throw a native IOException.
	if (LLEVENT_IMPL_available() < sizeof(jint)) {
		if (SNI_throwNativeIOException(EVENT_NOK,
		                               "Less than four bytes remaining in the extended data.") == SNI_ERROR) {
			LLEVENT_ERROR_TRACE(
				"ERROR during EventDataReader reading: Less than four bytes remaining in the extended data.\n");
			LLEVENT_ERROR_TRACE(
				"This function is not called within the virtual machine task or if the current thread is suspended.\n");
		}
		event_int = JFALSE;
	}

	if (event_int == JTRUE) {
		// Aligned = 4 -> read a new integer from the queue and delete the buffer (padding).
		// If there was data in the buffer -> add the skipped bytes to the offset.
		// There is data if the buffer is not empty (offset != -1) and everything has not been handled (offset < 4).
		if (offset_buffer_extended_data != (int8_t)-1 && offset_buffer_extended_data < (int8_t)4) {
			offset_extended_data_read += sizeof(jint) - (uint8_t)offset_buffer_extended_data;
		}
		// Reset the buffer used to read bytes of an extended event.
		buffer_extended_data = (uint32_t)NULL;
		offset_buffer_extended_data = (int8_t)-1;

		// Fetch a message from the OSAL queue. Suspend the thread if no message available.
		OSAL_status_t res = OSAL_queue_fetch(&event_queue_handle, (void **)&event_int, 0);
		if (res != OSAL_OK) {
			if (SNI_throwNativeIOException(EVENT_NOK, "No more data on the message queue.") == SNI_ERROR) {
				LLEVENT_ERROR_TRACE("ERROR during EventDataReader reading: No more data on the message queue.\n");
				LLEVENT_ERROR_TRACE(
					"This function is not called within the virtual machine task or if the current thread is suspended.\n");
			}
			event_int = JFALSE;
		} else {
			// Increment the offset of the data read.
			offset_extended_data_read += sizeof(jint);
			// Switch the alignment
			data_alignment = data_alignment == (uint8_t)0 ? 1 : 0;
		}
	}
	return event_int;
}

jlong read_eight_bytes(void) {
	// Method status
	jboolean read_status = JTRUE;
	jlong return_value = 0;
	// If less than 8 bytes available (or 4 bytes if the first par of the long value has been read), throw a native
	// IOException.
	if (LLEVENT_IMPL_available() < sizeof(jlong)) {
		if (SNI_throwNativeIOException(EVENT_NOK,
		                               "Less than eight bytes remaining in the extended data.") == SNI_ERROR) {
			LLEVENT_ERROR_TRACE(
				"ERROR during EventDataReader reading: Less than eight bytes remaining in the extended data.\n");
			LLEVENT_ERROR_TRACE(
				"This function is not called within the virtual machine task or if the current thread is suspended.\n");
		}
		read_status = JFALSE;
	}

	if (read_status == (jboolean)JTRUE) {
		union {
			jint int_values[2];
			jlong long_value;
		} event_long;

		jint event_value = 0;

		// Aligned = 8 -> delete the buffer (padding).
		// If there was data in the buffer -> add the skipped bytes to the offset.
		// There is data if the buffer is not empty (offset != -1) and everything has not been handled (offset < 4).
		if (offset_buffer_extended_data != (int8_t)-1 && offset_buffer_extended_data < (int8_t)4) {
			offset_extended_data_read += sizeof(jint) - (uint8_t)offset_buffer_extended_data;
		}
		// Reset the buffer used to read bytes of an extended event.
		buffer_extended_data = (uint32_t)NULL;
		offset_buffer_extended_data = (int8_t)-1;

		// If the data is not 8 bytes aligned, skip 4 bytes from the queue.
		if (data_alignment != (uint8_t)1) {
			OSAL_status_t res = OSAL_queue_fetch(&event_queue_handle, (void **)&event_value, 0);
			if (res != OSAL_OK) {
				if (SNI_throwNativeIOException(EVENT_NOK, "No more data on the message queue.") == SNI_ERROR) {
					LLEVENT_ERROR_TRACE("ERROR during EventDataReader reading: No more data on the message queue.\n");
					LLEVENT_ERROR_TRACE(
						"This function is not called within the virtual machine task or if the current thread is suspended.\n");
				}
				read_status = JFALSE;
			} else {
				// Increment the offset of the data read.
				offset_extended_data_read += sizeof(jint);
				// Switch the alignment
				data_alignment = data_alignment == (uint8_t)0 ? 1 : 0;
			}
		}
		// Continue to read the eight bytes if there is no error during alignment reading.
		if (read_status == (jboolean)JTRUE) {
			// If the first part of the long has not been read, read it and store it in a static variable.
			OSAL_status_t res = OSAL_queue_fetch(&event_queue_handle, (void **)&event_value, 0);
			if (res != OSAL_OK) {
				if (SNI_throwNativeIOException(EVENT_NOK, "No more data on the message queue.") == SNI_ERROR) {
					LLEVENT_ERROR_TRACE("ERROR during EventDataReader reading: No more data on the message queue.\n");
					LLEVENT_ERROR_TRACE(
						"This function is not called within the virtual machine task or if the current thread is suspended.\n");
				}
				read_status = JFALSE;
			} else {
				// Increment the offset of the data read.
				offset_extended_data_read += sizeof(jint);
				//Store the first part of the long in a static variable.
				first_long_value = event_value;
				// Switch the alignment
				data_alignment = data_alignment == (uint8_t)0 ? 1 : 0;
			}
			// Continue to read the eight bytes if there is no error during pervious call to
			// SNI_suspendCurrentJavaThreadWithCallback
			if (read_status == (jboolean)JTRUE) {
				// Get the second part of the long. Suspend the thread if no message available.
				OSAL_status_t res = OSAL_queue_fetch(&event_queue_handle, (void **)&event_value, 0);
				if (res != OSAL_OK) {
					if (SNI_throwNativeIOException(EVENT_NOK, "No more data on the message queue.") == SNI_ERROR) {
						LLEVENT_ERROR_TRACE(
							"ERROR during EventDataReader reading: No more data on the message queue.\n");
						LLEVENT_ERROR_TRACE(
							"This function is not called within the virtual machine task or if the current thread is suspended.\n");
					}
				} else {
					// Increment the offset of the data read.
					offset_extended_data_read += sizeof(jint);
					// Switch the alignment
					data_alignment = data_alignment == (uint8_t)0 ? 1 : 0;
					// Convert the two int32_t to a long value (64 bits).
					event_long.int_values[0] = first_long_value;
					event_long.int_values[1] = event_value;
					return_value = event_long.long_value;
				}
			}
		}
	}

	return return_value;
}
