#pragma once

// Shared registration for FreeRun RPC handlers.
// Used by EcatDaemon (production) and daemon_handler_test (unit) to ensure
// the same dispatch paths as shipped code.

class CommandDispatcher;
class FreeRunController;

void registerFreeRunHandlers(CommandDispatcher& dispatcher, FreeRunController& freeRun);
