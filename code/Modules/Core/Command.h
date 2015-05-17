#pragma once

#include <functional>
#include "String/String.h"
#include "String/StringBuilder.h"
#include "Threading/RWLock.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Args.h"

namespace Oryol {

typedef Array<String> CmdArgs;

/**
 * @brief A command is a string bound c++ function/method/lambda. You can bind
 * this to keys or execute on the console.
 */
class Command {
private:
    typedef Map<String, Command> CommandMap;
    typedef std::function<void(const CmdArgs&)> FunctionType;

    static CommandMap _cmds;
    static RWLock _lock;

    String _name;
    FunctionType _func;

    Command(String&& name, FunctionType&& func) :
            _name(std::move(name)), _func(std::move(func)) {
    }

public:
    static void registerCommand(String&& name, FunctionType&& func) {
        ScopedWriteLock lock(_lock);
        const Command c(std::move(name), std::move(func));
        _cmds.Add(c.name(), c);
    }

    static void unregisterCommand(const String& name) {
        ScopedWriteLock lock(_lock);
        _cmds.Erase(name);
    }

    static int execute(const String& command) {
        Array<String> commands;
        StringBuilder strBuilder(command);
        strBuilder.Tokenize(";\n", '\"', commands);

        int executed = 0;
        for (const String& c : commands) {
            Array<String> args;
            StringBuilder argsBuilder(c);
            argsBuilder.Tokenize(" \t\r\n", '\"', commands);
            if (args.Empty())
                continue;
            const String cmd = args[0];
            args.Erase(0);
            if (execute(cmd, args))
                ++executed;
        }
        return executed;
    }

    static bool execute(const String& command, const CmdArgs& args) {
        ScopedReadLock lock(_lock);
        int32 index = _cmds.FindIndex(command);
        if (index == InvalidIndex)
            return false;
        const Command& cmd = _cmds.ValueAtIndex(index);
        cmd._func(args);
        return true;
    }

    inline bool operator==(const Command& rhs) const {
        return rhs._name == _name;
    }

    inline const String& name() const {
        return _name;
    }
};

}
