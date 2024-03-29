///
/// \file	router.h
///		Support classes for the pluggable socket routing system.
///

/*
    Copyright (C) 2005-2008, Net Direct Inc. (http://www.netdirect.ca/)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License in the COPYING file at the
    root directory of this project for more details.
*/

#ifndef __LINBERRY_ROUTER_H__
#define __LINBERRY_ROUTER_H__

#include "dll.h"
#include <stdint.h>
#include <map>
#include <tr1/memory>
#include <stdexcept>
#include <pthread.h>
#include "dataqueue.h"

namespace Usb { class Device; }

namespace Linberry {

class DataHandle;

class BXEXPORT SocketRoutingQueue
{
	friend class DataHandle;

public:
	typedef void (*SocketDataHandler)(void *ctx, Data*);	//< See RegisterInterest() for information on this callback.
	struct QueueEntry
	{
		SocketDataHandler m_handler;
		void *m_context;
		DataQueue m_queue;

		QueueEntry(SocketDataHandler h, void *c)
			: m_handler(h)
			, m_context(c)
			{}
	};
	typedef std::tr1::shared_ptr<QueueEntry>	QueueEntryPtr;
	typedef uint16_t				SocketId;
	typedef std::map<SocketId, QueueEntryPtr>	SocketQueueMap;

private:
	Usb::Device * volatile m_dev;
	volatile int m_writeEp, m_readEp;

	volatile bool m_interest; // true if at least one socket has an interest.
				// used to optimize the reading

	mutable pthread_mutex_t m_mutex;// controls access to local data, but not
				// DataQueues, as they have their own
				// locking per queue

	pthread_mutex_t m_readwaitMutex;
	pthread_cond_t m_readwaitCond;

	DataQueue m_free;
	DataQueue m_default;
	SocketQueueMap m_socketQueues;

	// thread state
	pthread_t m_usb_read_thread;
	volatile bool m_continue_reading;// set to true when the thread is created,
				// then set to false in the destructor
				// to signal the end of the thread
				// and handle the join

protected:
	// Provides a method of returning a buffer to the free queue
	// after processing.  The DataHandle class calls this automatically
	// from its destructor.
	void ReturnBuffer(Data *buf);

	// Thread function for the simple read behaviour... thread is
	// created in the SpinoffSimpleReadThread() member below.
	static void *SimpleReadThread(void *userptr);

public:
	SocketRoutingQueue(int prealloc_buffer_count = 4);
	~SocketRoutingQueue();

	//
	// data access
	//
	int GetWriteEp() const { return m_writeEp; }
	int GetReadEp() const { return m_readEp; }


	// These functions connect the router to an external Usb::Device
	// object.  Normally this is handled automatically by the
	// Controller class, but are public here in case they are needed.
	void SetUsbDevice(Usb::Device *dev, int writeEp, int readEp);
	void ClearUsbDevice();
	bool UsbDeviceReady();
	Usb::Device* GetUsbDevice() { return m_dev; }

	// This class starts out with no buffers, and will grow one buffer
	// at a time if needed.  Call this to allocate count buffers
	// all at once and place them on the free queue.
	void AllocateBuffers(int count);

	// Returns the data for the next unregistered socket.
	// Blocks until timeout or data is available.
	// Returns false (or null pointer) on timeout and no data.
	// With the return version of the function, there is no
	// copying performed.
	bool DefaultRead(Data &receive, int timeout = -1);
	DataHandle DefaultRead(int timeout = -1);

	// Register an interest in data from a certain socket.  To read
	// from that socket, use the SocketRead() function from then on.
	// Any non-registered socket goes in the default queue
	// and must be read by DefaultRead()
	// If not null, handler is called when new data is read.  It will
	// be called in the same thread instance that DoRead() is called from.
	// Handler is passed the DataQueue Data pointer, and so no
	// copying is done.  Once the handler returns, the data is
	// considered processed and not added to the interested queue,
	// but instead returned to m_free.
	void RegisterInterest(SocketId socket, SocketDataHandler handler = 0, void *context = 0);

	// Unregisters interest in data from the given socket, and discards
	// any existing data in its interest queue.  Any new incoming data
	// for this socket will be placed in the default queue.
	void UnregisterInterest(SocketId socket);

	// Reads data from the interested socket cache.  Can only read
	// from sockets that have been previously registered.
	// Blocks until timeout or data is available.
	// Returns false (or null pointer) on timeout and no data.
	// With the return version of the function, there is no
	// copying performed.
	bool SocketRead(SocketId socket, Data &receive, int timeout = -1);
	DataHandle SocketRead(SocketId socket, int timeout = -1);

	// Returns true if data is available for that socket.
	bool IsAvailable(SocketId socket) const;

	// Called by the application's "read thread" to read the next usb
	// packet and route it to the correct queue.  Returns after every
	// read, even if a handler is associated with a queue.
	// Note: this function is safe to call before SetUsbDevice() is
	// called... it just doesn't do anything if there is no usb
	// device to work with.
	//
	// Timeout is in milliseconds.
	//
	// Returns false in the case of USB errors, and puts the error
	// message in msg.
	bool DoRead(std::string &msg, int timeout = -1);

	// Utility function to make it easier for the user to create the
	// USB pure-read thread.  If the user wants anything more complicated
	// in this background thread, he can implement it himself and call
	// the above DoRead() in a loop.  If only the basics are needed,
	// then this makes it easy.
	// Throws Linberry::ErrnoError on thread creation error.
	void SpinoffSimpleReadThread();
};


//
// DataHandle
//
/// std::auto_ptr like class that handles pointers to Data, but instead of
/// freeing them completely, the Data objects are turned to the
/// SocketRoutingQueue from whence they came.
///
class BXEXPORT DataHandle
{
private:
	SocketRoutingQueue &m_queue;
	mutable Data *m_data;

protected:
	void clear()
	{
		if( m_data ) {
			m_queue.ReturnBuffer(m_data);
			m_data = 0;
		}
	}

public:
	DataHandle(SocketRoutingQueue &q, Data *data)
		: m_queue(q)
		, m_data(data)
	{
	}

	DataHandle(const DataHandle &other)
		: m_queue(other.m_queue)
		, m_data(other.m_data)
	{
		// we now own the pointer
		other.m_data = 0;
	}

	~DataHandle()
	{
		clear();
	}

	Data* get()
	{
		return m_data;
	}

	Data* release()	// no longer owns the pointer
	{
		Data *ret = m_data;
		m_data = 0;
		return ret;
	}

	Data* operator->()
	{
		return m_data;
	}

	const Data* operator->() const
	{
		return m_data;
	}

	DataHandle& operator=(const DataHandle &other)
	{
		if( &m_queue != &other.m_queue )
			throw std::logic_error("Trying to copy DataHandles of different queues!");

		// remove our current data
		clear();

		// accept the new
		m_data = other.m_data;

		// we now own it
		other.m_data = 0;

		return *this;
	}

};


} // namespace Linberry

#endif

