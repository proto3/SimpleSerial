#include "SimpleSerialProtocol.h"

//----------------------------------------------------------------------------//
SimpleSerialProtocol::SimpleSerialProtocol()
: state(NO_MESSAGE), data_counter(0), valid(false)
{}
//----------------------------------------------------------------------------//
bool SimpleSerialProtocol::refresh()
{
    while(Serial1.available())
    {
        uint8_t byte = Serial1.read();

        if(byte & 0x80)
            process_control_byte(byte);
        else
            process_content_byte(byte);

        if(valid)
        {
            valid = false;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------//
uint8_t SimpleSerialProtocol::get_type()
{
    return type;
}
//----------------------------------------------------------------------------//
bool SimpleSerialProtocol::has_data()
{
    return data_counter == 5;
}
//----------------------------------------------------------------------------//
uint32_t SimpleSerialProtocol::get_data()
{
    return data32;
}
//----------------------------------------------------------------------------//
void SimpleSerialProtocol::process_control_byte(uint8_t byte)
{
     //set MSB to 0 to address control value from 0 to 127
    uint8_t control = byte & 0x7F;

    switch(control)
    {
        case START:
            reset_data();
            state = WAITING_TYPE;
            break;
        case END:
            if(WAITING_DATA)
                state = WAITING_CRC;
            else
                state = NO_MESSAGE;
            break;
    }
}
//----------------------------------------------------------------------------//
void SimpleSerialProtocol::process_content_byte(uint8_t byte)
{
    switch(state)
    {
        case WAITING_TYPE:
            type = byte;
            reset_partity(type);
            state = WAITING_DATA;
            break;
        case WAITING_DATA:
            if(data_counter < 5)
            {
                append_data(byte);
                add_partity(byte);
            }
            else //too much data in this packet, ignore it
                state = NO_MESSAGE;
            break;
        case WAITING_CRC:
            valid = check_parity(byte);
            state = NO_MESSAGE;
            break;
        case NO_MESSAGE:
            //Do nothing (communication error)
            break;
    }
}
//----------------------------------------------------------------------------//
void SimpleSerialProtocol::reset_data()
{
    data32 = 0;
    data_counter = 0;
}
//----------------------------------------------------------------------------//
void SimpleSerialProtocol::append_data(uint8_t byte)
{
    int shift = (24-(8*data_counter)) + data_counter + 1;
    uint32_t shifting_mem = byte;

    if(shift >= 0)
        data32 |= shifting_mem << shift;
    else
        data32 |= shifting_mem >> -shift;

    data_counter++;
}
//----------------------------------------------------------------------------//
void SimpleSerialProtocol::reset_partity(uint8_t byte)
{
    parity = byte;
}
//----------------------------------------------------------------------------//
void SimpleSerialProtocol::add_partity(uint8_t byte)
{
    parity ^= byte;
}
//----------------------------------------------------------------------------//
bool SimpleSerialProtocol::check_parity(uint8_t byte)
{
    return parity == byte && (data_counter == 0 || data_counter == 5);
}
//----------------------------------------------------------------------------//
