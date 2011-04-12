// Copied from mongo util/concurrency/mvar.h. Added noncopyable, read, and with. Fixed take so it does not construct a default T.

/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef MVAR_H_
#define MVAR_H_

#include <boost/thread/condition.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>

#define MVAR(T) boost::shared_ptr< var::MVar_< T > >

namespace var {

    /* This is based on haskell's MVar synchronization primitive:
     * http://www.haskell.org/ghc/docs/latest/html/libraries/base-4.2.0.0/Control-Concurrent-MVar.html
     *
     * It is a thread-safe queue that can hold at most one object.
     * You can also think of it as a box that can be either full or empty.
     */

    template <typename T>
    class MVar_ : boost::noncopyable {
    public:
        enum State {EMPTY=0, FULL};

        // create an empty MVar
        MVar_()
            : _state(EMPTY)
        {}

        // creates a full MVar
        MVar_(const T& val)
            : _state(FULL)
            , _value(val)
        {}

        // puts val into the MVar and returns true or returns false if full
        // never blocks
        bool tryPut(const T& val) {
            // intentionally repeat test before and after lock
            if (_state == FULL) return false;
            Mutex::scoped_lock lock(_mutex);
            if (_state == FULL) return false;

            _state = FULL;
            _value = val;

            // unblock threads waiting to 'take'
            _condition.notify_all();

            return true;
        }

        // puts val into the MVar
        // will block if the MVar is already full
        void put(const T& val) {
            Mutex::scoped_lock lock(_mutex);
            while (!tryPut(val)) {
                // unlocks lock while waiting and relocks before returning
                _condition.wait(lock);
            }
        }

        // takes val out of the MVar if exists, otherwise return Nothing.
        // never blocks
        boost::optional<T> tryTake() {
            // intentionally repeat test before and after lock
            if (_state == EMPTY) return boost::optional<T>(); //Nothing
            Mutex::scoped_lock lock(_mutex);
            if (_state == EMPTY) return boost::optional<T>();

            _state = EMPTY;
            // unblock threads waiting to 'put'
            _condition.notify_all();

            return boost::optional<T>(_value);
        }

        // takes val out of the MVar
        // will block if the MVar is empty
        T take() {
            boost::optional<T> oval;

            Mutex::scoped_lock lock(_mutex);
            while (!(oval = tryTake())) {
                // unlocks lock while waiting and relocks before returning
                _condition.wait(lock);
            }

            return *oval;
        }

        // Note: this is fast because there is no locking, but state could
        // change before you get a chance to act on it.
        // Mainly useful for sanity checks / asserts.
        State getState() { return _state; }

        // Get copy of current content without taking, wait if empty
        T read() {
        	T value = take();
        	put (value);
        	return value;
        }


    private:
        State _state;
        T _value;
        typedef boost::recursive_mutex Mutex;
        Mutex _mutex;
        boost::condition _condition;
    };

    // Execute action with exclusive access to contents of MVar
    template <class T, class A> A with (MVar_<T>& mvar, const boost::function1<A,T&> &action) {
    	T value = mvar.take();
    	try {
    		A result = action (value);
    		mvar.put (value);
    		return result;
    	} catch (...) {
    		mvar.put (value);
    		throw;
    	}
    }

    /** Same as 'with' above except use RAII scope. Contructor takes from the supplied MVar, destructor puts it back. Taken value is available through operator*. */
    template <class T> class Access {
    public:
    	Access (MVar_<T>& mvar) : mvar(&mvar), value (mvar.take()) {}
    	~Access () {mvar->put (value);}
    	T& operator* () {return value;}
    	T* operator->() {return &value;}
    private:
    	MVar_<T>* mvar;
    	T value;
    };

}

#endif /* MVAR_H_ */
