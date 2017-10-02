
template <typename T_Event>
Mailbox<T_Event> BasicEventManager::getMailbox(EventPort const& port) 
{
    auto& state = getMailboxState<T_Event>(port);
    return Mailbox<T_Event>(state.begin_iter, state.end_iter);
}

template <typename T_Event>
void BasicEventManager::pushEvent(T_Event const& payload, EventPort const& port, bool persistent)
{
    Event<T_Event> event(payload, port, persistent);

    FUG_LOG(LogLevel::Info) << "* Pushing " << event << std::endl;

    auto& state = getMailboxState<T_Event>(port);
    getEventVector<T_Event>().at(state.end_iter++._index) = event;

    // if ring buffer's head overlaps with the tail, pad the tail forward
    if (state.begin_iter._index == state.end_iter._index) {
        state.begin_iter++;
    }
}

template <typename T_Event>
void BasicEventManager::flushEvents(EventPort const& port)
{
    auto& state = getMailboxState<T_Event>(port);
    auto& vec = getEventVector<T_Event>();

    // loop through the events to check for persistent ones which won't be flushed
    auto old_begin = state.begin_iter;
    for (auto iter = old_begin; iter != state.end_iter; iter++) {
        if (iter->info.persistent) {
            vec.at(state.begin_iter++._index) = *iter;
        }
    }
    state.end_iter = state.begin_iter;
    state.begin_iter = old_begin;

    FUG_LOG(LogLevel::Info) << "* Mailbox (" << util::str(Event<T_Event>()) << ", port " << port << ") flushed" << std::endl;
}

template <typename T_Event>
std::vector<Event<T_Event>>& BasicEventManager::getEventVector()
{
    static std::vector<Event<T_Event>> v;
    return v;
}

template <typename T_Event>
auto BasicEventManager::accessMailboxStates() -> std::unordered_map<EventPort, MailboxState<T_Event>>& 
{
    static std::unordered_map<EventPort, MailboxState<T_Event>> states;
    return states;
}

template <typename T_Event>
auto BasicEventManager::getMailboxState(EventPort const& port) -> MailboxState<T_Event>& 
{
    auto& states =  accessMailboxStates<T_Event>();
    auto iter = states.find(port);
    
    if (iter == states.end()) {
        
        // Here we can assume the port is new for T_Event
        // Port's buffer needs to be allocated and MailboxState needs to be initialized:

        auto& vec = getEventVector<T_Event>();
        vec.resize(vec.size() + FUG_MAILBOX_SIZE);
        
        MailboxSize_t first_index = vec.size() - FUG_MAILBOX_SIZE;
        MailboxSize_t last_index = vec.size() - 1;

        auto buckets = states.bucket_count();
        iter = states.insert({port, {port,
                                     first_index,
                                     last_index,
                                     MailboxIterator<T_Event>(&vec, port, first_index, last_index, first_index),
                                     MailboxIterator<T_Event>(&vec, port, first_index, last_index, first_index)}}).first;
    }

    return iter->second;
}

template <typename T_Event>
void BasicEventManager::registerMailbox(Mailbox<T_Event>* mailbox)
{
    // Not implemented
}


template <typename T_Event>
void BasicEventManager::unRegisterMailbox(Mailbox<T_Event>* mailbox)
{
    // Not implemented
}
