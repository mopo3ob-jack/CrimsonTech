#include "../../include/script/ScriptHandler.hpp"
#include "../../include/utils/hdbAlloc.h"

#include <stdio.h>

namespace hdb {

namespace ct {

ScriptHandler::~ScriptHandler() {
	waitForCompletion();

	stopThreads = true;

	for (auto& t: threads) {
		t.join();
	}

	for (auto s: scripts) {
		s->end();
	}
}

void ScriptHandler::resizeThreads(unsigned int threadCount) {
	waitForCompletion();

	stopThreads = true;

	for (auto& t: threads) {
		t.join();
	}
	
	threads.clear();

	scriptMutex.lock();

	stopThreads = false;
	threadsRunning = false;

	for (unsigned int i = 0; i < threadCount; ++i) {
		threads.emplace_back(&ScriptHandler::_threadFunc, this, i);
	}

	currentScript = 0;

	scriptMutex.unlock();
}

void ScriptHandler::addScript(ScriptBase* script) {
	waitForCompletion();

	scriptMutex.lock();

	script->start();

	for (auto& s: scripts) {
		if (s == nullptr) {
			s = script;
			goto addedScript;
		}
	}

	scripts.push_back(script);

	addedScript:

	scriptMutex.unlock();
}

void ScriptHandler::removeScript(ScriptBase* script) {
	waitForCompletion();
	scriptMutex.lock();

	for (auto& s: scripts) {
		if (s == script) {
			s->end();
			s = nullptr;

			scriptMutex.unlock();
			return;
		}
	}

	scriptMutex.unlock();
}

void ScriptHandler::execute() {
	waitForCompletion();
	scriptMutex.lock();

	threadsRunning = true;
	currentScript = 0;

	scriptMutex.unlock();
}

void ScriptHandler::waitForCompletion() {
	while (threadsRunning);
}

void ScriptHandler::_threadFunc(unsigned int id) {
	while (!stopThreads) {
		scriptMutex.lock();

		if (currentScript < scripts.size() && threadsRunning) {
			ScriptBase* script = scripts[currentScript++];
			scriptMutex.unlock();

			if (script) {
				script->update();
			}
		} else {
			threadsRunning = false;
			scriptMutex.unlock();

			while (!threadsRunning) if (stopThreads) return;
		}
	}
}

}

}