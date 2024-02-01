#ifndef HDB_CT_SCRIPTHANDLER_HPP
#define HDB_CT_SCRIPTHANDLER_HPP

#include "ScriptBase.hpp"

#include <thread>
#include <mutex>
#include <concepts>
#include <vector>
#include <string.h>
#include <threads.h>
#include <condition_variable>

namespace hdb {

namespace ct {

class ScriptHandler {
public:
	ScriptHandler() { resizeThreadsToOptimalSize(); }
	ScriptHandler(unsigned int threadCount) { resizeThreads(threadCount); }

	~ScriptHandler();

	ScriptHandler(const ScriptHandler& handler) = delete;
	ScriptHandler operator=(const ScriptHandler& handler) = delete;

	void resizeThreadsToOptimalSize() { resizeThreads(std::thread::hardware_concurrency() - 1); }
	void resizeThreads(unsigned int threadCount);

	void addScript(ScriptBase* script);
	void removeScript(ScriptBase* script);

	void execute();
	void waitForCompletion();

protected:
	void _threadFunc(unsigned int id);

	std::vector<std::thread> threads;
	std::vector<ScriptBase*> scripts;
	std::mutex scriptMutex;

	std::atomic<unsigned int> currentScript;

	volatile unsigned int stopThreads : 1;
	volatile unsigned int threadsRunning : 1 = false;
};

}

}

#endif