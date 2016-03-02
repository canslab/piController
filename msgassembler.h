#ifndef MSGASSEMBLER_H
#define MSGASSEMBLER_H
#include <string>
#include <vector>
#include "protobuf/MsgChunk.pb.h"

/************************************************************************
 *
 *  This class provides convenient ways to serialized message using protobuf
 *
 *  Client just call assebleMsg() function and pass the set of arguments.
 *  and getAssembledMsg() function return the serialized message.
 *
 * *********************************************************************/
class MsgAssembler
{
public:
    // enumeration type define.
    enum MsgType
    {
        NORMAL_MSG,
        MOTOR_MSG,
    };

    enum MotorType
    {
        DC_MOTOR,
        SERVO_MOTOR_HORIZONTAL,
        SERVO_MOTOR_VERTICAL,
    };

public:
    // check whether serialization is done.
    bool isAssembleDone() const;
    // get the serialized message( string type, it is just of type container )
    const std::string& getAssembledMsg() const;

public:
    void assembleMsg(MsgType msgType, MotorType motorType, const std::vector<int>& values);
    void assembleMsg(MsgType msgType, const std::vector<int>& values);

public:
    MsgAssembler();

private:
    piController::MsgChunk_MsgType   p_getMsgType(MsgType type)     const;
    piController::MsgChunk_MotorType p_getMotorType(MotorType type) const;

    std::string m_assembledMsg;
    piController::MsgChunk m_msgChunk;
    bool m_assembleDone;

};

#endif // MSGASSEMBLER_H
