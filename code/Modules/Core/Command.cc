#include "Command.h"

namespace Oryol {

Command::CommandMap Command::_cmds;
RWLock Command::_lock;

}
