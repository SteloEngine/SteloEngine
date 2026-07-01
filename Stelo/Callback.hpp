/*
 * Copyright (c) 2026 Stelo Engine
 *
 * This file is part of Stelo Engine.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations
 * under the License.
*/

#pragma once

#include <functional>
#include <type_traits>
#include <utility>

namespace Stelo {
    template<typename SIGNATURE> class Callback;
    template<typename RETURN, typename... ARGS>
    class Callback<RETURN(ARGS...)> {
        template<typename F> struct IsInvokable {
        private:
            template<typename U>
            static auto test(int) -> typename std::is_convertible<
                decltype(std::declval<U>()(std::declval<ARGS>()...)),
                RETURN
            >::type;
            template<typename>
            static std::false_type test(...);
        public:
            static constexpr bool value = decltype(test<F>(0))::value;
        };

        static constexpr size_t SBO_SIZE  = sizeof(void*);
        static constexpr size_t SBO_ALIGN = alignof(void*);

        union Storage {
            alignas(SBO_ALIGN) char buf[SBO_SIZE];
            void* ptr;
        };

        template<typename T>
        static constexpr bool fits_sbo = sizeof(T) <= SBO_SIZE && alignof(T) <= SBO_ALIGN;

        using FuncType    = RETURN(*)(ARGS...);
        using InvokeFunc  = RETURN(*)(Storage&, ARGS...);
        using CopyFunc    = void(*)(Storage&, const Storage&);
        using MoveFunc    = void(*)(Storage&, Storage&);
        using DestroyFunc = void(*)(Storage&);

        struct NavTable {
            InvokeFunc  invoke;
            CopyFunc    copy;
            MoveFunc    move;
            DestroyFunc destroy;
        };

        Storage         _storage;
        const NavTable* _nav;

        static RETURN InvokeFuncPtr(Storage& s, ARGS... args) {
            return reinterpret_cast<FuncType>(s.ptr)(std::forward<ARGS>(args)...);
        }
        static void CopyFuncPtr   (Storage& dst, const Storage& src) { dst.ptr = src.ptr; }
        static void MoveFuncPtr   (Storage& dst, Storage& src)       { dst.ptr = src.ptr; src.ptr = nullptr; }
        static void DestroyFuncPtr(Storage&) {}

        static const NavTable* GetNavFuncPtr() {
            static const NavTable t{ InvokeFuncPtr, CopyFuncPtr, MoveFuncPtr, DestroyFuncPtr };
            return &t;
        }

        template<typename C>
        static RETURN InvokeSBO(Storage& s, ARGS... args) {
            return (*reinterpret_cast<C*>(s.buf))(std::forward<ARGS>(args)...);
        }
        template<typename C>
        static void CopySBO(Storage& dst, const Storage& src) {
            new (dst.buf) C(*reinterpret_cast<const C*>(src.buf));
        }
        template<typename C>
        static void MoveSBO(Storage& dst, Storage& src) {
            new (dst.buf) C(std::move(*reinterpret_cast<C*>(src.buf)));
            reinterpret_cast<C*>(src.buf)->~C();
        }
        template<typename C>
        static void DestroySBO(Storage& s) {
            reinterpret_cast<C*>(s.buf)->~C();
        }
        template<typename C>
        static const NavTable* GetNavSBO() {
            static const NavTable t{ InvokeSBO<C>, CopySBO<C>, MoveSBO<C>, DestroySBO<C> };
            return &t;
        }

        template<typename C>
        static RETURN InvokeHeap(Storage& s, ARGS... args) {
            return (*static_cast<C*>(s.ptr))(std::forward<ARGS>(args)...);
        }
        template<typename C>
        static void CopyHeap(Storage& dst, const Storage& src) {
            dst.ptr = new C(*static_cast<const C*>(src.ptr));
        }
        template<typename C>
        static void MoveHeap(Storage& dst, Storage& src) {
            dst.ptr = src.ptr;
            src.ptr = nullptr;
        }
        template<typename C>
        static void DestroyHeap(Storage& s) {
            delete static_cast<C*>(s.ptr);
        }
        template<typename C>
        static const NavTable* GetNavHeap() {
            static const NavTable t{ InvokeHeap<C>, CopyHeap<C>, MoveHeap<C>, DestroyHeap<C> };
            return &t;
        }

        template<typename AC>
        using EnableIfCallable = std::enable_if_t<
            !std::is_same_v<std::decay_t<AC>, Callback> &&
            !std::is_convertible_v<std::decay_t<AC>, RETURN(*)(ARGS...)> &&
            IsInvokable<std::decay_t<AC>>::value,
            int
        >;

    public:
        Callback() noexcept : _nav(nullptr) { _storage.ptr = nullptr; }

        Callback(const Callback& other) : _nav(other._nav) {
            if (_nav) _nav->copy(_storage, other._storage);
            else      _storage.ptr = nullptr;
        }

        Callback(Callback&& other) noexcept : _nav(other._nav) {
            if (_nav) { _nav->move(_storage, other._storage); other._nav = nullptr; }
            else      _storage.ptr = nullptr;
        }

        Callback(RETURN(*func)(ARGS...)) : _nav(GetNavFuncPtr()) {
            _storage.ptr = reinterpret_cast<void*>(func);
        }

        template<typename AC, EnableIfCallable<AC> = 0>
        Callback(AC&& callable) {
            using C = std::decay_t<AC>;
            if constexpr (fits_sbo<C>) {
                new (_storage.buf) C(std::forward<AC>(callable));
                _nav = GetNavSBO<C>();
            } else {
                _storage.ptr = new C(std::forward<AC>(callable));
                _nav = GetNavHeap<C>();
            }
        }

        ~Callback() {
            if (_nav) _nav->destroy(_storage);
        }

        Callback& operator=(const Callback& other) {
            if (this != &other) {
                if (_nav) _nav->destroy(_storage);
                _nav = other._nav;
                if (_nav) _nav->copy(_storage, other._storage);
                else      _storage.ptr = nullptr;
            }
            return *this;
        }

        Callback& operator=(Callback&& other) noexcept {
            if (this != &other) {
                if (_nav) _nav->destroy(_storage);
                _nav = other._nav;
                if (_nav) { _nav->move(_storage, other._storage); other._nav = nullptr; }
                else      _storage.ptr = nullptr;
            }
            return *this;
        }

        Callback& operator=(RETURN(*func)(ARGS...)) {
            if (_nav) _nav->destroy(_storage);
            _storage.ptr = reinterpret_cast<void*>(func);
            _nav = GetNavFuncPtr();
            return *this;
        }

        inline RETURN operator()(ARGS... args) const {
            if (!_nav) throw std::bad_function_call();
            return _nav->invoke(const_cast<Storage&>(_storage), std::forward<ARGS>(args)...);
        }

        inline operator bool() const noexcept {
            return IsValid();
        }
        inline bool IsValid() const noexcept {
            return _nav != nullptr;
        }
    };
}