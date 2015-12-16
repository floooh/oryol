#pragma once
//-----------------------------------------------------------------------------
/* #version:14#
    machine generated, do not edit!
*/
#include <cstring>
#include "Messaging/Message.h"
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
            MouseMoveEventId = Protocol::MessageId::NumMessageIds, 
            MouseButtonEventId,
            MouseScrollEventId,
            KeyEventId,
            WCharEventId,
            TouchTappedEventId,
            TouchDoubleTappedEventId,
            TouchPanningStartedEventId,
            TouchPanningEventId,
            TouchPanningEndedEventId,
            TouchPinchingStartedEventId,
            TouchPinchingEventId,
            TouchPinchingEndedEventId,
            NumMessageIds
        };
        static const char* ToString(MessageIdType c) {
            switch (c) {
                case MouseMoveEventId: return "MouseMoveEventId";
                case MouseButtonEventId: return "MouseButtonEventId";
                case MouseScrollEventId: return "MouseScrollEventId";
                case KeyEventId: return "KeyEventId";
                case WCharEventId: return "WCharEventId";
                case TouchTappedEventId: return "TouchTappedEventId";
                case TouchDoubleTappedEventId: return "TouchDoubleTappedEventId";
                case TouchPanningStartedEventId: return "TouchPanningStartedEventId";
                case TouchPanningEventId: return "TouchPanningEventId";
                case TouchPanningEndedEventId: return "TouchPanningEndedEventId";
                case TouchPinchingStartedEventId: return "TouchPinchingStartedEventId";
                case TouchPinchingEventId: return "TouchPinchingEventId";
                case TouchPinchingEndedEventId: return "TouchPinchingEndedEventId";
                default: return "InvalidMessageId";
            }
        };
        static MessageIdType FromString(const char* str) {
            if (std::strcmp("MouseMoveEventId", str) == 0) return MouseMoveEventId;
            if (std::strcmp("MouseButtonEventId", str) == 0) return MouseButtonEventId;
            if (std::strcmp("MouseScrollEventId", str) == 0) return MouseScrollEventId;
            if (std::strcmp("KeyEventId", str) == 0) return KeyEventId;
            if (std::strcmp("WCharEventId", str) == 0) return WCharEventId;
            if (std::strcmp("TouchTappedEventId", str) == 0) return TouchTappedEventId;
            if (std::strcmp("TouchDoubleTappedEventId", str) == 0) return TouchDoubleTappedEventId;
            if (std::strcmp("TouchPanningStartedEventId", str) == 0) return TouchPanningStartedEventId;
            if (std::strcmp("TouchPanningEventId", str) == 0) return TouchPanningEventId;
            if (std::strcmp("TouchPanningEndedEventId", str) == 0) return TouchPanningEndedEventId;
            if (std::strcmp("TouchPinchingStartedEventId", str) == 0) return TouchPinchingStartedEventId;
            if (std::strcmp("TouchPinchingEventId", str) == 0) return TouchPinchingEventId;
            if (std::strcmp("TouchPinchingEndedEventId", str) == 0) return TouchPinchingEndedEventId;
            return InvalidMessageId;
        };
    };
    typedef Ptr<Message> (*CreateCallback)();
    static CreateCallback jumpTable[InputProtocol::MessageId::NumMessageIds];
    class Factory {
    public:
        static Ptr<Message> Create(MessageIdType id);
    };
    class MouseMoveEvent : public Message {
        OryolClassDecl(MouseMoveEvent);
        OryolTypeDecl(MouseMoveEvent,Message);
    public:
        MouseMoveEvent() {
            this->msgId = MessageId::MouseMoveEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::MouseMoveEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        glm::vec2 Movement;
        glm::vec2 Position;
    };
    class MouseButtonEvent : public Message {
        OryolClassDecl(MouseButtonEvent);
        OryolTypeDecl(MouseButtonEvent,Message);
    public:
        MouseButtonEvent() {
            this->msgId = MessageId::MouseButtonEventId;
            this->Down = false;
            this->Up = false;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::MouseButtonEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        Oryol::Mouse::Button MouseButton;
        bool Down;
        bool Up;
    };
    class MouseScrollEvent : public Message {
        OryolClassDecl(MouseScrollEvent);
        OryolTypeDecl(MouseScrollEvent,Message);
    public:
        MouseScrollEvent() {
            this->msgId = MessageId::MouseScrollEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::MouseScrollEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        glm::vec2 Scroll;
    };
    class KeyEvent : public Message {
        OryolClassDecl(KeyEvent);
        OryolTypeDecl(KeyEvent,Message);
    public:
        KeyEvent() {
            this->msgId = MessageId::KeyEventId;
            this->Key = Oryol::Key::InvalidKey;
            this->Down = false;
            this->Up = false;
            this->Repeat = false;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::KeyEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        Oryol::Key::Code Key;
        bool Down;
        bool Up;
        bool Repeat;
    };
    class WCharEvent : public Message {
        OryolClassDecl(WCharEvent);
        OryolTypeDecl(WCharEvent,Message);
    public:
        WCharEvent() {
            this->msgId = MessageId::WCharEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::WCharEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        wchar_t WChar;
    };
    class TouchTappedEvent : public Message {
        OryolClassDecl(TouchTappedEvent);
        OryolTypeDecl(TouchTappedEvent,Message);
    public:
        TouchTappedEvent() {
            this->msgId = MessageId::TouchTappedEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchTappedEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        glm::vec2 Pos;
    };
    class TouchDoubleTappedEvent : public Message {
        OryolClassDecl(TouchDoubleTappedEvent);
        OryolTypeDecl(TouchDoubleTappedEvent,Message);
    public:
        TouchDoubleTappedEvent() {
            this->msgId = MessageId::TouchDoubleTappedEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchDoubleTappedEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        glm::vec2 Pos;
    };
    class TouchPanningStartedEvent : public Message {
        OryolClassDecl(TouchPanningStartedEvent);
        OryolTypeDecl(TouchPanningStartedEvent,Message);
    public:
        TouchPanningStartedEvent() {
            this->msgId = MessageId::TouchPanningStartedEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPanningStartedEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        glm::vec2 Pos;
        glm::vec2 StartPos;
    };
    class TouchPanningEvent : public Message {
        OryolClassDecl(TouchPanningEvent);
        OryolTypeDecl(TouchPanningEvent,Message);
    public:
        TouchPanningEvent() {
            this->msgId = MessageId::TouchPanningEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPanningEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        glm::vec2 Pos;
        glm::vec2 StartPos;
    };
    class TouchPanningEndedEvent : public Message {
        OryolClassDecl(TouchPanningEndedEvent);
        OryolTypeDecl(TouchPanningEndedEvent,Message);
    public:
        TouchPanningEndedEvent() {
            this->msgId = MessageId::TouchPanningEndedEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPanningEndedEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        glm::vec2 Pos;
        glm::vec2 StartPos;
    };
    class TouchPinchingStartedEvent : public Message {
        OryolClassDecl(TouchPinchingStartedEvent);
        OryolTypeDecl(TouchPinchingStartedEvent,Message);
    public:
        TouchPinchingStartedEvent() {
            this->msgId = MessageId::TouchPinchingStartedEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPinchingStartedEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        glm::vec2 Pos0;
        glm::vec2 Pos1;
        glm::vec2 StartPos0;
        glm::vec2 StartPos1;
    };
    class TouchPinchingEvent : public Message {
        OryolClassDecl(TouchPinchingEvent);
        OryolTypeDecl(TouchPinchingEvent,Message);
    public:
        TouchPinchingEvent() {
            this->msgId = MessageId::TouchPinchingEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPinchingEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        glm::vec2 Pos0;
        glm::vec2 Pos1;
        glm::vec2 StartPos0;
        glm::vec2 StartPos1;
    };
    class TouchPinchingEndedEvent : public Message {
        OryolClassDecl(TouchPinchingEndedEvent);
        OryolTypeDecl(TouchPinchingEndedEvent,Message);
    public:
        TouchPinchingEndedEvent() {
            this->msgId = MessageId::TouchPinchingEndedEventId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPinchingEndedEventId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        glm::vec2 Pos0;
        glm::vec2 Pos1;
        glm::vec2 StartPos0;
        glm::vec2 StartPos1;
    };
};
}
