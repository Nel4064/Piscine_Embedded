#include "i2c_status_msg.h"

const char *i2c_status_desc(uint8_t status_code)
{
    switch(status_code)
	{
        case TW_BUS_ERROR:              return "Bus Error";
        case TW_START:                  return "START transmitted";
        case TW_REP_START:              return "Repeated START transmitted";
        case TW_MT_SLA_ACK:             return "MT: SLA+W, ACK";
        case TW_MT_SLA_NACK:            return "MT: SLA+W, NACK";
        case TW_MT_DATA_ACK:            return "MT: Data, ACK";
        case TW_MT_DATA_NACK:           return "MT: Data, NACK";
        case TW_MT_ARB_LOST:            return "MT: Arbitration lost";
        case TW_MR_SLA_ACK:             return "MR: SLA+R, ACK";
        case TW_MR_SLA_NACK:            return "MR: SLA+R, NACK";
        case TW_MR_DATA_ACK:            return "MR: Data, ACK";
        case TW_MR_DATA_NACK:           return "MR: Data, NACK";
        case TW_SR_SLA_ACK:             return "SR: SLA+W, ACK";
        case TW_SR_ARB_LOST_SLA_ACK:    return "SR: Arb lost, SLA+W, ACK";
        case TW_SR_GCALL_ACK:           return "SR: General call, ACK";
        case TW_SR_ARB_LOST_GCALL_ACK:  return "SR: Arb lost, gen call, ACK";
        case TW_SR_DATA_ACK:            return "SR: Data, ACK";
        case TW_SR_DATA_NACK:           return "SR: Data, NACK";
        case TW_SR_GCALL_DATA_ACK:      return "SR: Gen call data, ACK";
        case TW_SR_GCALL_DATA_NACK:     return "SR: Gen call data, NACK";
        case TW_SR_STOP:                return "SR: STOP or repeated START";
        case TW_ST_SLA_ACK:             return "ST: SLA+R, ACK";
        case TW_ST_ARB_LOST_SLA_ACK:    return "ST: Arb lost, SLA+R, ACK";
        case TW_ST_DATA_ACK:            return "ST: Data, ACK";
        case TW_ST_DATA_NACK:           return "ST: Data, NACK";
        case TW_ST_LAST_DATA:           return "ST: Last data, ACK";
        case TW_NO_INFO:                return "No relevant info";
        default:                        return "Unknown status code";
    }

	// static const char *const desc[256] =
	// {
	// 	[TW_BUS_ERROR]              = "Bus Error (illegal START or STOP condition)",     // 0x00
	// 	[TW_START]                  = "START condition transmitted",                     // 0x08
	// 	[TW_REP_START]              = "Repeated START condition transmitted",            // 0x10
	// 	[TW_MT_SLA_ACK]             = "Master Transmitter: SLA+W transmitted, ACK received",  // 0x18
	// 	[TW_MT_SLA_NACK]            = "Master Transmitter: SLA+W transmitted, NACK received", // 0x20
	// 	[TW_MT_DATA_ACK]            = "Master Transmitter: Data transmitted, ACK received",   // 0x28
	// 	[TW_MT_DATA_NACK]           = "Master Transmitter: Data transmitted, NACK received",  // 0x30
	// 	[TW_MT_ARB_LOST]            = "Master: Arbitration lost",                        // 0x38
	// 	[TW_MR_SLA_ACK]             = "Master Receiver: SLA+R transmitted, ACK received",     // 0x40
	// 	[TW_MR_SLA_NACK]            = "Master Receiver: SLA+R transmitted, NACK received",    // 0x48
	// 	[TW_MR_DATA_ACK]            = "Master Receiver: Data received, ACK returned",    // 0x50
	// 	[TW_MR_DATA_NACK]           = "Master Receiver: Data received, NACK returned",   // 0x58
	// 	[TW_SR_SLA_ACK]             = "Slave Receiver: SLA+W received, ACK returned",    // 0x60
	// 	[TW_SR_ARB_LOST_SLA_ACK]    = "Slave Receiver: Arbitration lost, SLA+W received, ACK returned", // 0x68
	// 	[TW_SR_GCALL_ACK]           = "Slave Receiver: General call received, ACK returned",  // 0x70
	// 	[TW_SR_ARB_LOST_GCALL_ACK]  = "Slave Receiver: Arbitration lost, general call received, ACK returned", // 0x78
	// 	[TW_SR_DATA_ACK]            = "Slave Receiver: Data received, ACK returned",     // 0x80
	// 	[TW_SR_DATA_NACK]           = "Slave Receiver: Data received, NACK returned",    // 0x88
	// 	[TW_SR_GCALL_DATA_ACK]      = "Slave Receiver: General call data received, ACK returned",  // 0x90
	// 	[TW_SR_GCALL_DATA_NACK]     = "Slave Receiver: General call data received, NACK returned", // 0x98
	// 	[TW_SR_STOP]                = "Slave Receiver: STOP or repeated START received", // 0xA0
	// 	[TW_ST_SLA_ACK]             = "Slave Transmitter: SLA+R received, ACK returned", // 0xA8
	// 	[TW_ST_ARB_LOST_SLA_ACK]    = "Slave Transmitter: Arbitration lost, SLA+R received, ACK returned", // 0xB0
	// 	[TW_ST_DATA_ACK]            = "Slave Transmitter: Data transmitted, ACK received",    // 0xB8
	// 	[TW_ST_DATA_NACK]           = "Slave Transmitter: Data transmitted, NACK received",   // 0xC0
	// 	[TW_ST_LAST_DATA]           = "Slave Transmitter: Last data transmitted, ACK received", // 0xC8
	// 	[TW_NO_INFO]                = "No relevant state information",                   // 0xF8
	// };

	// return (desc[status_code] ? desc[status_code] : "Unknown status code");
}

// DS40002061B-page 227 / Status codes for Master Transmitter Mode
// DS40002061B-page 230 / Status codes for Master Receiver Mode
// DS40002061B-page 233 / Status Codes for Slave Receiver Mode
// DS40002061B-page 236 / Status Codes for Slave Transmitter Mode
// DS40002061B-page 237 / Miscellaneous States
