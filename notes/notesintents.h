#pragma once

#include <LingmoIntents/intentservice.h>

class NoteStore;

// notes.create and notes.append on com.lingmo.Notes /Intents. The window
// publishes them as Primary; `lingmo-notes --intents` (D-Bus activation)
// serves them headless until a window starts and takes the name over.
LingmoIntents::IntentService *publishNotesIntents(NoteStore *store, QObject *parent,
                                                  LingmoIntents::IntentService::Mode mode);
