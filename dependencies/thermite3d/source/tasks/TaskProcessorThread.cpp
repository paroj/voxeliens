/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#include "TaskProcessorThread.h"

#include "Task.h"

#include <QMutex>
#include <QSemaphore>

#include <algorithm>

namespace Thermite
{
	TaskProcessorThread::TaskProcessorThread(QObject* parent)
		:QThread(parent)
	{
		m_taskContainerMutex = new QMutex;
		m_noOfTasks = new QSemaphore;
	}

	TaskProcessorThread::~TaskProcessorThread(void)
	{
		delete m_taskContainerMutex;
		delete m_noOfTasks;
	}

	void TaskProcessorThread::run(void)
	{
		while(true)
		{
			m_noOfTasks->acquire();

			m_taskContainerMutex->lock();
			Task* task = m_taskContainer.front();
			m_taskContainer.pop_front();
			m_taskContainerMutex->unlock();

			task->run();

			if(task->autoDelete())
			{
				delete task;
			}

			msleep(0);
		}
	}

	void TaskProcessorThread::addTask(Task* task)
	{
		m_taskContainerMutex->lock();
		m_taskContainer.push_back(task);
		m_noOfTasks->release();
		m_taskContainerMutex->unlock();
	}

	bool TaskProcessorThread::removeTask(Task* task)
	{
		bool bRemoved = false;

		m_taskContainerMutex->lock();

		std::list<Task*>::iterator iterTasks = std::find(m_taskContainer.begin(), m_taskContainer.end(), task);
		if(iterTasks != m_taskContainer.end())
		{
			m_taskContainer.erase(iterTasks);
			m_noOfTasks->acquire();
			bRemoved = true;
		}

		m_taskContainerMutex->unlock();

		return bRemoved;
	}
}