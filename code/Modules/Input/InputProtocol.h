#pragma once
//-----------------------------------------------------------------------------
/* #version:12#
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
            MouseMoveId = Protocol::MessageId::NumMessageIds, 
            MouseButtonId,
            MouseScrollId,
            KeyId,
            WCharId,
            TouchTappedId,
            TouchDoubleTappedId,
            TouchPanningStartedId,
            TouchPanningId,
            TouchPanningEndedId,
            TouchPinchingStartedId,
            TouchPinchingId,
            TouchPinchingEndedId,
            NumMessageIds
        };
        static const char* ToString(MessageIdType c) {
            switch (c) {
                case MouseMoveId: return "MouseMoveId";
                case MouseButtonId: return "MouseButtonId";
                case MouseScrollId: return "MouseScrollId";
                case KeyId: return "KeyId";
                case WCharId: return "WCharId";
                case TouchTappedId: return "TouchTappedId";
                case TouchDoubleTappedId: return "TouchDoubleTappedId";
                case TouchPanningStartedId: return "TouchPanningStartedId";
                case TouchPanningId: return "TouchPanningId";
                case TouchPanningEndedId: return "TouchPanningEndedId";
                case TouchPinchingStartedId: return "TouchPinchingStartedId";
                case TouchPinchingId: return "TouchPinchingId";
                case TouchPinchingEndedId: return "TouchPinchingEndedId";
                default: return "InvalidMessageId";
            }
        };
        static MessageIdType FromString(const char* str) {
            if (std::strcmp("MouseMoveId", str) == 0) return MouseMoveId;
            if (std::strcmp("MouseButtonId", str) == 0) return MouseButtonId;
            if (std::strcmp("MouseScrollId", str) == 0) return MouseScrollId;
            if (std::strcmp("KeyId", str) == 0) return KeyId;
            if (std::strcmp("WCharId", str) == 0) return WCharId;
            if (std::strcmp("TouchTappedId", str) == 0) return TouchTappedId;
            if (std::strcmp("TouchDoubleTappedId", str) == 0) return TouchDoubleTappedId;
            if (std::strcmp("TouchPanningStartedId", str) == 0) return TouchPanningStartedId;
            if (std::strcmp("TouchPanningId", str) == 0) return TouchPanningId;
            if (std::strcmp("TouchPanningEndedId", str) == 0) return TouchPanningEndedId;
            if (std::strcmp("TouchPinchingStartedId", str) == 0) return TouchPinchingStartedId;
            if (std::strcmp("TouchPinchingId", str) == 0) return TouchPinchingId;
            if (std::strcmp("TouchPinchingEndedId", str) == 0) return TouchPinchingEndedId;
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
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
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
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
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
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
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
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
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
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
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
    class TouchTapped : public Message {
        OryolClassDecl(TouchTapped);
        OryolTypeDecl(TouchTapped,Message);
    public:
        TouchTapped() {
            this->msgId = MessageId::TouchTappedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchTappedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetPos(const glm::vec2& val) {
            this->pos = val;
        };
        const glm::vec2& GetPos() const {
            return this->pos;
        };
private:
        glm::vec2 pos;
    };
    class TouchDoubleTapped : public Message {
        OryolClassDecl(TouchDoubleTapped);
        OryolTypeDecl(TouchDoubleTapped,Message);
    public:
        TouchDoubleTapped() {
            this->msgId = MessageId::TouchDoubleTappedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchDoubleTappedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetPos(const glm::vec2& val) {
            this->pos = val;
        };
        const glm::vec2& GetPos() const {
            return this->pos;
        };
private:
        glm::vec2 pos;
    };
    class TouchPanningStarted : public Message {
        OryolClassDecl(TouchPanningStarted);
        OryolTypeDecl(TouchPanningStarted,Message);
    public:
        TouchPanningStarted() {
            this->msgId = MessageId::TouchPanningStartedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPanningStartedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetPos(const glm::vec2& val) {
            this->pos = val;
        };
        const glm::vec2& GetPos() const {
            return this->pos;
        };
        void SetStartPos(const glm::vec2& val) {
            this->startpos = val;
        };
        const glm::vec2& GetStartPos() const {
            return this->startpos;
        };
private:
        glm::vec2 pos;
        glm::vec2 startpos;
    };
    class TouchPanning : public Message {
        OryolClassDecl(TouchPanning);
        OryolTypeDecl(TouchPanning,Message);
    public:
        TouchPanning() {
            this->msgId = MessageId::TouchPanningId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPanningId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetPos(const glm::vec2& val) {
            this->pos = val;
        };
        const glm::vec2& GetPos() const {
            return this->pos;
        };
        void SetStartPos(const glm::vec2& val) {
            this->startpos = val;
        };
        const glm::vec2& GetStartPos() const {
            return this->startpos;
        };
private:
        glm::vec2 pos;
        glm::vec2 startpos;
    };
    class TouchPanningEnded : public Message {
        OryolClassDecl(TouchPanningEnded);
        OryolTypeDecl(TouchPanningEnded,Message);
    public:
        TouchPanningEnded() {
            this->msgId = MessageId::TouchPanningEndedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPanningEndedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetPos(const glm::vec2& val) {
            this->pos = val;
        };
        const glm::vec2& GetPos() const {
            return this->pos;
        };
        void SetStartPos(const glm::vec2& val) {
            this->startpos = val;
        };
        const glm::vec2& GetStartPos() const {
            return this->startpos;
        };
private:
        glm::vec2 pos;
        glm::vec2 startpos;
    };
    class TouchPinchingStarted : public Message {
        OryolClassDecl(TouchPinchingStarted);
        OryolTypeDecl(TouchPinchingStarted,Message);
    public:
        TouchPinchingStarted() {
            this->msgId = MessageId::TouchPinchingStartedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPinchingStartedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetPos0(const glm::vec2& val) {
            this->pos0 = val;
        };
        const glm::vec2& GetPos0() const {
            return this->pos0;
        };
        void SetPos1(const glm::vec2& val) {
            this->pos1 = val;
        };
        const glm::vec2& GetPos1() const {
            return this->pos1;
        };
        void SetStartPos0(const glm::vec2& val) {
            this->startpos0 = val;
        };
        const glm::vec2& GetStartPos0() const {
            return this->startpos0;
        };
        void SetStartPos1(const glm::vec2& val) {
            this->startpos1 = val;
        };
        const glm::vec2& GetStartPos1() const {
            return this->startpos1;
        };
private:
        glm::vec2 pos0;
        glm::vec2 pos1;
        glm::vec2 startpos0;
        glm::vec2 startpos1;
    };
    class TouchPinching : public Message {
        OryolClassDecl(TouchPinching);
        OryolTypeDecl(TouchPinching,Message);
    public:
        TouchPinching() {
            this->msgId = MessageId::TouchPinchingId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPinchingId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetPos0(const glm::vec2& val) {
            this->pos0 = val;
        };
        const glm::vec2& GetPos0() const {
            return this->pos0;
        };
        void SetPos1(const glm::vec2& val) {
            this->pos1 = val;
        };
        const glm::vec2& GetPos1() const {
            return this->pos1;
        };
        void SetStartPos0(const glm::vec2& val) {
            this->startpos0 = val;
        };
        const glm::vec2& GetStartPos0() const {
            return this->startpos0;
        };
        void SetStartPos1(const glm::vec2& val) {
            this->startpos1 = val;
        };
        const glm::vec2& GetStartPos1() const {
            return this->startpos1;
        };
private:
        glm::vec2 pos0;
        glm::vec2 pos1;
        glm::vec2 startpos0;
        glm::vec2 startpos1;
    };
    class TouchPinchingEnded : public Message {
        OryolClassDecl(TouchPinchingEnded);
        OryolTypeDecl(TouchPinchingEnded,Message);
    public:
        TouchPinchingEnded() {
            this->msgId = MessageId::TouchPinchingEndedId;
        };
        static Ptr<Message> FactoryCreate() {
            return Create();
        };
        static MessageIdType ClassMessageId() {
            return MessageId::TouchPinchingEndedId;
        };
        virtual bool IsMemberOf(ProtocolIdType protId) const override {
            if (protId == 'IPPT') return true;
            else return Message::IsMemberOf(protId);
        };
        void SetPos0(const glm::vec2& val) {
            this->pos0 = val;
        };
        const glm::vec2& GetPos0() const {
            return this->pos0;
        };
        void SetPos1(const glm::vec2& val) {
            this->pos1 = val;
        };
        const glm::vec2& GetPos1() const {
            return this->pos1;
        };
        void SetStartPos0(const glm::vec2& val) {
            this->startpos0 = val;
        };
        const glm::vec2& GetStartPos0() const {
            return this->startpos0;
        };
        void SetStartPos1(const glm::vec2& val) {
            this->startpos1 = val;
        };
        const glm::vec2& GetStartPos1() const {
            return this->startpos1;
        };
private:
        glm::vec2 pos0;
        glm::vec2 pos1;
        glm::vec2 startpos0;
        glm::vec2 startpos1;
    };
};
}
