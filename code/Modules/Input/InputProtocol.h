#pragma once
//-----------------------------------------------------------------------------
/* #version:10#
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
#include "Messaging/Serializer.h"
#include "Messaging/Protocol.h"
#include "glm/vec2.hpp"
#include "Input/Core/Mouse.h"
#include "Input/Core/Key.h"

namespace Oryol {
class InputProtocol {
public:
    static ProtocolIdType GetProtocolId() {
        return 'IPPT';
    };
    class MessageId {
    public:
        enum {
            MouseMoveId = Protocol::MessageId::NumMessageIds, 
            MouseButtonId,
            MouseScrollId,
            KeyId,
            WCharId,
            NumMessageIds
        };
        static const char* ToString(MessageIdType c) {
            switch (c) {
                case MouseMoveId: return "MouseMoveId";
                case MouseButtonId: return "MouseButtonId";
                case MouseScrollId: return "MouseScrollId";
                case KeyId: return "KeyId";
                case WCharId: return "WCharId";
                default: return "InvalidMessageId";
            }
        };
        static MessageIdType FromString(const char* str) {
            if (std::strcmp("MouseMoveId", str) == 0) return MouseMoveId;
            if (std::strcmp("MouseButtonId", str) == 0) return MouseButtonId;
            if (std::strcmp("MouseScrollId", str) == 0) return MouseScrollId;
            if (std::strcmp("KeyId", str) == 0) return KeyId;
            if (std::strcmp("WCharId", str) == 0) return WCharId;
            return InvalidMessageId;
        };
    };
    typedef Ptr<Message> (*CreateCallback)();
    static CreateCallback jumpTable[InputProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Ptr<Message> Create(MessageIdType id);
    };
    class MouseMove : public Message {
        OryolClassDecl(MouseMove);
        OryolTypeDecl(MouseMove,Message);
    public:
        MouseMove() {
            this->msgId = MessageId::MouseMoveId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::MouseMoveId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetMovement(const glm::vec2& val) {
            this->movement = val;
        };
        const glm::vec2& GetMovement() const {
            return this->movement;
        };
        void SetPosition(const glm::vec2& val) {
            this->position = val;
        };
        const glm::vec2& GetPosition() const {
            return this->position;
        };
private:
        glm::vec2 movement;
        glm::vec2 position;
    };
    class MouseButton : public Message {
        OryolClassDecl(MouseButton);
        OryolTypeDecl(MouseButton,Message);
    public:
        MouseButton() {
            this->msgId = MessageId::MouseButtonId;
            this->down = false;
            this->up = false;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::MouseButtonId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetMouseButton(const Oryol::Mouse::Button& val) {
            this->mousebutton = val;
        };
        const Oryol::Mouse::Button& GetMouseButton() const {
            return this->mousebutton;
        };
        void SetDown(bool val) {
            this->down = val;
        };
        bool GetDown() const {
            return this->down;
        };
        void SetUp(bool val) {
            this->up = val;
        };
        bool GetUp() const {
            return this->up;
        };
private:
        Oryol::Mouse::Button mousebutton;
        bool down;
        bool up;
    };
    class MouseScroll : public Message {
        OryolClassDecl(MouseScroll);
        OryolTypeDecl(MouseScroll,Message);
    public:
        MouseScroll() {
            this->msgId = MessageId::MouseScrollId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::MouseScrollId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetScroll(const glm::vec2& val) {
            this->scroll = val;
        };
        const glm::vec2& GetScroll() const {
            return this->scroll;
        };
private:
        glm::vec2 scroll;
    };
    class Key : public Message {
        OryolClassDecl(Key);
        OryolTypeDecl(Key,Message);
    public:
        Key() {
            this->msgId = MessageId::KeyId;
            this->key = Oryol::Key::InvalidKey;
            this->down = false;
            this->up = false;
            this->repeat = false;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::KeyId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetKey(const Oryol::Key::Code& val) {
            this->key = val;
        };
        const Oryol::Key::Code& GetKey() const {
            return this->key;
        };
        void SetDown(bool val) {
            this->down = val;
        };
        bool GetDown() const {
            return this->down;
        };
        void SetUp(bool val) {
            this->up = val;
        };
        bool GetUp() const {
            return this->up;
        };
        void SetRepeat(bool val) {
            this->repeat = val;
        };
        bool GetRepeat() const {
            return this->repeat;
        };
private:
        Oryol::Key::Code key;
        bool down;
        bool up;
        bool repeat;
    };
    class WChar : public Message {
        OryolClassDecl(WChar);
        OryolTypeDecl(WChar,Message);
    public:
        WChar() {
            this->msgId = MessageId::WCharId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::WCharId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetWChar(const wchar_t& val) {
            this->wchar = val;
        };
        const wchar_t& GetWChar() const {
            return this->wchar;
        };
private:
        wchar_t wchar;
    };
};
}
