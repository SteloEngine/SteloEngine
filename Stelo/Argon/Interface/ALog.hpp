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
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <chrono>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <string_view>
#include <utility>

class ALog {
  public:
    enum class Level : uint8_t { Debug = 0, Info = 1, Warning = 2, Error = 3 };

  private:
    struct LogHeader {
        uint64_t time;
        uint32_t messageLength;
        uint16_t categoryLength;
        uint8_t level;
        uint8_t reserved;
    };
    static constexpr size_t INLINE_CAPACITY = 256 - (sizeof(char*) + 2 * sizeof(size_t));

    alignas(uint64_t) char mInlineBuffer[INLINE_CAPACITY];
    char* pStorage;
    size_t mSize;
    size_t mCapacity;

  private:
    static_assert(sizeof(LogHeader) == 16, "LogHeader must remain compact");

    static uint64_t GetTimeMS() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    }
    static size_t Align8(size_t v) {
        return (v + 7ull) & ~7ull;
    }
    static uint16_t ClampCategoryLength(size_t length) {
        const size_t maxLength = static_cast<size_t>(std::numeric_limits<uint16_t>::max());
        return static_cast<uint16_t>(length > maxLength ? maxLength : length);
    }
    static uint32_t ClampMessageLength(size_t length) {
        const size_t maxLength = static_cast<size_t>(std::numeric_limits<uint32_t>::max());
        return static_cast<uint32_t>(length > maxLength ? maxLength : length);
    }
    static const char* LevelName(Level level) {
        switch (level) {
        case Level::Debug:
            return "Debug";
        case Level::Info:
            return "Info";
        case Level::Warning:
            return "Warning";
        case Level::Error:
            return "Error";
        }
        return "Info";
    }
    static void AppendUnsigned(std::string& out, uint64_t value) {
        char buffer[32];
        auto [end, error] = std::to_chars(buffer, buffer + sizeof(buffer), value);
        if (error == std::errc()) {
            out.append(buffer, static_cast<size_t>(end - buffer));
            return;
        }
        out += '0';
    }

    bool IsUsingInline() const {
        return pStorage == mInlineBuffer;
    }

    void MoveFrom(ALog&& other) noexcept {
        mSize = other.mSize;
        mCapacity = other.mCapacity;

        if (other.IsUsingInline()) {
            std::memcpy(mInlineBuffer, other.mInlineBuffer, other.mSize);
            pStorage = mInlineBuffer;
        } else {
            pStorage = other.pStorage;
            other.pStorage = other.mInlineBuffer;
            other.mCapacity = INLINE_CAPACITY;
            other.mSize = 0;
        }

        other.mSize = 0;
        other.mCapacity = INLINE_CAPACITY;
    }

  public:
    ALog() : pStorage(mInlineBuffer), mSize(0), mCapacity(INLINE_CAPACITY) {}
    ~ALog() {
        if (!IsUsingInline()) delete[] pStorage;
    }

    ALog(const ALog&) = delete;
    ALog& operator=(const ALog&) = delete;

    ALog(ALog&& other) noexcept {
        MoveFrom(std::move(other));
    }

    ALog& operator=(ALog&& other) noexcept {
        if (this == &other) return *this;
        if (!IsUsingInline()) delete[] pStorage;

        MoveFrom(std::move(other));

        return *this;
    }

  public:
    size_t Size() const {
        return mSize;
    }
    size_t Capacity() const {
        return mCapacity;
    }

    void Reset() {
        mSize = 0;
    }

    void Reserve(size_t capacity) {
        if (capacity <= mCapacity) return;

        char* newStorage = new char[capacity];
        std::memcpy(newStorage, pStorage, mSize);
        if (!IsUsingInline()) delete[] pStorage;

        pStorage = newStorage;
        mCapacity = capacity;
    }

    void EnsureCapacity(size_t additionalBytes) {
        size_t required = mSize + additionalBytes;
        if (required <= mCapacity) return;
        size_t newCapacity = mCapacity * 2;
        while (newCapacity < required)
            newCapacity *= 2;

        Reserve(newCapacity);
    }

    void Write(const char* text) {
        if (!text) return;
        Write(Level::Info, {}, std::string_view(text));
    }
    void Write(std::string_view text) {
        Write(Level::Info, {}, text);
    }
    void Write(Level level, std::string_view text) {
        Write(level, {}, text);
    }
    void Write(Level level, const char* category, std::string_view text) {
        Write(level, category ? std::string_view(category) : std::string_view(), text);
    }

    void Write(Level level, std::string_view category, std::string_view text) {
        const uint16_t categoryLength = ClampCategoryLength(category.size());
        const uint32_t messageLength = ClampMessageLength(text.size());
        const size_t writeSize = Align8(sizeof(LogHeader) + categoryLength + messageLength);

        EnsureCapacity(writeSize);

        auto* header = reinterpret_cast<LogHeader*>(pStorage + mSize);

        header->time = GetTimeMS();
        header->messageLength = messageLength;
        header->categoryLength = categoryLength;
        header->level = static_cast<uint8_t>(level);
        header->reserved = 0;

        char* categoryDst = reinterpret_cast<char*>(header + 1);
        char* messageDst = categoryDst + categoryLength;

        if (categoryLength != 0) {
            std::memcpy(categoryDst, category.data(), categoryLength);
        }
        if (messageLength != 0) {
            std::memcpy(messageDst, text.data(), messageLength);
        }
        mSize += writeSize;
    }

    std::string ToString() const {
        std::string out;
        out.reserve(mSize + 32);

        size_t offset = 0;
        while (offset < mSize) {
            auto* header = reinterpret_cast<const LogHeader*>(pStorage + offset);
            const char* category = reinterpret_cast<const char*>(header + 1);
            const char* message = category + header->categoryLength;

            out.push_back('[');
            AppendUnsigned(out, header->time);
            out += "] [";
            out += LevelName(static_cast<Level>(header->level));
            out.push_back(']');
            out.push_back(' ');

            if (header->categoryLength != 0) {
                out.append(category, header->categoryLength);
                out += ": ";
            }

            out.append(message, header->messageLength);
            out.push_back('\n');

            offset += Align8(sizeof(LogHeader) + header->categoryLength + header->messageLength);
        }
        return out;
    }
};
