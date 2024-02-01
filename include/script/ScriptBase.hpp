#ifndef HDB_CT_SCRIPTBASE_HPP
#define HDB_CT_SCRIPTBASE_HPP

namespace hdb {

namespace ct {

class ScriptBase {
public:
	virtual void start() {}
	virtual void update() {}
	virtual void end() {}
};

}

}

#endif