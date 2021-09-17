//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
//
// Ice version 3.7.5
//
// <auto-generated>
//
// Generated from file `io_channel_ice.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

#ifndef __io_channel_ice_h__
#define __io_channel_ice_h__

#include <IceUtil/PushDisableWarnings.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/ValueF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/StreamHelpers.h>
#include <Ice/Comparable.h>
#include <Ice/Proxy.h>
#include <Ice/Object.h>
#include <Ice/GCObject.h>
#include <Ice/Value.h>
#include <Ice/Incoming.h>
#include <Ice/FactoryTableInit.h>
#include <IceUtil/ScopedArray.h>
#include <Ice/Optional.h>
#include <IceUtil/UndefSysMacros.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 307
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 >= 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 5
#       error Ice patch level mismatch!
#   endif
#endif

#ifdef ICE_CPP11_MAPPING // C++11 mapping

namespace ChannelSdk
{

class IoChannel;
class IoChannelPrx;

}

namespace ChannelSdk
{

using bytes = ::std::vector<::Ice::Byte>;

}

namespace ChannelSdk
{

class IoChannel : public virtual ::Ice::Object
{
public:

    using ProxyType = IoChannelPrx;

    /**
     * Determines whether this object supports an interface with the given Slice type ID.
     * @param id The fully-scoped Slice type ID.
     * @param current The Current object for the invocation.
     * @return True if this object supports the interface, false, otherwise.
     */
    virtual bool ice_isA(::std::string id, const ::Ice::Current& current) const override;

    /**
     * Obtains a list of the Slice type IDs representing the interfaces supported by this object.
     * @param current The Current object for the invocation.
     * @return A list of fully-scoped type IDs.
     */
    virtual ::std::vector<::std::string> ice_ids(const ::Ice::Current& current) const override;

    /**
     * Obtains a Slice type ID representing the most-derived interface supported by this object.
     * @param current The Current object for the invocation.
     * @return A fully-scoped type ID.
     */
    virtual ::std::string ice_id(const ::Ice::Current& current) const override;

    /**
     * Obtains the Slice type ID corresponding to this class.
     * @return A fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

    virtual int send(::std::string nodeid, ::std::string msgid, bytes data, const ::Ice::Current& current) = 0;
    /// \cond INTERNAL
    bool _iceD_send(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

    /// \cond INTERNAL
    virtual bool _iceDispatch(::IceInternal::Incoming&, const ::Ice::Current&) override;
    /// \endcond
};

}

namespace ChannelSdk
{

class IoChannelPrx : public virtual ::Ice::Proxy<IoChannelPrx, ::Ice::ObjectPrx>
{
public:

    int send(const ::std::string& nodeid, const ::std::string& msgid, const bytes& data, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _makePromiseOutgoing<int>(true, this, &IoChannelPrx::_iceI_send, nodeid, msgid, data, context).get();
    }

    template<template<typename> class P = ::std::promise>
    auto sendAsync(const ::std::string& nodeid, const ::std::string& msgid, const bytes& data, const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(::std::declval<P<int>>().get_future())
    {
        return _makePromiseOutgoing<int, P>(false, this, &IoChannelPrx::_iceI_send, nodeid, msgid, data, context);
    }

    ::std::function<void()>
    sendAsync(const ::std::string& nodeid, const ::std::string& msgid, const bytes& data,
              ::std::function<void(int)> response,
              ::std::function<void(::std::exception_ptr)> ex = nullptr,
              ::std::function<void(bool)> sent = nullptr,
              const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _makeLamdaOutgoing<int>(std::move(response), std::move(ex), std::move(sent), this, &ChannelSdk::IoChannelPrx::_iceI_send, nodeid, msgid, data, context);
    }

    /// \cond INTERNAL
    void _iceI_send(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<int>>&, const ::std::string&, const ::std::string&, const bytes&, const ::Ice::Context&);
    /// \endcond

    /**
     * Obtains the Slice type ID of this interface.
     * @return The fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

protected:

    /// \cond INTERNAL
    IoChannelPrx() = default;
    friend ::std::shared_ptr<IoChannelPrx> IceInternal::createProxy<IoChannelPrx>();

    virtual ::std::shared_ptr<::Ice::ObjectPrx> _newInstance() const override;
    /// \endcond
};

}

/// \cond STREAM
namespace Ice
{

}
/// \endcond

/// \cond INTERNAL
namespace ChannelSdk
{

using IoChannelPtr = ::std::shared_ptr<IoChannel>;
using IoChannelPrxPtr = ::std::shared_ptr<IoChannelPrx>;

}
/// \endcond

#else // C++98 mapping

namespace IceProxy
{

namespace ChannelSdk
{

class IoChannel;
/// \cond INTERNAL
void _readProxy(::Ice::InputStream*, ::IceInternal::ProxyHandle< IoChannel>&);
::IceProxy::Ice::Object* upCast(IoChannel*);
/// \endcond

}

}

namespace ChannelSdk
{

class IoChannel;
/// \cond INTERNAL
::Ice::Object* upCast(IoChannel*);
/// \endcond
typedef ::IceInternal::Handle< IoChannel> IoChannelPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::ChannelSdk::IoChannel> IoChannelPrx;
typedef IoChannelPrx IoChannelPrxPtr;
/// \cond INTERNAL
void _icePatchObjectPtr(IoChannelPtr&, const ::Ice::ObjectPtr&);
/// \endcond

}

namespace ChannelSdk
{

typedef ::std::vector< ::Ice::Byte> bytes;

}

namespace ChannelSdk
{

/**
 * Base class for asynchronous callback wrapper classes used for calls to
 * IceProxy::ChannelSdk::IoChannel::begin_send.
 * Create a wrapper instance by calling ::ChannelSdk::newCallback_IoChannel_send.
 */
class Callback_IoChannel_send_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_IoChannel_send_Base> Callback_IoChannel_sendPtr;

}

namespace IceProxy
{

namespace ChannelSdk
{

class IoChannel : public virtual ::Ice::Proxy<IoChannel, ::IceProxy::Ice::Object>
{
public:

    ::Ice::Int send(const ::std::string& nodeid, const ::std::string& msgid, const ::ChannelSdk::bytes& data, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return end_send(_iceI_begin_send(nodeid, msgid, data, context, ::IceInternal::dummyCallback, 0, true));
    }

    ::Ice::AsyncResultPtr begin_send(const ::std::string& nodeid, const ::std::string& msgid, const ::ChannelSdk::bytes& data, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _iceI_begin_send(nodeid, msgid, data, context, ::IceInternal::dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_send(const ::std::string& nodeid, const ::std::string& msgid, const ::ChannelSdk::bytes& data, const ::Ice::CallbackPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_send(nodeid, msgid, data, ::Ice::noExplicitContext, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_send(const ::std::string& nodeid, const ::std::string& msgid, const ::ChannelSdk::bytes& data, const ::Ice::Context& context, const ::Ice::CallbackPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_send(nodeid, msgid, data, context, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_send(const ::std::string& nodeid, const ::std::string& msgid, const ::ChannelSdk::bytes& data, const ::ChannelSdk::Callback_IoChannel_sendPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_send(nodeid, msgid, data, ::Ice::noExplicitContext, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_send(const ::std::string& nodeid, const ::std::string& msgid, const ::ChannelSdk::bytes& data, const ::Ice::Context& context, const ::ChannelSdk::Callback_IoChannel_sendPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_send(nodeid, msgid, data, context, cb, cookie);
    }

    ::Ice::Int end_send(const ::Ice::AsyncResultPtr& result);

private:

    ::Ice::AsyncResultPtr _iceI_begin_send(const ::std::string&, const ::std::string&, const ::ChannelSdk::bytes&, const ::Ice::Context&, const ::IceInternal::CallbackBasePtr&, const ::Ice::LocalObjectPtr& cookie = 0, bool sync = false);

public:

    /**
     * Obtains the Slice type ID corresponding to this interface.
     * @return A fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

protected:
    /// \cond INTERNAL

    virtual ::IceProxy::Ice::Object* _newInstance() const;
    /// \endcond
};

}

}

namespace ChannelSdk
{

class IoChannel : public virtual ::Ice::Object
{
public:

    typedef IoChannelPrx ProxyType;
    typedef IoChannelPtr PointerType;

    virtual ~IoChannel();

#ifdef ICE_CPP11_COMPILER
    IoChannel() = default;
    IoChannel(const IoChannel&) = default;
    IoChannel& operator=(const IoChannel&) = default;
#endif

    /**
     * Determines whether this object supports an interface with the given Slice type ID.
     * @param id The fully-scoped Slice type ID.
     * @param current The Current object for the invocation.
     * @return True if this object supports the interface, false, otherwise.
     */
    virtual bool ice_isA(const ::std::string& id, const ::Ice::Current& current = ::Ice::emptyCurrent) const;

    /**
     * Obtains a list of the Slice type IDs representing the interfaces supported by this object.
     * @param current The Current object for the invocation.
     * @return A list of fully-scoped type IDs.
     */
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& current = ::Ice::emptyCurrent) const;

    /**
     * Obtains a Slice type ID representing the most-derived interface supported by this object.
     * @param current The Current object for the invocation.
     * @return A fully-scoped type ID.
     */
    virtual const ::std::string& ice_id(const ::Ice::Current& current = ::Ice::emptyCurrent) const;

    /**
     * Obtains the Slice type ID corresponding to this class.
     * @return A fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

    virtual ::Ice::Int send(const ::std::string& nodeid, const ::std::string& msgid, const bytes& data, const ::Ice::Current& current = ::Ice::emptyCurrent) = 0;
    /// \cond INTERNAL
    bool _iceD_send(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

    /// \cond INTERNAL
    virtual bool _iceDispatch(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

protected:

    /// \cond STREAM
    virtual void _iceWriteImpl(::Ice::OutputStream*) const;
    virtual void _iceReadImpl(::Ice::InputStream*);
    /// \endcond
};

/// \cond INTERNAL
inline bool operator==(const IoChannel& lhs, const IoChannel& rhs)
{
    return static_cast<const ::Ice::Object&>(lhs) == static_cast<const ::Ice::Object&>(rhs);
}

inline bool operator<(const IoChannel& lhs, const IoChannel& rhs)
{
    return static_cast<const ::Ice::Object&>(lhs) < static_cast<const ::Ice::Object&>(rhs);
}
/// \endcond

}

/// \cond STREAM
namespace Ice
{

}
/// \endcond

namespace ChannelSdk
{

/**
 * Type-safe asynchronous callback wrapper class used for calls to
 * IceProxy::ChannelSdk::IoChannel::begin_send.
 * Create a wrapper instance by calling ::ChannelSdk::newCallback_IoChannel_send.
 */
template<class T>
class CallbackNC_IoChannel_send : public Callback_IoChannel_send_Base, public ::IceInternal::TwowayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)(::Ice::Int);

    CallbackNC_IoChannel_send(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::TwowayCallbackNC<T>(obj, cb != 0, excb, sentcb), _response(cb)
    {
    }

    /// \cond INTERNAL
    virtual void completed(const ::Ice::AsyncResultPtr& result) const
    {
        IoChannelPrx proxy = IoChannelPrx::uncheckedCast(result->getProxy());
        ::Ice::Int ret;
        try
        {
            ret = proxy->end_send(result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::CallbackNC<T>::exception(result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::CallbackNC<T>::_callback.get()->*_response)(ret);
        }
    }
    /// \endcond

private:

    Response _response;
};

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::ChannelSdk::IoChannel::begin_send.
 */
template<class T> Callback_IoChannel_sendPtr
newCallback_IoChannel_send(const IceUtil::Handle<T>& instance, void (T::*cb)(::Ice::Int), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_IoChannel_send<T>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::ChannelSdk::IoChannel::begin_send.
 */
template<class T> Callback_IoChannel_sendPtr
newCallback_IoChannel_send(T* instance, void (T::*cb)(::Ice::Int), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_IoChannel_send<T>(instance, cb, excb, sentcb);
}

/**
 * Type-safe asynchronous callback wrapper class with cookie support used for calls to
 * IceProxy::ChannelSdk::IoChannel::begin_send.
 * Create a wrapper instance by calling ::ChannelSdk::newCallback_IoChannel_send.
 */
template<class T, typename CT>
class Callback_IoChannel_send : public Callback_IoChannel_send_Base, public ::IceInternal::TwowayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception& , const CT&);
    typedef void (T::*Sent)(bool , const CT&);
    typedef void (T::*Response)(::Ice::Int, const CT&);

    Callback_IoChannel_send(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::TwowayCallback<T, CT>(obj, cb != 0, excb, sentcb), _response(cb)
    {
    }

    /// \cond INTERNAL
    virtual void completed(const ::Ice::AsyncResultPtr& result) const
    {
        IoChannelPrx proxy = IoChannelPrx::uncheckedCast(result->getProxy());
        ::Ice::Int ret;
        try
        {
            ret = proxy->end_send(result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::Callback<T, CT>::exception(result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::Callback<T, CT>::_callback.get()->*_response)(ret, CT::dynamicCast(result->getCookie()));
        }
    }
    /// \endcond

private:

    Response _response;
};

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::ChannelSdk::IoChannel::begin_send.
 */
template<class T, typename CT> Callback_IoChannel_sendPtr
newCallback_IoChannel_send(const IceUtil::Handle<T>& instance, void (T::*cb)(::Ice::Int, const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_IoChannel_send<T, CT>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::ChannelSdk::IoChannel::begin_send.
 */
template<class T, typename CT> Callback_IoChannel_sendPtr
newCallback_IoChannel_send(T* instance, void (T::*cb)(::Ice::Int, const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_IoChannel_send<T, CT>(instance, cb, excb, sentcb);
}

}

#endif

#include <IceUtil/PopDisableWarnings.h>
#endif
