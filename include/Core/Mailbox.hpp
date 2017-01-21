#ifndef FUG_MAILBOX_HPP
#define FUG_MAILBOX_HPP


#include <functional>

#include "Implementation.hpp"
#include "MailboxIterator.hpp"

namespace fug {

    template <typename T_Event>
    class Mailbox {
    
	public:
        friend class FUG_IMPLEMENTATION_EVENT_MANAGER;

        ~Mailbox();

		Mailbox(Mailbox const&) = delete; // TODO
        Mailbox(Mailbox<T_Event>&&);
		Mailbox<T_Event>& operator=(Mailbox<T_Event> const&) = delete; // TODO
		Mailbox<T_Event>& operator=(Mailbox<T_Event>&&) = delete; // TODO

		MailboxIterator<T_Event> const& begin();
		MailboxIterator<T_Event> const& end();

    private:
		Mailbox(MailboxIterator<T_Event>, MailboxIterator<T_Event>);

		MailboxIterator<T_Event> _begin;
		MailboxIterator<T_Event> _end;

		static std::function<void(Mailbox<T_Event>*)>    _registerMailbox;
        static std::function<void(Mailbox<T_Event>*)>    _unRegisterMailbox;
    };
    
	#include "Mailbox.tcc"

}


#endif // FUG_RESOURCE_POINTER_HPP