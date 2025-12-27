#include "core/event.h"
#include "core/logger.h"

typedef struct registered_event {
    void* listener;
    PFN_on_event callback;
    registered_event();
    registered_event(void* l, PFN_on_event c);
} registered_event;

registered_event::registered_event(): listener{0}, callback{0} {};
registered_event::registered_event(void* l, PFN_on_event c): listener{l}, callback{c} {};

typedef struct event_code_entry {
    vector<registered_event*> events;
    event_code_entry();
} event_code_entry;

// This should be more than enough codes...
#define MAX_MESSAGE_CODES 16384

// State structure.
typedef struct event_system_state {
    // Lookup table for event codes.
    event_code_entry registered[MAX_MESSAGE_CODES];
    b8 is_initialized;
    event_system_state();
} event_system_state;

event_code_entry::event_code_entry(): events{vector<registered_event*>()}{};

event_system_state::event_system_state():registered{event_code_entry()}, is_initialized{TRUE}{};

/**
 * Event system internal state.
 */
static event_system_state state;

b8 event_initialize() {
    if(!state.is_initialized){
        KWARN("Event system already initialized!");
        return FALSE;
    }
    KDEBUG("Event system initialized");
    state = event_system_state();
    return TRUE;
}

void event_shutdown() {
    // Free the events arrays. And objects pointed to should be destroyed on their own.
    for(u16 i = 0; i < MAX_MESSAGE_CODES; ++i){
        if(state.registered[i].events.size() != 0) {
            state.registered[i].events.clear();
        }
    }
}

b8 event_register(u16 code, void* listener, PFN_on_event on_event) {
    if(state.is_initialized == FALSE) {
        return FALSE;
    }

    for(u64 i = 0; i < state.registered[code].events.size(); ++i) {
        if(state.registered[code].events[i]->listener == listener) {
            KWARN("Event already registered");
            return FALSE;
        }
    }

    // If at this point, no duplicate was found. Proceed with registration.
    registered_event* event = new registered_event(listener, on_event);
    //event->listener = listener;
    //event->callback = on_event;
    state.registered[code].events.push_back(event);
    //KDEBUG("Number of events registered for %d: %d",code,state.registered[code].events.size() );
    //KDEBUG("Event registered!");

    return TRUE;
}

b8 event_unregister(u16 code, void* listener, PFN_on_event on_event) {
    if(state.is_initialized == FALSE) {
        return FALSE;
    }

    // On nothing is registered for the code, boot out.
    if(state.registered[code].events.size() == 0) {
        KWARN("Event not unregistered due to none exist");
        return FALSE;
    }

    for(u64 i = 0; i < state.registered[code].events.size(); ++i) {
        registered_event* e = state.registered[code].events[i];
        if(e->listener == listener && e->callback == on_event) {
            // Found one, remove it
            state.registered[code].events.erase(state.registered[code].events.begin()+i);
            return TRUE;
        }
    }

    // Not found.
    KWARN("Event not unregistered due to none was found");
    return FALSE;
}

b8 event_fire(u16 code, void* sender, event_context context) {
    if(state.is_initialized == FALSE) {
        return FALSE;
    }

    // If nothing is registered for the code, boot out.
    if(state.registered[code].events.size() == 0) {
        return FALSE;
    }

    for(u64 i = 0; i < state.registered[code].events.size(); ++i) {
        registered_event* e = state.registered[code].events[i];
        if(e->callback(code, sender, e->listener, context)) {
            // Message has been handled, do not send to other listeners.
            return TRUE;
        }
    }

    // Not found.
    return FALSE;
}
