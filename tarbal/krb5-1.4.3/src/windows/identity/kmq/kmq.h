/*
 * Copyright (c) 2004 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* $Id$ */

#ifndef __KHIMAIRA_KMQ_H__
#define __KHIMAIRA_KMQ_H__

/*! \defgroup kmq NetIDMgr Message Queue */
/*@{*/

#include<khdefs.h>
#include<khlist.h>
#include<kherr.h>

/* general */
#ifdef _WIN32
typedef DWORD kmq_thread_id;
typedef DWORD kmq_timer;
#endif

#ifdef _WIN32
/*! \brief Window message for kmq

   This message is sent to the window procedure of a window if that
   window is a subscriber to KMQ messages.

    \see kmq_subscribe_hwnd() for more information about handling this
        window message
 */
#define KMQ_WM_DISPATCH (WM_APP+0x100)
#endif

/* callback */

/*! \brief A message callback

    Should return TRUE if the message is properly handled.  Otherwise
    return FALSE */
typedef khm_int32 (KHMAPI *kmq_callback_t)(khm_int32 msg_type, 
                                           khm_int32 msg_sub_type, 
                                           khm_ui_4 uparam, 
                                           void * vparam);

/* message */

/*! \brief A single response.

    Certain broadcast messages may user scatter-gather type
    notification and result gathering.  Individual subscribers to a
    message attach their individual responses to a ::kmq_response
    object and attach that to the message which can later be read by
    the sender of the message.
 */
typedef struct tag_kmq_response {
    kmq_thread_id thread;
    void * response;

    LDCL(struct tag_kmq_response);
} kmq_response;

/*! \brief A single message
 */
typedef struct tag_kmq_message {
    khm_int32 type;             /*!< Type of message */
    khm_int32 subtype;          /*!< Subtype of message */

    khm_ui_4 uparam;             /*!< Integer parameter */
    void * vparam;            /*!< Pointer to parameter blob */
	
    khm_int32 nSent;            /*!< Number of instances of message
                                  sent (for broadcast messages) */

    khm_int32 nCompleted;       /*!< Number of instances that have
                                  completed processing (for broadcast
                                  messages) */

    khm_int32 nFailed;          /*!< Number of instances that failed
                                  to process (for broadcast
                                  messages) */

    kmq_response * responses;   /*!< List of responses */
    HANDLE wait_o;              /*!< Event to wait on (only valid if
                                  the publisher of the message
                                  requested a handle to the call) */

    kmq_timer timeSent;         /*!< Time at which the message was
                                  sent */
    kmq_timer timeExpire;       /*!< Time at which the message
                                  expires */

    kherr_context * err_ctx;    /*!< Error context for the message */

    khm_int32 refcount;

    LDCL(struct tag_kmq_message);
} kmq_message;

/*! \brief A handle to a call
 */
typedef kmq_message *kmq_call;

/*! \brief Message reference */
typedef struct tag_kmq_message_ref {
    kmq_message * msg;          /*!< Message that we are referring
                                  to */
    kmq_callback_t recipient;   /*!< The recipient of the message */

    LDCL(struct tag_kmq_message_ref);
} kmq_message_ref;

/*! \brief Message queue

    Each thread gets its own message queue.  When a message is
    broadcast to which there is a subscriber in a particular thread, a
    reference to the message is placed in the message queue of the
    thread.  The dispatch procedure then dispatches the message as
    described in the message reference.
*/
typedef struct tag_kmq_queue {
    kmq_thread_id thread;       /*!< The thread id  */

    CRITICAL_SECTION cs;
    HANDLE wait_o;

    khm_int32 load;             /*!< Number of messages waiting to be
                                  processed on this message queue.  */
    kmq_timer last_post;        /*!< Time the last message was
                                  received */

    /*Q*/
    QDCL(kmq_message_ref);      /*!< Queue of message references  */

    /*Lnode*/
    LDCL(struct tag_kmq_queue);
} kmq_queue;

/*! \brief Message subscription

    A subscription binds a recipient with a message type.  These are
    specific to a thread. I.e. a subscription that was made in one
    thread will not receive messages in the context of another thread.
*/
typedef struct tag_kmq_msg_subscription {
    khm_int32 type;             /*!< Type of message */
    khm_int32 rcpt_type;        /*!< Type of recipient.  One of
                                  ::KMQ_RCPTTYPE_CB or
                                  ::KMQ_RCPTTYPE_HWND  */
    union {
        kmq_callback_t cb;      /*!< Callback if the subscription is
                                  of callback type */
        HWND hwnd;              /*!< Window handle if the subscription
                                  is a windows message type */
    } recipient;

    kmq_queue * queue;          /*!< Associated queue */

    /*lnode*/
    LDCL(struct tag_kmq_msg_subscription);
} kmq_msg_subscription;

/*! \brief Callback recipient type

    The recipient is a callback function */
#define KMQ_RCPTTYPE_CB     1

/*! \brief Windows recipient type

    The recipient is a window */
#define KMQ_RCPTTYPE_HWND   2

/* publishers */

/*! \brief A completion handler for a message

    Each message type can have a completion handler.  Once a message
    of this a specific type has been broadcast and handled by all the
    subscripbers, the message will be passed down to the completion
    handler before the associated data structures are freed.  This
    allows applications that define message type to also define clean
    up for each message.  For example, the completion handler can
    initiate another message if the messages form a sequence or free
    up blocks of memory that was passed as the parameter to the
    message.
 */
typedef void (KHMAPI *kmq_msg_completion_handler)(kmq_message *);

/*! \brief A message type
 */
typedef struct tag_kmq_msg_type {
    khm_int32 id;               /*!< Identifier for the message
                                  type. */
    kmq_msg_subscription * subs; /*!< The list of subscriptions */
    kmq_msg_completion_handler completion_handler; /*!< Completion
                                  handler for the message type */

    wchar_t * name;             /*!< Name of the message type for
                                  named types.  Message type names are
                                  language independant. */

    /*Lnode*/
    LDCL(struct tag_kmq_msg_type);
} kmq_msg_type;

/*! \brief The maximum number of message types
 */
#define KMQ_MSG_TYPE_MAX 255

/*! \brief Maximum number of characters in a message type name

    The count includes the terminating NULL
 */
#define KMQ_MAXCCH_TYPE_NAME 256

/*! \brief Maximum number of bytes in a message type name

    Type count includes the terminating NULL
 */
#define KMQ_MAXCB_TYPE_NAME (KMQ_MAXCCH_TYPE_NAME * sizeof(wchar_t))

KHMEXP khm_int32 KHMAPI kmq_init(void);

KHMEXP khm_int32 KHMAPI kmq_exit(void);

/*! \brief Register a message type

    Registers a custom message type.  The \a name parameter specifies
    a language independent name for the message type and must be
    unique and must be less than ::KMQ_MAXCCH_TYPE_NAME characters.

    \param[in] name Name of the message type.  Upto
        ::KMQ_MAXCCH_TYPE_NAME characters including terminating NULL.
        The \a name cannot be a zero length string.

    \param[out] new_id Receives the new message type ID.  Specify NULL
        if the new message type is not required.

    \see kmq_find_type() and kmq_unregister_type()

    \retval KHM_ERROR_INVALID_PARM The \a name parameter was invalid.
    \retval KHM_ERROR_EXISTS A message type with that name already exists.
    \retval KHM_ERROR_NO_RESOURCES Can't register any more message types.
    \retval KHM_ERROR_SUCCESS The operation succeeded.
 */
KHMEXP khm_int32 KHMAPI kmq_register_type(wchar_t * name, khm_int32 * new_id);

/*! \brief Find a message type

    Find the message type with the given name.  If found, the type ID
    is returned in \a id.

    \retval KHM_ERROR_SUCCESS A message type with the given name was
        found.
    \retval KHM_ERROR_NOT_FOUND A message type with the given name was
        not found.
 */
KHMEXP khm_int32 KHMAPI kmq_find_type(wchar_t * name, khm_int32 * id);

/*! \brief Unregister a message type

    Unregisters a message type that was registered using
    kmq_register_type().

    \retval KHM_ERROR_SUCCESS The specified message type was
        successfully unregistered.

    \retval KHM_ERROR_NOT_FOUND The message type was not found.
 */
KHMEXP khm_int32 KHMAPI kmq_unregister_type(khm_int32 id);

/*! \brief Subscribte to a message type.

    Adds a subscription to messages of type \a type.  Subscriptions
    are managed per thread.  Therefore the subscription is actually
    added to the subscription list for the current thread (the thread
    which calls kmq_subscribe()).

    When a message of type \a type is received by the thread, it is
    dispatched to the callback function identified by \a cb within the
    context of this thread.

    \note Calling kmq_subscribe() from within multiple threads with
        the same \a type and \a cb will result in multiple
        subscriptions.

    \see kmq_unsubscribe()
    \see kmq_dispatch()
*/
KHMEXP khm_int32 KHMAPI kmq_subscribe(khm_int32 type, kmq_callback_t cb);

/*! \brief Subscribe a window to a message type

    Adds the window specified by \a hwnd to the subscription list for
    the message type \a type.  When a message of this type is posted,
    then the window procedure of the window \a hwnd receives a message
    ::KMQ_WM_DISPATCH.

    When a window receives a ::KMQ_WM_DISPATCH message, it means that
    a message has been posted which is of a type that the window has
    subscribed for.  Because of the way Windows handles window
    messages and the way NetIDMgr message queues work, a thread which
    has a window (or thread) procedure can not call kmq_dispatch() to
    handle these messages.  For threads that have window or thread
    message loops, they must call kmq_subscribe_hwnd() to subscribe a
    particular window (for thread message loops, this would be the
    HWND of the message window for the thread) to NetIDMgr messages.

    There are two supported ways of handling the ::KMQ_WM_DISPATCH
    message.  Examples of both are provided below.

    Handling the message inline:

    \code
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    kmq_message * m;
    khm_int32 rv;
    ...
    switch(uMsg) {
    case WM_CREATE:
       ...
       kmq_subscribe_hwnd(KMSG_CRED, hwnd);
       ...
       break;

    case WM_DESTROY:
       ...
       kmq_unsubscribe_hwnd(KMSG_CRED, hwnd);
       ...
       break;

    ...
    case KMQ_WM_DISPATCH:
        kmq_wm_begin(lParam,&m);

	if(m->type == KMSG_CRED && m->subtype == KMSG_CRED_ROOTDELTA) {
	// do something
        rv = KHM_ERROR_SUCCESS;
	}

	return kmq_wm_end(m, rv);
    ...
    };
    ...
    }
    \endcode

    The other method is to dispatch the ::KMQ_WM_DISPATCH message to a
    secondary callback function:

    \code
    khm_int32 msg_handler(khm_int32 t, khm_int32 st, khm_ui_4 up, void * pb) {
        khm_int32 rv = KHM_ERROR_SUCCESS;

        //handle message

	return rv;
    }

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    kmq_message * m;
    khm_int32 rv;
    ...
    switch(uMsg) {
    ...

    case WM_CREATE:
       ...
       kmq_subscribe_hwnd(KMSG_CRED, hwnd);
       ...
       break;

    case WM_DESTROY:
       ...
       kmq_unsubscribe_hwnd(KMSG_CRED, hwnd);
       ...
       break;

    ...
    case KMQ_WM_DISPATCH:
        return kmq_wm_dispatch(lParam, msg_handler);
    ...
    };
    ...
    }
    \endcode

    \note Make sure you unsubscribe from the message type when the
        window is destroyed.

    \see kmq_unsubscribe_hwnd()
    \see kmq_wm_begin()
    \see kmq_wm_end()
    \see kmq_wm_dispatch()
 */
KHMEXP khm_int32 KHMAPI kmq_subscribe_hwnd(khm_int32 type, HWND hwnd);

#ifdef _WIN32
/*! \brief Begins handling a KMQ_WM_DISPATCH message

    \return The return value of this function should be ignored.

    \see kmq_subscribe_hwnd() for more details about handling ::KMQ_WM_DISPATCH
 */
KHMEXP LRESULT KHMAPI kmq_wm_begin(LPARAM lparm, kmq_message ** m);

/*! \brief Ends handling a KMQ_WM_DISPATCH message

    \return The return value of this function should be the return
        value of the window procedure.  See kmq_subscribe_hwnd()
        documentation for example

    \see kmq_subscribe_hwnd() for more details about handling ::KMQ_WM_DISPATCH
 */
KHMEXP LRESULT KHMAPI kmq_wm_end(kmq_message *m, khm_int32 rv);

/*! \brief Dispatches a KMQ_WM_DISPATCH message to a callback

    \return The return value of this function should be the return
        value of the window procedure.  See kmq_subscribe_hwnd()
        documentation for example.

    \see kmq_subscribe_hwnd() for more details about handling ::KMQ_WM_DISPATCH
 */
KHMEXP LRESULT KHMAPI kmq_wm_dispatch(LPARAM lparm, kmq_callback_t cb);
#endif

/*! \brief Unsubscribe a callback from a message type

    Removes the subscription for message type \a type for callback
    function \a cb from the subscription list for the current thread
    (the thread that calls kmq_unsubscribe()).

    \note kmq_unsubscribe() can only remove subscriptions for the subscription
        list for the current thread.

    \see kmq_subscribe()
    \see kmq_dispatch()
*/
KHMEXP khm_int32 KHMAPI kmq_unsubscribe(khm_int32 type, kmq_callback_t cb);

/*! \brief Unsubscribe a window from a message type

    Removes the specific window from the subsription list for message
    type \a type.

    \see kmq_subscribe_hwnd()
*/
KHMEXP khm_int32 KHMAPI kmq_unsubscribe_hwnd(khm_int32 type, HWND hwnd);

/*! \brief Create an ad-hoc subscription

    An ad-hoc subscription describes a callback point in a thread that
    can be dispatched messages to individually without broadcasting.

    \see kmq_post_sub_msg(), kmq_post_sub_msg_ex(),
        kmq_send_sub_msg(), kmq_post_subs_msg(),
        kmq_post_subs_msg_ex(), kmq_send_subs_msg(),
        kmq_delete_subscription()
*/
KHMEXP khm_int32 KHMAPI kmq_create_subscription(
    kmq_callback_t cb, 
    khm_handle * result);

/*! \brief Delete an ad-hoc subscription

    Deletes a subscriptoin that was created using
    kmq_create_subscription()
 */
KHMEXP khm_int32 KHMAPI kmq_delete_subscription(khm_handle sub);

/*! \brief Post a message to a subscription

    Equivalent of kmq_post_msg() but only posts the message to the
    specified subscription.
 */
KHMEXP khm_int32 KHMAPI kmq_post_sub_msg(
    khm_handle sub, 
    khm_int32 type, 
    khm_int32 subtype, 
    khm_ui_4 uparam, 
    void * vparam);

/*! \brief Post a message to a subscription and acquire a handle to the call
 */
KHMEXP khm_int32 KHMAPI kmq_post_sub_msg_ex(
    khm_handle sub, 
    khm_int32 type, 
    khm_int32 subtype, 
    khm_ui_4 uparam, 
    void * vparam, 
    kmq_call * call);

/*! \brief Send a synchronous message to a subscription

    \retval KHM_ERROR_SUCCESS The call succeeded, and no subscribers reported errors
    \retval KHM_ERROR_PARTIAL The call succeeded, but at least one subscriber reported errors
 */
KHMEXP khm_int32 KHMAPI kmq_send_sub_msg(
    khm_handle sub, 
    khm_int32 type, 
    khm_int32 subtype, 
    khm_ui_4 uparam, 
    void * vparam);

/*! \brief Post a message to a group of subscriptions

    The block of memory pointed to by \a subs should be an array of
    subscriptions.  The number of elements in that array should be \a
    n_subs.  A message as specified by the remaining parameters will
    be dispatched to all of the subscription points in the array.
 */
KHMEXP khm_int32 KHMAPI kmq_post_subs_msg(
    khm_handle * subs, 
    khm_size  n_subs, 
    khm_int32 type, 
    khm_int32 subtype, 
    khm_ui_4 uparam, 
    void * vparam);

/*! \brief Post a message to a group of subscriptions and acquire a handle to the call

    The block of memory pointed to by \a subs should be an array of
    subscriptions.  The number of elements in that array should be \a
    n_subs.  A message as specified by the remaining parameters will
    be dispatched to all of the subscription points in the array, and
    a handle to the call will be returned in \a call.

    The returned \a call will reference all of the dispatches that
    were made.
*/
KHMEXP khm_int32 KHMAPI kmq_post_subs_msg_ex(
    khm_handle * subs, 
    khm_int32 n_subs, 
    khm_int32 type, 
    khm_int32 subtype, 
    khm_ui_4 uparam, 
    void * vparam, 
    kmq_call * call);

/*! \brief Send a synchronous message to a group of subscriptions

    The block of memory pointed to by \a subs should be an array of
    subscriptions.  The number of elements in that array should be \a
    n_subs.  A message as specified by the remaining parameters will
    be dispatched to all of the subscription points in the array.  The
    function will not return until all of the calls have succeeded.

    \retval KHM_ERROR_SUCCESS The call succeeded, and no subscribers reported errors
    \retval KHM_ERROR_PARTIAL The call succeeded, but at least one subscriber reported errors
*/
KHMEXP khm_int32 KHMAPI kmq_send_subs_msg(
    khm_handle *subs, 
    khm_int32 n_subs,
    khm_int32 type, 
    khm_int32 subtype, 
    khm_ui_4 uparam, 
    void * vparam);

/*! \brief Dispatch a message for the current thread.

    This function opens the message list for the current thread and
    dispatches the first message instance that is found.  Note that if
    multiple callbacks subscribe to the same message type in the same
    thread, then when a message of that type is received, multiple
    message instances are added to the message queue corresponding to
    each subscription.

    If no message instances are waiting in the queue, kmq_dispatch()
    waits for the \a timeout period for a message.

    \param[in] timeout The timeout period in milliseconds.  Specify INFINITE for
        kmq_dispatch() to wait indefinitely.

    \retval KHM_ERROR_SUCCESS A message instance was dispatched
    \retval KHM_ERROR_TIMEOUT The timeout period elapsed
    \retval KHM_ERROR_EXIT The message found on the queue was <KMSG_SYSTEM,KMSG_SYSTEM_EXIT>
*/
KHMEXP khm_int32 KHMAPI kmq_dispatch(kmq_timer timeout);

/*! \brief Send a message

    The specified message will be posted to all the subscribers of the
    message type.  Then the function will wait for all the subscribers
    to finish processing the message before returning.
    
    \param[in] type The type of the message
    \param[in] subtype The subtype
    \param[in] uparam The khm_ui_4 parameter for the message
    \param[in] blob The parameter blob for the message

    \note The internal timeout for this function is INFINITE.  If you
        it is desirable to use a different timeout, use
        kmq_post_message_ex() and kmq_wait() functions.

    \retval KHM_ERROR_SUCCESS The call succeeded and no subscribers returned errors
    \retval KHM_ERROR_PARTIAL The call succeeded but at least one subscriber returned an error
*/
KHMEXP khm_int32 KHMAPI kmq_send_message(
    khm_int32 type, 
    khm_int32 subtype, 
    khm_ui_4 uparam, 
    void * blob);

/*! \brief Post a message

    The specified message will be posted to all the subscribers of the
    message type.  The function returns immediately.
    
    If you want to be able to wait for all the subscribers to finish
    processing the message, you should use kmq_post_message_ex()
    instead.

    \param[in] type The type of the message
    \param[in] subtype The subtype
    \param[in] uparam The khm_ui_4 parameter for the message
    \param[in] blob The parameter blob for the message
*/
KHMEXP khm_int32 KHMAPI kmq_post_message(
    khm_int32 type, 
    khm_int32 subtype, 
    khm_ui_4 uparam, 
    void * blob);

/*! \brief Post a message and acquire a handle to the call.

    The specified message is posted to all the subscribers.  In
    addition, a handle is obtained for the call which can be used in
    subsequent call to kmq_free_call() or kmq_wait().

    Call kmq_free_call() to free the handle.

    \param[in] type The type of the message
    \param[in] subtype The subtype
    \param[in] uparam The khm_ui_4 parameter for the message
    \param[in] blob The parameter blob for the message
    \param[out] call Receives the call handle.  Set to NULL if the call handle is not required.

    \see kmq_free_call()
*/
KHMEXP khm_int32 KHMAPI kmq_post_message_ex(
    khm_int32 type, 
    khm_int32 subtype, 
    khm_ui_4 uparam, 
    void * blob, 
    kmq_call * call);

/*! \brief Free a handle to a call obtained through kmq_post_message_ex()

    All call handles obtained through kmq_post_message_ex() must be
    freed via a call to kmq_free_call().
*/
KHMEXP khm_int32 KHMAPI kmq_free_call(kmq_call call);

/*! \brief Sends a <KMSG_SYSTEM,KMSG_SYSTEM_EXIT> message to the specified thread.

    The message itself will not be received by any callback function,
    however, any kmq_dispatch() function that is currently active of
    becomes active will exit with a KHM_ERROR_EXIT code.
    kmq_send_thread_quit_message() will wait for this to happen before
    returning.
    */
KHMEXP khm_int32 KHMAPI kmq_send_thread_quit_message(
    kmq_thread_id thread, 
    khm_ui_4 uparam);

/*! \brief Post a <KMSG_SYSTEM,KMSG_SYSTEM_EXIT> message to the specified thread.

    The message itself will not be received by any callback function,
    however, any kmq_dispatch() function that is currently active of
    becomes active will exit with a KHM_ERROR_EXIT code.
    kmq_post_thread_quit_message() will return immediately.
    */
KHMEXP khm_int32 KHMAPI kmq_post_thread_quit_message(
    kmq_thread_id thread, 
    khm_ui_4 uparam, 
    kmq_call * call);

KHMEXP khm_int32 KHMAPI kmq_get_next_response(kmq_call call, void ** resp);

/*! \brief Check if a specific call has completed

    \return TRUE if the call has completed. FALSE otherwise.
*/
KHMEXP khm_boolean KHMAPI kmq_has_completed(kmq_call call);

/*! \brief Wait for a call to complete.

    Waits for the specified call to complete.  If the call dispatched
    to multiple recipients, the function waits for all dispatches to
    complete.

    If the call has already completed, then the function returns
    immediately.

    If more than one thread is waiting for a single message to
    complete, then only one of them will be released when the message
    compeltes.  Each subsequent thread will be released as each
    released thread calls kmq_free_call().

    \param[in] call A handle to a call.
    \param[in] timeout Specifies, in milliseconds, the amount of time
        to wait for the call to complete. Specify INFINITE to wait
        indefinitely.

    \retval KHM_ERROR_SUCCESS The call completed
    \retval KHM_ERROR_TIMEOUT The timeout period expired
    \retval KHM_ERROR_INVALID_PARM One of the parameters were invalid.
*/
KHMEXP khm_int32 KHMAPI kmq_wait(kmq_call call, kmq_timer timeout);

/*! \brief Sets the completion handler for a specified message type.

    \note Only one completion handler can exist for one message type.
        Calling this function overwrites the previous completion
        handler.
*/
KHMEXP khm_int32 KHMAPI kmq_set_completion_handler(
    khm_int32 type, 
    kmq_msg_completion_handler hander);

/*@}*/
#endif
