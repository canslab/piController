#include "msgassembler.h"

using namespace piController;
using namespace std;


void MsgAssembler::assembleMsg(MsgType msgType, MotorType motorType, const vector<int>& values)
{
    assert(values.size() >= 0);

    assert(msgType == MsgType::MOTOR_MSG);

    assert(motorType == MotorType::DC_MOTOR
           || motorType == MotorType::SERVO_MOTOR_HORIZONTAL
           || motorType == MotorType::SERVO_MOTOR_VERTICAL);

    // first clear the m_msgChunk in order to be ready to contain other MsgChunk
    m_msgChunk.Clear();

    // set fields..
    m_msgChunk.set_msgtype(p_getMsgType(msgType));
    m_msgChunk.set_motortype(p_getMotorType(motorType));
    for(auto& eachValue : values)
    {
        m_msgChunk.add_values(eachValue);
    }

    // save To String Container. It is not of type string, just used to contain serialized message.
    // don't confuse it
    m_assembledMsg = m_msgChunk.SerializeAsString();

    // Assemble Complete Flag
    m_assembleDone = true;
}

void MsgAssembler::assembleMsg(MsgType msgType, const vector<int>& values)
{
    assert(values.size() >= 0);
    assert(msgType == MsgType::NORMAL_MSG);

    // first clear the m_msgChunk in order to be ready to contain other MsgChunk
    m_msgChunk.Clear();

    // set fields..
    m_msgChunk.set_msgtype(p_getMsgType(msgType));

    for(auto& eachValue : values)
    {
        m_msgChunk.add_values(eachValue);
    }

    // save To String Container. It is not of type string, just used to contain serialized message.
    // don't confuse it
    m_assembledMsg = m_msgChunk.SerializeAsString();

    // Assemble Complete Flag
    m_assembleDone = true;
}

MsgAssembler::MsgAssembler()
{
    this->m_assembleDone = false;
}

/**********************************************************************************
 *
 *          [   Const Methods   ]
 *
 * getAssembledMsg() => return serialized message.
 * isAssembledDone() => check whether serialization is done
 *
 * p_getMsgType(MsgType type) => convert type to internal protobuffer type
 *                               (that is known only in this class)
 *
 * p_getMotorType(MotorType type) => conver type to internal protobuffer type
 *                               (that is known only in this class)
 *
 * *******************************************************************************/

const string& MsgAssembler::getAssembledMsg() const
{
    assert(m_assembleDone == true);
    return m_assembledMsg;
}

bool MsgAssembler::isAssembleDone() const
{
    return m_assembleDone;
}

MsgChunk_MsgType MsgAssembler::p_getMsgType(MsgAssembler::MsgType type) const
{
    switch(type)
    {
        case MsgType::MOTOR_MSG:
            return piController::MsgChunk_MsgType_MOTOR_CONTRL;
        case MsgType::NORMAL_MSG:
            return piController::MsgChunk_MsgType_NORMAL;
    }
}

MsgChunk_MotorType MsgAssembler::p_getMotorType(MsgAssembler::MotorType type) const
{
    switch(type)
    {
        case MotorType::DC_MOTOR:
            return piController::MsgChunk_MotorType_DCMotor;

        case MotorType::SERVO_MOTOR_HORIZONTAL:
            return piController::MsgChunk_MotorType_ServoMotor_HORIZONTAL;

        case MotorType::SERVO_MOTOR_VERTICAL:
            return piController::MsgChunk_MotorType_ServoMotor_VERTICAL;
    }
}

