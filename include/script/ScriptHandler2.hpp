#ifndef HDB_CT_SCRIPTHANDLER_HPP
#define HDB_CT_SCRIPTHANDLER_HPP

#include "ScriptBase.hpp"

#include <thread>
#include <mutex>
#include <concepts>
#include <vector>
#include <string.h>
#include <threads.h>

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
	int _threadFunc(void*);

	std::vector<ScriptBase*> scripts;
	std::mutex scriptMutex;

	thrd_t* threads = nullptr;
	unsigned int threadCount;
	unsigned int threadsRunning;
	unsigned int scriptCount;
};

}

}

#endif