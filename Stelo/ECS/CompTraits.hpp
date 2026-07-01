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

#include <type_traits>
#include <utility>

#define DEFINE_HAS_MEMBER(member, param_type)                    	\
template <typename T, typename = void>                           	\
constexpr bool Has##member = false;                             	\
template <typename T>                                            	\
constexpr bool Has##member<T, std::void_t<decltype(&T::member)>> = std::is_invocable_v<decltype(&T::member), T&, param_type>;

#define DEFINE_HAS_MEMBER_NOPARAM(member)                       	\
template <typename T, typename = void>                          	\
constexpr bool Has##member = false;                            	    \
template <typename T>                                           	\
constexpr bool Has##member<T, std::void_t<decltype(&T::member)>> = std::is_invocable_v<decltype(&T::member), T&>;

namespace Stelo {
    DEFINE_HAS_MEMBER(PreFixedUpdate, float)
	DEFINE_HAS_MEMBER(FixedUpdate, float)
    
	DEFINE_HAS_MEMBER(PreUpdate, float)
	DEFINE_HAS_MEMBER(Update, float)
	DEFINE_HAS_MEMBER(PostUpdate, float)

	DEFINE_HAS_MEMBER_NOPARAM(Start)
    DEFINE_HAS_MEMBER_NOPARAM(PreRender)
	DEFINE_HAS_MEMBER_NOPARAM(Render)
    DEFINE_HAS_MEMBER_NOPARAM(PostRender)

	DEFINE_HAS_MEMBER_NOPARAM(OnAwake)
	DEFINE_HAS_MEMBER_NOPARAM(OnDestroy)
	DEFINE_HAS_MEMBER_NOPARAM(OnEnable)
	DEFINE_HAS_MEMBER_NOPARAM(OnDisable)
} // Stelo

#undef DEFINE_HAS_MEMBER
#undef DEFINE_HAS_MEMBER_NOPARAM